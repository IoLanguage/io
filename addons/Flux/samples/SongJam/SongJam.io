// 2005-05

doFile(Path with(System launchPath, "SoundExtensions.io"))
doFile(Path with(System launchPath, "SButton.io"))

//Collector debugOn

black := Color clone set(0,0,0,1)
white := Color clone set(1,1,1,1)
red   := Color clone set(1,0,0,1)
green := Color clone set(0,1,0,1)
blue  := Color clone set(0,0,1,1)
black := Color clone set(0,0,0,1)
darkGray   := Color clone set(.4,.4,.4,1)
mediumGray := Color clone set(.5,.5,.5,1)
lightGray  := Color clone set(.75,.75,.75,1)
vLightGray := Color clone set(.9,.9,.9,1)
offWhite   := Color clone set(.93,.93,.93,1)
white      := Color clone set(1,1,1,1)
charcoal   := Color clone set(.345,.345,.345,1)

TracksView backgroundColor = red
TracksView outlineColor = black

WaveView do(
	outlineColor := black
	//maskColor := Color clone set(.85, .85, .85, 1)
	maskColor := Color clone set(.77, .77, .77, 1)
	maskColor := Color clone set(.6, .6, .6, 1)
	barColor := Color clone set(0,0,0,.1)
)

TrackLabel backgroundColor = charcoal
TrackLabel outlineColor = black
TrackLabel textColor := lightGray

TrackView backgroundColor = lightGray 

TrackMeter backgroundColor = white 
TrackMeter outlineColor = black 
TrackMeter textColor = mediumGray 

Screen backgroundColor := charcoal //offWhite
Screen position set( 200, 100 )
Screen setSize(800, 400)

