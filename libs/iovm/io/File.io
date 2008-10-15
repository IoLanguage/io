File do(
	//doc File with(aPath) Returns a new instance with the provided path.
	with := method(path, self clone setPath(path))

	streamDestination ::= nil
	
	streamReadSize := 65536
	
	startStreaming := method(streamTo(streamDestination))

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
	//doc File lockFile Deprecated. The .lock file used for File locking.
	lockFile := method(File clone setPath(path .. ".lock"))

	//doc File lock Creates a path.lock file if it does not exist. If it does exist, it waits for it be removed by the process that created it first. Returns self.
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
	
	//doc File create Creates an empty file at the file's path. Returns self on success, nil on failure.
	create := method(
		if(open, close; self, nil)
	)
	
	baseName := method(
		name split(".") slice(0, -1) join(".")
	)
	
	thisSourceFile := method(
		deprecatedWarning
		File with(Path with(Directory currentWorkingDirectory, call message label))
	)
	
	containingDirectory := method(
		Directory with(path pathComponent)
	)
)

Sequence do(
	//doc Sequence asFile Returns a new File object with the receiver as it's path.
	asFile := method(
		File with(self)
	)
)
