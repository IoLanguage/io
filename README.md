
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

If you are using the homebrew package manager you can install Io (though it may not be the latest version) with:

	brew install io

Note: Assuming you wish to install to an alternate location, ensure you supply as an argument to the following command, a -DCMAKE_INSTALL_PREFIX=/path where /path is where you wish to install Io to. This is akin to setting INSTALL_PREFIX with the old build system if you are familiar with it, or --prefix with GNU autotools if you are familiar with that suite.

Ensure you are at the top level of the source tree, that is where this file lives. From here, you are in the right spot to enter these commands:

mkdir build && cd build
cmake ..
make install

If you do not wish to install, just run "make" instead of "make install". Currently there is no analogue to the old "make linkInstall". However, if you have used linkInstall in previous versions of Io, you should never have to run linkInstall again, since it created symbolic links to where your Io source was at that time. The only time you would have to do this again, is if you moved the Io source from one dir to another. Most people don't.

Any Linux Distribution
----------------------

Any Linux distribution will require one additional step be taken. This is because GNU ld is what's technically known as a "dumb" linker -- it has to be told to regenerate its hash of libraries if something changes, other platforms do not have this problem. To complete this step, run the following command:

ldconfig

If you are still getting an error when loading the "io" binary about not being able to find some shared library, then ensure you have the following path, in your /etc/ld.so.conf (or equivalent):

/usr/local/lib

The above path will change depending on what you set your CMAKE_INSTALL_PREFIX to, it will be whatever that is with /lib appended. The default is /usr/local so the above will work for the common case.

Debian or Ubuntu
----------------
See OSX instructions.

Gentoo
------
See OSX instructions.

MS WINDOWS
----------

For all the different methods explained here, some of the addons won't compile as they depend on libraries not provided by Io.

For methods A and B you must download and install CMake (at least v2.8) from here: http://www.cmake.org/cmake/resources/software.html

For method C you must install the CMake Cygwin package (at least v2.8) using the Cygwin package installer.

For the "make install" command, if you are on Windows 7/Vista you will need to run your command prompts as Administrator: right-click on the command prompt launcher->"Run as administrator" or something similar)

You will also need to add <install_drive>:\<install_directory>\bin and <install_drive>:\<install_directory>\lib to your PATH environment variable.

A) Building with MSVC
---------------------
1) Install Microsoft Visual C++ 2008 Express (should work with other versions).
2) Install Microsoft Windows SDK 7.0 (or newer).
3) Install CMake (v2.8 at least)
4) Run "Vistual Studio 2008 Command Prompt" from the "Microsoft Visual Studio 2008" start menu.
5) 'cd' to <install_drive>:\Microsoft SDKs\Windows\v7.0\Setup then run: "WindowsSdkVer.exe -version:v7.0" (without the quotes)
6) Close the command prompt window and run 4) again
7) Ensure CMake bin path is in the PATH environment variable (eg: echo %PATH% and see that the folder is there) if not you will have to add it to your PATH.
8) 'cd' to your Io root folder
9) We want to do an out-of-source build, so: "mkdir buildroot" and "cd buildroot" (without the quotes)
10a) cmake ..
or
10b) cmake -DCMAKE_INSTALL_PREFIX=<install_drive>:\<install_directory> .. (eg: cmake -DCMAKE_INSTALL_PREFIX=C:\Io ..)
11) nmake
12) nmake install


B) Building with MinGW
----------------------
For automatic MinGW install: http://sourceforge.net/projects/mingw/files/Automated%20MinGW%20Installer
For non-automatic MinGW install and detailed instructions refer to: http://www.mingw.org/wiki/InstallationHOWTOforMinGW

1) 'cd' to your Io root folder
2) We want to do an out-of-source build, so: "mkdir buildroot" and "cd buildroot" (without the quotes)
3a) cmake -G"MSYS Makefiles" ..
or
3b) cmake -G"MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=<install_drive>:/<install_directory> .. (eg: cmake -G"MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=C:/Io ..)
4) make
5) make install


C) Building with Cygwin
-----------------------
Install Cygwin from: http://www.cygwin.com/

1) 'cd' to your Io root folder
2) We want to do an out-of-source build, so: "mkdir buildroot" and "cd buildroot" (without the quotes)
3a) cmake ..
or
3b) cmake -DCMAKE_INSTALL_PREFIX=<install_drive>:/<install_directory> .. (eg: cmake -DCMAKE_INSTALL_PREFIX=C:/Io ..)
4) make
5) make install

note: If you also have CMake 2.8 for Windows installed (apart from CMake for Cygwin) check your PATH environment variable so you won't be running CMake for Windows instead of Cygwin version.


RUNNING TESTS
-------------

You should be inside your out-of-source build dir. The vm tests can be run with the command:

	io ../libs/iovm/tests/correctness/run.io


