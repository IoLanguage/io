
LoopPanel := OpenPanel clone do (

    init := method (
        resend
        SongJam sound := Sound clone
        self previewButton := Button clone
        previewButton setTitle( "Preview" )
        previewButton resizeWidth = 011
        previewButton setAction( "previewAction" )
        previewButton setActionTarget( self )
        previewButton placeLeftOf( self openButton )
        addSubview( previewButton )
    )

    previewAction := method ( button,
        panel := button superview
        item := panel browser selectedItem object
	  if ( item title endsWithSeq(".wav"),
	      item downloadIfNeeded
            SongJam sound open( item path )
            //SongJam sound setVolume( 1 )
            //AudioMixer setVolume( 1 )
            AudioMixer removeAllSounds
            AudioMixer addSound( SongJam sound )
            self isPlaying := nil
            Screen addTimerTargetWithDelay( self, 0 )
        )
    )

    timer := method (
        yield
        write( SongJam sound playLength, " ", SongJam sound position, "\n" )
        if( SongJam sound position == 0 and ( self isPlaying == nil ),
            Screen addTimerTargetWithDelay( self, 1 / 30 )
        )
        if( SongJam sound position != 0,
            self isPlaying = 1
            Screen addTimerTargetWithDelay( self, 1 / 30 )
        )
    )

    close := method (
        resend
        SongJam sound setPosition( 0 )
        AudioMixer removeAllSounds
    )

)

