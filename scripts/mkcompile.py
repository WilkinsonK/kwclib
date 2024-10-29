#!/bin/python3
"""
This script dictates how compile commands are generated for
the purposes of compiling C object files.
"""

import argparse
import os
import sys
from pathlib import Path
from typing import NoReturn

import config
from config import Config, Kind


class Context:
    """
    Arguments from command line which are common througout
    the interface.
    """

    kind:       Kind
    libraries:  list[str]
    no_compile: bool
    output:     Path
    target:     list[Path]

    no_warning: bool = False
    is_shared:  bool = False

    def __str__(self) -> str:
        buf = ""
        buf += "[build_context]" + os.linesep
        buf += f"  output:     {self.output!s}{os.linesep}"
        buf += f"  target(s):  {self.target}{os.linesep}"
        buf += f"  shared:     {self.is_shared}{os.linesep}"
        buf += f"  kind:       {self.kind}{os.linesep}"
        buf += f"  libraries:  {self.libraries}{os.linesep}"
        buf += f"  no_compile: {self.no_compile}{os.linesep}"
        buf += f"  no_warning: {self.no_warning}{os.linesep}"

        return buf


def context_resolve(conf: Config, context: Context):
    """
    Adjust context values according to project structure
    and configuration.
    """

    if context.output:
        project_out = conf.project.out
        name        = context.output.name
        match context.kind:
            case Kind.OBJ: output = project_out / "obj" / name
            case Kind.ARL: output = project_out / "lib" / name
            case Kind.SOL: output = project_out / "lib" / name
            case _:        output = project_out / name
        context.output = output.resolve()


def generate_flags(conf: Config, context: Context) -> list[str]:
    """Generate common compiler flags."""

    flags = []
    flags.append(conf.compile.compiler)

    if not context.no_warning:
        flags.extend((f"-W{w}" for w in conf.compile.warnings))

    flags.extend((f"-I{p.resolve()}" for p in conf.compile.includes))
    flags.append(f"-std={conf.compile.standard}")

    if context.no_compile:
        flags.append("-c")

    if context.kind is Kind.OBJ:
        flags.append("-fPIC")

    if context.is_shared:
        flags.append("-shared")

    if context.output:
        flags.append(f"-o {context.output}")

    if not context.no_compile:
        flags.extend((f"-L{l.resolve()}" for l in conf.compile.libraries))
        flags.extend((f"-l{l}" for l in context.libraries))
    flags.extend((str(p.resolve()) for p in context.target))

    return flags


def generate_executable(conf: Config, context: Context) -> str:
    """Generate a compiler command for a C object binary."""

    return " ".join(generate_flags(conf, context))


