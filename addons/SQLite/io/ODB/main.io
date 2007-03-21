#!./ioServer

doFile("ODB.io")


odb := ODB clone
r := odb open("Test.sqlite") rootObject
r a := 1