SongJam := Object clone 
SongJam do(
    screenView := View clone do(
	size set(Screen width, Screen height)
	resizeWithSuperview 
	textures := TextureGroup clone loadGroupNamed("Screen")
	draw := method(textures draw(width, height))
    )
    screenView setNextResponder(SongJam)
    Screen addSubview(screenView)
    
    tracksClipView := ClipView clone
    tracksClipView position set(0, HScroller height)
    tracksClipView setWidth(screenView width - VScroller width) setHeight(screenView height - 120 - HScroller height)
    tracksClipView resizeWidth  = 101
    tracksClipView resizeHeight = 101
    tracksClipView backgroundColor := mediumGray
    tracksClipView drawOutline := nil
    screenView addSubview(tracksClipView)
    
    tracksView := TracksView clone
    tracksView setDelegate(SongJam)
    tracksView position set(0, HScroller height - 1)
    tracksView setWidth(screenView width - VScroller width) setHeight(screenView height - 120 - HScroller height)
    tracksView resizeWidth   = 101
    tracksView resizeHeight  = 011
    tracksClipView setDocumentView(tracksView)

    vScroller := VScroller clone
    vScroller position set(screenView width - vScroller width, 16)
    vScroller resizeWidth  = 011
    vScroller resizeHeight = 101
    vScroller size setHeight(tracksView height)
    vScroller setAction("vScrollAction")
    vScroller setActionTarget(SongJam)
    tracksClipView setScrollerDelegate(vScroller) 
    screenView addSubview(vScroller)


    addTrackAtPath := method(path,
	tracksView addTrackAtPath(path)
    )

    meterClipView := ClipView clone
    meterClipView position set(TrackLabel width, tracksClipView position y + tracksClipView size height)
    meterClipView setWidth(screenView width - TrackLabel width - 16) setHeight(23)
    meterClipView resizeWidth  := 101
    meterClipView resizeHeight := 011
    meterClipView backgroundColor = Color clone set(.3333,.3333,.3333,1)
    meterClipView drawOutline := nil
    screenView addSubview(meterClipView)

    trackMeter := TrackMeter clone
    trackMeter position set(0, 0)
    trackMeter setWidth(screenView width - TrackLabel width) setHeight(32)
    trackMeter resizeWidth  := 110
    trackMeter resizeHeight := 110
    trackMeter setActionTarget(SongJam)
    trackMeter setAction("trackMeterAction")
    meterClipView setDocumentView(trackMeter)
    trackMeterAction := method(meter,
	tracksView tracks foreach(track, track sound setPlayPosition(meter selectedVolumePosition))
    )

    hScroller := HScroller clone
    hScroller position set(TrackLabel width, 0)
    hScroller size setWidth(screenView width - TrackLabel width - vScroller width)
    hScroller resizeWidth = 101
    hScroller resizeHeight = 110
    screenView addSubview(hScroller)
    hScroller setAction("hScrollAction")
    hScroller setActionTarget(SongJam)
    meterClipView setScrollerDelegate(hScroller) 

    rewindAction := method(
      tracksView tracks foreach(track, track sound setPosition(0))    
      Screen glutPostRedisplay
    )

    rightOffset := 5

    addLoopButton := MusicButton clone
    addLoopButton position set(13, screenView height - SButton height - 55)
    addLoopButton setActionTarget(SongJam)
    addLoopButton setAction("addLoopAction")
    addLoopButton setTexturesPath("Buttons/Music/Add/")
    screenView addSubview(addLoopButton)
    
    addLoopAction := method(
	stopPlaying
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
	//write("saving...\n")
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
    
    openLoopAction := method(panel,
        item := panel browser selectedItem object
	if (item title endsWithSeq(".wav"), 
	  item downloadIfNeeded( panel )
	  addTrackAtPath(item path)
	)
    )
    
    didAddTrack := method(
	//write("SongJam didAddTrack\n")
	tracksClipView update
	hScroller doAction    
    )
    
    rewindButton := MusicButton clone
    rewindButton placeRightOf(addLoopButton, rightOffset)
    rewindButton setActionTarget(SongJam)
    rewindButton setAction("rewindAction")
    rewindButton setTexturesPath("Buttons/Music/Previous/")
    screenView addSubview(rewindButton)
    
    playButton := PlayButton clone
    playButton placeRightOf(rewindButton, rightOffset)
    playButton setActionTarget(SongJam)
    playButton setAction("togglePlayPause")
    screenView addSubview(playButton)

    recordVocalsButton := MusicButton clone
    recordVocalsButton placeRightOf(playButton, rightOffset)
    recordVocalsButton setActionTarget(SongJam)
    recordVocalsButton setAction("recordVocalsAction")
    recordVocalsButton setTexturesPath("Buttons/Music/Record/")
    screenView addSubview(recordVocalsButton)
    
    disableAllButtons := method(
	//write("disabling buttons\n")
	addLoopButton disable
	rewindButton disable
	playButton disable
	//recordVocalsButton disable
	newSongButton disable
	openSongButton disable
	saveSongButton disable
	//helpButton disable
	//exitButton disable
    )

    enableAllButtons := method(
	//write("enabling buttons\n")
	addLoopButton enable
	rewindButton enable
	playButton enable
	//recordVocalsButton enable
	newSongButton enable
	openSongButton enable
	saveSongButton enable
	//helpButton enable
	//exitButton enable
    )
    
    // --- Record Vocals ---------------------------------------
    
    vocalsRecorder := nil
    
    recordVocalsAction := method(
        if( tempoLabel title fromTo( 7, tempoLabel title size ) asNumber != 120, return )
        if( tracksView tracks size == 0, return )
	if (vocalsRecorder, stopRecordingVocals; return)
    	w := SavePanel clone
	w setTitle("Record Vocals")
	w setActionTarget(self)
	w setAction("doRecordVocalsAction")
	//w resizeTo(400, 300)
	w setPath(Path with(System launchPath, "Sounds/Volume One/Vocals"))
	w setFileName("vocals.wav")
	w open
    )
    
    stopRecordingVocals := method(
        ///vocalsRecorder @stop
	vocalsRecorder stop
	self vocalsRecorder := nil
    )
    
    doRecordVocalsAction := method(savePanel, 	
	stopPlaying
	rewindAction
	disableAllButtons
	
	self vocalsRecorder := Object clone do(
	    chunk := 0
	    total := 0
	    bytesPerRecording := 4 * 60 * 44100 * 2 * 4
	    buf := Buffer clone sizeTo(bytesPerRecording) setSize(0)
	    //AudioDevice read
	    write := method(b,
	      //if (chunk > 1, 
	        buf appendSeq(AudioDevice read)
	      //)
	      chunk = chunk + 1
	      AudioDevice write(b)
	      if (buf size > bytesPerRecording,
		    buf setSize(bytesPerRecording)
		    save
	      )
	    )
	    save := method(
		    AudioMixer setAudioDevice(AudioDevice)
		    chunkSize := Sound bytesPerBeat * 4

                    // Try to sync vocals with song		    
		    b := buf fromTo( 25000 * 4, buf size - 1 )
		    b setSize( b size + 25000 * 4 )
		    for( i, b size, b size + 25000 * 4,
		      b atPut( i, 0 )
                    )
		    //b := buf
		    
		    //write("chunks before = ", buf size / chunkSize, "\n")
		    b setSize((b size / chunkSize) roundDown * chunkSize)
		    //write("chunks after = ", buf size / chunkSize, "\n")
		    s := Sound clone
		    s setSampleRate(44100)
		    s setChannels(2)
		    s setBuffer(b)
		    s setPath(path)
		    s write
		    s := nil
                    ///delegate @finishedVocalsRecording(self)
		    delegate finishedVocalsRecording(self)
	    )
	    stop := method(
		save
	    )
	)
	vocalsRecorder path := savePanel savePath
	vocalsRecorder delegate := self
	AudioMixer setAudioDevice(vocalsRecorder)
        AudioDevice read // clear read buffer?
	startPlaying
	recordVocalsButton select
    )
    
    finishedVocalsRecording := method(recorder,
	recordVocalsButton unselect
	enableAllButtons
	addTrackAtPath(recorder path)
	stopPlaying
	rewindAction
	Collector collect
    )
        
    // --- Record MP3 ---------------------------------------
    
    isRecording := nil
    recorder := nil
    
    recordAction := method(
	write("isRecording = ", isRecording, "\n")
	if (isRecording, stopRecording; return)
    	w := SavePanel clone
	w setTitle("Record wav")
	w setActionTarget(self)
	w setAction("doRecordAction")
	//w resizeTo(400, 300)
	w setPath(User homeDirectory path)
	w setFileName("my track.wav")
	w open
    )
    
    stopRecording := method(
	recorder stop
	///recorder @stop
	self recorder := nil
	isRecording = nil
    )
    
    songLengthInBytes := method(
	max := 0
    	tracksView tracks foreach(i, track,
	    lv := track sound lastVolume 
	    if(lv > max, max = lv)
	)
	max * Sound bytesPerBeat
    )
    
    doRecordAction := method(savePanel, 	
	isRecording = 1
	stopPlaying
	rewindAction
	disableAllButtons
	
	self recorder := Object clone 
	
	totalBytesPerRecording := 4 * 60 * 44100 * 2 * 4
	write("total = ", totalBytesPerRecording, "\n")
	
	recorder bytesPerRecording := songLengthInBytes 
	write("bytes = ", recorder bytesPerRecording, "\n")
	
	recorder do(
	    total := 0
	    buf := Buffer clone sizeTo(bytesPerRecording)
	    nonBlockingWrite := method(b,
		buf appendSeq(b)
		AudioDevice nonBlockingWrite(b)
		if (buf size > bytesPerRecording,
		    buf setSize(bytesPerRecording)
		    save
		)
	    )
	    write := method(b,
		buf appendSeq(b)
		AudioDevice write(b)
		if (buf size > bytesPerRecording,
		    buf setSize(bytesPerRecording)
		    save
		)
	    )
	    save := method(
		AudioMixer setAudioDevice(AudioDevice)
		s := Sound clone
		s setSampleRate(44100)
		s setChannels(2)
		s setBuffer(buf)
		s setPath(path)
		s write
		s := nil
		///delegate @finishedRecording
		delegate finishedRecording
		yield	    
	    )
	    stop := method(
		save
	    )
	)
	recorder path := savePanel savePath
	recorder delegate := self
	AudioMixer setAudioDevice(recorder)
	startPlaying
	//recordButton select
    )
    
    finishedRecording := method(
	write("finishedRecording\n")
	isRecording = nil
	write("isRecording = ", isRecording, "\n")
	//recordButton unselect
	enableAllButtons
	stopPlaying
	rewindAction
	Collector collect
    )
    
    // --------------------------------------------------

    drawButton := SButton clone
    drawButton setTitle("Draw")
    drawButton placeRightOf(recordVocalsButton, 30)
    drawButton position setY(drawButton position y + 7)
    drawButton setActionTarget(SongJam)
    drawButton setAction("drawAction")
    screenView addSubview(drawButton)
    
    drawAction := method(button,
	tracksView tracks foreach(track, track waveView drawModeOn)
	SongJam eraseButton enable
	button disable
    )

    eraseButton := SButton clone do(
	setTitle("Erase")
	setActionTarget(SongJam)
	setAction("eraseAction")
    )
    eraseButton placeRightOf(drawButton, rightOffset)
    screenView addSubview(eraseButton)
    
    eraseAction := method(button,
	tracksView tracks foreach(track, track waveView eraseModeOn)
	SongJam drawButton enable
	button disable
    )

    volumeSlider := VolumeSlider clone do(
	size setWidth(110) setHeight(16)
	setProportion(.2)
	resizeWidth  = 011
	resizeHeight = 011
	setValue(1)
    )    
    //volumeSlider position setX(screenView width - 351)
    //volumeSlider position setY(recordVocalsButton position y + 20)
    volumeSlider position setX(55)
    volumeSlider position setY(-1)

    volumeSlider setAction("volumeAction")
    volumeSlider setActionTarget(SongJam)
    volumeAction := method(slider,
	AudioMixer setVolume(slider value)
    )

//    screenView addSubview(volumeSlider)

    pitchSlider := HSlider clone do(
	size setWidth(150) setHeight(16)
	setProportion(.1)
	resizeWidth  = 011
	resizeHeight = 011
	setValue(.5)
    )    
    pitchSlider position setX(screenView width - 161)
    pitchSlider position setY(recordVocalsButton position y + 20)

//    pitchSlider setReleaseAction( "pitchReleaseAction", SongJam )
    pitchSlider setActionTarget( SongJam )
    pitchSlider setAction( "pitchAction", SongJam )
    pitchSlider setReleaseAction( "pitchReleaseAction", SongJam )
    pitchAction := method( slider,
      write( ((slider value * 12) - 6) roundUp, "\n" )
//      slider setValue( ((slider value * 12) roundUp) / 12 )
      //TracksView sound setPitch( (slider value * 24) - 12 )
//      tracksView tracks foreach( track,
//        track sound setPitch( (slider value * 12) - 6 )
//      )
      setPitchLabel( (slider value * 12) roundUp - 6 )
    )

    pitchReleaseAction := method( slider,
      setPitch( (slider value * 12) roundUp - 6 )
    )

    screenView addSubview(pitchSlider)

    setPitchLabel := method ( pitch,
      AudioMixer setPitchSemiTones( pitch )

      if( pitch < 0) then(
        pitchLabel setTitle( "Pitch:   " .. pitch asString )
      ) elseif( pitch > 0) then(
        pitchLabel setTitle( "Pitch:  +" .. pitch asString )
      ) else (
        pitchLabel setTitle( "Pitch:    " .. pitch asString )
      )      
    )

    setPitch := method ( pitch,
      pitchSlider setValue( pitch / 12 + .5 )
      AudioMixer setPitchSemiTones( pitch )

      if( pitch < 0) then(
        pitchLabel setTitle( "Pitch:   " .. pitch asString )
      ) elseif( pitch > 0) then(
        pitchLabel setTitle( "Pitch:  +" .. pitch asString )
      ) else (
        pitchLabel setTitle( "Pitch:    " .. pitch asString )
      )      
    )

    setTempo := method ( tempo,
      //tempoSlider setValue( tempo log * (0.5 / 2 log) + .5 )
      tempoSlider setValue( (tempo - 0.75) * 2 )
      AudioMixer setTempo( (tempo * 100) roundUp / 100 )
      
      tempoLabel setTitle( "Tempo: " .. ((120 * tempo) roundUp) asString )
      writeln( "tempo = ", (tempo * 100) roundUp / 100 )
    )

    tempoSlider := HSlider clone do(
	size setWidth(150) setHeight(16)
	setProportion(.1)
	resizeWidth  = 011
	resizeHeight = 011
	setValue(.5)
    )    
    tempoSlider position setX(screenView width - 161)
    tempoSlider position setY(recordVocalsButton position y + 0)

//    tempoSlider setReleaseAction( "tempoReleaseAction", SongJam )
    tempoSlider setActionTarget( SongJam )
    tempoSlider setAction( "tempoReleaseAction", SongJam )
    tempoReleaseAction := method( slider,
	//v := ((slider value * 1.5) + .5)
	//v = ((v * 10) floor / 10)
	//if (v < .1, v := .1)
//      slider setValue( ((slider value * 12) roundUp) / 12 )
//      v := Number constants e pow( (slider value -.5) / (0.5 / 2 log) )
      v := slider value * 0.5 + 0.75
      writeln( "tempoReleaseAction ", (v * 100) roundUp / 100 )
      //AudioMixer setTempo(v)
      setTempo( v )
      //AudioMixer setSampleRate(v*44100)
    )

    screenView addSubview(tempoSlider)
    
    pitchLabel := Label clone do(
	resizeWidth = 011
	resizeHeight = 011
	setTitle("Pitch:  0")
	textColor = Color clone set(1,1,1,1)
    )
    pitchLabel setPosition(pitchSlider position + Point clone set(-65, 4, 0))
    screenView addSubview(pitchLabel)

    tempoLabel := Label clone do(
	resizeWidth = 011
	resizeHeight = 011
	setTitle("Tempo: 120")
	textColor = Color clone set(1,1,1,1)
    )
    tempoLabel setPosition(tempoSlider position + Point clone set(-79, 4, 0))
    screenView addSubview(tempoLabel)
        
    AudioDevice openForReadingAndWriting

    isPlaying := nil

    Scheduler setSleepInterval(0.001)
    AudioMixer setSamplesPerBuffer(Sound samplesPerBeat /4) // 20000
    AudioMixer @@start

    stopPlaying := method(
	playButton turnOff
	write("stopPlaying\n")
    	//tracksView tracks foreach(track, AudioMixer removeSound(track sound))
	AudioMixer removeAllSounds
	isPlaying = nil
	yield
    )
    

    willRemoveTrack := method(track,
	AudioMixer removeSound(track sound)
    )
    
    hasOffset := method(tracksView tracks last sound ?offset)
    
    startPlaying := method(
	write("startPlaying\n")
	playButton turnOn
	if (isPlaying, return)
	
	lastSound := tracksView tracks last ?(sound) 
/*	
	if (lastSound ?offset) then(

            lastSound loopingOn
	    AudioMixer addSound(lastSound)
	
	    tracksView tracks foreach(track,
		sound := track sound
		sound loopingOn
		if (sound != lastSound) then(
		    AudioMixer addSoundOnSampleOfSound(sound, lastSound offset, lastSound)
		) else (
		    AudioMixer addSound(sound)
		)
	    )
	
	) else (
*/
	    tracksView tracks foreach(track,
		sound := track sound
		sound loopingOn
		AudioMixer addSound(sound)
	    )
	    	
//	)
	Screen addTimerTargetWithDelay(self, 0)
	isPlaying = 1
    )
    
    togglePlayPause := method(
	if (isPlaying, stopPlaying, startPlaying)
    )

    framesPerSecond := 10

    timer := method(
	yield
	//write("     timer\n")
	if (isPlaying or isTestPlaying, 
            Screen addTimerTargetWithDelay(self, 1/framesPerSecond)
        )
	Screen glutPostRedisplay
    )

    hScrollAction := method(scroller,
	tracksView scroll(scroller)
	meterClipView setXOffset(scroller value)
    )
    
    vScrollAction := method(scroller,
	tracksClipView setYOffset(scroller value)
    )
    
    setPlayLengthInBars := method(beats,
	tracksView tracks foreach(track, track waveView setPlayLengthInBars(beats))
	trackMeter setPlayLengthInBars(beats)
    )

    setPlayLengthInBars(Sound defaultPlayLengthInBars)
    
    newSongButton := SButton clone do(
	setTitle("New")
	setAction("newSongAction")
	setActionTarget(SongJam)
    )
    newSongButton position set(13, screenView height - 37)
    screenView addSubview(newSongButton)

    newSongAction := method(
	w := AlertPanel clone
	w setTitle("New Song")
	w setMessage("Are you sure you want to create a new song?")
	w setActionTarget(self)
	w setAction("doNewSongAction")
	
	w resizeTo(340, 160)
	w open
	//doNewSongAction
    )
    
    doNewSongAction := method(
	stopPlaying
	tracksView removeAllTracks
	setPitch( 0 )
	setTempo( 1 )
    )

    openSongButton := SButton clone do(
	setTitle("Open")
	setAction("openSongAction")
	setActionTarget(SongJam)
    )
    openSongButton placeRightOf(newSongButton, rightOffset)
    screenView addSubview(openSongButton)

    openSongAction := method(
	stopPlaying
	w := OpenPanel clone
	w setTitle("Open Song")
	w setActionTarget(self)
	w setAction("doOpenSongAction")
	w resizeTo(400, 300)
	w setPath(Path with(System launchPath, "Songs"))
	w open
	
	//tracksView open(Path with(System launchPath, "Songs/default.sjm"))
    )
    
    doOpenSongAction := method(savePanel, 
	openSongAtPath(savePanel openPath)
    )

    openSongAtPath := method(path, 
	write("openSongAtPath(", path, ")\n")
	tracksView open(path)
    )    
    
    // ------------------------

    saveSongButton := SButton clone do(
	setTitle("Save")
	setAction("saveSongAction")
	setActionTarget(SongJam)
    )
    saveSongButton placeRightOf(openSongButton, rightOffset)
    screenView addSubview(saveSongButton)
    
    saveSongAction := method(
    	w := SavePanel clone
	w setTitle("Save Song")
	w setActionTarget(self)
	w setAction("doSaveSongAction")
	//w resizeTo(400, 300)
	w setPath(Path with(System launchPath, "Songs"))
	w setFileName(tracksView path lastPathComponent)
	w open
    )
    
    doSaveSongAction := method(savePanel, 
	p := savePanel savePath
	if (p endsWithSeq(".sjm") == nil, p = p .. ".sjm")
	write("doSaveSongAction: ", p, "\n")
	tracksView save(p)
    )
    
    saveMp3Button := SButton clone do(
	setTitle("Save as wav")
	setAction("recordAction")
	setActionTarget(SongJam)
    )
    saveMp3Button size setWidth(128)
    saveMp3Button placeRightOf(saveSongButton, rightOffset)
    screenView addSubview(saveMp3Button)

    helpButton := SButton clone do(
	setTitle("Help")
	setAction("helpAction")
	setActionTarget(SongJam)
	//textColor := Color clone set(.5,.5,.5,1)
    )
    helpButton placeRightOf(saveMp3Button, rightOffset)
    screenView addSubview(helpButton)
    
    helpAction := method(
	url := "http://www.songjam.com/index.php?page=support"
        platform := System platform

        if( platform == "Darwin") then(
			writeln("open " .. url)
			System system("open " .. url)
        ) elseif( platform == "Windows NT") then(
			System system("cmd /c start " .. url)
			//System system("explorer " .. url)
        ) elseif( platform == "Windows 9X") then(
			System system("command /c start " .. url)
        ) else (
			writeln("unknown platform ", platform)
		)
    )

    exitButton := SButton clone do(
	setTitle("Exit")
	setAction("exitAction")
	setActionTarget(SongJam)
    )
    exitButton placeRightOf(helpButton, rightOffset)
    screenView addSubview(exitButton)
    exitAction := method(System exit)

    leftButton := Bar clone
    leftButton resizeHeight := 011
    leftButton position set(0, tracksClipView height + tracksClipView position y)
    leftButton size setWidth(TrackLabel width) setHeight(23)
    screenView addSubview(leftButton)
    
    rightButton := Bar clone
    rightButton resizeHeight := 011
    rightButton resizeWidth := 011
    rightButton position set(screenView width - VScroller width - 1, tracksClipView height + tracksClipView position y)
    rightButton size setWidth(VScroller width) setHeight(23)
    screenView addSubview(rightButton)
    
    bottomLeft := MiniBar clone
    bottomLeft resizeHeight := 110
    bottomLeft position set(0,0)
    bottomLeft size setWidth(TrackLabel width) setHeight(HScroller height)
    screenView addSubview(bottomLeft)
    bottomLeft addSubview(volumeSlider)

    volumeImage := ImageView clone
    volumeImage resizeHeight := 011
    volumeImage resizeWidth := 011
    volumeImage size set(16, 16)
    volumeImage position set(20,-1)
    volumeImage setImage(Image clone open(Path with(System launchPath, "Images/Volume.png")))
    //logoButton size set(64, 64)
    //logoButton position set(screenView width - logoButton width - 5, screenView height - logoButton height)
    //logoButton setImage(Image clone open(Path with(System launchPath, "Images/logo2.png")))
    bottomLeft addSubview(volumeImage)

    
    logoButton := Button clone
    logoButton resizeHeight := 011
    logoButton resizeWidth := 011
    logoButton size set(227, 47)
    logoButton position set(screenView width - logoButton width - 5, screenView height - logoButton height - 3)
    logoButton setImage(Image clone open(Path with(System launchPath, "Images/newlogo.png")))
    //logoButton size set(64, 64)
    //logoButton position set(screenView width - logoButton width - 5, screenView height - logoButton height)
    //logoButton setImage(Image clone open(Path with(System launchPath, "Images/logo2.png")))
    screenView addSubview(logoButton)
    
    
    keyboard := method(key, x, y,
	write("SongJam key ", key, "\n")
	
	if (key asCharacter == " ", playButton do(leftMouseDown; leftMouseUp))
	if (key asCharacter == "1" key asCharacter == "\n", rewindButton do(leftMouseDown; leftMouseUp))
	if (key asCharacter == "3", recordVocalsButton do(leftMouseDown; leftMouseUp))
    )
    screenView resizeTo(screenView width+1, screenView height+1)

    isTestPlaying := nil

    
    waveViewLeftMouseDown := method(wv,
	if (AudioDevice error, write("AudioDevice error = ", AudioDevice error, "\n"))
	if (AudioDevice isActive == nil, write("AudioDevice isActive == nil! \n"))
	//write("AudioDevice streamTime = ", AudioDevice streamTime, "\n")
	if (isPlaying == nil and wv drawMode == 1, 
	    isTestPlaying = 1
	    wv testSound setVolumeFromTo(255, 0, 100)
	    //write(wv uniqueId, " Down \n")
	    //AudioMixer removeAllSounds
	    //yield
	    AudioMixer addSound(wv testSound)
	    Screen addTimerTargetWithDelay(self, 0)
	)    
    )
        
    waveViewLeftMouseUp := method(wv,
	if (isTestPlaying, 
		//write(wv uniqueId, " Up \n")
		isTestPlaying = nil
	        //AudioMixer removeSound(wv testSound)
	        AudioMixer removeAllSounds  // 05-05-2005 Mike Austin
	        yield
	)    
    )
/*    
    setVocalsOffset := method(samples,
	tracksView tracks last waveView sound offset := samples
	tracksView tracks last waveView sound offset := 11025
    )
*/    
    openSongAtPath(Path with(System launchPath, "Songs/default.sjm"))
)
    
Screen setTitle("SongJam")
Screen open

