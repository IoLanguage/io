AddonBuilder clone do(
	libSgmlDirectory := if(platform == "darwin", "libsgml-1.1.4_osx", "libsgml-1.1.4")
	configure        := if(platform == "darwin", "", " ./configure &&")
	
	dependsOnLib("sgml")
	dependsOnHeader(libSgmlDirectory .. "/libsgml.h")
	
//	appendInstallCommand("apt-get", "(cd addons/SGML/source/libsgml && ./configure && make && make install && ldconfig)")
//	appendInstallCommand("emerge",  "(cd addons/SGML/source/libsgml && ./configure && make && make install && ldconfig)")
//	appendInstallCommand("port",    "(cd addons/SGML/source/libsgml && ./configure && make && make install)")
//	appendInstallCommand("urpmi",   "(cd addons/SGML/source/libsgml && ./configure && make && make install && ldconfig)")
	
	if(platform != "windows",
		hasLib := libSearchPaths detect(path, Directory with(path) files detect(name containsSeq("libsgm")))
		if(hasLib == nil,
			writeln("No libsgml installed - attempting to compile and install")
			//cmd := "cd addons/SGML/source/" .. libSgmlDirectory .. "; ./configure; echo \"sudo make install\"; sudo make install"
			cmd := "cd addons/SGML/source/" .. libSgmlDirectory .. " && " .. configure .. " make && echo \"sudo make install\" && sudo make install"
			writeln(cmd)
			System system(cmd)
		)
	)
	
	clean := method(
	      	 d := "addons/SGML/source/" .. libSgmlDirectory
		 if ( Directory exists ( d ),
		      resend
		      System system("cd " .. d .. "; make clean")
		 )
	)
)
