# FileImporter

An Importer for local source files.

## importPath(path)

Performs Lobby doFile(path). Can override to deal with other formats.

## ioFileSuffixes

A list of valid io source file suffixes.

## tryToImportProtoFromFolder(protoName,

path) Looks for the protoName with the valid ioFileSuffixes and calls importPath if found.

