PDB := Obsidian clone do(
	setPath("obsidian.tc")
	
	objectsToPersist ::= nil

	sync := method(
		setObjectsToPersist(Collector dirtyObjects)
		
		objectsToPersist foreach(obj,
			if(obj ppid == nil,
				obj setPdb(self) generatePid persistMetaData
			)

			obj persistData persistSlots
		)
		
		setObjectsToPersist(nil)

		self
	)
)