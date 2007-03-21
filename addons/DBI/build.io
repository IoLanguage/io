Binding clone do(
	dependsOnLib("dbi")
	dependsOnHeader("dbi/dbi.h")
	setIsServerBinding(true)

	debs    atPut("dbi", "libdbi0-dev")
	ebuilds atPut("dbi", "libdbi")
//	pkgs    atPut("dbi", "libdbi")
	rpms    atPut("dbi", "libdbi-devel")
)
