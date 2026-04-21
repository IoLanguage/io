# Importer

A simple search path based auto-importer.

## addSearchPath(path)

Add a search path to the auto importer. Relative paths are made absolute before adding.

## autoImportingForward

A forward method implementation placed in the Lobby when Importing is turned on.

## FileImporter

An Importer for local source files.

## import(originalCallMessage)

Imports an object or addon for the given Message.

## importers

List of Importer objects.

## paths

List of paths the proto importer will check while searching for protos to load.

## removeSearchPath(path)

Removes a search path from the auto importer. Relative paths should be removed from the same working directory as they were added.

## turnOff

Turns off the Importer. Returns self.

## turnOn

Turns on the Importer. Returns self.

