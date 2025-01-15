# SIP Project Deliverables

1. **Deliverable 1:** _Extend parser to support SIP language_ - refer to `SOLUTION-D1.md` for solution and process
2. **Deliverable 2:** _Extend AST tree build to support SIP language_ - refer to `SOLUTION-D2.md` for solution and process
3. **Deliverable 3:** _Extend the semantic analyses to support SIP langauge_ - refer to `SOLUTION-D3.md` for solution and process

<br />

[![Build Status](https://github.com/matthewbdwyer/tipc/actions/workflows/build-and-test.yml/badge.svg?branch=main)](https://github.com/matthewbdwyer/tipc/actions/workflows/build-and-test.yml)
[![codecov.io](https://codecov.io/gh/matthewbdwyer/tipc/coverage.svg?branch=main&flag=codecoverage)](https://codecov.io/gh/matthewbdwyer/tipc?branch=main)
[![codecov.io](https://img.shields.io/endpoint?logo=codecov&logoColor=blue&url=https%3A%2F%2Fmatthewbdwyer.github.io%2Ftipc%2Fdoccoverage.json)](https://codecov.io/gh/matthewbdwyer/tipc?branch=main)

# tipc

A compiler from TIP to llvm bitcode

## TIP Language, Interpreter, and Analyzers

TIP is a "Tiny Imperative Programming" language developed by Anders M&#248;ller and Michael I. Schwartzbach for the [Static Program Analysis](https://cs.au.dk/~amoeller/spa/ "Static Program Analysis") lecture notes that they developed for graduate instruction at Aarhus University.

Accompanying those notes is a [Scala implementation](https://github.com/cs-au-dk/TIP/) that provides a number of static analysis implementations and interpreter-based evaluators.

This project implements `tipc` which compiles TIP programs into LLVM bitcode. Linking that bitcode with the [runtime library](./rtlib) routines and standard libraries will produce an executable.

## Dependencies

`tipc` is implemented in C++17 and depends on a number of tools and packages, e.g., [ANTLR4](https://www.antlr.org), [Catch2](https://github.com/catchorg/Catch2), [CMake](https://cmake.org/), [Doxygen](https://www.doxygen.nl/), [loguru](https://github.com/emilk/loguru), [Java](https://www.java.com), [LLVM](https://www.llvm.org). To simplify dependency management the project provides a [bootstrap](bin/bootstrap.sh) script to install all of the required dependencies on linux ubuntu and mac platforms; if you are using `portal.cs.virginia.edu` to build then you can replace this script with running `module load <pathto>/tipc/conf/modulefiles/tipc/F24`, where `<pathto>` is the path to where you have installed `tipc`.

## Building tipc

The project uses [GitHub Actions](https://docs.github.com/en/actions) for building and testing and [CodeCov](https://codecov.io) for reporting code and documentation coverage. The [build-and-test.yml](.github/workflows/build-and-test.yml) file provides details of this process. If you would prefer to build and test manually then read on.

After cloning this repository you can build the compiler by moving to into the top-level directory and issuing these commands:

1. `./bin/bootstrap.sh`
2. `. ~/.bashrc`
3. `mkdir build`
4. `cd build`
5. `cmake ..`
6. `make`

The build process will download an up to date version of ANTLR4 if needed, build the C++ target for ANTLR4, and then build all of `tipc` including its substantial body of unit tests. This may take some time - to speed it up use multiple threads in the `make` command, e.g., `make -j6`.

You may see some warnings, e.g., CMake policy warnings, due to some of the packages we use in the project. As those projects are updated, to avoid CMake feature deprecation, these will go away.

When finished the `tipc` executable will be located in `build/src/`. You can copy it to a more convenient location if you like, but a number of scripts in the project expect it to be in this location so don't move it.

The project includes more than 300 unit tests grouped into several executables. The project also includes more than 90 system tests. These are TIP programs that have built in test oracles that check for the expected results. For convenience, there is a `runtests.sh` script provided in the `bin` directory. You can run this script to invoke the entire collection of tests. See the `README` in the bin directory for more information.

All of the tests should pass.

### Ubuntu Linux

Our continuous integration process builds on both Ubuntu 22.04 and 20.04, so these are well-supported. We do not support other linux distributions, but we know that people in the past have ported `tipc` to different distributions.

### Windows Subsystem for Linux

If you are using a Windows machine, tipc can be built in the Windows Subsystem for Linux (WSL). [Here](https://docs.microsoft.com/en-us/windows/wsl/install-win10#update-to-wsl-2) are instructions to install WSL and upgrade to WSL2. It is highly recommended to upgrade to WSL2. Once installed, you should install
[Ubuntu 20.04](https://docs.microsoft.com/en-us/windows/wsl/install-win10#update-to-wsl-2). Once finished, you can open a virtual instance of Ubuntu and follow
the instructions above to build tipc.

You may recieve an error saying "No CMAKE_CXX_COMPILER could be found" when running `cmake ..`. If this is the case, you should install g++ with the command: `sudo apt-get install g++`.

## Using tipc

The `tipc` compiler has a limited set of options available through the `--help` flag.

```
OVERVIEW: tipc - a TIP to llvm compiler

USAGE: tipc [options] <tip source file>

OPTIONS:

Generic Options:

  --help                 - Display available options (--help-hidden for more)
  --help-list            - Display list of available options (--help-list-hidden for more)
  --version              - Display the version of this program

tipc Options:
Options for controlling the TIP compilation process.

  --asm                          - emit human-readable LLVM assembly language
  --do                           - disable bitcode optimization
  --log=<logfile>                - log all messages to logfile (enables --verbose 3)
  -o=<outputfile>                - write output to <outputfile>
  --pa=<AST output file>         - print AST to a file in dot syntax
  --pcg=<call graph output file> - print call graph to a file in dot syntax
  --pi                           - perform polymorphic type inference
  --pp                           - pretty print
  --ps                           - print symbols
  --pt                           - print symbols with types (supercedes --ps)
  --verbose=<int>                - enable log messages (Levels 1-3)
                                    Level 1 - Basic logging for every phase.
                                    Level 2 - Level 1 and type constraints being unified.
                                    Level 3 - Level 2 and union-find solving steps.
```

By default it will accept a `.tip` file, parse it, perform a series of semantic analyses to determine if it is a legal TIP program, generate LLVM bitcode, and emit a `.bc` file which is a binary encoding of the bitcodes. You can see a human readable version of the bitcodes by running `llvm-dis` on the `.bc` file.

To produce an executable version of a TIP program, the `.bc` file must be linked with the bitcode for [tip_rtlib.c](rtlib/tip_rtlib.c). Running the `build.sh` script in the [rtlib](rtlib) directory once will create that library bitcode file.

The link step is performed using `clang` which will include additional libraries needed by [tip_rtlib.c](rtlib/tip_rtlib.c).

For convenience, we provide a script [build.sh](bin/build.sh) that will compile the tip program and perform the link step. The script can be used within this git repository, or if you define the shell variable `TIPDIR` to the path to the root of the repository you can run it from any location as follows:

```
$ cd
$ more hello.tip
main() { return 42; }
$ $HOME/tipc/bin/build.sh hello.tip
$ ./hello
Program output: 42
$ $HOME/tipc/bin/build.sh -pp -pt hello.tip
main()
{
  return 42;
}

Functions : {
  main : () -> int
}

Locals for function main : {

}
```

## Working with tipc

The instructions above, and the scripts described below, make it possible to develop from the command line. This gives you lots of control, but it means you will miss the benefit of modern IDEs. Below we describe how to set up the CLion IDE for use with the project.

During development you need only run build steps 1 through 5 a single time, unless you modify some `CMakeLists.txt` file. Just run `make` in the build directory to rebuild after making changes to the source.

If you do need to add a source file then you will have to edit the appropriate `CMakeLists.txt` file to add it. In this case, you should:

- `cd build`
- `rm CMakeCache.txt`
- `cmake ..`

which will regenerate the makefiles that you can then run, by typing `make`, to build.

Note that the `tipg4` directory has a standalone ANTLR4 grammar. It's README describes how to build it in isolation and run it using the ANTLR4 jar file.

### The bin directory

To facilitate development of `tipc` we have collected a number of helper scripts into the `bin` directory of the project. Among them are scripts to run the entire test bed (`runtests.sh`), to run a code coverage analysis (`gencov.sh`), and to generate the project documentation (`gendocs.sh`). Please see the `README` in the bin directory for example usages.

When rebuilding and rerunning tests you may get errors about
failing to merge `gcov` files. This happens when `gcov` files linger from previous
runs. To cleanup these messages, simply run the `cleancov.sh` script.

### Log Messages

When working on the tipc compiler, it may be helpful to enable logging messages when testing your changes on programs. We have inserted logging messages using loguru. These can be turned using the flag `--verbose [x]` where x is a number between 1-3. These messages get more verbose as you increase x. The first setting shows when symbols are added to the symbol table and when type constraints are generated for the type solver. The second setting shows the previous information and type constraints being unified. The third setting shows types being search for and added into the type graph. When adding to theses features, you can add logging messages by adding a line `LOG_S(x)` where x is an integer to describe the level of log verbosity you want. You can use the existing levels or make new levels.

## Code Style

tipc follows [llvm coding
standards](https://llvm.org/docs/CodingStandards.html#llvm-coding-standards).
`clang-format` is used to apply the llvm style rules. The following command can
be used to apply the llvm style across the tipc `src` directory.

```bash
find src -iname *.h -o -iname *.cpp | xargs clang-format -style=llvm -i
```

Using [pre-commit](https://pre-commit.com/) we can enforce styling before each
commit. This is encourged to keep a uniform style across the codebase. Install
pre-commit by following the
[instructions](https://pre-commit.com/#installation) in their documentation.
Then, install the tipc hooks by running,

```bash
pre-commit install
```

Now, `c++` and `cmake` formatting will be checked before each commit.
