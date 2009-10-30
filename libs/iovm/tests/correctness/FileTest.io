// On Windows, these tests may produce the message:
// "The process tried to write to a nonexistent pipe".
// This is normal behaviour when when a pipe executing
// a process is closed before the process completes.

FileTest := UnitTest clone do(
	/*
	testPOpenReadsFromCommandsStandardOutput := method(
		file := File with("echo hello") popen
		output := file readLines
		file close
		assertEquals(list("hello"), output)
	)

	testExistStatusIsNilBeforePOpenFinishes := method(
		file := File with("echo hello") popen
		assertEquals(nil, file exitStatus)
		file close
	)

	testPOpenSetsExitStatusToExitStatusOfProgram := method(
		file := File with("echo hello") popen
		file close
		assertEquals(0, file exitStatus)

		file := File with("test -f nonexistent-file") popen
		file close
		assertEquals(1, file exitStatus)
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
	*/
	nil
)
