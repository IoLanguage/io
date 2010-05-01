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

/*
	modulesInFolder := method(name,
		folder := Directory clone setPath(name)
		if(folder exists not, return List clone)
		subfolders := folder directories
		subfolders selectInPlace(fileNamedOrNil("build.io"))
		//subfolders selectInPlace(directoryNamed("build") fileNamedOrNil("build.io"))
		subfolders map(f,
			module := Lobby doString(f fileNamedOrNil("build.io") contents)
			//module := Lobby doString(f directoryNamed("build") fileNamedOrNil("build.io") contents)
			module folder setPath(f path)
			module
		)
	)

	addons := method(
		self addons := modulesInFolder("addons") sortInPlaceBy(block(x, y, x name < y name))
	)
	
 	buildAddon := method(name,
		currentAddon := addons detect(addon, addon name == name)
		if(currentAddon == nil,	Exception raise("No addon named " .. name .. " found!"))
		currentAddon build(options)
	)
*/

    modulesInFolder := method(path,
        modules := list()
        folder := Directory with(path)

        if(folder exists,
            folder walk(f, if(f name == "build.io",
                # Some modules, might have invalid build files, in that case
                # the error message will be printed.
                try(
                    module := Lobby doString(f contents)
                    module folder setPath(f path pathComponent)
                    modules append(module)
                ) catch(
                    ("Module `#{f path split(\"/\") at(-2)}` " interpolate ..
                     "has invalid build.io file!")  println
                )
            ))
        )
        modules
    )

    addons := lazySlot(modulesInFolder("addons") sortInPlace(name))

    buildAddon := method(name,
        currentAddon := addons detect(name == name)
        if(currentAddon isNil,
            # Probably it makes sense, to abstract this into the Project
            # method, like Project error(aString).
            "Addon `#{name}` not found!" interpolate println
            System exit(1)
        ,
            currentAddon build(options)
        )
    )

	availableAddon := method(addon,
		if(addon hasSlot("isAvailable"),
			//writeln(addon name, " isAvailable")
			return addon isAvailable
		)
		if(addon isDisabled,
			//writeln(addon name, " isDisabled")
			addon isAvailable := false
			return false
		)
		if(addon supportedOnPlatform(platform) not,
			errors addError(addon name .. " is not supported on " .. platform .. "\n")
			addon isAvailable := false
			return false
		)
		if(addon hasDepends(errors) not,
			return false
		)
		addon depends addons foreach(addonName,
			dependancy := addons detect(name == addonName)
			if(dependancy == nil,
				errors addError(addon name .. " is missing " .. addonName .. " addon\n")
				addon isAvailable := false
				return false
			)
			if(availableAddon(dependancy) not,
				errors addError(addon name .. " is missing " .. addonName .. " addon\n")
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
			//"-MDd -Zi -DWIN32 -D_DEBUG -DIOBINDINGS -D_CRT_SECURE_NO_DEPRECATE"
			"-MD -Zi -DWIN32 -DNDEBUG -DIOBINDINGS -D_CRT_SECURE_NO_DEPRECATE"
		,
			"-Os -g -Wall -pipe -fno-strict-aliasing -DSANE_POPEN -DIOBINDINGS"
		)
	)

	setupErrors := method(
		self errors := ErrorReport clone
		errors removeFile
	)

	build := method(
		setupErrors
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
		addons foreach(generateDocs)
		//build
		docsPath := "docs/reference"
		systemCall("rm -rf " .. Path with(docsPath))
		systemCall("mkdir " .. docsPath)
		systemCall("_build/binaries/io tools/io/docs2html.io " .. docsPath)
		writeln(docsPath .." generated")
	)

	cleanDocs := method(
		writeln("--- Project clean embedded docs ---")
		systemCall("cd libs/iovm; rm -rf docs/docs.txt")
		addons foreach(cleanDocs)
	)

	runUnitTests := method(
		setupErrors

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
