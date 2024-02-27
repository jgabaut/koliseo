# koliseo

## A C library for a simple arena allocator.

## Table of Contents

+ [What is this thing?](#witt)
  + [Basic example](#basic_example)
  + [Extra features](#extra_features)
    + [Region](#extra_region)
    + [Curses](#extra_curses)
    + [Debug](#extra_debug)
    + [Gulp](#extra_gulp)
    + [How to use extras](#extra_howto)
  + [Documentation](#docs)
  + [Prerequisites](#prerequisites)
  + [Configuration](#config)
  + [Building](#building)
  + [Supported platforms](#support)
+ [Credits](#credits)
+ [Todo](#todo)

## What is this thing? <a name = "witt"></a>

  This is a C library for an arena allocator, whose arenas are named `Koliseo`.
  It offers a basic API to perform initalisation, push/pop, reset and free of a `Koliseo`.

  If you compile it without defining any special macros, you will get the basic functionality.

## Basic example <a name = "basic_example"></a>

  This is a basic usage example, initialising a Koliseo and then pushing an example pointer.
  You can build it by running:

  `gcc static/basic_example.c src/koliseo.c -o basic_example`

  For a more complete example, including `Koliseo_Temp` usage, check out [demo.c](./static/demo.c) provided in `static` folder.


```c
typedef struct Example {
    int val;
} Example;

#include "../src/koliseo.h"

int main(void)
{
    //Init the arena
    Koliseo* kls = kls_new(KLS_DEFAULT_SIZE);

    //Use the arena (see demo for Koliseo_Temp usage)
    Example* e = KLS_PUSH(kls, Example);
    e->val = 42;

    //Show contents to stdout
    print_dbg_kls(kls);

    //Free the arena
    kls_free(kls);
    return 0;
}
```

  After including the `koliseo.h` header:

  - To initialise a default arena:
  ```c
  Koliseo* kls_new(size_t)
  ```
  - To request memory for a specific type:
  ```c
  Type* KLS_PUSH(Koliseo* kls, Type)
  ```
  - For C strings, you can use:
  ```c
  char* KLS_PUSH_STR(Koliseo* kls, char* cstring)
  ```
  - Also, a somewhat unreliable dupe utility macro:
  ```c
  char* KLS_STRDUP(Koliseo* kls, char* source, char* dest)
  ```
  - To free the arena:
  ```c
  void kls_free(Koliseo* kls)
  ```

  For more documentation on the available functions, see [this section.](#docs)

## Extra features <a name = "extra_features"></a>

  By default, extended functionalities are not included in the build, with each feature needing a preprocessor macro to be defined before including the library header.
  You can find hints on configuration [here,](#config) or the list of macros [here.](#extras_howto)

### Region <a name = "extra_region"></a>

  A ready-to-go index for every allocation you make.
  - It uses an intrusive linked list and (at the moment) has quite the memory overhead, due to hosting a couple static string buffers for the tags, so it may not be suited for all usecases.
  - Offers extended API with tagging arguments, to type/name your references
  - For now, two allocations backends can be chosen for the list, it can be stored:
    - In an inner Koliseo (this puts an extra limit to the total number of single allocations)
    - Using the global allocator (from malloc)
  - Extra utility functions
    - Help you estimate relative memory usage by some particular type of object. May prove useful in some scenarios.

### Curses <a name = "extra_curses"></a>

  Utility functions that extend ncurses API to provide debug info.

### Core debug <a name = "extra_debug"></a>

  Extra debug for core calls, may be too verbose for some applications.

### Gulp <a name = "extra_gulp"></a>

  Utility to memory-map a file (always the best idea, right?) to a C string, by providing the filepath.
  - Also includes a minimal string-view API, in case you want to work on the file contents differently.

### How to enable extra features<a name = "extra_howto"></a>

  To aid in building with extra features, see [this section.](#config)

  The preprocessor macros to enable them manually are:

  - Region: `KOLISEO_HAS_REGION`
  - Curses: `KOLISEO_HAS_CURSES`
  - Debug: `KLS_DEBUG_CORE`
  - Gulp: `KOLISEO_HAS_GULP`

## Documentation <a name = "docs"></a>

  HTML docs are available at [this Github Pages link](https://jgabaut.github.io/koliseo-docs/index.html).

  You can also get the ready pdf version of the docs from the latest release.

  If you have `doxygen` you can generate the HTML yourself, or even the pdf if you have `doxygen-latex` or equivalents.

## Prerequisites <a name = "prerequisites"></a>

  To build the `demo` binary, you need:
  * `automake` and `autoconf` to generate the needed `Makefile`
  * `make` to build the binary
  * `gcc` or `clang`, for building `demo`


  To bootstrap and use the `./anvil` tool to build all amboso-supported tags for `demo`, you also need either:

  * `bash >4.x, gawk` if you want to use `amboso`
  * `rustc` if you want to use `invil`


## Configuration <a name = "config"></a>

  To prepare the files needed by `autotools`, run:

  ```sh
  aclocal
  autoconf
  automake --add-missing
  ./configure # Optionally, with --enable-debug=yes or --host
  make
  ```

  You will get a `./configure` script, which you can use to enable debug mode or other features.

  - Run `./configure --host x86-64-w64-mingw32` to setup the `Makefile` appropriately for a `x86_64-w64-mingw32` build.
  - Run `./configure --enable-debug=yes` to setup the `Makefile` appropriately and build with `-DKLS_DEBUG_CORE` flag.
    - By default, enabling debug this way also adds `-DKLS_SETCONF_DEBUG` to the demo build. This preproc guard lets you really debug kls initialisation, by printing logs from inside `kls_set_conf()`.
  - Run `./configure --enable-region=yes` to setup the `Makefile` appropriately and build with `-DKOLISEO_HAS_REGION` flag.
  - Run `./configure --enable-curses=yes` to setup the `Makefile` appropriately and build with `-DKOLISEO_HAS_CURSES` flag.
  - Run `./configure --enable-gulp=yes` to setup the `Makefile` appropriately and build with `-DKOLISEO_HAS_GULP` flag.


## Building <a name = "building"></a>

  To build both the `libkoliseo.so` lib and `demo` binary, run:
  * `./configure`, which should generate the `Makefile`. See [Configuration](#config) section.
  * `make`, to build all target

## Supported platforms <a name = "support"></a>

  ATM the code should build for:
  - `x86_64-Linux`
  - `darwin-arm64`
  - `x86_64-w64-mingw32` to target `Windows`.
    - This build, while mostly identical to `Linux`/`macOS`, is less tested.

## Credits <a name = "credits"></a>

  Thanks to [skeeto](https://www.reddit.com/user/skeeto/) for showing me the original base implementation.

  Thanks to [Mako](https://www.instagram.com/mako_x_tattoo/) for the repo banner.

  Thanks to [Tsoding](https://github.com/tsoding) for its creative string view library ([repo](https://github.com/tsoding/sv)), which indeed does things so simply you mostly can't do anything different.

## Todo <a name = "todo"></a>

  - Break up internal extensions to the core functionality
    - Maybe move the guarded code into separate headers?
  - Model `KLS_Temp_Conf` to still be included without `Region` feature
  - At the moment, the arena can't grown its own underlying buffer.
    - Add backwards-compatible logic to enable growable arenas.
  - Clean up the `Windows` part of the includes, to have minimal definitions from `windows.h`.
