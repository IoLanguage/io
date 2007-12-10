
Preferences := Object clone do(
	file := method(
		File clone setPath(Path with(launchPath, "Prefs.io"))
	)
	
	map := Map clone
	
	map asString := method(
		b := Sequence clone
		self foreach(k, v,
			writeln(k, "= ", v, " ", v type)
			
			if(v type == "String" or v type == "Buffer", 
				b write("atPut(\"", k, "\", \"", v, "\")\n")
			)
			
			if(v type == "Number", 
				b write("atPut(\"",k, "\", ", v, ")\n")
			)
		)
		b
	)  
	
	save := method(
		write("saving: [", map asString, "]\n")
		f := file remove openForUpdating 
		f write(map asString) 
		f close
	)
	
	load := method(
		f := File clone setPath(Path with(launchPath, "Prefs.io"))
		if(f exists and f asString size > 0, map doString(f asString))
	)
)

Preferences load
