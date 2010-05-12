#!/bin/sh
# Build script for Steve

[ -d build ] && rm -rf build && mkdir build
cd build
cmake ..
make install
