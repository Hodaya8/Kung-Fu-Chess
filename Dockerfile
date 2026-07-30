FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    git \
    libasio-dev \
    libwebsocketpp-dev \
    nlohmann-json3-dev \
    libpqxx-dev \
    libhiredis-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir -p build && cd build && cmake .. && make KungFuChessServer

CMD ["./build/SERVER/KungFuChessServer"]