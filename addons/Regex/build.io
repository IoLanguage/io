AddonBuilder clone do(
	dependsOnLib("pcre")
	dependsOnHeader("pcre.h")

	dependsOnBinding("Range")

	debs atPut("pcre", "libpcre3-dev")
	ebuilds atPut("pcre", "pcre")
	pkgs atPut("pcre", "pcre")
	rpms atPut("pcre", "pcre-devel")
)
