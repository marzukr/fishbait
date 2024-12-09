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

WORKDIR /build
RUN cmake \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++ \
    # -DCMAKE_BUILD_TYPE=Debug
    -DCMAKE_BUILD_TYPE=Release \
    # -DTGT_SYSTEM=Graviton2
    -DCORES=1 \
    /ai
RUN cmake --build . -- -j 1

WORKDIR /ai
COPY dev_blueprint.sh .
RUN chmod +x dev_blueprint.sh
