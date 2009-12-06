AddonBuilder clone do(
	dependsOnLib("qdbm")
	debs atPut("qdbm", "libqdbm-dev")
	pkgs atPut("qdbm", "qdbm")
)
