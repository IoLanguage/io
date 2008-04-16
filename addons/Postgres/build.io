AddonBuilder clone do(
	dependsOnLib("pq")
	if(platform == "darwin", appendLibSearchPath("/opt/local/lib/postgresql81"))
//	libSearchPaths foreach(libSearchPath, appendLibSearchPath(libSearchPath .. "/postgresql81"))

	debs    atPut("pq", "libpq-dev")
	ebuilds atPut("pq", "libpq")
	pkgs    atPut("pq", "postgresql83")
	rpms    atPut("pq", "postgresql-devel")
)
