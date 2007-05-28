#!/usr/bin/env io

Flux

VideoApp := Application clone do(
	appDidStart := method(
        self videoView := VideoView clone
        videoView resizeWithSuperview
        mainWindow contentView addSubview(videoView)
        videoView resizeToFitSuperview
        videoView open("/Users/steve/Sites/Io/addons/AVCodec/samples/test.mpg")
        mainWindow reshapeToSize(vector(videoView image originalWidth, videoView image originalHeight))
    )
)

VideoApp run
