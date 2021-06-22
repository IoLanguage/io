Addon := Object clone do(
	//doc Addon rootPath Returns the rootPath of the addon's folder.
	//doc Addon setRootPath(aSequence) Sets rootPath of the addon's folder. Returns self.
	newSlot("rootPath")

	//doc Addon name Returns the name of the addon.
	//doc Addon setName(aSequence) Sets the name of the addon. Returns self.
	newSlot("name")

	//doc Addon platform Implemented as method(System platform asLowercase).
	platform := System platform asLowercase

	//doc Addon dllSuffix Returns the platform specific dll suffix.
	dllSuffix := method(
        list("cygwin", "mingw", "windows") detect(dllPlatform,
            self platform containsSeq(dllPlatform)
        ) ifNonNil(return("dll"))

        if(platform == "darwin", "dylib", "so")
	)

	//doc Addon dllName Return the name of the dll for the addon.
	dllName := method("libIo" .. name .. "." .. dllSuffix)

	//doc Addon addonPath Implemented as Path with(rootPath, name).
	addonPath := method(Path with(rootPath, name))

	//doc Addon dllPath Returns the path to the dll for the addon. Note: not all addons have dlls - some just contain io files.
	dllPath := method(Path with(addonPath, "_build/dll", dllName))

	//doc Addon sourcePath Returns the path of the source folder for the addon.
	sourcePath := method(Path with(addonPath, "source"))

 	ioFilesPath := method(Path with(addonPath, "io"))

	//doc Addon ioFiles Return list of io File objects for the io files in the io folder of the addon.
	ioFiles := method(
		d := Directory with(ioFilesPath)
		if(d exists not, return list())
		files := d files select(path endsWithSeq(".io"))
		files map(name) sort map(name, d fileNamed(name))
	)

	//doc Addon dependencies Returns the list of dependencies from the addon's depends file.
	dependencies := method(
		File with(Path with(addonPath, "depends")) contents split(" ")
	)

	//doc Addon loadDependencies Loads the addon's dependencies. Called from the load method.
	loadDependencies := method(
		//writeln(name, " depends on ", dependencies)
		dependencies foreach(d,
            depName := d asMutable strip
			if(Lobby getSlot(depName) == nil,
				//writeln("loading dependency ", d)
				if(AddonLoader hasAddonNamed(depName)) then(
				    AddonLoader loadAddonNamed(depName)
				) else(
				    Exception raise("Failed to load Addon " .. name .. " - Addon " .. name .. " depends on Addon " .. depName .. " but Addon " .. depName .. " cannot be found.")
				)
			)
		)
	)

	//doc Addon load Loads the addon.
	load := method(
		//writeln("ioFilesPath = ", ioFilesPath)
		Importer addSearchPath(ioFilesPath) // to avoid loops when a addon file refs another before it's loaded
			
		loadDependencies
		context := Object clone
		Protos Addons setSlot(name, context)
		Protos appendProto(context)
		//writeln(dllPath)
		if(File with(dllPath) exists,
			DynLib clone setPath(dllPath) open call("Io" .. name .. "Init", context)
		,
			// check for C files, if found then addon didn't compile
			if(Directory with(sourcePath) files size > 1,
				Exception raise("Failed to load Addon " .. name .. " - it appears that the addon exists but was not compiled. You might try running 'make " .. name .. "' in the Io source folder.")
			)
		)
		//ioFiles foreach(f, writeln("loading ", f path))
		ioFiles foreach(file, context doFile(file path))
		Importer removeSearchPath(ioFilesPath)
		Lobby getSlot(name)
	)

	//doc Addon exists Returns true if the addonPath exists, false otherwise.
	exists := method(Directory with(addonPath) exists)

	//doc Addon addonProtos Returns names of protos defined in the addon from the addon's protos file.
	addonProtos := method(
		f := File with(Path with(addonPath, "protos"))
		if(f exists, f contents split, list())
	)
)

AddonLoader := Object clone do(
    //doc Addon searchPaths Returns the list of paths to search for addons.

    searchPaths := method(
        platform := System platform asLowercase
        eerieBasePath := nil
        eerieActiveEnvPath := nil

        if(platform == "windows" or platform == "mingw",
            eerieBasePath := (System installPrefix .. "/eerie/base/addons") asOSPath
            eerieActiveEnvPath := (System installPrefix .. "/eerie/activeEnv/addons") asOSPath
            ,
            eerieBasePath := ("~/.eerie/base/addons" stringByExpandingTilde) asOSPath
            eerieActiveEnvPath := ("~/.eerie/activeEnv/addons" stringByExpandingTilde) asOSPath
        )

        return list("eerie/base/addons", "eerie/activeEnv/addons", eerieBasePath, eerieActiveEnvPath)
    )

    //doc Addon appendSearchPath(aSequence) Appends the argument to the list of search paths.
    appendSearchPath := method(p, searchPaths appendIfAbsent(p); self)

    //doc Addon addons Looks for all addons which can be found and returns them as a list of Addon objects. Caches the result the first time it is called.
    addons := method(
        searchFolders := searchPaths map(path, Directory with(path)) select(exists)
        addonFolders := searchFolders map(directories) flatten //select(isAccessible) select(v, v fileNames contains("protos") or v fileNames contains("build.io"))
        self addons := addonFolders map(f,
            Addon clone setRootPath(f path pathComponent) setName(f path lastPathComponent)
        )
        addons
    )

    //doc Addon addonFor(aName) Returns the Addon with the given name if it can be found or nil otherwise.
    addonFor := method(name,
        r := addons detect(name == name)
        if(r, return r)
        r := addons detect(addonProtos contains(name))
        r
    )

    //doc Addon hasAddonNamed(aName) Returns true if the named addon can be found, false otherwise.
    hasAddonNamed := method(name, addonFor(name) != nil)

    //doc Addon loadAddonNamed(aName) Loads the Addon with the given name if it can be found or nil otherwise.
    loadAddonNamed := method(name,
        addon := addonFor(name)
        if(addon, addon load, nil)
        Lobby getSlot(name)
    )
)
