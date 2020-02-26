FROM ubuntu:latest as builder

RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    git \
    libssl-dev \
    software-properties-common && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && \
    apt-get -y install gcc-9 g++-9 && \
    update-alternatives \
    --install /usr/bin/gcc gcc /usr/bin/gcc-9 90 \
    --slave /usr/bin/g++ g++ /usr/bin/g++-9 \
    --slave /usr/bin/gcov gcov /usr/bin/gcov-9 && \
    wget -qO- "https://cmake.org/files/v3.16/cmake-3.16.4-Linux-x86_64.tar.gz" | tar --strip-components=1 -xz -C /usr/local

COPY . .
RUN cmake -S . -B build -DGIT_SUBMODULE=OFF&& \
    cmake --build build --config Release --target QCEC_app

FROM ubuntu:latest

COPY --from=builder /build/QCEC_app /usr/local/bin/QCEC_app
