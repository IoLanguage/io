QDBM

assertEquals := method(a, b, 
	if(a != b, Exception raise(a .. " != " .. b))
)

qdbm := QDBM clone

File with("test.qdbm") remove

qdbm open("test.qdbm")

assertEquals(nil, qdbm at("foo"))

qdbm begin
qdbm atPut("a", "1")
qdbm commit

assertEquals("1", qdbm at("a"))

qdbm begin
qdbm atPut("b", "2")
qdbm commit

qdbm begin
qdbm atPut("c", "3")
qdbm commit

assertEquals("3", qdbm at("c"))
assertEquals("2", qdbm at("b"))
assertEquals("1", qdbm at("a"))

qdbm sync
qdbm close
qdbm open("test.qdbm")

assertEquals("1", qdbm at("a"))
assertEquals("2", qdbm at("b"))

qdbm begin
qdbm removeAt("a")
qdbm removeAt("b")
qdbm commit

assertEquals(nil, qdbm at("a"))
assertEquals(nil, qdbm at("b"))

qdbm sync
qdbm close
qdbm open("test.qdbm")

assertEquals(nil, qdbm at("a"))
assertEquals(nil, qdbm at("b"))

qdbm close

