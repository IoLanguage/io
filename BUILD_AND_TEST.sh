#!/bin/bash
# Complete build and test script for Io language

echo "=== Io Language Build Script ==="
echo "This script will build and test the Io language with our bug fixes"
echo ""

# Navigate to the Io directory
IO_DIR="/Users/david/vcs/git/github/davidcanhelp/io"
cd "$IO_DIR" || exit 1

# Clean up the nested build directories
echo "1. Cleaning up nested build directories..."
rm -rf build/build

# Check if we have a proper build directory
if [ -d "build" ]; then
    echo "   Build directory exists at: $IO_DIR/build"
    cd build
else
    echo "   Creating new build directory..."
    mkdir build
    cd build
fi

echo ""
echo "2. Configuring with CMake..."
if [ -f "CMakeCache.txt" ]; then
    echo "   CMake cache found, using existing configuration"
else
    cmake .. || { echo "CMake configuration failed"; exit 1; }
fi

echo ""
echo "3. Building Io..."
echo "   Using make to build the project..."

# Try to build using make
if command -v make &> /dev/null; then
    make -j4 || make || { echo "Build failed with make"; }
fi

# If make didn't work, try cmake --build
if [ ! -f "_build/binaries/io" ]; then
    echo "   Trying cmake --build..."
    cmake --build . --parallel 4 || cmake --build .
fi

echo ""
echo "4. Checking build results..."
if [ -f "_build/binaries/io" ]; then
    echo "   ✅ Io binary successfully built!"
    echo "   Location: $PWD/_build/binaries/io"
    
    echo ""
    echo "5. Running tests..."
    echo "   Testing our bug fixes..."
    
    # Test the Io binary
    ./_build/binaries/io -e "writeln(\"Hello from Io!\")" 2>/dev/null && echo "   ✅ Basic execution works!"
    
    # Test if we can run the test suite
    if [ -f "../libs/iovm/tests/correctness/run.io" ]; then
        echo "   Running test suite..."
        ./_build/binaries/io ../libs/iovm/tests/correctness/run.io 2>&1 | head -20
    fi
else
    echo "   ❌ Io binary not found. Build may have failed."
    echo "   Checking for build artifacts..."
    
    find . -name "io" -type f 2>/dev/null | head -5
    find . -name "*.a" -type f 2>/dev/null | head -5
fi

echo ""
echo "=== Summary of Changes ==="
echo "The following bug fixes have been applied:"
echo "1. IoFile.c - Fixed double evaluation bug in atPut()"
echo "2. IoDynLib.c - Fixed memory leak for BLOCK types"
echo "3. IoMessage_parser.c - Added proper error messages"
echo "4. run.io - Fixed cross-platform path handling"
echo ""
echo "Build script complete!"