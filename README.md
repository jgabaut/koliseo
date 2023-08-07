# koliseo

## A C library for a simple arena allocator.

## Table of Contents

+ [What is this thing?](#witt)
  + [Prerequisites](#prerequisites)
  + [Building](#building)
+ [Todo](#todo)

## What is this thing? <a name = "witt"></a>

  This is a C library for an arena allocator, whose arenas are named Koliseo.
  It offers a basic API to perform initalisation, push/pop, reset and free of a Koliseo.

## Prerequisites <a name = "prerequisites"></a>

  To build the `demo` binary, you need:
  * `automake` and `autoconf` to generate the needed `Makefile`
  * `make` to build the binary
  * `gcc`, for building `demo`

  To use `./anvil` to build all amboso-supported tags for `demo` , you also need the same packages.

## Building <a name = "building"></a>

  To build the `demo` binary, run:
  * `autoreconf`, which will complain about missing files
  * `automake --add-missing`, which should provide the missing files
  * `autoreconf`, which should work now
  * `./configure`, which should generate the `Makefile`
  * `make`, to build the binary

## Todo <a name = "todo"></a>
