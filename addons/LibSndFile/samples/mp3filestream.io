#!/usr/bin/env io


AudioMixer := Object clone do(
	init := method(
		self sources := List clone
		self mixed := Sequence clone
	)
	
	newSlot("streamDestination", AudioDevice)
	newSlot("processedSamples", 0)
	newSlot("isRunning", false)
	
	addSource    := method(s, sources append(s))
	removeSource := method(s, sources remove(s))
	
	process := method(sampleCount,
		byteCount := sampleCount * 8
		mixed setSize(byteCount)
		mixed zero
		sources foreach(source, 
			while(source outputBuffer size < byteCount, 
				//writeln("source outputBuffer size = ", source outputBuffer size, " < ", byteCount)
				yield
			)
			writeln("b ", source outputBuffer size)
			mixed float32ArrayAdd(source outputBuffer)
			source outputBuffer removeSlice(0, byteCount)
			writeln("a ", source outputBuffer size)
		)
		//mixed float32ArrayDivideBy(sources size)
		writeln("AudioMixer writing ", mixed size, " bytes to ", streamDestination type)
		streamDestination write(mixed)
		processedSamples = processedSamples + sampleCount
	)

	start := method(
		writeln("AudioMixer start")
		setIsRunning(true)
		while(isRunning, process(22050)) // 1/16th of a second
	)
	
	stop := method(
		setIsRunning(false)
	)
)

AudioDevice open

MP2Decoder := AVCodec clone setCodecName("mp2")

AVCodec withDestination := method(dest,
	decoder := self clone
	decoder setStreamDestination(dest)
	decoder @@startAudioDecoding
	yield
	decoder
)	

SampleRateConverter withDestination := method(dest,
	SampleRateConverter clone setStreamDestination(dest)
)


path1 := "/Users/steve/Downloads/Hp\ lovecraft/HP\ Lovecraft\ -\ The\ Fungi\ From\ Yuggoth/HP\ Lovecraft\ -\ Fungi\ From\ Yuggoth.mp3"
path1 := "/Users/steve/Music/Unnatural\ History\ III/Baby\ Food\ \(1994\).mp3"
//path2 := Path with(launchPath, "sounds/max.mp3")

File streamTo := method(dest,
	setStreamDestination(dest)
	startStreaming
	self
)

//file1 := File with(path1) streamTo(MP2Decoder withDestination(SampleRateConverter clone setStreamDestination(AudioDevice)))
file1 := File with(path1) 
file1 @@streamTo(MP2Decoder withDestination(SampleRateConverter clone))
//file2 := File with(path2) @streamTo(MP2Decoder withDestination(SampleRateConverter clone))
yield

writeln("AudioMixer clone")
mixer := AudioMixer clone
writeln("AudioMixer addSource ", file1  streamDestination type)
mixer addSource(file1 streamDestination streamDestination)
//mixer addSource(file2 streamDestination streamDestination)
writeln("AudioMixer start")
mixer start 

while(1, yield)


