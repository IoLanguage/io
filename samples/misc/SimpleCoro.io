#!/usr/bin/env io

yieldLoop := method(s,
    for(i, 1, 3, writeln(s, " ", i); yield)
)

coroDo(yieldLoop("a"))

yieldLoop("b")

writeln("done")
