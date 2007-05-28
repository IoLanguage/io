#!/usr/bin/env io
//Directory folders := method(items select(item, item type ==("Directory") and(item name != ".") and(item name != "..")))

writeln("")
writeln("items:")
Directory items foreach(path println)

writeln("")
writeln("folders:")
Directory folders foreach(name println)

//writeln("")
//writeln("files:")
//Directory files foreach(path println)

