FROM ubuntu:22.04 as base
RUN apt-get -y update \
    && apt-get -y install \
    build-essential \
    cmake \
    clang \
    && rm -rf /var/lib/apt/lists/*


FROM base as build
WORKDIR /app
COPY . .
RUN cmake -S . -B build \
    && cmake --build build \
    && ctest --test-dir build
