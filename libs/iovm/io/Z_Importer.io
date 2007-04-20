Importer := Object clone do(
    docDescription("A simple search path based auto-importer.")

    docSlot("paths", "List of paths the proto importer will check while searching for protos to load.")
	paths := method(FileImporter folders)

    docSlot("addSearchPath(path)", "Add a search path to the auto importer. Relative paths are made absolute before adding.")
    addSearchPath    := method(p, paths appendIfAbsent(Path absolute(p) asSymbol))

    docSlot("removeSearchPath(path)", "Removes a search path from the auto importer. Relative paths should be removed from the same working directory as they were added.")
    removeSearchPath := method(p, paths remove(Path absolute(p) asSymbol))

	FileImporter := Object clone do(
		importsFrom := "file"

		folders := list("")

		import := method(protoName,
			if(?launchPath, folders appendIfAbsent(launchPath))

			folders foreach(folder,
				path := Path with(folder, protoName .. ".io") asSymbol
				if(File with(path) exists,
					Lobby doFile(path)
					return true
				)
			)
			false
		)
	)

	AddonImporter := Object clone do(
		importsFrom := "dll"

		import := method(protoName,
			if(hasAddon := AddonLoader hasAddonNamed(protoName),
                AddonLoader loadAddonNamed(protoName)
			)
			hasAddon
		)
	)

	importers := list(FileImporter, AddonImporter)

    docSlot("import(protoName)", "Searches for and loads protoName. Should be used from a forward method.")
    import := method(protoName, targetType,
		if(protoName at(0) isUppercase and(importer := importers detect(import(protoName))),
			if(Lobby hasSlot(protoName) not,
				Exception raise("Importer slot '" .. protoName .. "' missing after " .. importer importsFrom .. " load")
			)
			Lobby getSlot(protoName)
		,
			Exception raise(targetType .. " does not respond to '" .. protoName .. "'")
		)
    )

	autoImportingForward := method(
		Importer import(call message name, call target type)
	)

    docSlot("turnOn", "Turns on the Importer. Returns self.")
    turnOn := method(
		Lobby forward := self getSlot("autoImportingForward")
		self
	)

    docSlot("turnOff", "Turns off the Importer. Returns self.")
    turnOff := method(
		Lobby removeSlot("forward")
		self
	)

	# Auto Importer is on by default
    turnOn
)

