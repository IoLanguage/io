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
)
