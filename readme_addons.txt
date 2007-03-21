Satisfying addon dependencies
=============================

To build Io's addons, you'll need to install the development libraries which 
they require. To do so you can follow the instructions below. On OSX you'll 
need to install DarwinPorts first (www.darwinports.com).

Missing avcodec library
=======================
  DarwinPorts/OSX > port install subversion ffmpeg
  Gentoo/Linux    > emerge -DN1 ffmpeg
  Ubuntu/Linux    > apt-get install libavcodec-dev && ldconfig

Missing dbi library
===================
  DarwinPorts/OSX > manually install libdbi (http://libdbi.sourceforge.net)
  Gentoo/Linux    > emerge -DN1 libdbi
  Ubuntu/Linux    > apt-get install libdbi0-dev && ldconfig

Missing event library
=====================
  DarwinPorts/OSX > port install libevent
  Gentoo/Linux    > emerge -DN1 libevent
  Ubuntu/Linux    > apt-get install libevent-dev && ldconfig

Missing ffi library
===================
  DarwinPorts/OSX > port install libffi
  Gentoo/Linux    > emerge -DN1 libffi
  Ubuntu/Linux    > apt-get install libffi-dev && ldconfig

Missing freetype library
========================
  DarwinPorts/OSX > port install freetype
  Gentoo/Linux    > emerge -DN1 freetype
  Ubuntu/Linux    > apt-get install libfreetype6-dev && ldconfig

Missing GL library
==================
  DarwinPorts/OSX > ?
  Gentoo/Linux    > emerge -DN1 mesa
  Ubuntu/Linux    > apt-get install libgl1-mesa-dev && ldconfig

Missing GLU library
===================
  DarwinPorts/OSX > ?
  Gentoo/Linux    > emerge -DN1 mesa
  Ubuntu/Linux    > apt-get install libglu1-mesa-dev && ldconfig

Missing glfw library
====================
  DarwinPorts/OSX > manually install glfw (http://glfw.sourceforge.net)
  Gentoo/Linux    > emerge -DN1 glfw
  Ubuntu/Linux    > manually install glfw (http://glfw.sourceforge.net)

Missing glut library
====================
  DarwinPorts/OSX > port install glut
  Gentoo/Linux    > emerge -DN1 media-libs/glut
  Ubuntu/Linux    > apt-get install freeglut3-dev && ldconfig

Missing gmp library
===================
  DarwinPorts/OSX > port install gmp
  Gentoo/Linux    > emerge -DN1 gmp
  Ubuntu/Linux    > apt-get install libgmp3-dev && ldconfig

Missing gnustep-gui library
===========================
  DarwinPorts/OSX > ?
  Gentoo/Linux    > emerge -DN1 gnustep-gui
  Ubuntu/Linux    > apt-get install libgnustep-gui-dev && ldconfig

Missing jpeg library
====================
  DarwinPorts/OSX > port install jpeg
  Gentoo/Linux    > emerge -DN1 jpeg
  Ubuntu/Linux    > apt-get install libjpeg62-dev && ldconfig

Missing ncurses library
=======================
  DarwinPorts/OSX > port install ncurses
  Gentoo/Linux    > emerge -DN1 ncurses
  Ubuntu/Linux    > apt-get install libncurses5-dev && ldconfig

Missing ode library
===================
  DarwinPorts/OSX > port install ode
  Gentoo/Linux    > emerge -DN1 ode
  Ubuntu/Linux    > apt-get install libode0-dev && ldconfig

Missing pcre library
====================
  DarwinPorts/OSX > port install pcre
  Gentoo/Linux    > emerge -DN1 libpcre
  Ubuntu/Linux    > apt-get install libpcre3-dev && ldconfig

Missing png library
===================
  DarwinPorts/OSX > port install libpng
  Gentoo/Linux    > emerge -DN1 libpng
  Ubuntu/Linux    > apt-get install libpng12-dev && ldconfig

Missing portaudio library
====================
  DarwinPorts/OSX > manually install portaudio (http://www.portaudio.com)
  Gentoo/Linux    > emerge -DN1 portaudio
  Ubuntu/Linux    > apt-get install libportaudio-dev && ldconfig

Missing pq library
==================
  DarwinPorts/OSX > port install postgresql
  Gentoo/Linux    > emerge -DN1 libpq
  Ubuntu/Linux    > apt-get install libpq-dev && ldconfig

Missing python2.4 library
=======================
  DarwinPorts/OSX > port install python24
  Gentoo/Linux    > emerge -DN1 python
  Ubuntu/Linux    > apt-get install python2.4-dev && ldconfig

Missing sgml library
====================
  DarwinPorts/OSX > cd Io/addons/SGML/source/libsgml && ./configure && make && make install
  Gentoo/Linux    > cd Io/addons/SGML/source/libsgml && ./configure && make && make install && ldconfig
  Ubuntu/Linux    > cd Io/addons/SGML/source/libsgml && ./configure && make && make install && ldconfig

Missing sndfile library
=======================
  DarwinPorts/OSX > port install libsndfile
  Gentoo/Linux    > emerge -DN1 libsndfile
  Ubuntu/Linux    > apt-get install libsndfile1-dev && ldconfig

Missing SoundTouch library
==========================
  DarwinPorts/OSX > port install soundtouch
  Gentoo/Linux    > emerge -DN1 libsoundtouch
  Ubuntu/Linux    > apt-get install libsoundtouch1-dev && ldconfig

Missing sqlite library
======================
  DarwinPorts/OSX > port install sqlite
  Gentoo/Linux    > emerge -DN1 sqlite
  Ubuntu/Linux    > apt-get install libsqlite0-dev && ldconfig

Missing sqlite3 library
=======================
  DarwinPorts/OSX > port install sqlite3
  Gentoo/Linux    > emerge -DN1 sqlite
  Ubuntu/Linux    > apt-get install libsqlite3-dev && ldconfig

Missing tag_c library
=====================
  DarwinPorts/OSX > port install taglib
  Gentoo/Linux    > emerge -DN1 taglib
  Ubuntu/Linux    > apt-get install libtagc0-dev && ldconfig

Missing tiff library
====================
  DarwinPorts/OSX > port install tiff
  Gentoo/Linux    > emerge -DN1 tiff
  Ubuntu/Linux    > apt-get install libtiff4-dev && ldconfig

Missing xml2 library
====================
  DarwinPorts/OSX > port install libxml2
  Gentoo/Linux    > emerge -DN1 libxml2
  Ubuntu/Linux    > apt-get install libxml2-dev && ldconfig
