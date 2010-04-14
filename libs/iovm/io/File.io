File do(
	//doc File with(aPath) Returns a new instance with the provided path.
	with := method(path, self clone setPath(path))

	//doc File streamDestination The buffer object to be used for future stream operations. 
	//doc File setStreamDestination(aBuffer) Set the buffer to be used for future stream operations. 
	streamDestination ::= nil
	
	//doc File streamReadSize Size of stream buffer used for streaming operations
	streamReadSize := 65536
	
	//doc File startStreaming Begin streamed read to stream destination set by setStreamDestination(aBuffer).
	startStreaming := method(streamTo(streamDestination))

	//doc File exitStatus System's close status (after file close).
  	exitStatus := nil

	//doc File termSignal Pipe's terminating signal (after file close opened via popen).
  	termSignal := nil

	//doc File streamTo(aBuffer) Perform streamed reading to given buffer. The buffer will be appended with chunks of size streamReadSize until EOF. The final chunk may be less than streamReadSize.
	streamTo := method(streamDestination,
		b := Sequence clone
		self open
		while(isAtEnd not,
			b empty
			readToBufferLength(b, streamReadSize)
			streamDestination write(b)
			yield
		)
	)
	
	//doc File streamToWithoutYielding(aBuffer) Perform streamed reading to given buffer without yielding between reads.
	streamToWithoutYielding := method(streamDestination,
		b := Sequence clone
		self open
		while(isAtEnd not,
			b empty
			readToBufferLength(b, streamReadSize)
			streamDestination write(b)
		)
	)
	
	//doc File copyToPathWithoutYielding(destinationPath) Copies the file to the specified path without yielding.
	copyToPathWithoutYielding := method(dstPath,
		dst := File with(dstPath) open
		self open streamToWithoutYielding(dst)
		dst close
		self close
		self
	)
	
	//doc File copyToPath(destinationPath) Copies the file to the specified path.
	copyToPath := method(dstPath,
		dst := File with(dstPath) open
		self open streamTo(dst)
		dst close
		self close
		self
	)

	/*
	// doc File lockFile Deprecated. The .lock file used for File locking.
	lockFile := method(File clone setPath(path .. ".lock"))

	// doc File lock Creates a path.lock file if it does not exist. If it does exist, it waits for it be removed by the process that created it first. Returns self.
	lock := method(timeout,
		if(timeout == nil, timeout = 10)
		lockFile := lockFile
		waitTime := .1
		waited := 0
		// a bit of a hack - this isn't atomic
		while(lockFile exists,
			wait(waitTime)
			waited = waited + waitTime
			if(waited > timeout, Exception raise("unable to acquire lock on " .. path))
		)
		lockFile open close
		self
	)

	unlock := method(lockFile remove)
	*/
	
	//doc File setContents(aSeq) Replaces the contents for the file with aSeq. Returns self.
	setContents := method(d, self truncateToSize(0) open write(d) close; self)
	
	//doc File appendToContents(aSeq) Appends to the contents of the file with aSeq. Returns self.
	appendToContents := method(
		openForAppending
		call evalArgs foreach(seq,
			self write(seq) 
		)
		close
		self
	)
	
	//doc File readToEnd(chunkSize) Reads chunkSize bytes (4096 by default) at a time until end of file is reached.  Returns a sequence containing the bytes read from the file.
	readToEnd := method(chunkSize,
		if(chunkSize not, chunkSize = 4096)
		buffer := Sequence clone
		while(readToBufferLength(buffer, chunkSize) == chunkSize, nil)
		buffer
	)
	
	//doc File create Creates an empty file at the file's path. Returns self on success, nil on failure.
	create := method(
		if(open, close; self, nil)
	)
	
	//doc File baseName Returns File's name without an extension (returned Sequence consists of all characters up to final period ["."] character).
	baseName := method(
		name split(".") slice(0, -1) join(".")
	)
	
	//doc File thisSourceFile Returns a File representing the system file in which this Io code exists.
	thisSourceFile := method(
		File with(Path with(call message label))
	)
	
	//doc File containingDirectory Deprecated. See parentDirectory.
	containingDirectory := method(
		deprecatedWarning("parentDirectory")
		parentDirectory
	)
	
	//doc File parentDirectory Returns a File for the directory that is the parent directory of this object's path. 
	parentDirectory := method(
		Directory with(path pathComponent)
	)
	
	standardOutput := File standardOutput
	standardInput  := File standardInput
)

Sequence do(
	//doc Sequence asFile Returns a new File object with the receiver as its path.
	asFile := method(
		File with(self)
	)
)
