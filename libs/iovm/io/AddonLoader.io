Addon := Object clone do(
	docSlot("rootPath", "Returns the rootPath of the addon's folder.")
	docSlot("setRootPath(aSequence)", "Sets rootPath of the addon's folder. Returns self.")
	newSlot("rootPath")
	
	docSlot("name", "Returns the name of the addon.")
	docSlot("setName(aSequence)", "Sets the name of the addon. Returns self.")
	newSlot("name")

	docSlot("platform", "Implemented as method(System platform).")
	platform := System platform
	
	docSlot("dllSuffix", "Returns the platform specific dll suffix.")
	dllSuffix := method(
	  list("cygwin", "mingw", "windows") detect(dllPlatform, platform containsSeq(dllPlatform)) ifNonNil(return("dll"))
	  if(platform == "darwin", return "dylib")
	  "so"
	)

	docSlot("dllName", "Return the name of the dll for the addon.")
	dllName := method("libIo" .. name .. "." .. dllSuffix)

	docSlot("addonPath", "Implemented as Path with(rootPath, name).")
	addonPath := method(Path with(rootPath, name))

	docSlot("dllPath", "Returns the path to the dll for the addon. Note: not all addons have dlls - some just contain io files.")
	dllPath := method(Path with(addonPath, "_build/dll", dllName))

	docSlot("sourcePath", "Returns the path of the source folder for the addon.")
	sourcePath := method(Path with(addonPath, "source"))

	docSlot("ioFiles", "Return list of io File objects for the io files in the io folder of the addon.")
	ioFiles := method(
		d := Directory with(addonPath) folderNamed("io")
		if(d == nil, return list())
		files := d files select(path endsWithSeq(".io"))
		files map(name) sort map(name, d fileNamed(name))
	)

	docSlot("dependencies", "Returns the list of dependencies from the addon's depends file.")
	dependencies := method(
		File with(Path with(addonPath, "depends")) contents split(" ")
	)

	docSlot("loadDependencies", "Loads the addon's dependencies. Called from the load method.")
	loadDependencies := method(
		//writeln(name, " depends on ", dependencies)
		dependencies foreach(d,
			if(Lobby getSlot(d) == nil,
				//writeln("loading dependency ", d)
				AddonLoader loadAddonNamed(d)
			)
		)
	)

	docSlot("load", "Loads the addon.")
	load := method(
		//writeln("Addon ", name, " loading from ", addonPath)
		loadDependencies
		context := Object clone
		Protos Addons setSlot(name, context)
		Protos appendProto(context)
		//writeln(dllPath)
		if(File with(dllPath) exists,
			DynLib clone setPath(dllPath) open call("Io" .. name .. "Init", context)
		,
			// check for C files, if found then addon didn't compile
			if(Directory with(sourcePath) size > 1,
				Exception raise("Failed to load Addon " .. name .. " - it appears that the addon exists but was not compiled. You might try running 'make " .. name .. "' in the Io source folder.")
			)
		)
		//ioFiles foreach(f, writeln("loading ", f path))
		ioFiles foreach(file, context doFile(file path))
		Lobby getSlot(name)
	)

	docSlot("exists", "Returns true if the addonPath exists, false otherwise.")
	exists := method(Directory with(addonPath) exists)

	docSlot("addonProtos", "Returns names of protos defined in the addon from the addon's protos file.")
	addonProtos := method(
		f := File with(Path with(addonPath, "protos"))
		if(f exists, f contents split, list())
	)
)

AddonLoader := Object clone do(
	docSlot("searchPaths", "Returns the list of paths to search for addons.")
	searchPaths := list("io/addons", System installPrefix .. "/lib/io/addons")

	docSlot("appendSearchPath(aSequence)", "Appends the argument to the list of search paths.")
	appendSearchPath := method(p, searchPaths appendIfAbsent(p); self)

	docSlot("addons", "Looks for all addons which can be found and returns them as a list of Addon objects. Caches the result the first time it is called.")
	addons := method(
		searchFolders := searchPaths map(path, Directory with(path)) select(exists)
		addonFolders := searchFolders map(folders) flatten select(isAccessible) select(fileNames contains("build.io"))
		addons := addonFolders map(f, Addon clone setRootPath(f path pathComponent) setName(f path lastPathComponent))
		addons
	)

	docSlot("addonFor(aName)", "Returns the Addon with the given name if it can be found or nil otherwise.")
	addonFor := method(name,
		r := addons detect(name == name)
		if(r, return r)
		addons detect(addonProtos contains(name))
	)

	docSlot("hasAddonNamed(aName)", "Returns true if the named addon can be found, false otherwise.")
	hasAddonNamed := method(name, addonFor(name) != nil)

	docSlot("loadAddonNamed(aName)", "Loads the Addon with the given name if it can be found or nil otherwise.")
	loadAddonNamed := method(name,
		//writeln("loadAddonNamed(", name, ")")
		addon := addonFor(name)
		if(addon, addon load, nil)
		Lobby getSlot(name)
	)
)
