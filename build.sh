#!/bin/bash

set -e
set -x

IMAGE=spielhuus/toolchain:1.0.1
PID=$(sudo docker run -itd -v $(pwd):/repo -v $(pwd)/.build:/.build $IMAGE /bin/sh)

echo "build avcpp version: $2"
DOCKER_EXEC="sudo docker exec $PID /bin/sh -c"
$DOCKER_EXEC "conan user -p $1 -r conan-cpp squawkcpp"
$DOCKER_EXEC "cd /repo && \
    conan export . conan-cpp/latest && \
    conan install avcpp/0.2.3@conan-cpp/latest --build=missing && \
    conan upload avcpp/0.2.3@conan-cpp/latest --all -r=conan-cpp"

sudo docker rm -f $PID
