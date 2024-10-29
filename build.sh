#!/bin/sh

if [[ $# != 1 ]]; then
    echo '\033[31merror\033[0m: Not enough arguments.'
    echo 'Must provide at least one of the following options:'
    echo '  build   - builds the project.'
    echo '  clean   - removes build artifacts.'
    echo '  rebuild - rebuilds this script.'
    exit 1
fi

build() {
    [[ ! -d target/lib ]] && mkdir -p target/lib
    [[ ! -d target/obj ]] && mkdir -p target/obj

    gcc -Wall -Wextra -Wpedantic -I/Users/kwilkinson/dev/lib/kwclib/include -std=c11 -c -fPIC -o /Users/kwilkinson/dev/lib/kwclib/target/obj/system.o /Users/kwilkinson/dev/lib/kwclib/src/system.c
    gcc -std=c11 -shared -o /Users/kwilkinson/dev/lib/kwclib/target/lib/libsystem.so -L/Users/kwilkinson/dev/lib/kwclib/target/lib /Users/kwilkinson/dev/lib/kwclib/target/obj/system.o
    gcc -Wall -Wextra -Wpedantic -I/Users/kwilkinson/dev/lib/kwclib/include -std=c11 -c -fPIC -o /Users/kwilkinson/dev/lib/kwclib/target/obj/monad.o /Users/kwilkinson/dev/lib/kwclib/src/monad.c
    gcc -std=c11 -shared -o /Users/kwilkinson/dev/lib/kwclib/target/lib/libmonad.so -L/Users/kwilkinson/dev/lib/kwclib/target/lib -lsystem /Users/kwilkinson/dev/lib/kwclib/target/obj/monad.o
    gcc -Wall -Wextra -Wpedantic -I/Users/kwilkinson/dev/lib/kwclib/include -std=c11 -c -fPIC -o /Users/kwilkinson/dev/lib/kwclib/target/obj/main.o /Users/kwilkinson/dev/lib/kwclib/src/main.c
    gcc -Wall -Wextra -Wpedantic -I/Users/kwilkinson/dev/lib/kwclib/include -std=c11 -o /Users/kwilkinson/dev/lib/kwclib/target/main -L/Users/kwilkinson/dev/lib/kwclib/target/lib -lmonad /Users/kwilkinson/dev/lib/kwclib/target/obj/main.o
}

clean() {
    rm -rf target
}

rebuild() {
    $(which python3) scripts/mkbuild.py
}

time $@
