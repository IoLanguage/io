//metadoc Flux description AutoImporter for the Flux Io UI framework.

Flux := Object clone do(
	Views := Object clone
	Views type := "Object"
	appendProto(Views)
	
	fluxPath := if(Directory with("io/addons/Flux") exists, "io/addons/Flux", "/usr/local/lib/io/addons/Flux")
	fluxSource := fluxPath .. "/io/Flux"
	fluxViews  := fluxPath .. "/io/Flux/Views"
	
	Importer addSearchPath(fluxSource)
	Importer addSearchPath(fluxViews)

	loadAll := method(
		Directory with(fluxSource) files select(name endsWithSeq(".io")) foreach(f,
			//writeln(f path)
			self doFile(f path)
		)

		Directory with(fluxViews) files select(name endsWithSeq(".io")) foreach(f,
			//writeln(f path)
			name := f name beforeSeq(".io")
			Views setSlot(name, self getSlot(name))
			//writeln(name, " = ", self getSlot(name) type)
			//self doFile(f path)
		)
	)
)
