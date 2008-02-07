Importer addSearchPath("build")

Directory fileNamedOrNil := method(path,
	f := fileNamed(path)
	if(f exists, f, nil)
)

Directory setCurrentWorkingDirectory(System launchPath)

project := Project clone
args := System args clone
if(args at(1) == "-a") then(
	project buildAddon(args at(2))
) elseif (args at(1)) then(
	project clone doString(args at(1))
) else (
	project clone build
)
