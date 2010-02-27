
TagDB do(
	uniqueTagNames := method(
		//doc TagDB uniqueTagNames Returns a list of all unique tag names.
		names := List clone 
		allUniqueTagIds foreach(id, names append(symbolForId(id)))
		names
	)
)
