AddonBuilder clone do(
	dependsOnLib("dbi")
	dependsOnHeader("dbi/dbi.h")

	debs    atPut("dbi", "libdbi0-dev")
	ebuilds atPut("dbi", "libdbi")
	pkgs    atPut("dbi", "libdbi")
	rpms    atPut("dbi", "libdbi-devel")
)
