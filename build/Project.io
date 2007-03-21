Project := Object clone do(
	cc  := method(System getenv("CC")  ifNilEval(return "cc"))
	cxx := method(System getenv("CXX") ifNilEval(return "g++"))
	platform := System platform split at(0) asLowercase
	platformVersion := System platformVersion

	systemCall := method(s,
		r := trySystemCall(s)
		if(r == 256, System exit(1))
		r
	)

	trySystemCall := method(s,
		//if(folder path != ".", s := "cd " .. folder path .. "; " .. s)
		//writeln(s, "\n")
		result := System system("sh -c '" .. s .. "'")
		result
	)

	modulesInFolder := method(name,
		folder := Directory clone setPath(name)
		if(folder exists not, return List clone)
		subfolders := folder folders
		subfolders selectInPlace(fileNamedOrNil("build.io"))
		subfolders map(f,
			module := Lobby doString(f fileNamedOrNil("build.io") contents)
			module folder setPath(f path)
			module
		)	
	)

	addons := method(
		self addons := modulesInFolder("addons") sortInPlaceBy(block(x, y, x name < y name))
	)

	availableAddon := method(addon,
		if(addon hasSlot("isAvailable"), return addon isAvailable)
		if(addon supportedOnPlatform(platform) not,
			error := (addon name .. " is not supported on " .. platform .. "\n") print
			File clone openForAppending("errors") write(error) close
			addon isAvailable := false
			return false
		)
		if(addon hasDepends not, return false)
		addon depends addons foreach(addonName,
			dependancy := addons detect(name == addonName)
			if(dependancy == nil,
				error := (addon name .. " is missing " .. addonName .. " addon\n") print
				File clone openForAppending("errors") write(error) close
				addon isAvailable := false
				return false
			)
			if(availableAddon(dependancy) not,
				error := (addon name .. " is missing " .. addonName .. " addon\n") print
				File clone openForAppending("errors") write(error) close
				addon isAvailable := false
				return false
			)
		)
		addon isAvailable := true
		true
	)

	availableAddons := method(
		addons selectInPlace(addon, availableAddon(addon))
	)

	options := method(
                if (platform == "windows",
                        "-MDd -DWIN32 -D_DEBUG -DIOBINDINGS"
                ,
                        "-Os -g -Wall -DSANE_POPEN -DIOBINDINGS"
                )
        )

	build := method(
		File clone with("errors") remove close
		buildAddons
		//buildServer
		//buildDesktop
		writeln("\n--- build complete ---\n")
	)

	orderedAddons := method(
		ordered := List clone
		dependencies := availableAddons clone
		while(0 < dependencies size,
			ordered = dependencies clone appendSeq(ordered) unique
			dependencies = dependencies map(depends addons map(a, addons detect(name == a))) flatten unique
		)
		ordered
	)

	buildAddons := method(
		writeln("\n--- building addons ---\n")

		//orderedAddons foreach(name println)

		orderedAddons foreach(build(options))
		self
	)

	generateAddonsInitFileFor := method(initAddons,
		f := File with("IoAddonsInit.c") remove open
		f write("\n")
		f write("#include \"IoObject.h\"\n")
		initAddons foreach(b, f write("void Io" .. b folder name .. "Init(void *context);\n"))
		f write("\n")
		f write("void IoAddonsInit(void *context)\n{\n")
		//f write("void *self = ((IoObject *)context)->tag->state;\n")
		initAddons foreach(b, f write("	Io" .. b folder name .. "Init(context);\n"))
		f write("}\n\n")
		f close
	)

	buildServer := method(
		writeln("\n--- building ioServer ---\n")
		buildExeWithAddons("_binaries/ioServer", availableAddons select(isServerBinding))
	)

	buildDesktop := method(
		writeln("\n--- building ioDesktop ---\n")
		buildExeWithAddons("_binaries/ioDesktop", availableAddons)
	)

	libPaths := Directory with("libs") folders map(path)

	baseLinks := method(libtype,
		baselibs := list("iovm", "coroutine", "garbagecollector", "skipdb", "basekit")
		baselibs map(a, "-Llibs/" .. a .. "/" .. libtype .. " -l" .. a)
	)

	otherLibPaths := method(libtype,
		if(libtype == nil, libtype = "_build/dll")
		addons := availableAddons
		l := List clone
		l appendSeq(addons map(a, "-Laddons/" .. a name .. "/" .. libtype))
		l unique
	)

	links := method(libtype,
		if(libtype == nil, libtype = "_build/dll")

		l := List clone
		addons := availableAddons
		if(list("darwin", "freebsd", "netbsd", "openbsd") contains(platform) not, l append("-lm"))
		if(list("linux", "sunos", "syllable") contains(platform), l append("-ldl"))
		if(platform == "darwin", l append("-L/opt/local/lib"))
		
		//l appendSeq(addons map(a, "-lIo" .. a name) flatten unique)

		l appendSeq(addons map(depends libs) flatten unique map(k, " -l" .. k))
		//l appendSeq(addons map(a, " -lIo" .. a name ))
		l appendSeq(addons map(a, " addons/" .. a name .. "/_build/lib/libIo" .. a name .. ".a"))
		

		l appendSeq(addons map(depends frameworks) flatten unique map(k, "-framework " .. k))
		//l appendSeq(addons map(depends linkOptions) flatten unique map(" " .. k .. " "))

		l unique
	)

	includes := method(
		libPaths map(s, "-I" .. s .. "/_build/headers")
	)

	buildExeWithAddons := method(exeName, inAddons,
		generateAddonsInitFileFor(inAddons)
		libPathsString := Sequence clone

		s := cxx .. " " .. options .. " " .. includes join(" ") .. " " .. " -c -o IoAddonsInit.o IoAddonsInit.c "
		systemCall(s)

		//s := cxx .. " " .. options .. " -o " .. exeName .. " tools/_build/objs/main.o IoAddonsInit.o " .. otherLibPaths("_build/lib") join(" ") .. " " .. libPathsString  .. " ".. baseLinks("_build/lib") join(" ") .. " " .. links("_build/lib") join(" ") .. " "

		ll := " " .. libPathsString  .. " ".. baseLinks("_build/lib") join(" ") .. " " .. links("_build/lib") join(" ") .. " "
		s := cxx .. " " .. options .. " " .. ll .. " -o " .. exeName .. " tools/_build/objs/main.o IoAddonsInit.o "

		systemCall(s)
	)

	cleanAddons := method(
		writeln("--- Project cleanAddons ---")
		addons foreach(clean)
	)

	clean := method(
		writeln("--- Project clean ---")
		cleanAddons
		trySystemCall("rm -rf projects/*/build")
		trySystemCall("rm -f IoAddonsInit.*")
	)

	docs := method(
		writeln("--- Project generate embedded docs ---")
		systemCall("cd libs/iovm; ../../_build/binaries/io ../../tools/io/DocsExtractor.io .")
		addons foreach(generateDocs)
	)

	cleanDocs := method(
		writeln("--- Project clean embedded docs ---")
		systemCall("cd vm; make cleanDocs")
		addons foreach(cleanDocs)
	)

	runUnitTests := method(
		//systemCall("cd tools; make test")
		failures := 0
		//maxName := availableAddons map(name size) max
		//writeln("maxName = ", maxName)
		
		availableAddons foreach(addon,
			path := Path with(addon folder path, "tests/run.io")
			//writeln("path = ", path)
			if(File clone setPath(path) exists,
				write(addon name alignLeft(10), " - ")
				File standardOutput flush
				r := System system("./_build/binaries/io " .. path)
				if(r == 0, writeln("PASSED"), writeln("FAILED ", r, " TESTS"))
				failures = failures + r
			)
		)
		writeln("")
		if(failures == 0, writeln("ALL TESTS PASSED"), writeln("FAILED ", failures, " tests"))
		failures
	)

	installDependenciesFor := method(name,
		addons map(installCommands) map(at(name)) select(!=nil) foreach(c,
			c println
			System system(c)
		)
	)

	aptget := method(installDependenciesFor("aptget"))
	emerge := method(installDependenciesFor("emerge"))
	port   := method(installDependenciesFor("port"))
	urpmi  := method(installDependenciesFor("urpmi"))
)
