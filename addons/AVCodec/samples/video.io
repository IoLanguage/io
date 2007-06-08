#!/usr/bin/env io

Flux

VideoApp := Application clone do(
	appDidStart := method(
        self videoView := VideoView clone
        mainWindow contentView addSubview(videoView)
        videoView resizeWithSuperview resizeToFitSuperview
        videoView open(System args at(1))
        mainWindow reshapeToSize(videoView video videoSize)
    )
)

VideoApp run
