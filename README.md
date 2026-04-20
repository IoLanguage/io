# The Io Language

_This is a reference for building and running Io. For a guide to the language itself, see <http://iolanguage.org/guide/guide.html>._

## Contents

* [What is Io?](#what-is-io)
	* [Example Code](#example-code)
	* [Quick Links](#quick-links)
* [Platform](#platform)
* [Building](#building)
* [Running](#running)
* [Browser REPL](#browser-repl)
* [Running Tests](#running-tests)
* [Extending Io](#extending-io)

What is Io?
===========

Io is a dynamic prototype-based programming language in the same family as
Smalltalk and Self. Everything is a message send from one object to another.

The programming guide and reference manual live in the `docs` folder.


Example Code
------------

Basic math:

```Io
Io> 1 + 1
==> 2

Io> 2 sqrt
==> 1.4142135623730951
```

Lists:

```Io
Io> d := List clone append(30, 10, 5, 20)
==> list(30, 10, 5, 20)

Io> d := d sort
==> list(5, 10, 20, 30)

Io> d select (>10)
==> list(20, 30)
```

Objects:

```Io
Io> Contact := Object clone
==>  Contact_0x7fbc3bc8a6d0:
  type = "Contact"

Io> Contact name ::= nil
Io> Contact address ::= nil
Io> Contact city ::= nil

Io> holmes := Contact clone setName("Holmes") setAddress("221B Baker St") setCity("London")
==>  Contact_0x7fbc3be2b470:
  address          = "221B Baker St"
  city             = "London"
  name             = "Holmes"

Io> Contact fullAddress := method(list(name, address, city) join("\n"))
Io> holmes fullAddress
==> Holmes
221B Baker St
London
```


Quick Links
-----------

* Wikipedia overview: <https://en.wikipedia.org/wiki/Io_(programming_language)>
* c2 wiki discussion: <http://wiki.c2.com/?IoLanguage>


Platform
========

Io targets WebAssembly. A single WASI binary runs under any WASI host
(wasmtime, Node, browsers via WASI shims) and ships with a browser REPL and
a bidirectional Io↔JavaScript bridge.

If you need the previous native build (CMake, dynamic addons, Eerie package
manager), use the `native` branch or the `2026.04.20-native-final` tag. The
`native` path accepts bug fixes; new development lands on `master`.


Building
========

Prerequisites: [wasi-sdk](https://github.com/WebAssembly/wasi-sdk) and
[wasmtime](https://wasmtime.dev/).

```
git clone --recursive https://github.com/IoLanguage/io.git
cd io
make
```

If wasi-sdk is not installed at `~/wasi-sdk`, point `WASI_SDK` at it:

```
make WASI_SDK=/opt/wasi-sdk
```

Make targets:

| Target              | What it does                                      |
|---------------------|---------------------------------------------------|
| `make`              | Build `build/bin/io_static` (WASI CLI binary)     |
| `make test`         | Build `build/bin/test_iterative_eval`             |
| `make check`        | Run both test suites under wasmtime               |
| `make browser`      | Build `browser/io_browser.wasm` (browser REPL)    |
| `make serve`        | Serve the browser REPL at <http://localhost:8000> |
| `make check-browser`| Run browser tests under Playwright                |
| `make regenerate`   | Regenerate `IoVMInit.c` from `.io` sources        |
| `make clean`        | Remove build artifacts                            |


Running
=======

Run a one-liner:

```
wasmtime build/bin/io_static -e '"hello" println'
```

Run a script (pass `--dir=.` so the WASI sandbox can read the file):

```
wasmtime --dir=. build/bin/io_static path/to/script.io
```

Start the REPL:

```
wasmtime build/bin/io_static
```


Browser REPL
============

```
make serve
```

Then open <http://localhost:8000>. See `browser/README.md` for details on
embedding the REPL in a page and calling between Io and JavaScript.


Running Tests
=============

```
make check
```

Or run the suites individually:

```
wasmtime build/bin/test_iterative_eval
wasmtime --dir=. --dir=/tmp build/bin/io_static libs/iovm/tests/correctness/run.io
```

Set `IO_TEST_VERBOSE=1` for per-test output.


Extending Io
============

The native addon system (`DynLib`, `AddonLoader`, Eerie) is not available on
WebAssembly. Host integration goes through the JavaScript bridge instead: Io
code can call JS functions, and JS can call Io methods. See
`browser/README.md` and `agents/wasm/Bridge.md`.
