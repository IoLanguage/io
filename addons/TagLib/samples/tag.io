#!/usr/bin/env io

TagLib do(
newSlot("path")
newSlot("title")
newSlot("artist")
newSlot("album")
newSlot("year")
newSlot("comment")
newSlot("track")
newSlot("genre")
)

path := Path with(launchPath, "sounds/max.mp3")
tag := TagLib clone setPath(path) load
writeln(tag)
tag setComment("max headroom saying 'hello, this is max headroom'")
tag save
