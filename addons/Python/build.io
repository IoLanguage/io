Binding clone do(
	dependsOnHeader("Python.h")

	version := list("2.6", "2.5", "2.4", "2.3", "2.2") detect(v, System system("python" .. v .. " -V 2> /dev/null") == 0)
	version ifNil(version = "2.4")

	dependsOnFrameworkOrLib("Python", "python" .. version)
	headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/python" .. version))

	debs    atPut("python2.4", "python2.4-dev")
	ebuilds atPut("python2.4", "python")
	pkgs    atPut("python2.4", "python24")
	rpms    atPut("python2.4", "python-devel")
)
