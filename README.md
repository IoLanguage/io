# The Io Language

_Note: This document is intended to be used as a reference for setting up and configuring Io. For a guide on how to use the language itself, please visit the website at <http://iolanguage.org/guide/guide.html>._

# Table of Contents

* [Table of Contents](#table-of-contents)
* [What is Io?](#what-is-io)
	* [Example Code](#example-code)
	* [Quick Links](#quick-links)
* [Installing](#installing)
	* [From a Package Manager](#from-a-package-manager)
	* [From Source](#from-source)
		* [Linux Build Instructions](#linux-build-instructions)
		* [macOS Build Instructions](#macOS-build-instructions)
		* [Windows Build Instructions](#windows-build-instructions)
			* [Building with MinGW-W64](#building-with-mingw-w64)
			* [Building with MinGW](#building-with-mingw-recommended)
			* [Building with MSVC](#building-with-msvc)
			* [Building with Cygwin](#building-with-cygwin)
* [Running Tests](#running-tests)
* [Installing Addons](#installing-addons)

What is Io?
=====

Io is a dynamic prototype-based programming language in the same realm as
Smalltalk and Self. It revolves around the idea of message passing from object
to object.

For further information, the programming guide and reference manual can be found
in the docs folder.


Example Code
---
Basic Math

```Io
Io> 1 + 1
==> 2

Io> 2 sqrt
==> 1.4142135623730951
```

Lists

```Io
Io> d := List clone append(30, 10, 5, 20)
==> list(30, 10, 5, 20)

Io> d := d sort
==> list(5, 10, 20, 30)

Io> d select (>10)
==> list(20, 30)
```

Objects

```Io
Io> Contact := Object clone
==>  Contact_0x7fbc3bc8a6d0:
  type = "Contact"

Io> Contact name ::= nil
==> nil

Io> Contact address ::= nil
==> nil

Io> Contact city ::= nil
==> nil

Io> holmes := Contact clone setName("Holmes") setAddress("221B Baker St") setCity("London")
==>  Contact_0x7fbc3be2b470:
  address          = "221B Baker St"
  city             = "London"
  name             = "Holmes"

Io> Contact fullAddress := method(list(name, address, city) join("\n"))
==> method(
    list(name, address, city) join("\n")
)

Io> holmes fullAddress
==> Holmes
221B Baker St
London
```




Quick Links
---
* The Wikipedia page for Io has a good overview and shows a few interesting
  examples of the language:
  <https://en.wikipedia.org/wiki/Io_(programming_language)>.
* The entry on the c2 wiki has good discussion about the merits of the language:
  <http://wiki.c2.com/?IoLanguage>.


Installing
==========

From a Package Manager
---

Io is currently only packaged for OS X. To install it, open a terminal and type:

```
brew install io
```

Note that this package may not be as updated as the version from the source
repository.

To install via Homebrew on an M1 Mac, first install Homebrew under x86_64, into /usr/local:

```
arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
```

Then install io with this installation of Homebrew:

```
arch -x86_64 /usr/local/Homebrew/bin/brew install io
```

Following that, you can run io under Rosetta 2 with:

```
arch -x86_64 io
```

From Source
---

First, make sure that this repo and all of its submodules have been cloned to
your computer by running `git clone` with the `--recursive` flag:

```
git clone --recursive https://github.com/IoLanguage/io.git
```

Io uses the [CMake build system](https://cmake.org/) and supports all of the
normal flags and features provided by CMake. 

In a production environment, pass the flag `-DCMAKE_BUILD_TYPE=release` to the
`cmake` command to ensure that the C compiler does the proper optimizations.
Without this flag, Io is built in debug mode without standard C optimizations.

To install to a specific folder, pass the flag
`-DCMAKE_INSTALL_PREFIX=/path/to/your/folder/` to the `cmake` command.

### Linux Build Instructions

To prepare the project for building, run the following commands:

```
cd io/           # To get into the cloned folder
mkdir build      # To contain the CMake data
cd build/
cmake ..         # This populates the build folder with a Makefile and all of the related things necessary to begin building
```

In a production environment, pass the flag `-DCMAKE_BUILD_TYPE=release` to the
`cmake` command to ensure that the C compiler does the proper optimizations.
Without this flag, Io is built in debug mode without standard C optimizations.

To install to a different folder than `/usr/local/bin/`, pass the flag
`-DCMAKE_INSTALL_PREFIX=/path/to/your/folder/` to the `cmake` command.

To build without Eerie, the Io package manager, pass the flag
`-DWITHOUT_EERIE=1` to the `cmake` command.

Once CMake has finished preparing the build environment, ensure you are inside
the build folder, and run:

```
make
sudo make install
```

Finally, install [Eerie](https://github.com/IoLanguage/eerie), the Io package
manager (see Eerie [repo](https://github.com/IoLanguage/eerie) for installation
options):

```
export PATH=$PATH:_build/binaries/; . ./install_unix.sh
```

Io can then be run with the `io` command and Eerie can be run with the `eerie`
command.


### macOS Build Instructions

See the [Linux build instructions](#linux-build-instructions).

Note: Building Io for arm64-based macOS machines is unsupported. To build and run
on an M1 or newer, build Io for x86_64 by adding
`-DCMAKE_OSX_ARCHITECTURES="x86_64"` to your CMake invocation.

### Windows Build Instructions

You need CMake or CMake Cygwin (at least v2.8), depending on the building method
you choose.

For the `make install` command, if you are on Windows 7/Vista you will need to
run your command prompts as Administrator: right-click on the command prompt
launcher->"Run as administrator" or something similar.

You will also need to add `<install_drive>:\<install_directory>\bin` and
`<install_drive>:\<install_directory>\lib` to your `PATH` environment variable.


#### Building with MinGW-W64 (Recommended)

We use this method in our CI, so this should be considered an official/supported
method of building on Windows.

1. `cd` to your Io root folder
2. We want to do an out-of-source build, so: `mkdir buildroot` and `cd buildroot`
3. a) `cmake -G"MinGW Makefiles" ..`

	or

	b) `cmake -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=<install_drive>:/<install_directory> ..` (eg: `cmake -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=C:/Io ..`)
4. `mingw32-make`
5. `mingw32-make install` (if you use cmd.exe, you should run it as
   Administrator)
6. Install [Eerie](https://github.com/IoLanguage/eerie), the Io package manager
   (see Eerie [repo](https://github.com/IoLanguage/eerie) for installation
   options): `_build\binaries\io_static setup.io`.


#### Building with MinGW

For automatic MinGW install:
<http://sourceforge.net/projects/mingw/files/Automated%20MinGW%20Installer>

For non-automatic MinGW install and detailed instructions refer to:
<http://www.mingw.org/wiki/InstallationHOWTOforMinGW>

1. `cd` to your Io root folder
2. We want to do an out-of-source build, so: `mkdir buildroot` and `cd buildroot`
3. a) `cmake -G"MSYS Makefiles" ..`

	or

	b) `cmake -G"MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=<install_drive>:/<install_directory> ..` (eg: `cmake -G"MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=C:/Io ..`)
4. `make`
5. `make install`
6. Install [Eerie](https://github.com/IoLanguage/eerie), the Io package manager
   (see Eerie [repo](https://github.com/IoLanguage/eerie) for installation
   options): `./_build/binaries/io_static setup.io`.
   
   
#### Building with MSVC

1. Install Microsoft Visual C++ 2008 Express (should work with other versions).
2. Install Microsoft Windows SDK 7.0 (or newer).
3. Install CMake (v2.8 at least)
4. Run "Visual Studio 2008 Command Prompt" from the "Microsoft Visual Studio
   2008" start menu.
5. `cd` to `<install_drive>:\Microsoft SDKs\Windows\v7.0\Setup` then run:
   `WindowsSdkVer.exe -version:v7.0`
6. Close the command prompt window and run step 4 again
7. Ensure CMake bin path is in the `PATH` environment variable (eg: `echo
   %PATH%` and see that the folder is there) if not you will have to add it to
   your `PATH`.
8. `cd` to your Io root folder
9. We want to do an out-of-source build, so: `mkdir buildroot` and `cd buildroot`
10. a) `cmake ..`

	or

	b) `cmake -DCMAKE_INSTALL_PREFIX=<install_drive>:\<install_directory> ..` (eg: `cmake -DCMAKE_INSTALL_PREFIX=C:\Io ..`)
11. `nmake`
12. `nmake install`
13. Install [Eerie](https://github.com/IoLanguage/eerie), the Io package manager
    (see Eerie [repo](https://github.com/IoLanguage/eerie) for installation
    options): `./_build/binaries/io_static setup.io`.


#### Building with Cygwin

Install Cygwin from: <http://www.cygwin.com/>

1. `cd` to your Io root folder
2. We want to do an out-of-source build, so: `mkdir buildroot` and `cd buildroot`
3. a) `cmake ..`

	or

    b) `cmake -DCMAKE_INSTALL_PREFIX=<install_drive>:/<install_directory> ..`
    (eg: `cmake -DCMAKE_INSTALL_PREFIX=C:/Io ..`)
4. `make`
5. `make install`
6. Install [Eerie](https://github.com/IoLanguage/eerie), the Io package manager
    (see Eerie [repo](https://github.com/IoLanguage/eerie) for installation
    options): `./_build/binaries/io_static setup.io`.

Note: If you also have CMake 2.8 for Windows installed (apart from CMake for
Cygwin) check your `PATH` environment variable so you won't be running CMake for
Windows instead of Cygwin version.


Running Tests
===

You should be inside your out-of-source build dir. The vm tests can be run with
the command:

	io ../libs/iovm/tests/correctness/run.io

Installing Addons
===

Many of the common features provided by the Io language aren't prepackaged in
the Io core. Instead, these features are contained in addons that get loaded
when launching the Io VM. In the past, these addons were automatically installed
by the build process, but now they must be installed through
[Eerie](https://github.com/IoLanguage/eerie), the Io package manager.

Most of these addons are housed under the IoLanguage group on GitHub:
https://github.com/IoLanguage.

To install an addon, ensure both Io and Eerie are installed correctly, then run:

```
eerie install <link to the git repository>
```

For example, to build and install the `Range` addon, run the command:

```
eerie install https://github.com/IoLanguage/Range.git
```

To ensure that an addon installed correctly, pull up an Io interpreter and type
the name of the object provided by the addon. It should load dynamically and
automatically into the interpreter session, populating a slot in `Lobby Protos
Addons`.
