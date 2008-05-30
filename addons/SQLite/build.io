AddonBuilder clone do(
	dependsOnLib("sqlite")
	dependsOnHeader("sqlite.h")

	debs    atPut("sqlite", "libsqlite0-dev")
	ebuilds atPut("sqlite", "sqlite")
	pkgs    atPut("sqlite", "sqlite2")
	rpms    atPut("sqlite", "sqlite-devel")
)
