# Io Developer Quick Start

Welcome to Io development! This guide gets you up and running in minutes.

## 🚀 One-Line Setup

```bash
./setup.sh
```

This script will check requirements, build Io, run tests, and set up your development environment.

## 📦 Manual Setup

If you prefer manual setup:

```bash
# 1. Clone with submodules
git clone --recursive https://github.com/IoLanguage/io.git
cd io

# 2. Build
make build    # or: make debug for debug build

# 3. Test
make test

# 4. Start coding!
make repl
```

## 🛠️ Development Workflow

### Common Tasks

```bash
make help      # Show all available commands
make dev       # Quick incremental build
make test      # Run test suite
make format    # Format C code
make check     # Static analysis
```

### Quick Commands

- `make b` - Build
- `make t` - Test  
- `make r` - REPL
- `make c` - Clean

### Testing Your Changes

```bash
# Run all tests
make test

# Run specific test
./build/_build/binaries/io libs/iovm/tests/correctness/ListTest.io

# Test samples
make test-all
```

### Before Committing

1. Run tests: `make test`
2. Check formatting: `make format`
3. Run static analysis: `make check`

The pre-commit hook will automatically run quick checks.

## 🏗️ Project Structure

```
io/
├── libs/
│   ├── basekit/        # Foundation utilities
│   ├── coroutine/      # Coroutine implementation
│   ├── garbagecollector/ # Memory management
│   └── iovm/           # Virtual machine core
│       ├── source/     # C implementation
│       └── io/         # Io standard library
├── samples/            # Example programs
├── docs/              # Documentation
└── build/             # Build output (generated)
```

## 🐛 Debugging

### Debug Build

```bash
make debug
```

### With GDB

```bash
gdb ./build/_build/binaries/io
(gdb) run script.io
```

### With Valgrind

```bash
valgrind --leak-check=full ./build/_build/binaries/io script.io
```

### Print Debugging in Io

```io
obj println          # Print object
obj slotNames        # Show slots
obj proto            # Show prototype
thisContext          # Current context
System backtrace     # Stack trace
```

## 🔍 Finding Things

### Search for C functions
```bash
grep -r "IoObject_" libs/iovm/source/
```

### Search for Io methods
```bash
grep -r "method(" libs/iovm/io/
```

### Find test examples
```bash
ls libs/iovm/tests/correctness/*Test.io
```

## 📝 Code Style

### C Code
- Use tabs for indentation
- Pattern: `IoObjectName_methodName`
- Keep functions focused

### Io Code
- Use tabs for indentation
- camelCase for methods
- PascalCase for objects

## 🚢 Submitting Changes

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests
5. Submit a pull request

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

## 📦 Cutting a Release

Io uses date-based version tags (`2026.04.20-native-final`). Pushing one
triggers `.github/workflows/release.yml`, which builds the WASM artifacts, runs
the test suite, and attaches the results to a GitHub Release:

```bash
git tag 2026.08.15-example
git push origin 2026.08.15-example
```

Attached artifacts:

| file | what it is |
|------|------------|
| `io_static.wasm` | The VM as a WASI 0.1 module (wasmtime, wasmer, Node) |
| `io_component.wasm` | The VM as a WASI 0.2 component (`wasm32-wasip2`) |
| `io_browser.wasm` | Reactor module for in-browser embedding |
| `io-browser-<tag>.tar.gz` | Browser module plus the `io.js` loader |
| `SHA256SUMS.txt` | Checksums |

Tags containing `alpha`, `beta`, `rc`, or `pre` are published as prereleases.
The release body is generated from the commit log; the curated summary lives on
the [Release Notes](https://iolanguage.org/Release%20Notes/) page and is written
by hand.

The workflow can also be run by hand from the Actions tab against any ref, with
a draft option — useful for a dry run before tagging for real.

It builds with `make`, so it only works on trees that have the WASM Makefile.
Every tag up to and including `2026.04.20-native-final` comes from the earlier
CMake-based native build and cannot be built this way, so artifacts cannot be
backfilled onto them.

## 💡 Tips

- Start with small changes to understand the codebase
- Read existing tests for API examples
- The `samples/` directory has many examples
- Join discussions on GitHub Issues
- Ask questions! The community is helpful

## 📚 Resources

- [Official Guide](https://iolanguage.org/docs/Guide)
- [CONTRIBUTING.md](CONTRIBUTING.md) - Contribution guidelines
- [README.md](README.md) - General information
- `docs/` - API documentation

## 🆘 Getting Help

- GitHub Issues for bugs/features
- GitHub Discussions for questions
- IRC: #io on irc.freenode.net
- Mailing list: iolanguage@googlegroups.com

Happy hacking! 🎉
