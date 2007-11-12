#!/usr/bin/env io

ioFiles := Directory filesWithExtension("io")
ioFiles select(f, f name != "runAll.io" and f name != "sample.Io") foreach(file,
	doFile(file name)
)
