File do(
	docSlot("with(aPath)", "Returns a new instance with the provided path.")
	with := method(path, self clone setPath(path))

	newSlot("streamDestination")
	newSlot("streamReadSize", 1024*64)
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
	
	copyToPath := method(dstPath,
		dst := File with(dstPath) open
		self open streamTo(dst)
		dst close
		self close
	)
	
	lockFile := method(File clone setPath(path .. ".lock"))
	
	lock := method(timeout,
		if(timeout == nil, timeout = 10)
		lockFile := lockFile
		waitTime := .1
		waited := 0
		while(lockFile exists,
			wait(waitTime)
			waited = waited + waitTime
			if(waited > timeout, Exception raise("unable to acquire lock on " .. path))
		)
		lockFile open close
	)
	
	unlock := method(lockFile remove)
)

Sequence asFile := method(File with(self))
