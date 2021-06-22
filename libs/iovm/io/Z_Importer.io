Importer := Object clone do(
	//metadoc Importer description A simple search path based auto-importer.

	//doc Importer paths List of paths the proto importer will check while searching for protos to load.
	paths := method(FileImporter directories)

	//doc Importer addSearchPath(path) Add a search path to the auto importer. Relative paths are made absolute before adding.
	addSearchPath := method(p, paths appendIfAbsent(Path absolute(p) asSymbol))

	//doc Importer removeSearchPath(path) Removes a search path from the auto importer. Relative paths should be removed from the same working directory as they were added.
	removeSearchPath := method(p, paths remove(Path absolute(p) asSymbol))

	//doc Importer FileImporter An Importer for local source files.
	FileImporter := Object clone do(
		importsFrom := "file"

		directories := list("")

		import := method(protoName, originalCall,
			if(System ?launchPath, directories appendIfAbsent(System launchPath))
			if(System getEnvironmentVariable("IOIMPORT"),
				ioImportEnv := System getEnvironmentVariable("IOIMPORT")
				if(System platform == "Windows",
					ioImportEnv split(";") foreach(p,
						directories appendIfAbsent(Path absolute(p) asSymbol)
					)
				,
					ioImportEnv split(":") foreach(p,
						directories appendIfAbsent(Path absolute(p) asSymbol)
					)
				)
			)

			directories foreach(folder,
				if(tryToImportProtoFromFolder(protoName, folder, originalCall), return true)
			)
			false
		)


		//doc FileImporter ioFileSuffixes A list of valid io source file suffixes. 
		ioFileSuffixes ::= list("io", "ioe")

		//doc FileImporter tryToImportProtoFromFolder(protoName, path) Looks for the protoName with the valid ioFileSuffixes and calls importPath if found.
		tryToImportProtoFromFolder := method(protoName, folder, originalCall,
			importedFrom := Path absolute(originalCall message label) asMutable lowercase

			ioFileSuffixes foreach(suffix,
				path := Path with(folder, protoName .. "." .. suffix) asSymbol
				normalized := Path absolute(path) asMutable lowercase

				// skip file in which 'import' was called
				if(normalized == importedFrom,
					continue
				)

				//writeln("looking for ", path)
				if(File with(path) exists,
					return importPath(path)
				)
			)
			false
		)

		key := "deafultKey"

		//doc FileImporter importPath(path) Performs Lobby doFile(path). Can override to deal with other formats.
		importPath := method(path,
			//writeln("importing: ", path)
			if(path endsWithSeq("ioe"),
				Lobby doString(decryptSourceFile(path), path)
				didLoadPath(path)
				return true
			)
			Lobby doFile(path)
			didLoadPath(path)
			return true
		)

		decryptSourceFile := method(path,
			//writeln("decrypting " .. path)
			return Blowfish clone setKey(key) decrypt(File with(path) contents)
		)

		encryptSourceFile := method(path,
			//writeln("encrypting " .. path)
			es := Blowfish clone setKey(key) encrypt(File with(path) contents)
			File with(path beforeSeq(".io") .. ".ioe") setContents(es)
		)

		didLoadPath := method(path,
			//writeln("FileImporter didLoadPath ", path)
			//encryptSourceFile(path)
			//File with(path) remove
			nil
		)
	)


	//doc Importer FolderImporter An Importer for objects laid out as folders with files as methods.
	FolderImporter := Object clone do(
		importsFrom := "folder"

		import := method(protoName,
			//writeln("FolderImporter import(", protoName, ")")
			if(hasAddon := AddonLoader hasAddonNamed(protoName),
				AddonLoader loadAddonNamed(protoName)
			)
			hasAddon
		)
	)

	//doc Importer AddonImporter An Importer for addon modules.
	AddonImporter := Object clone do(
		importsFrom := "dll"

		import := method(protoName,
			//writeln("AddonImporter import(", protoName, ")")
			if(hasAddon := AddonLoader hasAddonNamed(protoName),
				AddonLoader loadAddonNamed(protoName)
			)
			hasAddon
		)
	)


	//doc Importer importers List of Importer objects.
	importers := list(FileImporter, FolderImporter, AddonImporter)

	//doc Importer import(originalCallMessage) Imports an object or addon for the given Message.
	import := method(originalCall,
		protoName := originalCall message name
		//writeln("Importer looking for '", protoName, "'")

		if(protoName at(0) isUppercase and(importer := importers detect(import(protoName, originalCall))),
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
