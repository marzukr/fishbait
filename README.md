# FISHBAIT 🐟
**F**ully **I**nsightful **S**uperhuman **H**old'em **B**ot with
**A**rtificially **I**ntelligent **T**hought. An AI agent designed to play
poker. Inspired by the Pluribus poker bot developed by CMU and Facebook.

## Build Steps
From the root directory:

1. `mkdir -p build`
2. `cd build`
3. `cmake ..`
    - Use `cmake -DCMAKE_BUILD_TYPE=Debug ..` to debug with gdb or lldb.
    - Use `cmake -DCMAKE_BUILD_TYPE=Release ..` to run in release mode. Release
      mode will add optimizations that make the code run faster.
    - `cmake ..` will use the last value specified for `CMAKE_BUILD_TYPE`. If no
      value is specified, it will use neither.
4. `make`
    - Note: You can also run `make <target>` to only build a specific part of
      the project where `<target>` is one of the following:
      - `clustering.out`
      - `deck`
      - `hand_strengths`
      - `hand_strengths.out`
      - `sequence_table`
      - `tests.out`

All executables will now be located in `build/bin`. So, for example, to execute
`poker_demo.out`, simply run `./bin/poker_demo.out` from the `build` directory.

## Testing
Unit tests can be run with `./bin/tests.out` from the `build` directory.
A code coverage report can be generated with `make coverage` from the `build`
directory. `gcov` and `lcov` need to be installed for code coverage.

## Other Notes
* Do not put commas in the Catch2 tester descriptions
* Confirmed compilers and commits
  * Apple clang version 12.0.0