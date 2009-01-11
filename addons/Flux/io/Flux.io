//metadoc Flux description AutoImporter for the Flux Io UI framework.

Color Red    := Color clone set(1,0,0,1)
Color Green  := Color clone set(0,1,0,1)
Color Blue   := Color clone set(0,0,1,1)
Color Yellow := Color clone set(1,1,0,1)
Color Purple := Color clone set(1,0,1,1)
Color Black  := Color clone set(0,0,0,1)
Color White  := Color clone set(1,1,1,1)

Flux := Object clone do(
	Views := Object clone
	appendProto(Views)
	
	//doc Message fluxPath  The root directory of the Flux addon
	fluxPath := if(Directory with("io/addons/Flux") exists, "io/addons/Flux", "/usr/local/lib/io/addons/Flux")
	//doc Message fluxSource  The directory of Flux where Flux.io is located. This directory is added to the Importer searchPaths.
	fluxSource := Path with(fluxPath, "io/Flux")
	//doc Message fluxPath  The directory where the .io files for the Flux views are located. This directory is added to the Importer searchPaths.
	fluxViews  := Path with(fluxPath, "io/Flux/Views")
	
	Importer addSearchPath(fluxSource)
	Importer addSearchPath(fluxViews)

	/*
	loadAll := method(
		Directory with(fluxSource) files select(name endsWithSeq(".io")) foreach(f,
			self doFile(f path)
		)

		Directory with(fluxViews) files select(name endsWithSeq(".io")) foreach(f,
			name := f name beforeSeq(".io")
			Views setSlot(name, self getSlot(name))
		)
	)
	*/
)
