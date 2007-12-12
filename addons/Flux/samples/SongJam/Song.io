
Song := Object clone do(
    tracks := List clone
    
    removeAllTracks := method(
	tracks empty
    )
    
    newSlot("path", Path with(launchPath, "Songs/Default.songjam"))
    
    open := method(
	removeAllTracks
	context := Object clone
	context song := self 
	context doFile(path)
    )
    
    save := method(
	file := File clone setPath(path) open
	tracks foreach(t, t writeToFile(file))
    )
    
    addTrackAtPath := method(path,
      
    )
)

