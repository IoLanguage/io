#!/bin/bash
# Direct build script for Io

echo "Starting Io build..."
cd /Users/david/vcs/git/github/davidcanhelp/io

# Clean up nested directories
rm -rf build/build/build

# Enter build directory
if [ -d "build" ]; then
    cd build
    echo "Using existing build directory"
else
    mkdir build
    cd build
    echo "Created new build directory"
fi

# Configure if needed
if [ ! -f "Makefile" ]; then
    echo "Configuring with CMake..."
    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_C_FLAGS="-O2" \
          -DCMAKE_OSX_ARCHITECTURES=arm64 \
          ..
fi

# Build
echo "Building with make..."
make -j8 VERBOSE=1

# Check result
if [ -f "_build/binaries/io" ]; then
    echo "✓ Build successful!"
    echo "Binary at: $(pwd)/_build/binaries/io"
    
    # Test it
    echo "Testing binary..."
    ./_build/binaries/io -e 'writeln("Io is working!")'
else
    echo "Build may have failed. Checking for artifacts..."
    find . -name "io" -type f 2>/dev/null | head -5
fi