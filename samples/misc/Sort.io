#!/usr/bin/env io
words := "video killed the radio star" split
writeln("original: ", words join(" "))
words = words sortBy(method(a, b, a < b))
writeln("sortBy:   ", words join(" "))
