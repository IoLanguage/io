
Sound do(
    defaultPlayLengthInBars := 120
    defaultPixelsPerVolume   := 15
    bpm := 120
    beatsPerBar := 4
    pixelsPerBeat := 10

    secondsPerBeat  := method(60 / bpm)
    beatsPerSecond  := method(bpm / 60)
    pixelsPerSample := method(pixelsPerSecond / sampleRate)
    samplesPerBeat  := method(secondsPerBeat * sampleRate)
    bytesPerBeat  := method(samplesPerBeat * 2 * 4)

    setPlayLengthInBars := method(bars,
	//write("setPlayLengthInBars ", beats, "\n")
	setPlayLength(bars * beatsPerBar * secondsPerBeat * sampleRate)
	setSamplesPerVolume(samplesPerBeat)
	setPixelsPerVolume(defaultPixelsPerVolume)
    )

    beats := method(playLength / (secondsPerBeat * sampleRate * 2))
    
    volumeCount := method(beats)
    
    copyVolumesFrom := method(s,
	setVolumesString(s volumesString)
    )
    
    setVolumesString := method(s,
	one := "1" at(0)
	s foreach(i, char, 
	    v := if (char == one, 255, 0) 
	    setVolumeFromTo(v, i, i)
	)
        self
    )

    volumesString := method(
	b := Buffer clone
	for (i, 0, volumeCount, 
	    b appendSeq(if (volumeAt(i) == 0, "0", "1"))
	)
	b asString
    )
    
    lastVolume := method(
	max := 0
	for (i, 0, volumeCount, 
	    if (volumeAt(i) != 0, max = i)
	)
	max	
    )
    
    realSetPlayPosition := getSlot("setPlayPosition")
    setPlayPosition := method(p,
	realSetPlayPosition(p)
	lastPlayPosition := p
    )
        
    lastPlayPosition := 0
    lastPositionDate := nil
    
    visiblePlayPosition := method(
	if (isPlaying == nil, return playPosition)
	p := playPosition
	now := Date clone now
	// if position is same, we guess
	if (p == lastPlayPosition) then(
//	    p = p + (2 * 44100 / SongJam framesPerSecond)

	    if (lastPositionDate == nil, lastPositionDate = now)
	    duration := now - lastPositionDate
	    p = p + (duration seconds * sampleRate)

	) else ( 
	    lastPlayPosition = p
	    lastPositionDate = now
	)
	p
    )
)

