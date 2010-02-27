//metadoc ObsidianClient category Databases

ObsidianClient := Object clone

/*
sample client code

client := MDOConnection clone setHost("127.0.0.1") setPort(8000) connect
client onAtPut("1", "aKey", "aSlot")
client onAtPut("1", "cKey", "cSlot")
client onAtPut("1", "bKey", "bSlot")
client first("1", 5) println
a := client onAt("1", "aKey")
writeln("a = ", a)
b := client onAt("1", "bKey")
writeln("b = ", b)
client close
*/


