TokyoCabinet

assertEquals := method(a, b, 
	if(a != b, Exception raise(a .. " != " .. b))
)

tc := TokyoCabinet clone

File with("test.tc") remove

tc open("test.tc", "VL_CMPPTH")

tc begin
tc atPut("ccc", "1")
tc atPut("ccc/aaa", "1")
tc atPut("bbb/bbb", "1")
tc atPut("aaa/ddd", "1")
tc atPut("aaa/bbb", "1")
tc atPut("ccc/bbb", "1")
tc atPut("aaa/bbb", "1")
tc atPut("bbb/ccc", "1")
tc atPut("aaa", "1")
tc atPut("ddd", "1")
tc atPut("a/b", "1")
tc atPut("a/a/b", "1")
tc commit


c := tc cursor
c first
while(c key,
	//writeln(tc cursorKey, ":", tc cursorValue)
	writeln(c key)
	c next
)

c close

tc close

