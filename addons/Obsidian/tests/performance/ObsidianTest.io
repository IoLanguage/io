

ob := Obsidian clone setPath("test.tc") open

objId := UUID uuidTime
max := 10000

s1 := Date secondsToRun(
	for(i, 1, max,
		ob onAtPut(objId, i asString, "vvvvv")
	)
)

writeln((max/s1) asString(0,0), " writes per second")

s1 := Date secondsToRun(
	for(i, 1, max,
		ob onAt(objId, i asString)
	)
)

writeln((max/s1) asString(0,0), " reads per second")

ob close
