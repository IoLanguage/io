Responder releaseAction := nil
Responder releaseActionTarget := nil
Responder grabAction := nil
Responder grabActionTarget := nil

HSlider setGrabAction := method ( action, object,
  self grabAction := action
  self grabActionTarget := object
)

HSlider setReleaseAction := method ( action, object,
  self releaseAction := action
  self releaseActionTarget := object
)

HKnob leftMouseUp = method (
  backgroundColor = unselectedColor
  if( superview releaseAction,
    superview releaseActionTarget perform( superview releaseAction, superview )
  )
)

HKnob leftMouseDown = method (
  backgroundColor = selectedColor
  if( superview grabAction,
    superview grabActionTarget perform( superview grabAction, superview )
  )
)

VSlider := VScroller clone do(
    textures := TextureGroup clone loadGroupNamed("Slider/Vertical")
    size setWidth(textures top width)
    
    knob textures := TextureGroup clone loadGroupNamed("Slider/Vertical/Knob")
    knob maxSpace := 16
    knob setHeight(16)

    draw := method(
	textures draw(width, height)
        knob setHeight(15)
    )

    setGrabAction := method ( action, object,
      self grabAction := action
      self grabActionTarget := object
    )

    setReleaseAction := method ( action, object,
      self releaseAction := action
      self releaseActionTarget := object
    )
)

TrackLabel := View clone do(
    protoName := "TracksLabel"
    
    position set(100, 100)
    size set(220, 54)
    textColor := Color clone set(.5,.5,.5,1)
    font := method(systemFont)
    title := ""
    setTitle := method(t, label setTitle(t))
    clippingOn
    //labelImage := Image clone open(Path with(System launchPath, "Guitar.png"))
    delegate := nil
    setDelegate := method(d, 
      delegate = d
      imageButton setDelegate(d)
    )
    
    init := method(
	resend
	Window
	self closeButton := WindowCloseButton clone
	//closeButton textures = TextureGroup clone loadGroupNamed("Window/TitleBar/CloseButton")
	//closeButton size set(16, 16)
	//closeButton clippingOff
	w := closeButton width
	closeButton position set(width - 20, height - 22)
	closeButton setAction("closeAction")
	closeButton setActionTarget(self)
	addSubview(closeButton)

        ImageManager addPath( Path with(System launchPath,  "Images" ) )

        self muteButton := Button clone
        muteButton resizeHeight := 011
        muteButton setTexturesPath( "MuteButton/Normal" )
	muteButton position set(width - 40, 5)
        muteButton size set(16, 16)
        muteButton clippingOff
        muteButton setAction( "muteAction" )
        muteButton setActionTarget( self )
        muteButton isChecked := nil
        addSubview( muteButton )

        self soloButton := Button clone
        soloButton resizeHeight := 011
        soloButton setTexturesPath( "SoloButton/Normal" )
	soloButton position set(width - 20, 5)
        soloButton size set(16, 16)
        soloButton clippingOff
        soloButton setAction( "soloAction" )
        soloButton setActionTarget( self )
        soloButton isChecked := nil
        addSubview( soloButton )

	self volumeSlider := VolumeSlider clone
	volumeSlider size setWidth(110) 
	volumeSlider setProportion(.2)
	volumeSlider position set(55, 6)
	volumeSlider setAction("volumeAction")
	volumeSlider setActionTarget(self)
	volumeSlider setValue(1)
	addSubview(volumeSlider)

	self pitchSlider := VSlider clone
	pitchSlider size setWidth( 20 ) 
	pitchSlider setProportion( 0 )
	pitchSlider position set( width - 60, height - 20 )
	pitchSlider setAction( "pitchAction" )
	pitchSlider setActionTarget( self )
	pitchSlider setGrabAction( "pitchGrabAction", self )
	pitchSlider setReleaseAction( "pitchReleaseAction", self )
	pitchSlider setValue( .5 )
//	addSubview( pitchSlider )
/*
	// >
	self pitchLabel := Label clone
	pitchLabel setTitle( "pitch" )
	pitchLabel textColor = textColor
	pitchLabel size setWidth( 100 )
	pitchLabel position set( 55, 40 )
	addSubview( pitchLabel )
	// <
*/
	self label := Label clone
	label setTitle("Unnamed")
	label textColor = textColor
	label size setWidth(200) 
	label position set(55, height - 18)
	addSubview(label)
	
	self imageButton := ImageView clone
	imageButton position set(5, 0)
	imageButton size set(48, 52)
	imageButton leftMouseDown := method(delegate ?chooseNewLoop)
	imageButton leftMouseUp := method(Nop)
	imageButton leftMouseMotion := method( writeln("motion") )
	imageButton scale set(.4,.4,1)
	imageButton setImage(nil)
	//imageButton setImage(Image clone open(Path with(System launchPath, "Guitar.png")))
	addSubview(imageButton)

    )
    
    setImage := method(image, imageButton setImage(image))
    
    closeAction := method(superview closeAction)
    volumeAction := method(slider, superview volumeAction(slider))

    pitchAction := method ( slider,
      nil
    )

    pitchGrabAction := method ( slider,
      slider size setY( 50 )
      slider position setY( -10 )
    )
    
    pitchReleaseAction := method ( slider,
      write( ((slider value * 24) - 12) roundUp, "\n" )
      slider setValue( ((slider value * 24) roundUp) / 24 )
      superview sound setPitch( (slider value * 24) - 12 )
    )

    muteAction := method ( button,
        if( button isChecked) then(
            button setTexturesPath( "MuteButton/Normal" )
            button isChecked = nil
        ) else (
            button setTexturesPath( "MuteButton/Select" )
            button isChecked = 1
        )        
        superview muteAction( button )
    )

    soloAction := method ( button,
        if( button isChecked) then(
            button setTexturesPath( "SoloButton/Normal" )
            button isChecked = nil
        ) else (
            button setTexturesPath( "SoloButton/Select" )
            button isChecked = 1
        )        
        superview soloAction( button )
    )

    drawOutline := nil //getSlot("drawOutlineLine")

    draw := method(
	backgroundColor glColor
	size drawQuad

	glDisable(GL_LINE_SMOOTH)
	glLineWidth(1.1)

	//glColor4d(0, 0, 0, 1)
	//size drawLineLoopi  

	glColor4d(0, 0, 0, 1)
	glBegin(GL_LINES)
	glVertex2d(0, 0)
	glVertex2d(width, 0)
	glEnd
/*
	glColor4d(.6, .6, .6, 1)
	glBegin(GL_LINES)
	glVertex2d(0, height-1)
	glVertex2d(width, height-1)
	glEnd
*/
    )

    update := method(
      volumeSlider setValue(superview sound volume)
    )
)

