# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Common Development Commands

### Building Io

```bash
# Create build directory (one-time setup)
mkdir build && cd build

# Configure for development (debug mode)
cmake ..

# Configure for production (with optimizations)
cmake -DCMAKE_BUILD_TYPE=release ..

# Build the project
make

# Install (requires sudo on Unix systems)
sudo make install
```

### Running Tests

```bash
# Run the main test suite (from build directory)
io ../libs/iovm/tests/correctness/run.io

# Run a specific test file
io ../libs/iovm/tests/correctness/<TestName>Test.io
```

### Development Workflow

```bash
# Clean rebuild
cd build && make clean && make

# Run the Io REPL
io

# Execute an Io script
io script.io
```

## Architecture Overview

### Core Structure

The Io language is a dynamic, prototype-based programming language implemented in C. The architecture consists of:

1. **Virtual Machine Core** (`libs/iovm/`): The interpreter and runtime system
   - Objects clone from prototypes (no classes)
   - Everything is an object that responds to messages
   - Message passing is the fundamental operation
   - Coroutines provide lightweight concurrency

2. **Foundation Libraries** (`libs/`):
   - `basekit`: Cross-platform C utilities and data structures
   - `coroutine`: Architecture-specific context switching (x86, x86_64, ARM64, PowerPC)
   - `garbagecollector`: Mark-and-sweep garbage collection

3. **Standard Library**: Split between C (`libs/iovm/source/`) and Io (`libs/iovm/io/`) implementations
   - Core objects like IoObject, IoMessage, IoState in C
   - Higher-level functionality in Io for flexibility

### Key Design Patterns

- **Prototype-based OOP**: Objects clone from prototypes rather than instantiating classes
- **Message Passing**: All operations are messages sent to objects
- **C Naming Convention**: `IoObjectName_methodName` for C functions
- **Minimal Core**: Keep VM small, implement features in Io when possible

### Testing Approach

Tests are written in Io using the built-in UnitTest framework. The test runner (`libs/iovm/tests/correctness/run.io`) discovers and executes all test files ending with `Test.io`.

### Build System

CMake-based build system with hierarchical CMakeLists.txt files. Each library manages its own build configuration and dependencies.