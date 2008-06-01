
EventManager setListenTimeout(0)

ObsidianServer clone setHost("127.0.0.1") setPort(8000) @start
yield

keys := list()
max := 1000
max repeat(keys append(Random value asString))

objId := UUID uuidTime
value := "v" repeated(8)

conn := MDOConnection clone setHost("127.0.0.1") setPort(8000) connect
conn @receiveLoop

client := conn remoteObject

s1 := Date secondsToRun(
	keys foreach(key, client onAtPut(objId, key, value))
)

writeln((max/s1) asString(0,0), " writes per second")

s1 := Date secondsToRun(
	keys foreach(key, client onAt(objId, key))
)

writeln((max/s1) asString(0,0), " reads per second")

conn close
ObsidianServer stop
