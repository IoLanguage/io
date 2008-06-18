//metadoc ObsidianServer copyright Steve Dekorte 2008
//metadoc ObsidianServer license BSD revised
//metadoc ObsidianServer category Databases
/*metadoc ObsidianServer description 

A network interface for Obsidian.

<h4>Starting a Server</h4>

<pre>
oServer := ObsidianServer clone
oServer localObject setName("foo") open
oServer start
</pre>

<h4>Example Client Code</h4>

<pre>
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
</pre>

The messages accepted by the Obsidian server include:

<pre>
onAtPut(id, key, value)
onAt(id, key)
onRemoveAt(id, key)
onFirst(id, count)
onLast(id, count)
onAfter(id, key, count)
onBefore(id, key, count)
</pre>
*/

//ObsidianServer := MDOServer clone setLocalObject(Obsidian clone open)
