// On Windows, these tests may produce the message:
// "The process tried to write to a nonexistent pipe".
// This is normal behaviour when when a pipe executing
// a process is closed before the process completes.

FileTest := UnitTest clone do(
	testPOpenReadsFromCommandsStandardOutput := method(
		file := File with("echo hello") popen
		output := file readLines
		file close
		assertEquals(list("hello"), output)
	)

	testExitStatusIsNilBeforePOpenFinishes := method(
		file := File with("echo hello") popen
		assertEquals(nil, file exitStatus)
		file close
	)

	testPOpenSetsExitStatusToExitStatusOfProgram := method(
		// Note that this produces output. Closing before complete may result in SIGPIPE.
		file := File with("echo hello") popen
		file readToEnd
		file close
		assertEquals(0, file exitStatus)

		file := File with("test -f nonexistent-file") popen
		file close
		assertEquals(1, file exitStatus)
	)

    plat := System platform
	isOnWindows := plat beginsWithSeq("Windows") or plat beginsWithSeq("mingw")

	testPOpenSetsTermStatusToSignalWhenSignaled := method(
        isWindows := (System platform containsAnyCaseSeq("windows") or(
                            System platform containsAnyCaseSeq("mingw")))
        # OK, I've checked this on multiple platforms and everything's fine with
        # this test. But it's always failed on Github Actions! Even with the
        # same Ubuntu machine image. So we disable this test there.
        #
        # It seems like it's not about setting the termination status, because
        # it sets it. It just doesn't set SIGPIPE. So it looks like the problem
        # is in generating SIGPIPE rather then with the implemenation of File
        # termSignal.
        #
        # If this test fails on your system, please, open an issue for further
        # discussion. Or just fix it if you know how to do it...
        isGithubActions := System getEnvironmentVariable("GITHUB_ACTIONS")
        if((isWindows or isGithubActions) not,
            // try to open and close pipe quickly so that SIGPIPE is generated
            sigpipes := 0
            othersignals := 0
            100 repeat(
                file := File with("echo hello") popen close
                if(file termSignal == 13,
                    sigpipes = sigpipes + 1
                ,
                    if(file termSignal isNil not, othersignals = othersignals + 1))
            )
            assertEquals(0, othersignals)
            assertNotEquals(0, sigpipes)
        )
	)

	testPOpenReadToEndPreventsSIGPIPE := method(
		sigpipes := 0
		othersignals := 0
		exitSuccesses := 0
		100 repeat(
			file := File with("echo hello") popen
			output := file readToEnd
			file close
			assertEquals(output, "hello\n")
			if(file exitStatus == 0,
				exitSuccesses = exitSuccesses + 1
			,
					if(file termSignal == 13,
						sigpipes = sigpipes + 1
					,
						othersignals = othersignals + 1))
		)
		assertEquals(0, othersignals)
		assertEquals(0, sigpipes)
		assertEquals(100, exitSuccesses)
	)

	testFileAsBuffer := method(
		file := Directory with(Directory currentWorkingDirectory) files first
		e := try(
			file asBuffer
			# File exists so we should get here
			assertTrue(true)
		)
		e catch(
			# File exists so we should not get here
			assertTrue(false)
		)
		
		file := File with("Not-#3xistingF1l3" .. ((System thisProcessPid + Date hour) * Date minute))
		e := try(
			file asBuffer
			# File does not exist so we should not get here
			assertTrue(false)
		)
		e catch(
			# File does not exist so we should get here
			assertTrue(true)
		)
	)

	nil
)
