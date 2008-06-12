
MediaView := VideoView clone do(
    setMedia := method(media,
        stop
        AudioDevice close
        if(media type == "Video",
            setVideo(media)
            media seekVideoFrame(0)
            media readNextFrame
            setImage(media image)
            media setAudioOn(true)
            play
        ,
            setImage(media)
        )
        self
    )
)
