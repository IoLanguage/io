Sequence prepend := method(s, s .. self)

AddonBuilder := Object clone do(
	isDisabled := false
	disable := method(isDisabled = true)
	errors := ErrorReport clone

	platform := System platform split at(0) asLowercase
	cflags := method(System getEnvironmentVariable("CFLAGS") ifNilEval(""))
	if (platform == "windows",
		cc := method(System getEnvironmentVariable("CC") ifNilEval(return "cl -nologo"))
		cxx := method(System getEnvironmentVariable("CXX") ifNilEval(return "cl -nologo"))
		ccOutFlag := "-Fo"
		linkdll := "link -link -nologo"
		linkDirPathFlag := "-libpath:"
		linkLibFlag := ""
		linkOutFlag := "-out:"
		linkLibSuffix := ".lib"
		ar := "link -lib -nologo"
		arFlags := "-out:"
		ranlib := nil
	,
		cc := method(System getEnvironmentVariable("CC") ifNilEval(return "cc"))
		cxx := method(System getEnvironmentVariable("CXX") ifNilEval(return "g++"))
		ccOutFlag := "-o "
		linkdll := cc
		linkDirPathFlag := "-L"
		linkLibFlag := "-l"
		linkLibSuffix := ""
		linkOutFlag := "-o "
		linkLibSuffix := ""
		ar := method(System getEnvironmentVariable("AR") ifNilEval(return "ar"))
		arFlags := "rcu "
		ranlib := method(System getEnvironmentVariable("RANLIB") ifNilEval(return "ranlib"))
	)

	supportedOnPlatform := true

	/*

	frameworkSearchPaths := List clone
	frameworkSearchPaths append("/System/Library/Frameworks")
	frameworkSearchPaths append("/Library/Frameworks")
	frameworkSearchPaths append("/Local/Library/Frameworks")
//	frameworkSearchPaths append("~/Library/Frameworks")

	searchPrefixes := List clone
	searchPrefixes append(System installPrefix)
	searchPrefixes append("/usr")
	if(platform != "darwin", searchPrefixes append("/usr/X11R6"))
	if(platform == "mingw", searchPrefixes append("/mingw"))
	searchPrefixes append("/usr/local")
	searchPrefixes append("/usr/pkg")
	searchPrefixes append("/opt/local")
	searchPrefixes append("/sw")
	// on windows there is no such thing as a standard place
	// to look for these things
	searchPrefixes append("i:/io/addonLibs", "C:/io/addonLibs")

	headerSearchPaths := List clone
	appendHeaderSearchPath := method(v, if(File clone setPath(v) exists, headerSearchPaths appendIfAbsent(v)))
	searchPrefixes foreach(searchPrefix, appendHeaderSearchPath(searchPrefix .. "/include"))

	libSearchPaths := List clone
	appendLibSearchPath := method(v, if(File clone setPath(v) exists, libSearchPaths appendIfAbsent(v)))
	searchPrefixes foreach(searchPrefix, appendLibSearchPath(searchPrefix .. "/lib"))
*/

	setupPaths := method(

			self frameworkSearchPaths := List clone
			frameworkSearchPaths append("/System/Library/Frameworks")
			frameworkSearchPaths append("/Library/Frameworks")
			frameworkSearchPaths append("/Local/Library/Frameworks")
		//	frameworkSearchPaths append("~/Library/Frameworks")

			self searchPrefixes := List clone
			searchPrefixes append(System installPrefix)
			searchPrefixes append("/usr")
			if(platform != "darwin", searchPrefixes append("/usr/X11R6"))
			if(platform == "mingw", searchPrefixes append("/mingw"))
			searchPrefixes append("/usr/local")
			searchPrefixes append("/usr/pkg")
			searchPrefixes append("/opt/local")
			searchPrefixes append("/sw")
			// on windows there is no such thing as a standard place
			// to look for these things
			searchPrefixes append("i:/io/addonLibs", "C:/io/addonLibs")
					
	self headerSearchPaths := List clone
	self appendHeaderSearchPath := method(v, if(File clone setPath(v) exists, headerSearchPaths appendIfAbsent(v)))
	searchPrefixes foreach(searchPrefix, appendHeaderSearchPath(searchPrefix .. "/include"))

	self libSearchPaths := List clone
	self appendLibSearchPath := method(v, if(File clone setPath(v) exists, libSearchPaths appendIfAbsent(v)))
	searchPrefixes foreach(searchPrefix, appendLibSearchPath(searchPrefix .. "/lib"))
	)

	debs    := Map clone
	ebuilds := Map clone
	pkgs    := Map clone
	rpms    := Map clone

	init := method(
		self folder := Directory clone

		self depends := Object clone do(
			headers := List clone
			libs := List clone
			frameworks := List clone
			syslibs := List clone
			includes := List clone
			linkOptions := List clone
			addons := List clone
		)
		
		self defines := List clone

		setupPaths
	)

	mkdir := method(relativePath,
		path := Path with(folder path, relativePath)
		if(Directory exists(path) not,
			writeln("mkdir -p ", relativePath)
			Directory with(path) createIfAbsent
		)
	)

	pathForFramework := method(name,
		frameworkname := name .. ".framework"
		frameworkSearchPaths detect(path,
			Directory with(path .. "/" .. frameworkname) exists
		)
	)

	pathForHeader := method(name,
		headerSearchPaths detect(path,
			File with(path .. "/" .. name) exists
		)
	)

	pathForLib := method(name,
		libNames := list("." .. dllSuffix, ".a", ".lib") map(suffix, "lib" .. name .. suffix)
		libSearchPaths detect(path,
			libDirectory := Directory with(path)
			libNames detect(libName, libDirectory fileNamed(libName) exists)
		)
	)

	addDefine := method(v, defines appendIfAbsent(v))
	dependsOnBinding := method(v, depends addons appendIfAbsent(v))
	dependsOnHeader := method(v, depends headers appendIfAbsent(v))
	dependsOnLib := method(v,
		depends libs contains(v) ifFalse(
			depends libs append(v)
			searchPrefixes appendIfAbsent(v)
		)
	)
	dependsOnFramework := method(v, depends frameworks appendIfAbsent(v))
	dependsOnInclude := method(v, depends includes appendIfAbsent(v))
	dependsOnLinkOption := method(v, depends linkOptions appendIfAbsent(v))
	dependsOnSysLib := method(v, depends syslibs appendIfAbsent(v))

	dependsOnFrameworkOrLib := method(v, w,
		path := pathForFramework(v)
		if(path != nil, dependsOnFramework(v) ; appendHeaderSearchPath(path .. "/" .. v .. ".framework/Headers"), dependsOnLib(w))
	)

	optionallyDependsOnLib       := method(v, a := pathForLib(v) != nil; if(a, dependsOnLib(v)); a)
	optionallyDependsOnFramework := method(v, a := pathForFramework(v) != nil; if(a, dependsOnFramework(v)); a)

	missingFrameworks := method(errors,
		writeln("errors: ", errors type)
		depends frameworks select(p,
			if(pathForFramework(p) == nil,
				errors addError(self name .. " is missing " .. p .. " framework\n")
				self isAvailable := false
				true
			)
		)
	)

	missingHeaders := method(errors,
		depends headers select(p,
			if(pathForHeader(p) == nil,
				errors addError(self name .. " is missing " .. p .. " header\n")
				self isAvailable := false
				true
			)
		)
	)

	missingLibs := method(errors,
		depends libs select(p,
			//writeln("pathForLib(", p, ") = ", pathForLib(p))
			if(pathForLib(p) == nil,
				errors addError(self name .. " is missing " .. p .. " library\n")
				self isAvailable := false
				true
			)
		)
	)

	hasDepends := method(errors, missingFrameworks(errors) size == 0 and missingLibs(errors) size == 0 and missingHeaders(errors) size == 0)

	installCommands := method(
		commands := Map clone
		errors := ErrorReport clone
		missingLibs(errors) foreach(p,
			if(debs at(p), commands atPut("aptget", "apt-get install " .. debs at(p) .. " && ldconfig"))
			if(ebuilds at(p), commands atPut("emerge", "emerge -DN1 " .. ebuilds at(p)))
			if(pkgs at(p), commands atPut("port", "port install " .. pkgs at(p)))
			if(rpms at(p), commands atPut("urpmi", "urpmi " .. rpms at(p) .. " && ldconfig"))
		)
		commands
	)

	with := method(path,
		module := self clone
		module folder setPath(path)
		module
	)

	systemCall := method(s,
		if(trySystemCall(s) == 256, System exit(1))
	)

	trySystemCall := method(s,
		oldPath := nil
		if (folder path != ".",
			oldPath := Directory currentWorkingDirectory
			//writeln("::::::::: folder path = ", folder path)
			Directory setCurrentWorkingDirectory(folder path)
		)
		writeln("::::::::: Directory currentWorkingDirectory: ", Directory currentWorkingDirectory)
		writeln(s)
		result := System system(s)
		if (oldPath != nil,
			Directory setCurrentWorkingDirectory(oldPath)
		)
		result
	)

	// ------------------------------------

	name := method(folder name)
	oldDate := Date clone setYear(1970)

	libName := method("libIo" .. folder name ..  ".a")

	libFile := method(folder fileNamedOrNil(libName))
	objsFolder := method(self objsFolder := folder createSubdirectory("_build/objs"))
	sourceFolder := method(folder directoryNamed("source"))
	cFiles := method(
		files := sourceFolder filesWithExtension("cpp") appendSeq(sourceFolder filesWithExtension("c"))
		if(platform != "windows", files appendSeq(sourceFolder filesWithExtension("m")))
		files select(f, f name beginsWithSeq("._") not)
	)

	libsFolder   := method(Directory with("libs"))
	addonsFolder := method(Directory with("addons"))

	includePaths := method(
		includePaths := List clone
		includePaths appendSeq(libsFolder directories map(path) map(p, Path with(p, "_build/headers")))
		includePaths appendSeq(depends addons map(n, "addons/" .. n .. "/_build/headers"))
		includePaths
	)

	build := method(options,
		writeln(("--- " .. folder name .. " ") alignLeft(79, "-"))
		writeln("build.io: Entering directory `", folder path, "'")
		mkdir("_build/headers")
		mkdir("source")
		if(Directory with(Path with(folder path, "source")) filesWithExtension(".h") size > 0,
			trySystemCall("cp source/*.h _build/headers")
		)

		generateInitFile

		options := options ifNilEval("") .. cflags .. " " .. defines map(d, "-D" .. d) join(" ")
		cFiles foreach(f,
			obj := f name replaceSeq(".cpp", ".o") replaceSeq(".c", ".o") replaceSeq(".m", ".o")
			objFile := objsFolder fileNamedOrNil(obj)
			if((objFile == nil) or(objFile lastDataChangeDate < f lastDataChangeDate),

				includes := includePaths map(v, "-I" .. Path with("../../", v))
				includes appendSeq(headerSearchPaths map(v, "-I" .. v))

				s := cc .. " " .. options .. " " .. depends includes join(" ") .. " " .. includes join(" ") .. " -I. "
				if(list("cygwin", "mingw", "windows") contains(platform) not,
					s = s .. "-fPIC "
				,
					s = s .. "-DBUILDING_"
					s = s .. name asUppercase
					s = s .. "_ADDON "
				)

				s = s .. "-c " .. ccOutFlag .. "_build/objs/" .. obj .. " source/" .. f name
				systemCall(s)
			)
		)

		buildLib
		buildDynLib
		if(platform == "windows", embedManifest)
		writeln("build.io: Leaving directory `", folder path, "'")
		writeln
	)

	buildLib := method(
		mkdir("_build/lib")
		systemCall(ar .. " " .. arFlags ..  "_build/lib/" .. libName .. " _build/objs/*.o")
				if (ranlib != nil, systemCall(ranlib .. " _build/lib/" .. libName))
	)

	dllSuffix := method(
		if(list("cygwin", "mingw", "windows") contains(platform), return "dll")
		if(platform == "darwin", return "dylib")
		"so"
	)

	dllNameFor := method(s, "lib" .. s .. "." .. dllSuffix)

	dllCommand := method(
		if(platform == "darwin",
			//"-dynamiclib -single_module -read_only_relocs suppress"
			"-dynamiclib -single_module"
		,
			if (platform == "windows",
				"-dll -debug"
			,
				"-shared"
			)
		)
	)

	buildDynLib := method(
		mkdir("_build/dll")

		links := depends addons map(b, linkDirPathFlag .. "../" .. b .. "/_build/dll")

		links appendSeq(depends addons map(v, linkLibFlag .. "Io" .. v .. linkLibSuffix))
		if(platform == "windows",
			links appendSeq(depends syslibs map(v, v .. ".lib"))
		)
		if(platform != "darwin" and platform != "windows",
			links appendSeq(depends addons map(v, "-Wl,--rpath -Wl," .. System installPrefix .. "/lib/io/addons/" .. v .. "/_build/dll/"))
		)
		links appendSeq(libSearchPaths map(v, linkDirPathFlag .. v))
		links appendSeq(depends libs map(v, linkLibFlag .. v .. linkLibSuffix))
		links appendSeq(list(linkDirPathFlag .. "../../_build/dll", linkLibFlag .. "iovmall" .. linkLibSuffix))

		links appendSeq(depends frameworks map(v, "-framework " .. v))
		links appendSeq(depends linkOptions)

		libname := dllNameFor("Io" .. self name)

		s := ""
		if(platform == "darwin",
			links append("-flat_namespace")
			s := " -install_name " .. System installPrefix .. "/lib/io/addons/" .. self name .. "/_build/dll/" .. libname
			//s := " -install_name io/addons/" .. self name .. "/_build/dll/" .. libname
		)

		systemCall(linkdll .. " " .. cflags .. " " .. dllCommand .. " " .. s .. " " .. linkOutFlag .. "_build/dll/" .. libname .. " _build/objs/*.o" .. " " .. links join(" "))
	)

	embedManifest := method(
		dllFilePath := "_build/dll/" .. dllNameFor("Io" .. name)
		manifestFilePath := dllFilePath .. ".manifest"
			systemCall("mt.exe -manifest " .. manifestFilePath .. " -outputresource:" .. dllFilePath)
		writeln("Removing manifest file: " .. manifestFilePath)
		File with(folder path .. "/" .. manifestFilePath) remove
	)

	clean := method(
		writeln(folder name, " clean")
		trySystemCall("rm -rf _build")
		trySystemCall("rm -f source/Io*Init.c")
		self removeSlot("objsFolder")
	)

	ioCodeFolder := method(folder directoryNamed("io"))
	ioFiles      := method(ioCodeFolder filesWithExtension("io"))
	initFileName := method("source/Io" .. folder name .. "Init.c")

	isStatic := false

	generateInitFile := method(
		if(platform != "windows" and folder directoryNamed("source") filesWithExtension("m") size != 0, return)
		initFile := folder fileNamed(initFileName) create
		initFile remove open
		initFile write("#include \"IoState.h\"\n")
		initFile write("#include \"IoObject.h\"\n\n")

		sourceFiles := folder directoryNamed("source") files
		iocFiles := sourceFiles select(f, f name beginsWithSeq("Io") and(f name endsWithSeq(".c")) and(f name containsSeq("Init") not) and(f name containsSeq("_") not))
		iocppFiles := sourceFiles select(f, f name beginsWithSeq("Io") and(f name endsWithSeq(".cpp")) and(f name containsSeq("Init") not) and(f name containsSeq("_") not))

		iocFiles appendSeq(iocppFiles)
		extraFiles := sourceFiles select(f, f name beginsWithSeq("Io") and(f name endsWithSeq(".c")) and(f name containsSeq("Init") not) and(f name containsSeq("_")))

		orderedFiles := List clone appendSeq(iocFiles)

		iocFiles foreach(f,
			d := f open readLines detect(line, line containsSeq("docDependsOn"))
			f close

			if(d,
				prerequisitName := "Io" .. d afterSeq("(\"") beforeSeq("\")") .. ".c"
				prerequisit := orderedFiles detect(of, of name == prerequisitName )
				orderedFiles remove(f)
				orderedFiles insertAfter(f, prerequisit)
			)
		)

		iocFiles = orderedFiles

		iocFiles foreach(f,
			initFile write("IoObject *" .. f name fileName .. "_proto(void *state);\n")
		)

		extraFiles foreach(f,
			initFile write("void " .. f name fileName .. "Init(void *context);\n")
		)

		if (platform == "windows",
			initFile write("__declspec(dllexport)\n")
		)
		initFile write("\nvoid Io" .. folder name .. "Init(IoObject *context)\n")
		initFile write("{\n")
		if(iocFiles size > 0,
			initFile write("\tIoState *self = IoObject_state((IoObject *)context);\n\n")
		)

		iocFiles foreach(f,
			initFile write("\tIoObject_setSlot_to_(context, SIOSYMBOL(\"" .. f name fileName asMutable removePrefix("Io") .. "\"), " .. f name fileName .. "_proto(self));\n\n")
		)

		extraFiles foreach(f,
			initFile write("\t" .. f name fileName .. "Init(context);\n")
		)

		if(ioCodeFolder and isStatic,
			ioFiles foreach(f, initFile write(codeForIoFile(f)))
		)

		initFile write("}\n")
		initFile close
	)

	codeForIoFile := method(f,
		code := Sequence clone
		if (f size > 0,
			code appendSeq("\t{\n\t\tchar *s = ")
			code appendSeq(f contents splitNoEmpties("\n") map(line, "\"" .. line escape .. "\\n\"") join("\n\t\t"))
			code appendSeq(";\n\t\tIoState_on_doCString_withLabel_(self, context, s, \"" .. f name .. "\");\n")
			code appendSeq("\t}\n\n")
		)
		code
	)

	generateDocs := method(
		systemCall("../../_build/binaries/io ../../tools/io/DocsExtractor.io .")
	)

	cleanDocs := method(
		systemCall("rm -f docs/docs.txt")
	)
)
