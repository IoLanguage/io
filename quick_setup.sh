#!/bin/bash
# Quick setup script for optimal Io development on Mac

echo "🚀 Io Language Quick Setup for Mac"
echo ""

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "📦 Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Install optimal build tools
echo "📦 Installing build tools..."
brew install cmake ninja ccache

# Set up ccache
if command -v ccache &> /dev/null; then
    # Configure ccache for all C compilers
    export CC="ccache clang"
    export CXX="ccache clang++"
    
    # Set ccache directory
    export CCACHE_DIR="$HOME/.ccache"
    
    # Show ccache stats
    ccache -s
fi

echo ""
echo "✅ Setup complete!"
echo ""
echo "Quick start commands:"
echo "  make -f Makefile.optimized        # Show all available commands"
echo "  make -f Makefile.optimized build  # Build Io"
echo "  make -f Makefile.optimized test   # Run tests"
echo "  make -f Makefile.optimized quick  # Fast incremental build"
echo ""
echo "Or use the optimized build script directly:"
echo "  ./build_optimized.sh              # Build with all optimizations"
echo ""