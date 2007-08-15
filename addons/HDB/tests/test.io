
assert := method(t, if(t, nil, Exception raise("assertion failed: " .. call argAt(0))))

fileName := "test.hdb"
File with(fileName) remove

h := HDB clone
h setPath(fileName)
h open
assert(h size == 0)

d := "He's just this guy, you know?"
users := h root at("users")
assert(users exists not)
users setData("")
assert(users exists)
assert(h size == 1)
assert(users data == "")

steve := users atCreateIfAbsent("steve") 
rich := users atCreateIfAbsent("rich") 
bob := users atCreateIfAbsent("bob")

assert(users size == 3)
assert(h size == 4)

users foreach(k, v, writeln("'", k, "'"))

//h show
h close

writeln("all tests passed")
