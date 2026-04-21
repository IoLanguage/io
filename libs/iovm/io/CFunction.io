getSlot("CFunction") do(
	//metadoc CFunction category Core
	//metadoc CFunction description A wrapper around a C function exposed to Io as an activatable slot.

	//doc CFunction name Returns the CFunction name in the form CFunction_Pointer.
	name := method(typeName ifNilEval("Object") .. "_" .. uniqueName ifNilEval("<unnamed>") .. "()")

	//doc CFunction asSimpleString Returns the CFunction name.
	asSimpleString := method(name)

	//doc CFunction asString Same as asSimpleString.
	asString := getSlot("asSimpleString")
)
