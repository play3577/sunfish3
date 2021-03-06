Sunfish
========

Sunfish is computer Shogi program.

This program participated in World Computer Shogi Championship(WCSC) 2015.

Requirement
-----------

- CMake
- C++ Compiler(GCC4.9/Clang/MSVC2013)

Build
-----

### GNU Make

```
make release

# parallel
make release -j

# clean
make clean

# unit test
make test

# profiling
make profile
```

### Xcode

```
mkdir -p build/xcode
cd build/xcode
cmake -G Xcode ../../src
```

### Visual Studio

Set parameters into CMake as follows.

| Name                        | Value                  |
|:----------------------------|-----------------------:|
| Where is the source code    | path/to/sunfish3/src   |
| Where to build the binaries | path/to/sunfish3/build |

Usage
-----

### Execute

```
./sunfish

# help
./sunfish --help

# network (CSA protocol)
./sunfish -n
```

### Import fv.bin

```
rm eval.bin
cp path/to/bonanza/fv.bin ./fv.bin
./sunfish
```

License
-------

MIT License
