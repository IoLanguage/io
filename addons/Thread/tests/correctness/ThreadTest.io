Thread

ThreadTest := UnitTest clone do(	
    testBasic := method(
        //writeln("Thread threadCount:", Thread threadCount)
		assertEquals(Thread threadCount, 1)
		Thread	createThread("""1+1""")
        //writeln("Thread threadCount:", Thread threadCount)
		assertEquals(Thread threadCount, 2)
		System sleep(1)
		assertEquals(Thread threadCount, 1)
	)
) 
