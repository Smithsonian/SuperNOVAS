# SuperNOVAS CMake Build

As an alternative to the traditional Makefile build, we provide a CMake build as well.
This was done primarily to expand cross-platform support and simplify inclusion in external CMake builds.
This build is not as feature-complete as the Makefile, but should support the most typical usecase.

## Quick Start

```bash
# Basic build
cmake -B build
cmake --build build
```

## Build Options

- `BUILD_SHARED_LIBS=ON|OFF` (default: ON) - Build shared libraries
- `BUILD_STATIC_LIBS=ON|OFF` (default: ON) - Build static libraries in addition to shared
- `BUILD_EXAMPLES=ON|OFF` (default: OFF) - Build the included examples
- `ENABLE_CALCEPH=ON|OFF` (default: OFF) - Enable CALCEPH ephemeris support. Requires CALCEPH installed.

## Build Examples

### Debug Build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

### Full Release Build with CALCEPH
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_CALCEPH=ON
cmake --build build
```

## Installation

This will install all the libraries, headers, CIO data files, CMake config files, and a pkg-config file.

```bash
cmake --build build
cmake --install build --prefix /usr/local
```

## Using in Your Project

After installation:

```cmake
# Link core library
find_package(SuperNOVAS REQUIRED)
target_link_libraries(your_target PRIVATE SuperNOVAS::supernovas)

# If you built with CALCEPH support, link the plugin library
target_link_libraries(your_target PRIVATE SuperNOVAS::solsys-calceph)
```

## Key Differences from Makefile

This CMake build generally follows the Makefile, except for a few key differences:
- Always enables `BUILTIN_SOLSYS3=1` and `DEFAULT_SOLSYS=3`
- Removes legacy compatibility options
- Only supports CALEPH integration
- Doesn't support buildilng the docs, tests, or benchmarks
- *Does* support practically every platform you might want to build for (MacOS/Windows (MinGW)/FreeBSD)
