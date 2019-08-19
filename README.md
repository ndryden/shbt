# SHBT: Signal Handler/Backtrace library

_Note: SHBT is still in early development and APIs may change._

SHBT provides a simple API for collecting backtraces and installing
signal handlers for C/C++ applications. The signal handlers provide
detailed signal information and a backtrace.

### Motivation

I find it a very useful debugging aide to get a backtrace whenever
fatal signals (e.g. SIGSEGV) occur. However, writing a signal handler
that is _actually_ safe (`async-signal-safe`) is difficult; memory
management (`malloc`, etc.) and buffered I/O are unsafe, as is the
builtin `backtrace` method. In many cases, this doesn't matter; your
program is going to crash and burn anyway, so you might as well get
what information you can out of it. In some cases, however, this can
lead to very weird behavior, like a segmentation fault causing a hang
because of a deadlock between your application and the signal handler.

Some major projects (Chromium, for example) have addressed this within
their codebases, but I could not find any standalone libraries to
accomplish this. Hopefully it is useful to someone.

## Supported platforms

SHBT has been tested on the following operating systems / hardware
combinations:

OS | Architecture
-- | ------------
Linux | x86-64
Linux | POWER / PPC64
macOS | x86-64

If you would like to test and/or port SHBT to a different OS / arch
combination, please feel free.

## Getting Started

SHBT uses the [CMake](https://cmake.org/) build system. To build you
will need a standard C/C++ toolchain and
[libunwind](https://www.nongnu.org/libunwind/) (see also
[the LLVM version](https://github.com/llvm-mirror/libunwind)),
which is probably installed by default with your toolchain.

_Note_: On macOS, libunwind is not installed automatically. The
easiest solution is to use Homebrew to install LLVM and then point
CMake to it with `-D LIBUNWIND_DIR`.

A simple build and install is the following:
```
cd shbt
mkdir build install
cd build
cmake -D CMAKE_INSTALL_PREFIX=../install ..
make
make install
```

### Example

SHBT can add signal handlers for all the standard fatal signals.
Basically:

```C
#include "shbt/shbt.h"

// Inside your main/startup/init function:
shbt_register_fatal_handlers();
```

### Build Options

There are a few options for customizing the build (beyond the standard
CMake ones). They are passed as options when running `cmake`.

* `-D SHBT_ENABLE_MPI=YES|NO` (default: `NO`): Build with MPI support,
  which will have the signal handler print out which rank in
  `MPI_COMM_WORLD` the signal occurred on. This requires an MPI
  installation, which is located using the standard
  [FindMPI](https://cmake.org/cmake/help/latest/module/FindMPI.html)
  CMake package.
* `-D SHBT_DEMANGLER=BUILTIN_IA64|ABI` (default: `BUILTIN_IA64`):
  Select the symbol demangler to use for demangling symbols, in order
  to provide more human-readable function names for C++ code. Options:
  * `BUILTIN_IA64` (the default): Uses a built-in, signal-handler-safe
    demangler. This currently does not support the full demangling
    process, but can generally give a good indication. When using
    this, the original, mangled symbol is printed as well, in case
    demangling fails. Something like `c++filt` can then be used to
    demangle it.
  * `ABI`: Uses the builtin C++ ABI demangling facilities.
    **WARNING**: This is unsafe within signal handlers (it uses memory
    allocation internally), and is intended only for unusual cases.

## Documentation

Coming soon...

## Testing

There is no comprehensive set of tests (yet), but see `tests` for some
examples of triggering signals and catching them.

## Versioning

This project uses [Semantic Versioning](https://semver.org/). The
project is currently in development. See
[the tags](https://github.com/ndryden/shbt/tags) for available
versions.

## Authors

* [Nikoli Dryden](https://ndryden.com/)

See also the
[list of contributors](https://github.com/ndryden/shbt/contributors).

## License

This project is licensed under the
[Apache License, version 2.0](https://www.apache.org/licenses/LICENSE-2.0).
See LICENSE for details.

## Acknowledgements

This project is somewhat based on the signal handler/backtrace
infrastructure in Chromium -- see
[here](http://phajdan-jr.blogspot.com/2013/01/signal-handler-safety-re-entering-malloc.html)
for some details.

This project also includes code adapted from the
[Chromium](https://github.com/chromium/chromium)
and
[glog](https://github.com/google/glog)
projects.
