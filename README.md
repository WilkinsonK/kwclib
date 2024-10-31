# KW C Library #
A collection of useful tools and utilities for writing
C projects.

## How to Build ##
This library uses its own build system located in the
`./scripts/` subdirectory. Here you will find the
scripts where the resulting `./build.sh` script is
generated.

### The Build Command ####
In order to build this library you would promptly run the
existing `./build.sh build` command.

```sh
$ ./build.sh build
```

This will build the project into the target build directory
along with any build artifacts required to compose the final
`*.so` file.

### Modifying the Build Configuration ###
If you wish to modify the `./build.toml` config file, it is
recommended to rebuild the script. The script is
bootstrapped, and as such, can be regenerated using the
following:

```sh
$ ./build.sh rebuild
```
