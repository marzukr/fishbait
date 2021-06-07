# FISHBAIT 🐟
**F**ully **I**nsightful **S**uperhuman **H**old'em **B**ot with
**A**rtificially **I**ntelligent **T**hought. An AI agent designed to play
poker. Inspired by the Pluribus poker bot developed by CMU and Facebook.

## Build Steps
From the root directory:

1. `mkdir -p build`
2. `cd build`
3. `cmake ..`
  - Add `-D CMAKE_C_COMPILER=<c-compiler>` and 
    `-D CMAKE_CXX_COMPILER=<cpp-compiler>` before `..` to set the compilers.
4. `cmake --build . --config Debug`
  - Use `--config Debug` to debug with gdb or lldb.
  - Use `--config Release` to run in release mode. Release mode will add
    optimizations that makes the code run faster.

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
