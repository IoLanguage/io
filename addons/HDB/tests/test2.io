#!/usr/bin/env io

root := HDB with("test.hdb") rootProxy


p := HDB clone setPath("test.hdb") open root proxy

visited := p visitedThumbPageUrls 
writeln(visited size)

