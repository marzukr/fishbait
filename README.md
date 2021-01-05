# FISHBAIT
**F**ully **I**nsightful **S**uperhuman **H**old'em **B**ot with
**A**rtificially **I**ntelligent **T**hought. An AI agent designed to play
poker. Inspired by the Pluribus poker bot developed by CMU and Facebook.

## Build Steps
From the root directory:

1. `mkdir -p build`
2. `cd build`
3. `cmake ..`
4. `make`
    - Note: You can also run `make clustering`, `make distribution_gen`,
      `make simulator`, or `make tests` to only build those respective parts of
      the project.

All executables will now be located in `build/bin`. So, for example, to execute
`simulator.out`, simply run `./bin/simulator.out`.
