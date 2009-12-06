
Directory do(
	//doc Directory with(aPath) Returns a new instance with the provided path.
	with := method(path, self clone setPath(path))

	//doc Directory recursiveFilesOfTypes(suffixes) Returns a list containing all files in the directory or any subdirectory that has any of the specified suffixes.
	recursiveFilesOfTypes := method(types, aList,
		if(aList == nil, aList := List clone)
		aList appendSeq(files select(file, types containsSeq(file path pathExtension)))
		directories foreach(recursiveFilesOfTypes(types, aList))
		aList
	)

	//doc Directory recursiVeFilesOftypes(suffixes) Deprecated. Alias for recursiveFilesOfTypes(suffixes).
	recursiVeFilesOfTypes := method(types, aList,
		deprecatedWarning("recursiveFilesOfTypes")
		recursiveFilesOfTypes(types, aList)
	)

	//doc Directory createIfAbsent Creates the directory (and any missing ancestor directories) if they don't exist. Returns self.
	createIfAbsent := method(
		if(self exists not, 
			if(parentDirectory and parentDirectory exists not, parentDirectory createIfAbsent)
			self create
		) 
		self
	)

	//doc Directory parentDirectory Returns the parent directory object or nil if there is no parent directory.
	parentDirectory := method(
		if(path == ".", return nil)
		p := self path pathComponent
		if(p == "", p = ".")
		Directory clone setPath(p)
	)

	//doc Directory directories Returns a list of subdirectories in the receiver.
	directories := method(
		items select(item, item type ==("Directory") and(item name != ".") and(item name != ".."))
	)
	
	//doc Directory folders Deprecated. Replaced by directories().
	folders := method(
		deprecatedWarning("directories()")
		directories
	)

	//doc Directory isAccessible Returns true if the directory can be accessed, false otherwise.
	isAccessible := method(
		r := true
		e := try(items)
		e catch(Exception, r = false)
		r
	)

	//doc Directory ancestorDirectories Returns a list of ancestor directories.
	ancestorDirectories := method(
		list := List clone
		d := self
		while(d = d parentDirectory, list append(d))
		list reverseInPlace
	)

	//doc Directory accessibleAncestors Returns a list of accessizble ancestor directories.
	accessibleAncestors := method(ancestorDirectories select(isAccessible))
	
	//doc Directory parents Deprecated. Replaced by ancestorDirectories.
	parents := method(
		deprecatedWarning("ancestorDirectories")
		ancestorDirectories
	)
	
	//doc Directory accessibleParents Deprecated. Replaced by accessibleAncestors.
	accessibleParents := method(
		deprecatedWarning("accessibleAncestors")
		accessibleAncestors
	)
	
	//doc Directory files Returns a list of File objects for the files in the Directory (no directories).
	files := method(items select(type == "File"))

	//doc Directory fileNames Returns a list of strings for the names of the files (no subdirectories) in the directory.
	fileNames := method(files mapInPlace(name))

	//doc Directory fileNamed(aName) Returns a File object for the specified file name whether it exists or not.
	fileNamed := method(name,
		//files detect(i, v, v name == name)
		File clone setPath(Path with(path, name))
	)

	//doc Directory filesWithExtension(aString) Returns a list of File objects for the files in the Directory (no directories) with the specified file extension.
	filesWithExtension := method(ext,
		if(ext containsSeq(".") not, ext = "." .. ext)
		files select(f, f name endsWithSeq(ext))
	)

	//doc Directory directoryNamed(aName) Returns a Directory object for the specified file name whether or not it exists.
	directoryNamed := method(name,
		Directory with(Path with(path, name))
		//directories detect(i, v, v name == name)
	)
	
	//doc Directory folderNamed(aName) Deprecated. Replaced by directoryNamed().
	folderNamed := method(name,
		deprecatedWarning("directoryNamed(name)")
		directoryNamed(name)
	)
	
	//doc Directory remove Removes the directory.
	remove := method(
		File clone setPath(self path) remove
	)

	//doc Directory folderNamedCreateIfAbsent(name) Deprecated. Returns a new Directory object in the directory with the specified name.
	folderNamedCreateIfAbsent := method(name,
		deprecatedWarning("directoryNamed(name) create")
		f := folderNamed(name)
		if(f, return f)
		createSubdirectory(name)
		folderNamed(name)
	)

	//doc Directory createFileNamed(name) Deprecated. Returns a new file object in the directory with the specified name.
	createFileNamed := method(name,
		deprecatedWarning("fileNamed(name) create")
		f := fileNamed(name)
		if(f, return f)
		return File clone setPath(Path with(path, name))
	)

	//doc Directory moveTo(aPath) Moves the directory to the specified path.
	moveTo := method(path,
		File with(self path) moveTo(path)
		self
	)
)
