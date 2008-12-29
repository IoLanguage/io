
TracksView := View clone do(
    protoName := "TracksView"

    tracks := method(subviews)
    delegate ::= nil
    soloTracks := 0
    
    addTrack := method(track,
	last := tracks last
	if (last, 
	    //write("last sound playPosition = ", last sound playPosition, "\n")
	    track sound setPlayPosition(last sound playPosition)
	)

	addSubview(track)
	track resizeTo(width, track height)
	organize
	
	track setDelegate(delegate)
	SongJam didAddTrack(self)
    )

    removeAllTracks := method(track, 
	while(subviews size > 0, removeTrack(subviews last))
	Collector collect
    )
        
    removeTrack := method(track, 
	SongJam willRemoveTrack(track)
	removeSubview(track)
	organize
	superview update
	glutPostRedisplay
    )

    organize := method(
	last := nil
	size setHeight(tracks size * TrackView height)
	//position set(0, superview height - height)
	position set(0, 0)
	tracks foreach(track,
	    if (last) then(
		track position set(0, last position y - last size height)
	    ) else (
		track position set(0, height - track height)
	    )
	    track size set(width, track height)
	    last = track
	)
    )
    

    draw := nil
    drawOutline := nil
    leftMouseMotion := nil

    scroll := method(scroller,
	tracks foreach(track, track setOffset(scroller value))
    )

    /* --- Song -------------------------------- */

    path ::= Path with(System launchPath, "Songs/Default.songjam")
    
    open := method(p, 
	if (p, setPath(p))
	removeAllTracks
	self doFile(path)
	tracks foreach(t, t update)
    )
    
    save := method(p,
	if (p, setPath(p))
	write("saving to path: ", path, "\n")
	file := File clone setPath(path) 
	file remove 
	file open
      file write( "SongJam setPitch( " .. SongJam pitchSlider value * 12 - 6 .. " )\n\n" )
//      file write( "SongJam setTempo( " .. Number constants e pow(
//       (SongJam tempoSlider value -.5) / (0.5 / 2 log) ) .. " )\n\n" )
      file write( "SongJam setTempo( " .. SongJam tempoSlider value * 0.5 + 0.75 .. " )\n\n" )
      tracks foreach(t, t writeToFile(file))
      file close
    )
    
    addTrackAtPath := method(path,
	if (path beginsWithSeq("/") == nil, path = Path with(System launchPath, path))
      	t := TrackView clone openFile(path)
        //t sound setPitch( (SongJam pitchSlider value * 12) - 6 )
	addTrack(t)
	t
    )

    didResize := method(
        write("didResize ", size x , " ", size y, "\n")
        if (size x != size x roundDown or size y != size y roundDown,
           self resizeTo(size x roundDown, size y roundDown)
        )
    )
)

