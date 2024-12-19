#!/bin/bash
set -e

apt-get update
apt-get install -y valgrind

cd /build
valgrind --leak-check=summary ./bin/tests.out 2>&1 \
    | tee /dev/stderr \
    | grep -q "All heap blocks were freed -- no leaks are possible"
