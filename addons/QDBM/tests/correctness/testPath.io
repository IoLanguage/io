QDBM

assertEquals := method(a, b, 
	if(a != b, Exception raise(a .. " != " .. b))
)

qdbm := QDBM clone

File with("test.qdbm") remove

qdbm open("test.qdbm", "VL_CMPPTH")

qdbm begin
qdbm atPut("ccc", "1")
qdbm atPut("ccc/aaa", "1")
qdbm atPut("bbb/bbb", "1")
qdbm atPut("aaa/ddd", "1")
qdbm atPut("aaa/bbb", "1")
qdbm atPut("ccc/bbb", "1")
qdbm atPut("aaa/bbb", "1")
qdbm atPut("bbb/ccc", "1")
qdbm atPut("aaa", "1")
qdbm atPut("ddd", "1")
qdbm atPut("a/b", "1")
qdbm atPut("a/a/b", "1")
qdbm commit


qdbm cursorFirst
while(qdbm cursorKey,
	//writeln(qdbm cursorKey, ":", qdbm cursorValue)
	writeln(qdbm cursorKey)
	qdbm cursorNext
)

qdbm close

