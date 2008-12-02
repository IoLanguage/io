#!/usr/bin/env io
//Directory directories := method(items select(item, item type ==("Directory") and(item name != ".") and(item name != "..")))

writeln("")
writeln("items:")
Directory items foreach(path println)

writeln("")
writeln("directories:")
Directory directories foreach(name println)

//writeln("")
//writeln("files:")
//Directory files foreach(path println)

