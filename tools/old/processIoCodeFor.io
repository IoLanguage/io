processIoCodeFor := method(path,
	d := Directory clone setPath(path)

	processed := d folderNamedCreateIfAbsent("_ioCodeProcessed")

	d folderNamed("_ioCode") filesWithExtension(".io") foreach(file,
		processed fileNamed(file name) openForUpdating truncateToSize(0) write("__noShuffling__;\n") write(file contents asMessage asString) close
	)
)
