Binding clone do(
	dependsOnLib("pcre")
	dependsOnHeader("pcre.h")
	setIsServerBinding(true)

	debs    atPut("pcre", "libpcre3-dev")
	ebuilds atPut("pcre", "pcre")
	pkgs    atPut("pcre", "libpcre")
	rpms    atPut("pcre", "pcre-devel")
)
