# Fishbait 🐟
**F**ully **I**nsightful **S**uperhuman **H**old'em **B**ot with
**A**rtificially **I**ntelligent **T**hought. An AI agent designed to play
poker. Inspired by the Pluribus poker bot developed by CMU and Facebook.

## Build Steps
From the root directory:

1. `mkdir -p build`
2. `cd build`
3. `cmake <options> ..`
    * Possible Options:
        * `-DCMAKE_C_COMPILER=<c-compiler>` and 
          `-DCMAKE_CXX_COMPILER=<cpp-compiler>`
        * `-DCMAKE_BUILD_TYPE=Debug` to debug with gdb or lldb.
        * `-DCMAKE_BUILD_TYPE=Release` to run in release mode. Release mode
          will add optimizations that makes the code run faster.
        * `-DTGT_SYSTEM=Graviton2` for AWS Graviton2 optimizations.
        * `-DCORES=<core_count>` to instruct parallel algorithms to use
          `<core_count>` cores.
4. `cmake --build .`

All executables will now be located in `build/bin`. So, for example, to execute
`poker_demo.out`, simply run `./bin/poker_demo.out` from the `build` directory.

## Testing
Unit tests can be run with `./bin/tests.out` from the `build` directory.
If the project is built in debug mode, a code coverage report can be generated
with `make coverage` from the `build` directory. `gcov` and `lcov` need to be
installed for code coverage.

## Requirements
- C++17
- [CMake](https://cmake.org) >= 3.20
- [Gcov](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) (for code coverage)
- [LCOV](http://ltp.sourceforge.net/coverage/lcov.php) (for code coverage)
- Python 3
- [cpplint](https://github.com/cpplint/cpplint) (runs during the pre-commit
  hook)
- [pylint](https://pylint.org) (runs during the pre-commit hook)

## Other Notes
* Do not put commas in the Catch2 tester descriptions
* This project follows the [Google C++ Style guide](https://google.github.io/styleguide/cppguide.html)
  with the following exceptions:
    * Exceptions are allowed
    * Constructors are allowed to do work and fail
    * Integer types other than `int` are allowed
    * Type names with acronyms (like `OCHS_N`) may have underscores.
* This project follows the [Google Python Style guide](https://google.github.io/styleguide/pyguide.html)
  with the following exceptions:
    * 2 spaces are used for indentation
* Needs to be run on a processor where 32 bit integer reads and writes are
  atomic.
