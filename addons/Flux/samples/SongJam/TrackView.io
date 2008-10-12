
TrackView := View clone do(
    protoName := "TrackView"
    position set(100, 100)
    size set(Screen width, 54)
    clippingOn
    resizeWidth = 101
    resizeHeight = 011
    
    delegate := nil
    setDelegate := method(d, 
	delegate = d
	waveView setDelegate(d)
	trackLabel setDelegate(self)
    )

    //backgroundColor := Color clone set(0, 1, 0, 1)

    resizeWidthWithSuperview
    labelWidth := 200
    sampleOffset := 0

    setOffset := method(v, waveClipView setXOffset(v))

    init := method(
	resend

	self trackLabel := TrackLabel clone
	trackLabel size set(TrackLabel width, height)
	trackLabel position set(0, 0)
	trackLabel aappendProto(self)
	addSubview(trackLabel)

	self waveClipView := ClipView clone do(
	    position set(TrackLabel width, 0)
	    resizeWidth = 101
	    resizeHeight = 110
	    draw := method(Nop)
	)
	w := width - TrackLabel width
	waveClipView size set(w, height)
	waveClipView drawOutline := nil
	//write(" width = ",  width, "\n")
	//write(" TrackLabel width = ",  TrackLabel width, "\n")
	//write(" waveClipView width = ", waveClipView width, "\n\n")
	addSubview(waveClipView)

	self waveView := WaveView clone
	waveView appendProto(self)
	waveView position set(0, 0)
	waveView size setHeight(height)
	waveClipView setDocumentView(waveView)
    )

    imagePath := method(
        path := waveView sound path
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

    openFile := method(path,
	sound := Sound clone open(path)

	waveView setSound(sound)
	trackLabel setTitle(sound path fileName)
	
	//imagePath := Path with(sound path pathComponent, ".dir.png")
	write("imagePath = ", imagePath, "\n")
	//if (Directory exists(imagePath),
	  //write("exists\n")
	try(
	  trackLabel setImage(Image clone open(imagePath))
	)
	//)
	self
    )

    sound := method(waveView sound)
    
    closeAction := method(
	superview removeTrack(self)
    )
    
    volumeAction := method(slider,
	v := slider value
	//write("sound setVolume(", v, ")\n")
	sound setVolume(v)
	waveView setVolume(v)
    )

    muteAction := method( button,
        if( button superview soloButton isChecked, return )

        if( button isChecked) then(
            sound setVolume( 0 )
            waveView setVolume( 0 )
        ) else (
            sound setVolume( button superview volumeSlider value )
            waveView setVolume( sound volume )
        )
    )

    soloAction := method( button,
        if( button isChecked) then(
            superview soloTracks = superview soloTracks + 1
        ) else (
            superview soloTracks = superview soloTracks - 1
        )
        if( superview soloTracks > 0) then(
            superview tracks foreach( track,
                if( track trackLabel soloButton isChecked) then(
                    track sound setVolume( track trackLabel volumeSlider value )
                    track waveView setVolume( track sound volume )
                ) else (
                    track sound setVolume( 0 )
                    track waveView setVolume( 0 )
                )
            )
        ) else (
            superview tracks foreach( track,
                track muteAction( track trackLabel muteButton )
            )
        )
    )

    drawOutline := nil
    draw := nil
    
    writeToFile := method(file,
	p := sound path removePrefix(System launchPath) removePrefix("/")
	file write("track := addTrackAtPath(\"" .. p .. "\")\n ")
	file write("track sound setVolumesString(\"" .. sound volumesString .. "\") ")
	file write("setVolume(" .. sound volume .. ")\n ")
	file write("\n")
    )
    
    playTestSound := method(
	delegate waveViewLeftMouseDown(waveView)
    )
    stopTestSound := method(
	delegate waveViewLeftMouseUp(waveView)
    )
    
    XchooseNewLoop := method(
	delegate stopPlaying
	w := OpenPanel clone
	w setTitle("Add Loop")
	w setActionTarget(self)
	w setAction("newLoopAction")
	w resizeTo(400, 300)
	w setPath(Path with(System launchPath, "Sounds"))
	w open
    )
    
    XnewLoopAction := method(panel,
	openFile(panel openPath)
    )

    chooseNewLoop := method(
	SongJam stopPlaying
	write("SongJamNode isLoggedIn = ", SongJamNode isLoggedIn, "\n")
	if (SongJamNode isLoggedIn, return browseLoops)
	
	username := Preferences map at("username")
	password := Preferences map at("password")

	w := LoginPanel clone
	w setTitle("SongJam Login")
	if (username and password, 
	  write("username = [", username, "] ", username type, "\n")
	  w usernameTextField setString(username)
	  w passwordTextField setString(password)
	  w usernameTextField cursorPosition = 0
	  //w usernameTextField endCursorPosition = username find( "@" )
	  w usernameTextField endCursorPosition = username size
	  Screen setFirstResponder( w usernameTextField )
	)
	
	w okButton setActionTarget(self) setAction("loginDoneAction")
	w cancelButton setTitle("Use Default Loops")
	w cancelButton setActionTarget(self) setAction("cancelLoopAction")
	w open
    )

    loginDoneAction := method(button,
        panel := button superview
        un := panel username asMutable rstrip(" ") rstrip("\t") asSymbol
	//un foreach(i, c, write(c asCharacter, " = ", c, "\n"))
	//write("panel username = [", un, "]\n")
	//write("panel password = [", panel password, "]\n")
	SongJamNode setUsername(un)
	SongJamNode setPassword(panel password)

	Preferences map atPut("username", SongJamNode username)
	Preferences map atPut("password", SongJamNode password)
	write("saving...\n")
	Preferences save
		
	SongJamNode login
	
	write("SongJamNode username = ", SongJamNode username, "\n")
	write("SongJamNode password = ", SongJamNode password, "\n")
	write("SongJamNode sessionKey = [", SongJamNode sessionKey, "]\n")
	if (SongJamNode isLoggedIn == nil,
	  panel shake 
	  panel setTitle("Invalid Login")
	  URL clone setURL("https://www.songjam.com/index.php?page=joinform") openOnDesktop
	  return
	)
	panel close
	browseLoops
    )

    cancelLoopAction := method(button,
        panel := button superview
        panel close
        write("cancelLoopAction\n")
	browseLoops
    )

    browseLoops := method(	
	w := LoopPanel clone
	w setTitle("Add Loop")
	w setActionTarget(self)
	w setAction("openLoopAction")
	w resizeTo(210*2+2, 300)
	w setPath(Path with(System launchPath, "Sounds"))
	w setRootItem(SJItem clone setPath(Path with(System launchPath, "Sounds")))

	w open    
    )

    openLoopAction := method(panel,
        item := panel browser selectedItem object
	if (item title endsWithSeq(".wav"), 
	  item downloadIfNeeded( panel )
	  //addTrackAtPath(item path)
	  openFile(item path)
	)
    )

    update := method(trackLabel update)
    
)

