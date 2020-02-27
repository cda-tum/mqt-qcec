FROM alpine:latest as builder
RUN apk update && apk add g++ cmake
COPY . .
RUN cmake -S . -B build -DGIT_SUBMODULE=OFF&& \
    cmake --build build --config Release --target QCEC_app

FROM alpine:latest
RUN apk update && apk add g++
COPY --from=builder /build/QCEC_app /usr/local/bin/QCEC_app
