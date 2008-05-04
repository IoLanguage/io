Project := Object clone do(
	cc  := method(System getEnvironmentVariable("CC")  ifNilEval(return "cc"))
	cxx := method(System getEnvironmentVariable("CXX") ifNilEval(return "g++"))
	platform := System platform split at(0) asLowercase

	systemCall := method(s,
		r := trySystemCall(s)
		if(r == 256, System exit(1))
		r
	)

	trySystemCall := method(s,
		result := System system("sh -c '" .. s .. "'")
		result
	)

	modulesInFolder := method(name,
		folder := Directory clone setPath(name)
		if(folder exists not, return List clone)
		subfolders := folder directories
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

	buildAddon := method(name,
		addons detect(addon, addon name == name) build(options)
	)

	availableAddon := method(addon,
		if(addon hasSlot("isAvailable"), return addon isAvailable)
		if(addon isDisabled,
			addon isAvailable := false
			return false
		)
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
			"-MD -Zi -DWIN32 -DNDEBUG -DIOBINDINGS -D_CRT_SECURE_NO_DEPRECATE"
		,
			"-Os -g -Wall -pipe -fno-strict-aliasing -DSANE_POPEN -DIOBINDINGS"
		)
	)

	build := method(
		File clone with("errors") remove close
		buildAddons
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

		orderedAddons foreach(build(options))
		self
	)

	libPaths := Directory with("libs") directories map(path)

	otherLibPaths := method(libtype,
		if(libtype == nil, libtype = "_build/dll")
		addons := availableAddons
		l := List clone
		l appendSeq(addons map(a, "-Laddons/" .. a name .. "/" .. libtype))
		l unique
	)

	includes := method(
		libPaths map(s, "-I" .. s .. "/_build/headers")
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
		writeln("--- Project generate docs from source file comments ---")
		systemCall("cd libs/iovm; ../../_build/binaries/io ../../tools/io/DocsExtractor.io .")
		//addons foreach(generateDocs)
		//build
		systemCall("_build/binaries/io tools/io/docs2html.io > docs/IoReference.html")
	)

	cleanDocs := method(
		writeln("--- Project clean embedded docs ---")
		systemCall("cd vm; make cleanDocs")
		addons foreach(cleanDocs)
	)

	runUnitTests := method(
		failures := 0

		maxNameSize := availableAddons max(name size) name size
		availableAddons foreach(addon,
			path := Path with(addon folder path, "tests/correctness/run.io")
			
			if(File clone setPath(path) exists,
				write(addon name alignLeft(maxNameSize), " - ")
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
