"""
Utility module intended for providing shared objects and
functions used across all build scripts.
"""

import enum
import os
import tomllib
from pathlib import Path

CONFIG_DEFAULT = "build.toml"


class Kind(enum.StrEnum):
    """
    Kind of compile operation meant to be generated.
    """

    BIN = "bin"
    OBJ = "obj"
    EXE = "exe"
    ARL = "static"
    SOL = "shared"


class Config(dict):
    """Build configuration."""

    class Build(dict):

        def __str__(self) -> str:
            buf = ""
            buf += f"{self.name!s}{os.linesep}"
            buf += f"  kind:      {self.kind}{os.linesep}"
            buf += f"  libraries: {self.libraries}{os.linesep}"
            buf += f"  output:    {self.output}{os.linesep}"
            buf += f"  sources:   {self.sources}{os.linesep}"
            return buf

        @property
        def name(self) -> Path:
            return Path(self.get("name", "a.out")).resolve()

        @property
        def output(self) -> Path:
            return Path(self.get("output", self.name)).resolve()

        @property
        def kind(self) -> Kind:
            return Kind(self.get("kind", "exe"))

        @property
        def libraries(self) -> list[str]:
            return self.get("libraries", [])

        @property
        def sources(self) -> list[Path]:
            return list(map(Path, self.get("sources", [])))

    class Compile(dict):

        def __str__(self) -> str:
            buf = ""
            buf += f"[compiler_settings]{os.linesep}"
            buf += f"  compiler: {self.compiler}{os.linesep}"
            buf += f"  includes: {self.includes}{os.linesep}"
            buf += f"  standard: {self.standard}{os.linesep}"
            buf += f"  warnings: {self.warnings}{os.linesep}"
            return buf

        @property
        def compiler(self) -> str:
            return self.get("compiler", "gcc")

        @property
        def debug_level(self) -> int:
            return int(self.get("debug_level", "0"))

        @property
        def includes(self) -> list[Path]:
            return list(map(Path, self.get("includes", [])))

        @property
        def libraries(self) -> list[Path]:
            libs = list(map(Path, self.get("libraries", [])))
            return libs

        @property
        def standard(self) -> str:
            return self.get("standard", "c99")

        @property
        def warnings(self) -> list[str]:
            return self.get("warnings", [])

    class Project(dict):

        def __str__(self) -> str:
            buf = ""
            buf += f"[project_settings]{os.linesep}"
            buf += f"  name:    {self.name}{os.linesep}"
            buf += f"  out:     {self.out!s}{os.linesep}"
            buf += f"  version: {self.version}{os.linesep}"
            return buf

        @property
        def name(self) -> str:
            return self.get("name", "")

        @property
        def out(self) -> Path:
            return Path(self.get("out", "target"))

        @property
        def version(self) -> str:
            return self.get("version", "0.0.0")

    def __str__(self) -> str:
        buf = ""
        buf += str(self.project) + os.linesep
        buf += str(self.compile) + os.linesep

        buf += f"[binary]{os.linesep}"
        buf += str(self.bin) + os.linesep

        buf += f"[libraries]{os.linesep}"
        for lib in self.lib:
            buf += str(lib) + os.linesep

        return buf

    @property
    def bin(self) -> Build | None:
        return Config.Build(self.get("bin", dict()))

    @property
    def compile(self) -> Compile:
        return Config.Compile(self.get("compile", dict()))

    @property
    def lib(self) -> list[Build]:
        return list(map(Config.Build, self.get("lib", dict())))

    @property
    def project(self) -> Project:
        return Config.Project(self.get("project", dict()))


def load(path: Path | None = None) -> Config:
    """Loads project build configuration."""

    path = (path or Path(CONFIG_DEFAULT))
    conf = tomllib.loads(path.read_text())
    return Config(**conf)


if __name__ == "__main__":
    conf = load()
    print(conf)
