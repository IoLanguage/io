// popen tests removed: not available on WASM

FileTest := UnitTest clone do(
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
