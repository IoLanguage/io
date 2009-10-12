
ABOUT
-----

Io is a dynamic prototype-based programming language. 
The programming guide and reference manual can be found in the docs folder.

INSTALLING
----------

Io is two parts - the vm and the addons/packages. Don't worry if all the addons don't compile for you - some are platform specific or not well supported.

OSX
---
cd to Io folder
make vm; sudo make install; sudo make port; sudo make install

Debian or Ubuntu
----------------
cd to Io folder
make vm; sudo make install; su -c "sudo make aptget"; make; sudo make install

Gentoo
------
make vm; sudo make install; su -c "sudo make emerge"; make; sudo make install



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


