#!/usr/bin/env io

b1 := Box clone set(Point clone set(0,0), Point clone set(100,100))
b1 print
write("\n")

b2 := Box clone set(Point clone set(20,20), Point clone set(120,120))
b2 print
write("\n")

b1 Union(b2)
write("union := ")
b1 print
write("\n")
