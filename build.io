Importer addSearchPath("build")

Directory fileNamedOrNil := method(path,
	f := fileNamed(path)
	if(f exists, f, nil)
)

Directory setCurrentWorkingDirectory(System launchPath)

args := System args clone
if(args at(1) == "-a") then(
	Project clone buildAddon(args at(2))
) elseif (args at(1)) then(
	Project clone doString(args at(1))
) else (
	Project clone build
)
