FROM ubuntu:24.04

RUN apt-get update && \
    apt-get install -y \
        clang build-essential cmake wget && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /ai
COPY ./external ./external
COPY ./src ./src
COPY ./tests ./tests
COPY CMakeLists.txt dev_blueprint.sh .

ARG MAX_THREADS=1

WORKDIR /build
RUN cmake \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    # -DCMAKE_BUILD_TYPE=Debug
    -DCMAKE_BUILD_TYPE=Release \
    # -DTGT_SYSTEM=Graviton2
    -DCORES=${MAX_THREADS} \
    /ai && \
    cmake --build . -- -j ${MAX_THREADS}

WORKDIR /ai
COPY dev_blueprint.sh check_leaks.sh .
RUN chmod +x dev_blueprint.sh check_leaks.sh
