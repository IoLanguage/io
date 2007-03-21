Binding clone do(
	dependsOnLib("sqlite")
	dependsOnHeader("sqlite.h")
	setIsServerBinding(true)

	debs    atPut("sqlite", "libsqlite0-dev")
	ebuilds atPut("sqlite", "sqlite")
	pkgs    atPut("sqlite", "sqlite")
	rpms    atPut("sqlite", "sqlite-devel")
)
