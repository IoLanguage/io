/* CFFI - A C function interface for Io
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

AddonBuilder clone do(
	dependsOnLib("ffi")
	dependsOnHeader("ffi.h")

	headerSearchPaths foreach(path, appendHeaderSearchPath(path .. "/libffi"))
	libSearchPaths foreach(libSearchPath, appendLibSearchPath(libSearchPath .. "/libffi"))

	debs    atPut("libffi", "libffi-dev")
	ebuilds atPut("libffi", "libffi")
	pkgs    atPut("libffi", "libffi")
	rpms    atPut("libffi", "libffi-devel")
)
