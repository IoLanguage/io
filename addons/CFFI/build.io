/* CFFI - A C function interface for Io
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

AddonBuilder clone do(
	dependsOnLib("ffi")
	dependsOnHeader("ffi.h")

	headerSearchPaths foreach(path, appendHeaderSearchPath(path .. "/libffi"))
	libSearchPaths foreach(libSearchPath, appendLibSearchPath(libSearchPath .. "/libffi"))

	debs    atPut("ffi", "libffi-dev")
	ebuilds atPut("ffi", "libffi")
	pkgs    atPut("ffi", "libffi")
	rpms    atPut("ffi", "libffi-devel")
)
