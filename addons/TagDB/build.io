AddonBuilder clone do(
	dependsOnLib("qdbm")
	dependsOnLib("tagdb")
	appendHeaderSearchPath("/usr/local/include/tagdb")
	appendHeaderSearchPath("/usr/include/qdbm")
)
