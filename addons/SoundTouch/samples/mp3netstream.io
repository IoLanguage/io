#!/usr/bin/env io


AudioDevice open

mp2Decoder := AVCodec clone do(
	setCodecName("mp2")
	setStreamDestination(AudioDevice)
	@@startAudioDecoding
)

s := URL with("http://www.somafm.com/groovesalad.pls") fetch betweenSeq("File1=", "\n")
s := "http://www.dekorte.com/Library/cowboy.mp3"
URL with(s) setStreamDestination(mp2Decoder) startStreaming
writeln("done streaming")
while(1, yield)

