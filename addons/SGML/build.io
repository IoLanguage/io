AddonBuilder clone do(
	dependsOnLib("sgml")
	dependsOnHeader("sgml/libsgml.h")
//	appendInstallCommand("apt-get", "(cd addons/SGML/source/libsgml && ./configure && make && make install && ldconfig)")
//	appendInstallCommand("emerge",  "(cd addons/SGML/source/libsgml && ./configure && make && make install && ldconfig)")
//	appendInstallCommand("port",    "(cd addons/SGML/source/libsgml && ./configure && make && make install)")
//	appendInstallCommand("urpmi",   "(cd addons/SGML/source/libsgml && ./configure && make && make install && ldconfig)")
	
	if(platform != "windows",
		hasLib := libSearchPaths detect(path, Directory with(path) files detect(name containsSeq("libsgm")))
		hasLib = nil
		if(hasLib == nil,
			writeln("No libsgml installed - attempting to compile and install")
			System system("cd addons/SGML/source/libsgml; ./configure; echo \"sudo make install\"; sudo make install")
		)
	)
	
	clean := method(
		resend
		System system("cd addons/SGML/source/libsgml; make clean")
	)
)
