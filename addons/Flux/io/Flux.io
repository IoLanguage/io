
Importer addSearchPath("/usr/local/lib/io/addons/Flux/io/Flux")
Importer addSearchPath("/usr/local/lib/io/addons/Flux/io/Flux/Views")

Flux := Object clone do(
	type := "Object"
	Views := Object clone
	Views type := "Object"
	appendProto(Views)
	
	loadAll := method(
		Directory with("/usr/local/lib/io/addons/Flux/io/Flux") files select(name endsWithSeq(".io")) foreach(f, 
			//writeln(f path)
			self doFile(f path)
		)
		
		Directory with("/usr/local/lib/io/addons/Flux/io/Flux/Views") files select(name endsWithSeq(".io")) foreach(f, 
			//writeln(f path)
			name := f name beforeSeq(".io")
			Views setSlot(name, self getSlot(name))
			//writeln(name, " = ", self getSlot(name) type)
			//self doFile(f path)
		)
	)
)