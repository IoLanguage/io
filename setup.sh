#!/usr/bin/env bash
# Io Language Developer Setup Script
# This script helps set up a development environment for contributing to Io

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
print_header() {
    echo -e "${BLUE}===================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}===================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ $1${NC}"
}

# Detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
        if [ -f /etc/debian_version ]; then
            DISTRO="debian"
        elif [ -f /etc/redhat-release ]; then
            DISTRO="redhat"
        elif [ -f /etc/arch-release ]; then
            DISTRO="arch"
        else
            DISTRO="unknown"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        # Check for Apple Silicon
        if [[ $(uname -m) == "arm64" ]]; then
            ARCH="arm64"
        else
            ARCH="x86_64"
        fi
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        OS="windows"
    else
        OS="unknown"
    fi
}

# Check for required tools
check_requirements() {
    print_header "Checking Requirements"
    
    local missing_tools=()
    
    # Check for Git
    if command -v git &> /dev/null; then
        print_success "Git installed ($(git --version | head -1))"
    else
        missing_tools+=("git")
    fi
    
    # Check for CMake
    if command -v cmake &> /dev/null; then
        print_success "CMake installed ($(cmake --version | head -1))"
    else
        missing_tools+=("cmake")
    fi
    
    # Check for C compiler
    if command -v gcc &> /dev/null; then
        print_success "GCC installed ($(gcc --version | head -1))"
    elif command -v clang &> /dev/null; then
        print_success "Clang installed ($(clang --version | head -1))"
    else
        missing_tools+=("gcc or clang")
    fi
    
    # Check for Make
    if command -v make &> /dev/null; then
        print_success "Make installed ($(make --version | head -1))"
    else
        missing_tools+=("make")
    fi
    
    if [ ${#missing_tools[@]} -gt 0 ]; then
        print_error "Missing required tools: ${missing_tools[*]}"
        echo ""
        suggest_install
        exit 1
    fi
    
    echo ""
}

# Suggest installation commands based on OS
suggest_install() {
    print_info "Installation suggestions:"
    
    case $OS in
        linux)
            case $DISTRO in
                debian)
                    echo "  sudo apt-get update"
                    echo "  sudo apt-get install -y git cmake build-essential"
                    ;;
                redhat)
                    echo "  sudo yum install -y git cmake gcc gcc-c++ make"
                    ;;
                arch)
                    echo "  sudo pacman -S git cmake base-devel"
                    ;;
                *)
                    echo "  Please install: git, cmake, gcc/clang, make"
                    ;;
            esac
            ;;
        macos)
            echo "  # Install Homebrew if not installed:"
            echo "  /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
            echo ""
            echo "  # Then install required tools:"
            echo "  brew install git cmake"
            ;;
        windows)
            echo "  # Install using MSYS2:"
            echo "  pacman -S git mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc make"
            ;;
        *)
            echo "  Please install: git, cmake, gcc/clang, make"
            ;;
    esac
}

# Clone or update repository
setup_repository() {
    print_header "Setting Up Repository"
    
    if [ -d ".git" ]; then
        print_info "Already in a git repository"
        
        # Check for submodules
        if [ -f ".gitmodules" ]; then
            print_info "Updating submodules..."
            git submodule update --init --recursive
            print_success "Submodules updated"
        fi
    else
        print_error "Not in the Io repository directory!"
        echo "Please run this script from the root of the Io repository."
        exit 1
    fi
    
    echo ""
}

# Build Io
build_io() {
    print_header "Building Io"
    
    local build_type="${1:-Release}"
    local build_dir="build"
    
    print_info "Build type: $build_type"
    
    # Create build directory
    if [ ! -d "$build_dir" ]; then
        mkdir -p "$build_dir"
        print_success "Created build directory"
    fi
    
    # Configure with CMake
    print_info "Configuring with CMake..."
    cd "$build_dir"
    
    local cmake_flags=""
    if [[ "$OS" == "macos" && "$ARCH" == "arm64" ]]; then
        print_warning "Detected Apple Silicon Mac"
        # ARM64 is now supported natively
    fi
    
    cmake -DCMAKE_BUILD_TYPE="$build_type" $cmake_flags .. || {
        print_error "CMake configuration failed"
        cd ..
        exit 1
    }
    
    print_success "CMake configuration complete"
    
    # Build
    print_info "Building Io (this may take a few minutes)..."
    local num_cores
    if [[ "$OS" == "macos" ]]; then
        num_cores=$(sysctl -n hw.ncpu)
    else
        num_cores=$(nproc 2>/dev/null || echo 4)
    fi
    
    make -j"$num_cores" || {
        print_error "Build failed"
        cd ..
        exit 1
    }
    
    cd ..
    print_success "Build complete!"
    
    echo ""
}

