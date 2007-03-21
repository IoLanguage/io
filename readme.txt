
About Io
========

Io is a small, prototype-based programming language. 

Details and docs can be found at:

	http://www.iolanguage.com/ 


Building and Installing on Unix/OSX
===================================

make
sudo make install

After it creates io and io_static in Io/_build/binaries/, 
the build process will attempt to build the addons in
Io/addons/.  The process detects missing required libraries 
and writes a log to Io/errors.

Refer to readme_addons.txt for platform and 
distribution-specific information.


uninstalling on Unix/OSX
========================

sudo make uninstall


Building on Windows
===================

This is a bit of a dark art at the moment, but 
building under Cygwin and Mingw should be possible 
with a bit of work. There is also a Visual C project 
in the projects folder but it usually requires some 
updating. Note: the OpenGL binding requires GLUT 3.


Getting Help
============

If you need help or have bug reports or suggestions, 
please join in the community discussions. Your input
is welcome and there are usually friendly folks willing
to answer questions.

mailing list: 

	http://groups.yahoo.com/group/iolanguage/

irc: 
	
	irc.freenode.net#io


Cheers,
Steve Dekorte
steve at dekorte dot com
