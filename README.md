# koliseo

## A C library for a simple arena allocator.

## Table of Contents

+ [What is this thing?](#witt)
  + [Basic example](#basic_example)
  + [Prerequisites](#prerequisites)
  + [Configuration](#config)
  + [Building](#building)
  + [Supported platforms](#support)
+ [Credits](#credits)
+ [Todo](#todo)

## What is this thing? <a name = "witt"></a>

  This is a C library for an arena allocator, whose arenas are named Koliseo.
  It offers a basic API to perform initalisation, push/pop, reset and free of a Koliseo.

## Basic example <a name = "basic_example"></a>

  This is a basic usage example, initialising a Koliseo and then pushing an example pointer.
  For a more complete example, check out [demo.c](./static/demo.c)

```c

typedef struct Example {
    int val;
} Example;

#include "koliseo.h"

int main(void)
{
    //Init the arena
    Koliseo* kls = kls_new(KLS_DEFAULT_SIZE);

    //Use the arena (see demo for Koliseo_Temp usage)
    Example* e = KLS_PUSH(kls,Example,1);
    e->val = 42;

    //Free the arena
    kls_free(kls);
    return 0;
}
```

## Prerequisites <a name = "prerequisites"></a>

  To build the `demo` binary, you need:
  * `automake` and `autoconf` to generate the needed `Makefile`
  * `make` to build the binary
  * `gcc` or `clang`, for building `demo`

  To use `./anvil` to build all amboso-supported tags for `demo` , you also need the same packages.

## Configuration <a name = "config"></a>

  To prepare the files needed by `autotools`, run:

  - `autoreconf`, which will complain about missing files
  - `automake --add-missing`, which should provide the missing files
  - `autoreconf`, which should work now.

  You will get a `./configure` script, which you can use to enable debug mode or other features.

  - Run `./configure --host x86-64-w64-mingw32` to setup the `Makefile` appropriately for a `x86_64-w64-mingw32` build.
  - Run `./configure --enable-debug=yes` to setup the `Makefile` appropriately and build the `demo` binary with `-DKLS_DEBUG_CORE` flag.


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

## Todo <a name = "todo"></a>
