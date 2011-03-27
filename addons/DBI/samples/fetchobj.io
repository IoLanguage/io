#!/usr/bin/env io

doRelativeFile("dbi_init.io")
doRelativeFile("dummydata.io")

c := DBI with("sqlite3")
c optionPut("dbname", "test.db")
c optionPut("sqlite3_dbdir", ".")
c connect

createDummyData(c)

r := c query("SELECT * FROM people")
r foreach(Person, p, // See Person.io for Person object definition
	("Fullname: " .. p fullName alignLeft(12)) print
	(     "Dob: " .. p dob asString("%A, %B %d, %Y") alignLeft(30)) print
	(     "Age: " .. p age) println
)
r done

dropDummyData(c)

c close
