FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    gcc \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -B build -G Ninja
RUN cmake --build build
