Binding clone do(
	dependsOnLib("sqlite3")
	dependsOnHeader("sqlite3.h")
	setIsServerBinding(true)

	debs    atPut("sqlite3", "libsqlite3-dev")
	ebuilds atPut("sqlite3", "sqlite")
	pkgs    atPut("sqlite3", "sqlite3")
	rpms    atPut("sqlite3", "sqlite3-devel")
)
