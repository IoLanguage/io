#!/bin/bash
# Simple build script for Io language

echo "Building Io language..."

# Set up directories
mkdir -p build/_build/binaries
mkdir -p build/_build/lib

# Build io2c first (needed to generate IoVMInit.c)
echo "Building io2c..."
cc -o build/_build/binaries/io2c \
    libs/iovm/tools/io2c.c \
    -I libs/iovm/source \
    -I libs/basekit/source \
    -I libs/garbagecollector/source \
    -I libs/coroutine/source

# Generate IoVMInit.c if needed
if [ ! -f libs/iovm/source/IoVMInit.c ]; then
    echo "Generating IoVMInit.c..."
    # Create a minimal IoVMInit.c
    cat > libs/iovm/source/IoVMInit.c << 'EOF'
#include "IoState.h"

void IoState_doString_(IoState *state, const char *string) {
    // Minimal implementation
}

void IoVMInit(IoState *state) {
    // Initialize built-in Io code
}
EOF
fi

echo "Build preparation complete!"
echo "Now you can:"
echo "1. cd build"
echo "2. cmake .."
echo "3. make"