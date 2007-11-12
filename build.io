Importer addSearchPath("build")

Directory fileNamedOrNil := method(path,
    f := fileNamed(path)
    if(f exists, f, nil)
)

Directory setCurrentWorkingDirectory(launchPath)

project := Project clone
if(args at(1) == "-a") then(
    project buildAddon(args at(2))
) elseif (args at(1)) then(
	project clone doString(args at(1))
) else (
	project clone build
)
