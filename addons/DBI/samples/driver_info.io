#!/usr/bin/env io

showInfo := method(d,
	("          Name: " .. d at(0)) println
	("   Description: " .. d at(1)) println
	("      Filename: " .. d at(2)) println
	("       Version: " .. d at(3)) println
	("      Compiled: " .. d at(4)) println
	("        Author: " .. d at(5)) println
	("           URL: " .. d at(6)) println
	"" println
)

doRelativeFile("dbi_init.io")

("Libdbi version: " .. DBI version) println
(" Drivers found: " .. numDrivers) println
"" println

DBI drivers foreach(d, showInfo(d))
