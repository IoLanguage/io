#!/usr/bin/env io

Flux

VideoApp := Application clone do(
	appDidStart := method(
		self videoView := VideoView clone
		videoView resizeWithSuperview
		mainWindow contentView addSubview(videoView)
		videoView resizeToFitSuperview
		path := System args at(1)
		writeln(" path = ", path)
		videoView open(path)
		mainWindow reshapeToSize(vector(videoView image width, videoView image height))
	)
)

VideoApp run