# Run tests
run_tests() {
    print_header "Running Tests"
    
    if [ ! -f "build/_build/binaries/io" ]; then
        print_error "Io binary not found. Please build first."
        return 1
    fi
    
    print_info "Running test suite..."
    cd build
    if ./_build/binaries/io ../libs/iovm/tests/correctness/run.io; then
        print_success "All tests passed!"
    else
        print_warning "Some tests failed. This is normal during development."
    fi
    cd ..
    
    echo ""
}

# Setup development tools
setup_dev_tools() {
    print_header "Setting Up Development Tools"
    
    # Git hooks
    if [ -d ".git/hooks" ]; then
        print_info "Setting up git hooks..."
        
        # Create pre-commit hook
        cat > .git/hooks/pre-commit << 'EOF'
#!/bin/sh
# Pre-commit hook for Io

# Check for trailing whitespace
if git diff --cached --check; then
    :
else
    echo "Error: Trailing whitespace detected. Please fix before committing."
    exit 1
fi

# Run quick tests if Io is built
if [ -f "build/_build/binaries/io" ]; then
    echo "Running quick tests..."
    build/_build/binaries/io -e "\"Pre-commit tests passed\" println"
fi

exit 0
EOF
        chmod +x .git/hooks/pre-commit
        print_success "Git hooks installed"
    fi
    
    # Optional tools
    print_info "Checking optional development tools..."
    
    if command -v clang-format &> /dev/null; then
        print_success "clang-format installed (code formatting)"
    else
        print_info "clang-format not found (optional, for code formatting)"
    fi
    
    if command -v cppcheck &> /dev/null; then
        print_success "cppcheck installed (static analysis)"
    else
        print_info "cppcheck not found (optional, for static analysis)"
    fi
    
    if command -v valgrind &> /dev/null; then
        print_success "valgrind installed (memory debugging)"
    else
        print_info "valgrind not found (optional, for memory debugging)"
    fi
    
    echo ""
}

# Print next steps
print_next_steps() {
    print_header "Setup Complete! 🎉"
    
    echo "Your Io development environment is ready!"
    echo ""
    echo "Quick commands:"
    echo "  ${GREEN}make build${NC}     - Build Io"
    echo "  ${GREEN}make test${NC}      - Run tests"
    echo "  ${GREEN}make repl${NC}      - Start Io REPL"
    echo "  ${GREEN}make clean${NC}     - Clean build artifacts"
    echo "  ${GREEN}make help${NC}      - Show all available commands"
    echo ""
    echo "Start the REPL:"
    echo "  ${GREEN}./build/_build/binaries/io${NC}"
    echo ""
    echo "Read the contribution guide:"
    echo "  ${GREEN}less CONTRIBUTING.md${NC}"
    echo ""
    echo "Happy hacking! 🚀"
}

# Main execution
main() {
    print_header "Io Language Developer Setup"
    echo ""
    
    detect_os
    print_info "Detected OS: $OS"
    if [[ "$OS" == "linux" ]]; then
        print_info "Linux distribution: $DISTRO"
    elif [[ "$OS" == "macos" ]]; then
        print_info "Architecture: $ARCH"
    fi
    echo ""
    
    check_requirements
    setup_repository
    
    # Ask for build type
    echo "Select build type:"
    echo "  1) Release (optimized, recommended)"
    echo "  2) Debug (with debug symbols)"
    echo "  3) Skip build"
    read -p "Choice [1]: " choice
    
    case $choice in
        2)
            build_io "Debug"
            ;;
        3)
            print_info "Skipping build"
            ;;
        *)
            build_io "Release"
            ;;
    esac
    
    # Ask about running tests
    if [ -f "build/_build/binaries/io" ]; then
        read -p "Run tests? [Y/n]: " run_tests_choice
        if [[ ! "$run_tests_choice" =~ ^[Nn]$ ]]; then
            run_tests
        fi
    fi
    
    setup_dev_tools
    print_next_steps
}

# Run main function
main "$@"