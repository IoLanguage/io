#!/usr/bin/env io


AudioDevice openForReadingAndWriting

loop( 
	buf := AudioDevice asyncRead
	write(buf size, "\n")
	while(AudioDevice needsData == false,
		yield
	)
	AudioDevice asyncWrite(buf)
)

