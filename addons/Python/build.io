AddonBuilder clone do(
	dependsOnHeader("Python.h")

	version := list("2.6", "2.5", "2.4", "2.3", "2.2") detect(v, System system("python" .. v .. " -V 2> /dev/null") == 0)
	version ifNil(version = "2.4")
	versionShort := version asMutable removeSeq(".")

	dependsOnFrameworkOrLib("Python", "python" .. version)
	headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/python" .. version))

	debs    atPut("python" .. version, "python" .. version .. "-dev")
	ebuilds atPut("python" .. version, "python")
	pkgs    atPut("python" .. version, "python" .. versionShort)
	rpms    atPut("python" .. version, "python-devel")
)
