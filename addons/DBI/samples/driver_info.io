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

("Libdbi version: " .. DBI version) println
(" Drivers found: " .. DBI init) println
"" println

DBI drivers foreach(d, showInfo(d))
