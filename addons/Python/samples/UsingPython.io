#!/usr/bin/env io

# Import a module
sys := Python import("sys")

"Which version of python are we running?" println
sys version println

"System path is returned as a list" println
sys path foreach(p, p println)

"Load the string module" println
string := Python import("string")

"Split a string" println
str := "Brave brave Sir Robin"
str println
string split(str) println

"Load a C module (.so)" println
t := Python import("time")

writeln("Current time is: ", t time)

"Another way to invoke a method" println
str = "UPPERlower"
write(str, " --> ")
string invoke("swapcase", str) println

