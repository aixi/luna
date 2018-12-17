#!/usr/bin/env bash

echo "Configuring and building luna..."

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4