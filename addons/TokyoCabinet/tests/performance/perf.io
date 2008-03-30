max := 10000

db := TokyoCabinet clone
File with("test.db") remove		
db open("test.db")

s1 := Date secondsToRun(
		db begin
		for(i, 1, max, db atPut("001/" .. i, "v"))
		db commit
)
writeln((max/s1) asString(0, 0), " group writes per second")

s1 := Date secondsToRun(
		for(i, 1, max, db at("001/" .. i))
)
writeln((max/s1) asString(0, 0), " reads per second")

s1 := Date secondsToRun(
		c := db cursor
		i := 1
		c first
		while(c next, c key; i = i + 1)
		if(i != max, Exception raise("cursor read " .. i .. " entries instead of " .. max))
		c close
)
writeln((max/s1) asString(0, 0), " cursor reads per second")


s1 := Date secondsToRun(	
		db begin
		for(i, 1, max, db removeAt("001/" .. i))
		db commit
)
writeln((max/s1) asString(0, 0), " group removes per second\n")


s1 := Date secondsToRun(	
		for(i, 1, max, db transactionalAtPut("001/" .. i, "v"))
)
writeln((max/s1) asString(0, 0), " individual writes per second")


s1 := Date secondsToRun(	
		for(i, 1, max, db transactionalRemoveAt("001/" .. i))
)
writeln((max/s1) asString(0, 0), " individual removes per second")


// -----------------------------------------------


db close

