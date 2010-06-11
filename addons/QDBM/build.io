AddonBuilder clone do(
	dependsOnLib("qdbm")
	debs atPut("qdbm", "libqdbm-dev")
	pkgs atPut("qdbm", "qdbm")
	kegs atPut("qdbm", "qdbm")
	headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/qdbm"))
)
