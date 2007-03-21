
Importer addSearchPath("build")
//doFile("vm/processIoCodeFor.io")

Directory do(    
    nonUnderscoreFolders := method(folders select(name beginsWithSeq("_") not))
    fileNamedOrNil := method(path, f := fileNamed(path); if(f exists, f, nil))
)

Directory setCurrentWorkingDirectory(launchPath)

if(args at(1) == "-a") then(
	Project clone addons detect(name == args at(2)) build
) elseif (args at(1)) then(
	Project clone doString(args at(1))
) else (
	Project clone build
)
