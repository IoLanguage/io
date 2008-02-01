Importer := Object clone do(
	//metadoc Importer description A simple search path based auto-importer.

	//doc Importer paths List of paths the proto importer will check while searching for protos to load.
	paths := method(FileImporter folders)

	//doc Importer addSearchPath(path) Add a search path to the auto importer. Relative paths are made absolute before adding.
	addSearchPath    := method(p, paths appendIfAbsent(Path absolute(p) asSymbol))

	//doc Importer removeSearchPath(path) Removes a search path from the auto importer. Relative paths should be removed from the same working directory as they were added.
	removeSearchPath := method(p, paths remove(Path absolute(p) asSymbol))

	//doc Importer FileImporter An Importer for local source files.
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

	//doc Importer AddonImporter An Importer for addon modules.
	AddonImporter := Object clone do(
		importsFrom := "dll"

		import := method(protoName,
			if(hasAddon := AddonLoader hasAddonNamed(protoName),
				AddonLoader loadAddonNamed(protoName)
			)
			hasAddon
		)
	)

	//doc Importer importers List of Importer objects.
	importers := list(FileImporter, AddonImporter)

	//doc Importer import(originalCallMessage) Imports an object or addon for the given Message.
	import := method(originalCall,
		protoName := originalCall message name

		if(protoName at(0) isUppercase and(importer := importers detect(import(protoName))),
			if(Lobby hasSlot(protoName) not,
				Exception raiseFrom(originalCall, "Importer slot '" .. protoName .. "' missing after " .. importer importsFrom .. " load")
			)
			Lobby getSlot(protoName)
		,
			targetType := originalCall target type
			Exception raiseFrom(originalCall, targetType .. " does not respond to '" .. protoName .. "'")
		)
	)

	//doc Importer autoImportingForward A forward method implementation placed in the Lobby when Importing is turned on.
	autoImportingForward := method(
		Importer import(call)
	)

	//doc Importer turnOn Turns on the Importer. Returns self.
	turnOn := method(
		Lobby forward := self getSlot("autoImportingForward")
		self
	)

	//doc Importer turnOff Turns off the Importer. Returns self.
	turnOff := method(
		Lobby removeSlot("forward")
		self
	)

	// Auto Importer is on by default
	turnOn
)

