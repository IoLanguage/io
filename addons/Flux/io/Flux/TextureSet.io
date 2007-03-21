
TextureSet := Object clone do(
    normalPath   := method(Path with(path, "Normal"))
    activePath   := method(Path with(path, "Active"))
    disabledPath := method(Path with(path, "Disabled"))
  
    openSet := method(path,
		self path := path
	
		if (ImageManager itemOrNil(Path with(path, "Normal/Middle"))) then(
			self normal := TextureGroup clone loadGroupNamed(normalPath)
			
			self active := TextureGroup clone loadGroupNamedOrNil(activePath)
			if(active == nil, 
			  //write("no active found for ", activePath, "\n")
			  active = normal
			)
			
			self disabled := TextureGroup clone loadGroupNamedOrNil(disabledPath)
			if(disabled == nil, disabled = normal)
			
		) else (
			self normal := TextureGroup clone loadGroupNamed(path)
			self active := normal
			self disabled := normal
		)
		self
    )
)


