
TagDB do(
	uniqueTagNames := method(
		names := List clone 
		allUniqueTagIds foreach(id, names append(symbolForId(id)))
		names
	)
)
