# Fishbait 🐟
**F**ully **I**nsightful **S**uperhuman **H**old'em **B**ot with
**A**rtificially **I**ntelligent **T**hought. An AI agent designed to play
poker. Inspired by the Pluribus poker bot developed by CMU and Facebook.

## Development Setup
```sh
python -m venv api/venv
api/venv/bin/pip install -r api/requirements.txt
cp .githooks/pre-commit .git/hooks/pre-commit
git submodule update --init --recursive
cp ai/src/mccfr/hyperparameters.h.dev ai/src/mccfr/hyperparameters.h
docker compose -f docker-compose.dev.yml build ai
docker run --rm -v ./ai/out:/build/out fishbait-ai /ai/dev_blueprint.sh
docker compose -f docker-compose.dev.yml build
docker compose -f docker-compose.dev.yml up
```

## Deployment
1. Build the project as detailed above.
2. `cp ai/src/mccfr/hyperparameters.h.big ai/src/mccfr/hyperparameters.h`
3. `cp nginx.conf.example nginx.conf`
    * Set the `server_name` property to be the deployment url of the interface
4. `cp .env.example .env` and configure
5. `docker compose -f docker-compose.prod.yml up`
6. Configure HTTPS with AWS Application Load Balancer

## Testing
```sh
docker run --rm fishbait-ai sh -c "cd /build && ./bin/tests.out"
```

## Requirements
- Docker
- Python 3.12
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
