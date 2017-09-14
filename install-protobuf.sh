#!/bin/sh
set -ex
wget https://github.com/google/protobuf/releases/download/v3.1.0/protobuf-cpp-3.1.0.tar.gz
tar -xzvf protobuf-cpp-3.1.0.tar.gz
cd protobuf-3.1.0 && ./configure --prefix=/usr && make -j4 && sudo make install