
SJItem := Object clone do(
  path ::= "/"

  init := method(
    self items := nil
  )

  title := method(path lastPathComponent)

  hasNodeNamed := method(name, 
    items detect(i, v, v title == name)
  )

  nodePath := method(path asMutable removePrefix(rootPath) removePrefix("/"))
  
  rootPath := method(Path with(System launchPath, "Sounds"))
  
  subitems := method(
    if (items, return items)
    self items := List clone
    if (Directory clone setPath(path) exists == nil, return items)

    Directory clone setPath(path) items foreach(item,
      if (item path lastPathComponent beginsWithSeq(".") == nil,
        items append(SJItem clone setPath(item path))
      )
    )

    if (SongJamNode isLoggedIn,
	write("nodePath = [ '", nodePath, "' ", nodePath type, "]\n")
	SongJamNode clone setPath(nodePath) subitems foreach(node,
	  write("node path: ", node path, "\n")
	  if (hasNodeNamed(node title) == nil,
	    filePath := Path with(rootPath, node path)
	    if (filePath endsWithSeq(".wav")) then(
	      item := SJItem clone setPath(filePath)
	      items append(item)
	    ) else (
	      Directory clone setPath(filePath) create
	      items append(SJItem clone setPath(filePath))
	    )
	  )
	)
    )
    
    items
  )

  hasSubitems := method(
    d := Directory clone setPath(path)
    if (d exists, d hasSubitems, nil)
  )
  
      imagePath := method( path,
	m := nil
	if (path contains("Bass"), m = "Bass.png")
	if (path contains("Drum"), m = "Drum.png")
	if (path contains("Guitar"), m = "Guitar.png")
    	if (path contains("Full Band"), m = "Full Band.png")
	if (path contains("FX"), m = "FX.png")
	if (path contains("Horns"), m = "Horns.png")
	if (path contains("Keyboard"), m = "Keyboard.png")
	if (path contains("Orchestra"), m = "Orchestra.png")
	if (path contains("Percussion"), m = "Percussion.png")
	if (path contains("Vocals"), m = "Vocals.png")
	if (path contains("World"), m = "World.png")
	if (m, return Path with(System launchPath, "Images/Instruments", m))
	nil
    )

  downloadIfNeeded := method( panel,
    write("path = ", path, "\n")
    f := File clone setPath(path)
    if (f exists == nil,
      node := SongJamNode clone setPath( nodePath )
      progressPanel := ProgressPanel clone
      progressPanel size set( 200, 32 )
      imageButton := ImageView clone
      imageButton position set( 100 - 16, 0 )
      imageButton size set( 32, 32 )
      imageButton scale set( .25, .25, 1 )
      imageButton setImage( Image clone open( imagePath( nodePath ) ) )
      progressPanel addSubview( imageButton )
      Screen addSubview( progressPanel )
      progressPanel open
      
      data := node dataWithProgress(
        block( bytes, total,
          write( "progress: ", bytes / total, "\n" )
          progressPanel progress = bytes / total
          Screen display
        )
      )
      progressPanel close
      f openForUpdating write( data )
      f close
    )
    return f
  )
)

