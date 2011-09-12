//mkdir package
//cp /usr/local/bin/io package
//cp /usr/local/bin/libiovmall.dylib package

Socket 

File dllDependencies := method(
	lines := SystemCall clone setCommand("otool") setArguments(list("-L", path)) run split("\n")
	lines select(beginsWithSeq("\t")) map(strip beforeSeq(" ("))
)

AddonFolder := Object clone do(
	dir ::= nil

	dllDir := method(
		dir directoryNamed("_build") directoryNamed("dll")
	)

	dllFile := method(
		if(dllDir exists not, return nil)
		dllDir files detect(name endsWithSeq(".dylib"))
	)

	hasDll := method(dllFile != nil)

	dependencyPaths := method(
		dir path println
		dllFile dllDependencies 
	)

	pathConsideredExternal := method(path,
		skippers := list("libiovmall", "/io/", "libSystem", "/System/Library/Frameworks", ".framework")
		skippers foreach(skip, if(path containsSeq(skip), return false))
		true
	)

	externalDependencyPaths := method(
		dllFile dllDependencies select(path, pathConsideredExternal(path))
	)
	
	dependsFolder := Directory currentWorkingDirectory

	copyExternalDependencies := method(
		
	)

	patchDependenciesToUseCopies := method(

	)
)

addons := Directory with("addons") directories map(dir, AddonFolder clone setDir(dir))
addons select(hasDll) map(externalDependencyPaths)



