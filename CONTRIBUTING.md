# Contributing to Io

Welcome! We're thrilled that you're interested in contributing to the Io programming language. This guide will help you get started quickly and make your contribution process smooth.

## 🚀 Quick Start

```bash
# Clone the repository with submodules
git clone --recursive https://github.com/IoLanguage/io.git
cd io

# Build Io
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=release ..
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

# Run tests to verify your setup
io ../libs/iovm/tests/correctness/run.io

# Start hacking!
io
```

## 📋 Ways to Contribute

### For First-Time Contributors
- **Documentation**: Help improve guides, fix typos, add examples
- **Tests**: Add test coverage for existing functionality
- **Bug Reports**: File detailed bug reports with reproduction steps
- **Platform Testing**: Test Io on your platform and report issues

### For Experienced Contributors
- **Bug Fixes**: Pick an issue labeled `bug` and dive in
- **Features**: Implement features from issues labeled `enhancement`
- **Performance**: Profile and optimize bottlenecks
- **Platform Support**: Improve Windows, ARM64, or other platform support

## 🔧 Development Setup

### Prerequisites
- C compiler (gcc, clang, or MSVC)
- CMake 3.5+
- Git
- Make (or Ninja)

### Platform-Specific Notes

#### macOS (Intel)
```bash
brew install cmake
# Follow standard build instructions
```

#### macOS (Apple Silicon/M1)
```bash
brew install cmake
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=release ..
make -j$(sysctl -n hw.ncpu)
```

#### Linux
```bash
sudo apt-get install cmake build-essential  # Debian/Ubuntu
sudo yum install cmake gcc gcc-c++ make      # RHEL/CentOS
# Follow standard build instructions
```

#### Windows
See detailed instructions in README.md for MinGW-W64, MSVC, or Cygwin builds.

## 🧪 Testing

### Running Tests
```bash
# Run all tests (from build directory)
io ../libs/iovm/tests/correctness/run.io

# Run specific test
io ../libs/iovm/tests/correctness/ListTest.io

# Run with verbose output
IO_TEST_VERBOSE=1 io ../libs/iovm/tests/correctness/run.io
```

### Writing Tests
Create test files in `libs/iovm/tests/correctness/` ending with `Test.io`:

```io
// ExampleTest.io
TestCase clone do(
    testBasicAssertion := method(
        (1 + 1) assertEquals(2)
    )
    
    testStringComparison := method(
        "hello" assertEquals("hello")
    )
)
```

## 📝 Code Style

### C Code
- Use tabs for indentation
- Follow the pattern: `IoObjectName_methodName`
- Place opening braces on the same line
- Keep functions concise and focused

```c
IoObject *IoObject_performWithArgList(IoObject *self, IoSymbol *methodName, IoList *args) {
    IoMessage *m = IoMessage_newWithName_(IOSTATE, methodName);
    // Implementation
    return IoObject_activate(self, target, m, locals);
}
```

### Io Code
- Use tabs for indentation
- Use camelCase for method names
- Use PascalCase for object names
- Document public APIs

```io
MyObject := Object clone do(
    // Public method
    doSomething := method(arg,
        // Implementation
        self
    )
    
    // Private method (underscore prefix)
    _helperMethod := method(
        // Implementation
    )
)
```

## 🔄 Pull Request Process

1. **Fork & Branch**
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b fix/issue-description
   ```

2. **Make Changes**
   - Write clean, focused commits
   - Add tests for new functionality
   - Update documentation if needed

3. **Test Thoroughly**
   ```bash
   # Build and test
   make clean && make
   io ../libs/iovm/tests/correctness/run.io
   ```

4. **Commit Messages**
   ```
   type: Brief description (max 50 chars)
   
   Longer explanation if needed. Explain what and why,
   not how. Reference issues like #123.
   ```
   
   Types: `feat`, `fix`, `docs`, `test`, `perf`, `refactor`, `style`, `chore`

5. **Submit PR**
   - Fill out the PR template
   - Link related issues
   - Ensure CI passes
   - Be responsive to feedback

## 🐛 Reporting Issues

### Before Reporting
- Search existing issues
- Try the latest version
- Attempt to reproduce consistently

### Issue Template
```markdown
**Description**
Clear description of the issue

**Steps to Reproduce**
1. Step one
2. Step two
3. ...

**Expected Behavior**
What should happen

**Actual Behavior**
What actually happens

**Environment**
- Io version: (output of `io --version`)
- OS: (e.g., macOS 12.0, Ubuntu 20.04)
- Architecture: (e.g., x86_64, ARM64)

**Additional Context**
Any other relevant information
```

## 🏗️ Architecture Overview

### Core Components
- **libs/basekit**: Foundation utilities and cross-platform abstractions
- **libs/coroutine**: Coroutine implementation for cooperative multitasking
- **libs/garbagecollector**: Memory management
- **libs/iovm**: The virtual machine and core language implementation

### Key Concepts
- **Prototype-based**: Objects clone from prototypes (no classes)
- **Message passing**: All operations are messages
- **Actors**: Lightweight concurrency via coroutines
- **Lazy evaluation**: Many operations are lazy by default

## 🤝 Community

### Communication Channels
- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: General questions and ideas
- **Mailing List**: [iolanguage@googlegroups.com](mailto:iolanguage@googlegroups.com)

### Code of Conduct
- Be respectful and inclusive
- Welcome newcomers and help them get started
- Focus on constructive criticism
- Assume good intentions

## 📚 Resources

### Documentation
- [Official Guide](http://iolanguage.org/guide/guide.html)
- [API Reference](docs/reference/index.html)
- Source code comments and examples

### Learning Io
- Start with `samples/misc/` for basic examples
- Read the test files for usage patterns
- Explore the standard library in `libs/iovm/io/`

## 🙏 Recognition

Contributors are recognized in:
- The AUTHORS file
- Release notes
- The project README

Thank you for helping make Io better! Every contribution, no matter how small, is valued and appreciated.

## 📄 License

By contributing, you agree that your contributions will be licensed under the same license as the project (BSD 3-Clause).