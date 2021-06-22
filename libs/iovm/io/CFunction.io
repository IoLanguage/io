getSlot("CFunction") do(

	//doc CFunction name Returns the CFunction name in the form CFunction_Pointer.
	name := method(typeName ifNilEval("Object") .. "_" .. uniqueName ifNilEval("<unnamed>") .. "()")

	//doc CFunction asSimpleString Returns the CFunction name.
	asSimpleString := method(name)

	//doc CFunction asString Same as asSimpleString.
	asString := getSlot("asSimpleString")
)
