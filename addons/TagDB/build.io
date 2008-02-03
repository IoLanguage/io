AddonBuilder clone do(
	dependsOnLib("qdbm")
	dependsOnLib("tagdb")
	appendHeaderSearchPath("/usr/local/include/tagdb")
	appendHeaderSearchPath("/usr/include/qdbm")
	
	if(platform != "windows",
		hasLib := libSearchPaths detect(path, Directory with(path) files detect(name containsSeq("libtagdb")))
		writeln("hasLib = ", hasLib)
		if(hasLib == nil,
			writeln("No libtagdb installed - attempting to compile and install")
			System system("cd addons/TagDB/source/tagdb; ./configure; echo \"sudo make install\"; sudo make install")
		)
	)

	clean := method(
		resend
		System system("cd addons/TagDB/source/tagdb; make clean")
	)
)
