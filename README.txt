
ABOUT
-----

Io is a dynamic prototype-based programming language. 
The programming guide and reference manual can be found in the docs folder.

INSTALLING
----------

Io is two parts - the vm and the addons/packages. Don't worry if all the addons don't compile for you - some are platform specific or not well supported.

There are a couple ways you can go about building Io, I will give the recommended way, and a note about how to do it alternatively.

OSX
---

Note: Assuming you wish to install to an alternate location, ensure you supply as an argument to the following command, a -DCMAKE_INSTALL_PREFIX=/path where /path is where you wish to install Io to. This is akin to setting INSTALL_PREFIX with the old build system if you are familiar with it, or --prefix with GNU autotools if you are familiar with that suite.

Ensure you are at the top level of the source tree, that is where this file lives. From here, you are in the right spot to enter these commands:

mkdir build && cd build
cmake ..
make install

If you do not wish to install, just run "make" instead of "make install". Currently there is no analogue to the old "make linkInstall". However, if you have used linkInstall in previous versions of Io, you should never have to run linkInstall again, since it created symbolic links to where your Io source was at that time. The only time you would have to do this again, is if you moved the Io source from one dir to another. Most people don't.

Debian or Ubuntu
----------------
See OSX instructions.

Gentoo
------
See OSX instructions.

MS WINDOWS WITH CYGWIN
----------------------

1) [once] Ensure your VisualStudio has the Windows SDK. 

2) Start up a Visual Studio Command Prompt using the start menu link (inside the Visual Studio Tools subfolder), or from a command prompt execute "%PROGRAMFILES%\Microsoft Visual Studio 9.0\VC\vcvarsall.bat". This sets up environment variables needed by the compiler.

3) 'cd' to your Git source root folder for Io.

4) Run "bash" so that Cygwin tools, "make" in particular, are available.

5) [once] Run "which link" and ensure it finds VisualStudio's link, not Cygwin's (/bin/link). If this finds Cygwin's, you need to edit your ~/.bashrc file so that the $PATH will contain /bin after the VS bin folder, then restart from step 2.

6) make SYS=Windows vm
Note that "make CC=cl vm" doesn't work. If SYS is unset, it gets set in the makefile as `uname -s`, which is something like CYGWIN_NT-5.1, and all the command line options will be those of gcc, which are inappropriate for compiling with the Microsoft compiler, cl. INSTALL_PREFIX is no longer needed here on Windows; you can move the installation around, and Io will still work.

7) make SYS=Windows addons
Builds addons. Some won't build because needed libraries (header files) aren't present. Some others fail to build with errors. Still, some build and are usable.

8) make SYS=Windows INSTALL_PREFIX=C:/Io install
Installs Io where specified. Note, use forward slashes here, not backslashes.


