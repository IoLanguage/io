

tdb := TagDB clone

tdb setPath("test")
tdb open

writeln("size = ", tdb size)
tdb atKeyPutTags("f430 for sale", list("red", "ferrari"))
tdb atKeyPutTags("lotus esprit", list("lotus", "esprit"))
writeln("size = ", tdb size)
keys := tdb keysForTags(list("lotus"))
writeln("keys = ", tdb symbolForId(keys at(0)))
tdb close
tdb delete

