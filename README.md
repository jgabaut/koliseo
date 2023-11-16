# koliseo

## A C library for a simple arena allocator.

## Table of Contents

+ [What is this thing?](#witt)
  + [Basic example](#basic_example)
  + [Documentation](#docs)
  + [Prerequisites](#prerequisites)
  + [Configuration](#config)
  + [Building](#building)
  + [Supported platforms](#support)
+ [Credits](#credits)
+ [Todo](#todo)

## What is this thing? <a name = "witt"></a>

  This is a C library for an arena allocator, whose arenas are named Koliseo.
  It offers a basic API to perform initalisation, push/pop, reset and free of a Koliseo.

  It also has a tagging system to type/name your references, and a couple functions to help you estimate relative memory usage by some particular type of object.
  You can also use it to memory-map a file (always the best idea, right?) to a C string, by providing the filepath.

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
    Example* e = KLS_PUSH(kls,Example,1);
    e->val = 42;

    //Show contents to stdout
    print_dbg_kls(kls);

    //Free the arena
    kls_free(kls);
    return 0;
}
```

## Documentation <a name = "docs"></a>

  HTML docs are available at [this Github Pages link](https://jgabaut.github.io/koliseo-docs/index.html).

  You can also get the ready pdf version of the docs from the latest release.

  If you have `doxygen` you can generate the HTML yourself, or even the pdf if you have `doxygen-latex` or equivalents.

## Prerequisites <a name = "prerequisites"></a>

  To build the `demo` binary, you need:
  * `automake` and `autoconf` to generate the needed `Makefile`
  * `make` to build the binary
  * `gcc` or `clang`, for building `demo`

  To use `./anvil` to build all amboso-supported tags for `demo` , you also need the same packages.

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
  - Run `./configure --enable-debug=yes` to setup the `Makefile` appropriately and build the `demo` binary with `-DKLS_DEBUG_CORE` flag.
    - By default, enabling debug this way also adds `-DKLS_SETCONF_DEBUG` to the demo build. This preproc guard lets you really debug kls initialisation, by printing logs from inside `kls_set_conf()`.


## Building <a name = "building"></a>

  To build the `demo` binary, run:
  * `./configure`, which should generate the `Makefile`. See [Configuration](#config) section.
  * `make`, to build the binary

## Supported platforms <a name = "support"></a>

  ATM the code should build for:
  - `x86_64-Linux`
  - `x86_64-w64-mingw32` to target `Windows`, but ATM there is no guarantee it works as intended there.

## Credits <a name = "credits"></a>

  Thanks to [skeeto](https://www.reddit.com/user/skeeto/) for showing me the original base implementation.

  Thanks to [Mako](https://www.instagram.com/mako_x_tattoo/) for the repo banner.

  Thanks to [Tsoding](https://github.com/tsoding) for its creative string view library ([repo](https://github.com/tsoding/sv)), which indeed does things so simply you mostly can't do anything different.

## Todo <a name = "todo"></a>
