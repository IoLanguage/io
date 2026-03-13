#!/bin/bash
# Optimized build script for Io on Apple Silicon Macs (M1/M2/M3)
# This script maximizes performance on ARM64 architecture

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect system information
CORES=$(sysctl -n hw.ncpu)
ARCH=$(uname -m)
OS=$(uname -s)

echo -e "${BLUE}=== Io Language Optimized Build for Mac ===${NC}"
echo -e "System: ${GREEN}$OS on $ARCH${NC}"
echo -e "CPU Cores: ${GREEN}$CORES${NC}"
echo ""

# Set optimal flags for M3 MacBook Air
if [[ "$ARCH" == "arm64" ]]; then
    echo -e "${GREEN}✓ Detected Apple Silicon (ARM64)${NC}"
    export CFLAGS="-O3 -march=armv8.5-a -mtune=native -flto=thin -ffast-math"
    export LDFLAGS="-flto=thin -Wl,-dead_strip_dylibs"
    BUILD_TYPE="Release"
else
    echo -e "${YELLOW}⚠ Non-ARM64 architecture detected${NC}"
    export CFLAGS="-O2 -march=native -mtune=native"
    export LDFLAGS=""
    BUILD_TYPE="RelWithDebInfo"
fi

# Function to print step headers
print_step() {
    echo ""
    echo -e "${BLUE}━━━ $1 ━━━${NC}"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check for required tools
print_step "Checking Dependencies"

if ! command_exists cmake; then
    echo -e "${RED}✗ CMake not found${NC}"
    echo "Install with: brew install cmake"
    exit 1
else
    CMAKE_VERSION=$(cmake --version | head -1 | cut -d' ' -f3)
    echo -e "${GREEN}✓ CMake $CMAKE_VERSION${NC}"
fi

if ! command_exists make; then
    echo -e "${RED}✗ Make not found${NC}"
    echo "Install Xcode Command Line Tools: xcode-select --install"
    exit 1
else
    echo -e "${GREEN}✓ Make found${NC}"
fi

if command_exists ninja; then
    echo -e "${GREEN}✓ Ninja build system found (faster builds!)${NC}"
    USE_NINJA=1
    GENERATOR="Ninja"
    BUILD_CMD="ninja"
else
    echo -e "${YELLOW}⚠ Ninja not found (install with: brew install ninja for faster builds)${NC}"
    USE_NINJA=0
    GENERATOR="Unix Makefiles"
    BUILD_CMD="make"
fi

# Navigate to project root
cd "$(dirname "$0")"
PROJECT_ROOT=$(pwd)

print_step "Preparing Build Directory"

# Clean up nested build directories if they exist
if [ -d "build/build" ]; then
    echo "Cleaning up nested build directories..."
    rm -rf build/build
fi

# Create or clean build directory
if [ "$1" == "clean" ]; then
    echo "Performing clean build..."
    rm -rf build
    mkdir build
elif [ ! -d "build" ]; then
    mkdir build
fi

cd build

print_step "Configuring with CMake"

# Configure with optimal settings for M3
CMAKE_ARGS=(
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    -DCMAKE_C_COMPILER="$(which clang)"
    -DCMAKE_OSX_ARCHITECTURES="$ARCH"
    -DCMAKE_INSTALL_PREFIX="/usr/local"
    -G "$GENERATOR"
)

# Add ccache if available for faster rebuilds
if command_exists ccache; then
    echo -e "${GREEN}✓ Using ccache for faster rebuilds${NC}"
    CMAKE_ARGS+=(-DCMAKE_C_COMPILER_LAUNCHER=ccache)
fi

# Run CMake configuration
if [ ! -f "CMakeCache.txt" ] || [ "$1" == "reconfigure" ]; then
    cmake "${CMAKE_ARGS[@]}" ..
else
    echo "Using existing CMake configuration (use './build_optimized.sh reconfigure' to reconfigure)"
fi

print_step "Building Io"

# Determine optimal job count (use all cores for M3)
if [[ "$ARCH" == "arm64" ]]; then
    # M3 has excellent thermal management, use all cores
    JOBS=$CORES
else
    # For Intel Macs, leave one core free
    JOBS=$((CORES - 1))
fi

echo -e "Building with ${GREEN}$JOBS${NC} parallel jobs..."

# Build with timing information
BUILD_START=$(date +%s)

if [ "$USE_NINJA" == "1" ]; then
    ninja -j"$JOBS" || ninja -j1  # Fallback to single job on error
else
    make -j"$JOBS" || make -j1  # Fallback to single job on error
fi

BUILD_END=$(date +%s)
BUILD_TIME=$((BUILD_END - BUILD_START))

print_step "Build Complete"

# Check if build was successful
if [ -f "_build/binaries/io" ]; then
    echo -e "${GREEN}✓ Build successful!${NC}"
    echo -e "Build time: ${GREEN}${BUILD_TIME} seconds${NC}"
    
    # Get binary size
    SIZE=$(du -h "_build/binaries/io" | cut -f1)
    echo -e "Binary size: ${GREEN}$SIZE${NC}"
    
    # Test the binary
    echo ""
    echo -e "${BLUE}Testing Io binary...${NC}"
    if ./_build/binaries/io -e 'writeln("Hello from optimized Io!")' 2>/dev/null; then
        echo -e "${GREEN}✓ Io is working!${NC}"
    else
        echo -e "${YELLOW}⚠ Basic test failed${NC}"
    fi
    
    # Show binary location
    echo ""
    echo -e "${GREEN}Io binary location:${NC}"
    echo "  $(pwd)/_build/binaries/io"
    
    # Create convenience symlink
    if [ ! -L "$PROJECT_ROOT/io" ]; then
        ln -sf "$(pwd)/_build/binaries/io" "$PROJECT_ROOT/io"
        echo ""
        echo -e "${GREEN}Created symlink:${NC}"
        echo "  $PROJECT_ROOT/io -> $(pwd)/_build/binaries/io"
    fi
    
else
    echo -e "${RED}✗ Build failed${NC}"
    echo "Check the build output above for errors"
    exit 1
fi

print_step "Next Steps"

echo "You can now:"
echo "  1. Run Io directly: ${GREEN}./io${NC}"
echo "  2. Run tests: ${GREEN}./io libs/iovm/tests/correctness/run.io${NC}"
echo "  3. Install system-wide: ${GREEN}cd build && sudo make install${NC}"
echo ""
echo "Build options:"
echo "  ${BLUE}./build_optimized.sh${NC}          - Normal build"
echo "  ${BLUE}./build_optimized.sh clean${NC}     - Clean build from scratch"
echo "  ${BLUE}./build_optimized.sh reconfigure${NC} - Reconfigure and build"
echo ""