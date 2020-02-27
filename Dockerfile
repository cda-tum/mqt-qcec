FROM alpine:latest as builder
RUN apk update && apk add g++ make cmake
COPY . .
RUN cmake -S . -B build -DGIT_SUBMODULE=OFF&& \
    cmake --build build --config Release --target qcec_app

FROM alpine:latest
RUN apk update && apk add g++
COPY --from=builder /build/qcec_app /usr/local/bin/qcec_app
