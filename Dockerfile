FROM alpine:3.20.3 AS builder

RUN apk add --no-cache \
    git \
    cmake \
    make \
    g++

WORKDIR /root/wordle

COPY words.txt .
COPY cmake/ cmake/
COPY CMakeLists.txt .

RUN mkdir -p app/ && echo "" > app/dummy.cpp && \
    mkdir -p src/ && echo "" > src/dummy.cpp && \
    mkdir -p test/ && echo "" > test/dummy.cpp && \
    cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=23 \
    -DCMAKE_EXE_LINKER_FLAGS="-static" && \
    cmake --build build -t Catch2WithMain && \
    cmake --build build -t screen && \
    cmake --build build -t dom && \
    cmake --build build -t component

COPY include/ include/
COPY src/ src/

RUN cmake -B build && \
    cmake --build build -t wordle

COPY app/ app/
COPY test/ test/

RUN cmake -B build && \
    cmake --build build -t main && \
    cmake --build build -t test_all && \
    ./build/test_all

FROM alpine:3.20.3

WORKDIR /root/wordle

COPY --from=builder /root/wordle/build/wordle .
COPY words.txt .

CMD ["./wordle"]
