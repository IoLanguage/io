
LibSndFile do(
	newSlot("path")
	//doc LibSndFile path Returns path to audio file.
	//doc LibSndFile setPath(aPath) Sets path to audio file.
	
	newSlot("sampleRate")
	//doc LibSndFile sampleRate Returns sample rate read from the audio file.
	
	newSlot("channels")
	//doc LibSndFile channels Returns channels read from the audio file.
	
	newSlot("format")
	//doc LibSndFile format Returns format read from the audio file.
	//doc LibSndFile setFormat(aSeq) Sets the format used for writing. Use the formatNames method to get a list of supported format names. Returns self.
)

Sound := Object clone do(
	//metadoc Sound category Audio
	//metadoc Sound description Encapsulates a audio file. Usefull for reading an audio file using LibSndFile.

	newSlot("path")
	//doc Sound path Returns path to audio file.
	//doc Sound setPath(aPath) Sets path to audio file.
		
	newSlot("buffer")
	//doc Sound buffer Returns the buffer used to read the audio file.
	
	newSlot("sampleRate")
	//doc Sound sampleRate Returns sample rate read from the audio file.
	
	newSlot("channels")
	//doc Sound sampleRate Returns number of channels read from the audio file.
	
	newSlot("format")
	//doc Sound format Returns format read from the audio file.
	
	load := method(
		//doc Sound load Returns Loads the file data into the buffer in interleaved stereo 32bit float format and sets the sampleRate, channels and format slots. Returns self.
		lsf := LibSndFile clone
		setBuffer(lsf outputBuffer)
		lsf setPath(path)
		lsf openForReading
		while(lsf read(16384), nil)
		setSampleRate(lsf sampleRate)
		setChannels(lsf channels)
		setFormat(lsf format)
		lsf close
		self
	)
)
