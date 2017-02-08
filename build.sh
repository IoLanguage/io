#!/bin/sh
# Build script for Steve

INSTALL_PREFIX="${INSTALL_PREFIX:-/usr/local}"

if [ X"$1" = X"linkInstall" ]; then
	rm -rf $INSTALL_PREFIX/lib/io $INSTALL_PREFIX/bin/io
	for i in $PWD/build/_build/dll/*; do ln -s $i $INSTALL_PREFIX/lib/`basename $i`; done
	mkdir $INSTALL_PREFIX/lib/io
	ln -s $PWD/addons $INSTALL_PREFIX/lib/io/
elif [ X"$1" = X"install" ]; then
	cd build
	make install
elif [ X"$1" = X"help" ]; then
	echo "Io build and installation"
	echo "Usage: $0 [help|install|linkInstall|clean]"
	echo -e "help\n\tThis message"
	echo -e "install\n\tInstalls Io (must have built it first)"
	echo -e "linkInstall\n\tLike above, but creates links into this hierarchy"
	echo -e "clean\n\tRemoves all build files. This will break a linkInstall."
	echo -e "Any other arguments, or lack thereof will build Io."
elif [ X"$1" = X"clean" ]; then
	[ -d build ] && rm -rf build || echo "Nothing to clean."
else
	mkdir -p build
	cd build
	cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" ..
	make all
fi

