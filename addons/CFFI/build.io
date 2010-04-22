/* CFFI - A C function interface for Io
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

AddonBuilder clone do(

	//headerSearchPaths foreach(path, appendHeaderSearchPath(path .. "/libffi"))
	//libSearchPaths foreach(libSearchPath, appendLibSearchPath(libSearchPath .. "/libffi"))

	if(System platform == "Linux",	
		//Debian/Ubuntu x86
		appendHeaderSearchPath("/usr/include/i486-linux-gnu")

		//Debian/Ubuntu amd64
		appendHeaderSearchPath("/usr/include/x86_64-linux-gnu")
	)

	dependsOnLib("ffi")
	dependsOnHeader("ffi.h")

	debs    atPut("libffi", "libffi-dev")
	ebuilds atPut("libffi", "libffi")
	pkgs    atPut("libffi", "libffi")
	rpms    atPut("libffi", "libffi-devel")
)
