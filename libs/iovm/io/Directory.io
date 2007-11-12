
Directory do(
    docSlot("with(aPath)", "Returns a new instance with the provided path.")
    with := method(path, self clone setPath(path))

	createIfAbsent := method(if(self exists not, self create); self)

    parentDirectory := method(
		if(path == ".", return nil)
		p := self path pathComponent	
		if(p == "", p = ".")
		Directory clone setPath(p)
    )

    folders := method(
		items select(item, item type ==("Directory") and(item name != ".") and(item name != ".."))
    )

    isAccessible := method(
		r := true
		e := try(items)
		e catch(Exception, r = false)
		r
    )

    parents := method(
		list := List clone
		d := self
		while(d = d parentDirectory, list append(d))
		list reverse
    )

    accessibleParents := method(parents select(isAccessible))
    
    files := method(items select(type == "File"))
    
    fileNames := method(files mapInPlace(name))
    
    fileNamed := method(name,
		//files detect(i, v, v name == name)
		File clone setPath(Path with(path, name))
    )

    filesWithExtension := method(ext,
		if(ext containsSeq(".") not, ext = "." .. ext)
		files select(f, f name endsWithSeq(ext))
    )

    folderNamed := method(name,
		folders detect(i, v, v name == name)
    )

    remove := method(
        File clone setPath(self path) remove
    )

    folderNamedCreateIfAbsent := method(name,
		f := folderNamed(name)
		if(f, return f)
		createSubdirectory(name)
		folderNamed(name)
    )

    createFileNamed := method(name,
		f := fileNamed(name)
		if(f, return f)
		return File clone setPath(Path with(path, name))
    )

	moveTo := method(path,
		File with(self path) moveTo(path)
		self
	)
)


