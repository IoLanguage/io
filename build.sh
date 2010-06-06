#!/bin/sh
# Build script for Steve

INSTALL_PREFIX="/usr/local"

if [ X"$1" = X"linkInstall" ]; then
	rm -rf $INSTALL_PREFIX/lib/io $INSTALL_PREFIX/bin/io
	for i in $PWD/build/_build/dll/*; do ln -s $i $INSTALL_PREFIX/lib/`basename $i`; done
	mkdir $INSTALL_PREFIX/lib/io
	ln -s $PWD/addons $INSTALL_PREFIX/lib/io/
elif [ X"$1" = X"help" ]; then
	echo "Io build and installation"
	echo "Usage: $0 [help|linkInstall]"
	echo "Any argument that isn't help or linkInstall will build and install Io."
else
	[ -d build ] && rm -rf build && mkdir build
	cd build
	cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" ..
	make all install
fi

