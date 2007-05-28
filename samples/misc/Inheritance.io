#!/usr/bin/env io

// Define a Dog object

Dog := Object clone
Dog barkPhrase := "woof!"
Dog bark := method(barkPhrase print)

// Create Chiwawa subclass of Dog

Chiwawa := Dog clone
Chiwawa barkPhrase := "yip!"

write("Dog bark: ")
Dog bark
writeln

write("Chiwawa bark: ")
Chiwawa bark
writeln

// make an instance
myChiwawa := Chiwawa clone
myChiwawa barkPhrase := "Yo Quiero Taco Bell"

write("myChiwawa bark: ")
myChiwawa bark
writeln
