# Fishbait 🐟
**F**ully **I**nsightful **S**uperhuman **H**old'em **B**ot with
**A**rtificially **I**ntelligent **T**hought. An AI agent designed to play
poker. Inspired by the Pluribus poker bot developed by CMU and Facebook.

## Development Setup
From the root directory:

1. Create the build folder: `mkdir -p build`
2. Create the hyperparameters file: `cp src/blueprint/hyperparameters.h.dev src/blueprint/hyperparameters.h`
3. Navigate to the build folder: `cd build`
4. Generate the build files: `cmake <options> ..`
    * Possible Options:
        * `-DCMAKE_C_COMPILER=<c-compiler>` and
          `-DCMAKE_CXX_COMPILER=<cpp-compiler>`
        * `-DCMAKE_BUILD_TYPE=Debug` to debug with gdb or lldb.
        * `-DCMAKE_BUILD_TYPE=Release` to run in release mode. Release mode
          will add optimizations that makes the code run faster.
        * `-DTGT_SYSTEM=Graviton2` for AWS Graviton2 optimizations.
        * `-DCORES=<core_count>` to instruct parallel algorithms to use
          `<core_count>` cores.
5. Build the code: `cmake --build .`
   * `cmake --build . -- -j CORES` to run a parallel build.
6. Download the card cluster files to `build/out/clustering`
   * [Flop](https://drive.google.com/file/d/1Q_9M-KGe0855QksD6sro9DI0V4aUwyk9/view?usp=sharing)
   * [Turn](https://drive.google.com/file/d/1KRE-eHi8ryvrnbBjCNCGVTujuCLi9hKz/view?usp=sharing)
   * [River](https://drive.google.com/file/d/1qNSfJKBzAZ2CQGYvplQqjAXPHIkbe3sI/view?usp=sharing)
7. Generate the dev strategy: `./bin/blueprint.out`
8. Convert the dev strategy to the hdf5 format: `./bin/avg_to_hd5.out out/blueprint/dev/average_final.cereal out/blueprint/dev/blueprint_dev.hdf`
9. Create the api config file: `cp ../src/interface/api/settings.py.example ../src/interface/api/settings.py`
10. Start the development web server: `./bin/interface.sh`
   * `./bin/interface.sh frontend` to only start the frontend server
   * `./bin/interface.sh backend` to only start the backend server

All executables will now be located in `build/bin`. So, for example, to execute
`poker_demo.out`, simply run `./bin/poker_demo.out` from the `build` directory.

## Deployment
1. Build the project as detailed above.
2. Copy `src/interface/fishbait.nginx.example` to
   `/etc/nginx/sites-available/fishbait.nginx` and replace the placeholders
     * Set the `host` property to the absolute path of
       `build/src/interface/build`
     * Set the `server_name` property to be whatever url the interface is to be
       deployed on.
3. `sudo ln -s /etc/nginx/sites-available/fishbait.nginx /etc/nginx/sites-enabled/fishbait.nginx`
4. `sudo systemctl reload nginx`
5. Configure `src/interface/api/settings.py`
6. Copy `src/interface/fishbait.service.example` to
   `/etc/systemd/system/fishbait.service` and replace the placeholders
     * Set the `WorkingDirectory` to the absolute path of `src/interface`
     * Set the placeholder in `ExecStart` to be the absolute path of
       `build/src/interface/venv/bin/gunicorn`
7. `sudo systemctl daemon-reload`
8. `sudo systemctl start fishbait`
9. Setup https/ssl with the appropriate
   [certbot instructions](https://certbot.eff.org/)

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
- nginx (to deploy the interface)

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
