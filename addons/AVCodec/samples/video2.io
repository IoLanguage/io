#!/usr/bin/env io

Flux

VideoApp := Application clone do(
	appDidStart := method(
		self videoView := VideoView clone
		videoView resizeWithSuperview
		mainWindow contentView addSubview(videoView)
		videoView resizeToFitSuperview
		videoView open("test2.mpg")
		mainWindow reshapeToSize(vector(videoView image width, videoView image height))
	)
)

VideoApp run
