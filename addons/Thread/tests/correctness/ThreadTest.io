Thread

ThreadTest := UnitTest clone do(	
    testBasic := method(
        writeln("Thread threadCount:", Thread threadCount)
		Thread	createThread("""writeln("hello world")""")
        writeln("Thread threadCount:", Thread threadCount)
		//System sleep(1)
	)
) 
