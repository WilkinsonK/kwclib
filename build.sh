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

    echo '\033[34mbuilding\033[0m /Users/kwilkinson/dev/lib/kwclib/target/obj/kwsys.o'
    gcc -DDEBUG=0 -Wall -Wextra -Wpedantic -I/Users/kwilkinson/dev/lib/kwclib/include -std=c11 -c -fPIC -o /Users/kwilkinson/dev/lib/kwclib/target/obj/kwsys.o /Users/kwilkinson/dev/lib/kwclib/src/kwsys.c
    echo '\033[34mbuilding\033[0m /Users/kwilkinson/dev/lib/kwclib/target/lib/libkwsys.so'
    gcc -DDEBUG=0 -std=c11 -shared -o /Users/kwilkinson/dev/lib/kwclib/target/lib/libkwsys.so -L/Users/kwilkinson/dev/lib/kwclib/target/lib /Users/kwilkinson/dev/lib/kwclib/target/obj/kwsys.o
    echo '\033[34mbuilding\033[0m /Users/kwilkinson/dev/lib/kwclib/target/obj/main.o'
    gcc -DDEBUG=0 -Wall -Wextra -Wpedantic -I/Users/kwilkinson/dev/lib/kwclib/include -std=c11 -c -fPIC -o /Users/kwilkinson/dev/lib/kwclib/target/obj/main.o /Users/kwilkinson/dev/lib/kwclib/src/main.c
    echo '\033[34mbuilding\033[0m /Users/kwilkinson/dev/lib/kwclib/target/main'
    gcc -DDEBUG=0 -Wall -Wextra -Wpedantic -I/Users/kwilkinson/dev/lib/kwclib/include -std=c11 -o /Users/kwilkinson/dev/lib/kwclib/target/main -L/Users/kwilkinson/dev/lib/kwclib/target/lib -lkwsys /Users/kwilkinson/dev/lib/kwclib/target/obj/main.o
}

clean() {
    rm -rf target
}

rebuild() {
    echo '\033[34mrebuilding\033[0m build.sh'
    $(which python3) scripts/mkbuild.py
}

time $@
