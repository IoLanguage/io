max := 10000

keys := list()

value := "v" repeated(8)

Random setSeed(Date now asNumber)
for(i, 1, max, keys append("001/" .. Random value asString))
//r := Random value asString
//for(i, 1, max, keys append("001/" .. r))

db := TokyoCabinet clone
File with("test.db") remove		
db open("test.db")

s1 := Date secondsToRun(
		db begin
		keys foreach(k, db atPut(k, value))
		db commit
)
writeln((max/s1) asString(0, 0), " group writes per second")

s1 := Date secondsToRun(
		keys foreach(k, db at(k))
)
writeln((max/s1) asString(0, 0), " reads per second")

s1 := Date secondsToRun(
		c := db cursor
		c first
		while(c next, c key; c value)
		//i := 1
		//while(c next, c key; c value; i = i + 1)
		//if(i != max, Exception raise("cursor read " .. i .. " entries instead of " .. max))
		c close
)
writeln((max/s1) asString(0, 0), " cursor reads per second")



s1 := Date secondsToRun(	
		db begin
		keys foreach(k, db removeAt(k))
		db commit
)
writeln((max/s1) asString(0, 0), " group removes per second")


s1 := Date secondsToRun(	
		keys foreach(k, db transactionalAtPut(k, value))
)
writeln((max/s1) asString(0, 0), " individual writes per second")


s1 := Date secondsToRun(	
		keys foreach(k, db transactionalRemoveAt(k))
)
writeln((max/s1) asString(0, 0), " individual removes per second")


writeln("\nreads and write info: ")
writeln("  key size:   " .. keys first size .. " bytes")
writeln("  value size: " .. value size .. " bytes")
writeln("  order is random except for cursor reads, which are sequential")


db close

