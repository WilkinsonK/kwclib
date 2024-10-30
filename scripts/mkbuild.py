#!/bin/python3
"""
Configures and generates a build script for the target
project.
"""

import contextlib
import io
import os
import textwrap

import config
import mkcompile


def writer_wrapper(fd: io.TextIOWrapper, padding: int | None = None):
    class _fd:
        def write(self, line: str) -> None:
            fd.write(textwrap.indent(line, " "*(padding or 4)))
        def writeln(self, line: str) -> None:
            fd.write(textwrap.indent(line+os.linesep, " "*(padding or 4)))

    return _fd()


@contextlib.contextmanager
def write_function(fd: io.TextIOWrapper, name: str):
    fd.write(f"{name}() {{{os.linesep}")
    yield writer_wrapper(fd)
    fd.write(f"}}{os.linesep}")


@contextlib.contextmanager
def write_ifstatement(fd: io.TextIOWrapper, condition: str):
    fd.write(f"if [[ {condition} ]]; then{os.linesep}")
    yield writer_wrapper(fd)
    fd.write(f"fi{os.linesep}")


def write_script(fd: io.TextIOWrapper):
    conf       = config.load()
    statements = mkcompile.generate_from_build(conf, conf.bin) #type: ignore

    fd.write("#!/bin/sh" + os.linesep)

    fd.write(os.linesep)
    with write_ifstatement(fd, "$# != 1") as f:
        f.writeln(f"echo '\\033[31merror\\033[0m: Not enough arguments.'")
        f.writeln(f"echo 'Must provide at least one of the following options:'")
        f.writeln(f"echo '  build   - builds the project.'")
        f.writeln(f"echo '  clean   - removes build artifacts.'")
        f.writeln(f"echo '  rebuild - rebuilds this script.'")
        f.writeln(f"exit 1")

    fd.write(os.linesep)
    with write_function(fd, "build") as f:
        out = conf.project.out
        f.writeln(f"[[ ! -d {(out / 'lib')!s} ]] && mkdir -p {(out / 'lib')!s}")
        f.writeln(f"[[ ! -d {(out / 'obj')!s} ]] && mkdir -p {(out / 'obj')!s}")
        f.writeln("")

        for _, file, command in statements:
            f.writeln(f"echo '\\033[34mbuilding\\033[0m {file}'")
            f.writeln(f"{command}")

    fd.write(os.linesep)
    with write_function(fd, "clean") as f:
        f.writeln(f"rm -rf {out!s}")

    fd.write(os.linesep)
    with write_function(fd, "rebuild") as f:
        f.writeln(f"echo '\\033[34mrebuilding\\033[0m build.sh'")
        f.writeln(f"$(which python3) scripts/mkbuild.py")

    fd.write(os.linesep)
    fd.write(f"time $@{os.linesep}")


def main() -> int:
    # Prewrite everything to a buffer to not affect the
    # script during runtime.
    buf = io.StringIO()
    write_script(buf)
    buf.seek(0)

    # Dump contents of buffer into file.
    with open("build.sh", "w") as fd:
        fd.write(buf.read())

    return 0


if __name__ == "__main__":
    exit(main())
