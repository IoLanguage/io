#!/usr/bin/env io

/*
if (?args == nil, 
  write("requires name of sound file as argument")
)
*/

s := Sound clone setPath(Path with(launchPath, "sounds/max.wav")) load

AudioDevice open 
//AudioDevice asyncWrite(s buffer)

loop( 
	while(AudioDevice needsData not, yield)
	writeln("writing")
	AudioDevice asyncWrite(s buffer)
)
