# ozo

[![Build Status](https://travis-ci.org/yandex/ozo.svg?branch=master)](https://travis-ci.org/yandex/ozo)
[![codecov](https://codecov.io/gh/yandex/ozo/branch/master/graph/badge.svg)](https://codecov.io/gh/yandex/ozo)

## What's this

OZO is a C++17 library for asyncronous communication with PostgreSQL DBMS.
The library leverages the power of template metaprogramming, providing convenient mapping from C++ types to SQL along with rich query building possibilities. OZO supports different concurrency paradigms (callbacks, futures, coroutines), using Boost.Asio under the hood. Low-level communication with PostgreSQL server is done via libpq. All concepts in the library are designed to be easily extendable (even replaceable) by the user to simplify adaptation to specific project requirements.

### API

Since the project is on early state of development it lacks of documentation. We understand the importance of good docs and are working hard on this problem. Complete documentation is on the way, but now:

* look at our brand new [HOW TO](docs/howto.md),
* try our [generated from sources documentation](https://yandex.github.io/ozo/) - it is under construction but readable,
* learn more about main use-cases from [unit tests](tests/integration/request_integration.cpp),
* See our [C++Now'18 talk about OZO](https://youtu.be/-1zbaxuUsMA) with [presentation](https://github.com/boostcon/cppnow_presentations_2018/blob/master/05-09-2018_wednesday/design_and_implementation_of_dbms_asynchronous_client_library__roman_siromakha__cppnow_05092018.pdf).

## Compatibilities

For the time OZO is not compatible with new executors models that are used by default since Boost 1.74. The `BOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT` macro needs to be defined. See Boost 1.74 [changelog](https://www.boost.org/doc/libs/1_74_0/doc/html/boost_asio/history.html#boost_asio.history.asio_1_18_0___boost_1_74) for the details.
## Dependencies

These things are needed:

* **CMake** is used as build system
* **GCC** or **Clang** C++ compiler with C++17 support (tested with GCC 7.0, Clang 5.0 and Apple LLVM version 9.0.0)
* **Boost** >= 1.66 with `BOOST_HANA_CONFIG_ENABLE_STRING_UDL` defined.
* **libpq** >= 9.3
* Ozo uses the [resource_pool](https://github.com/elsid/resource_pool) library as a git submodule, so in case of using a package version, this dependency should be satisfied too.

If you want to run integration tests and/or build inside Docker container:
* **Docker** >= 1.13.0
* **Docker Compose** >= 1.10.0

## Build

The library is header-only, but if you want to build and run unit-tests you can do it as listed below.

### Build and run tests on custom environment

First of all you need to satsfy requirements listed above. You can run tests using these commands.

```bash
mkdir -p build
cd build
cmake .. -DOZO_BUILD_TESTS=ON
make -j$(nproc)
ctest -V
```

Or use [build.sh](scripts/build.sh) which accepts folowing commands:

```bash
scripts/build.sh help
```

prints help.

```bash
scripts/build.sh <compiler> <target>
```

build and run tests with specified **compiler** and **target**, the **compiler** parameter can be:

* **gcc** - for build with gcc,
* **clang** - for build with clang.

The **target** parameter depends on **compiler**.
For **gcc**:

* **debug** - for debug build and tests
* **release** - for release build and tests
* **coverage** - for code coverage calculation

For **clang**:

* **debug** - for debug build and tests
* **release** - for release build and tests
* **asan** - for [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html) launch
* **ubsan** - for [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) launch
* **tsan** - for [ThreadSanitizer](https://clang.llvm.org/docs/ThreadSanitizer.html) launch

```bash
scripts/build.sh all
```

build all possible configuration.

```bash
scripts/build.sh docs
```

generates documentation.

### Build and run tests on MacOS 10.X

For MacOS the best way to satisfy minimum requirements is [brew](https://brew.sh/)

```bash
brew install cmake boost libpq postresql
```

### Build and run tests within Docker

To build code and run tests inside docker container:

```bash
scripts/build.sh docker <compiler> <target>
```

To generate documentation using docker container:

```bash
scripts/build.sh docker docs
```

### Test against a local postgres

You can use `scripts/build.sh` but add `pg` first:

```bash
scripts/build.sh pg <compiler> <target>
```

or if you want build code in docker:

```bash
scripts/build.sh pg docker <compiler> <target>
```

This will attempt to launch postgres:alpine from your Docker registry.
Or you can point ozo tests to a postgres of your choosing by setting these environment variables prior to building:

```bash
export OZO_BUILD_PG_TESTS=ON
export OZO_PG_TEST_CONNINFO='your conninfo (connection string)'

scripts/build.sh gcc debug
```