def generate_from_build(conf: Config, build: Config.Build) -> tuple[tuple[int, Path, str], ...]:
    """
    Generates an entire compile sequence for a build
    configuration.

    Unlike other functions in this API, it instead returns
    a sequence of commands that can be called one after
    another.
    Commands are returned as a `tuple` triplet: (Order
    of Priority, Output File, Compile Command).
    """

    # The 'order', determines the order of priority in
    # which a command most likely needs to be executed.
    order    = 1
    sequence = []

    # Some of the libraries a build can be dependant on may
    # exist within our project and/or still need to be
    # built.
    # The 'order' of sequences of these libraries are
    # decreased to have a higher priority.
    for lib in conf.lib:
        if lib.name.name not in build.libraries:
            continue
        s = map(lambda e: (e[0]*0.1, *e[1:]), generate_from_build(conf, lib))
        sequence.extend(s)

    # Group sources according to shared names.
    # i.e., if file 'a.h' and 'a.c', then group
    # as "a": (a.h, a.c).
    sources = {}
    for file in build.sources:
        tname = file.name.split(".")[0]
        sources[tname] = (file, *sources.get(tname, ()))

    # Sources grouped together are then rendered
    # as object file compile commands.
    target = []
    for name, files in sources.items():
        ctx = Context()
        ctx.kind       = Kind.OBJ
        ctx.libraries  = build.libraries
        ctx.no_compile = True
        ctx.output     = Path(f"{name}.o")
        ctx.target     = list(files)
        context_resolve(conf, ctx)

        target.append(ctx.output)
        sequence.append((0.1*order, ctx.output, generate_executable(conf, ctx)))
        order += 1

    # Sources have been resolved at this point, we can now
    # move on to render the final compile command.
    ctx = Context()
    ctx.kind       = build.kind
    ctx.libraries  = build.libraries
    ctx.no_compile = build.kind is Kind.OBJ
    ctx.output     = build.output
    ctx.target     = target
    context_resolve(conf, ctx)

    sequence.append((0.1*order, ctx.output, generate_from_kind(conf, ctx)))
    # Sort the command sequence to prepare for resolving
    # duplicates.
    sequence.sort(key=lambda s: s[1] and s[0])

    final_sequence = [sequence.pop(0)]
    while len(sequence):
        s1 = sequence.pop(0)
        s2 = final_sequence.pop(0)
        # If s1 and s2 point to the same file, we remove
        # the entry with the higher 'order'.
        if s2[1] == s1[1] and s2[0] < s1[0]:
            final_sequence.append(s2)
        elif s2[1] == s1[1] and s2[0] > s1[0]:
            final_sequence.append(s1)
        # If s1 and s2 do not point to the same file, we
        # push them back onto final_sequence.
        else:
            final_sequence.extend([s2, s1])
    # Sort again to ensure order of execution is preserved.
    final_sequence.sort(key=lambda s: s[1] and s[0])

    return tuple(final_sequence)


def generate_from_kind(conf: Config, context: Context) -> str:
    """
    Renders compiler command depending on the kind of
    binary that is expected.
    """

    # Special case. When we are wanting to compile an
    # entire project.
    if context.kind is Kind.BIN:
        buf = ""
        for _, _, cmd in generate_from_build(conf, conf.bin): #type: ignore[argument]
            buf += os.linesep + f"echo '{cmd}'"
            buf += os.linesep + cmd
        return buf

    match context.kind:
        case Kind.ARL: func = generate_static_lib
        case Kind.EXE: func = generate_executable
        case Kind.SOL: func = generate_shared_lib
        case _:        func = generate_unknown_lib

    return func(conf, context)


def generate_shared_lib(conf: Config, context: Context) -> str:
    """Generate a compiler command for a C shared library."""

    context.is_shared = True
    flags = generate_flags(conf, context)
    return " ".join(filter(lambda a: a[:2] not in ("-W", "-I"), flags))


def generate_static_lib(conf: Config, context: Context) -> str:
    """Generate an archive command for a C static library."""

    flags = []
    flags.append("ar")
    flags.append("rcs")

    flags.extend(map(lambda p: str(p.resolve()), context.target))

    output = context.output or Path(f"lib{context.target[0].name}.a")
    libout = str(output.resolve())
    flags.insert(2, libout.replace(".c", "").replace(".o", ""))

    return " ".join(flags)


def generate_unknown_lib(conf: Config, context: Context) -> NoReturn:
    """Handles unknown generation directives."""

    print(f"error: invalid compile type {context.kind!s}", file=sys.stderr)
    exit(-1)


def main() -> int:
    ap = argparse.ArgumentParser(os.path.basename(__file__), description=__doc__)
    ap.add_argument("-k", "--kind", type=Kind, default=Kind.BIN)
    ap.add_argument("-o", "--output", type=Path)
    ap.add_argument("-c", "--no-compile", default=False, action="store_true")
    ap.add_argument("target", type=Path, nargs="*")

    ctx = ap.parse_args(namespace=Context())
    ctx.libraries = []

    conf = config.load()
    print(generate_from_kind(conf, ctx))

    return 0


if __name__ == "__main__":
    exit(main())
    # conf = config.load()
    # for _, _, command in generate_from_build(conf, conf.bin):
    #     print(command)
