

all:
	cp -fR ../../IoDesktop/IoResources build/IoApp.app/Contents/Resources
	cp -fR ../../IoDesktop/Apps/SongJam/* build/IoApp.app/Contents/Resources
	rm -rf build/IoApp.app/Contents/Resources/Sounds/*
	rm  build/IoApp.app/Contents/Resources/Songs/default.sjm
	touch build/IoApp.app/Contents/Resources/Songs/default.sjm

	if test -d build/SongJam.app; then (rm -Rf build/SongJam.app); fi 
	cp -fR build/IoApp.app build/SongJam.app

	if test -e build/SongJam.app.tar; then (rm build/SongJam.app.tar); fi 
	cd build; tar -cvf SongJam.app.tar SongJam.app

	cd build; if test -e SongJam.app.tar.gz; then (rm SongJam.app.tar.gz); fi 
	cd build; gzip -9 SongJam.app.tar

	cd build; if test -e SongJam.app.tgz; then (rm SongJam.app.tgz); fi 
	cd build; mv SongJam.app.tar.gz SongJam.app.tgz
	cd build; cp SongJam.app.tgz /Library/WebServer/dekorte/Software/OSX/SongJam 