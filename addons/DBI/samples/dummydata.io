#!/usr/bin/env io

createDummyData := method(c, 
	c execute("BEGIN TRANSACTION")
	c execute("""CREATE TABLE people (id integer, first varchar(15),
		last varchar(15), dob date)""")
	c executef("INSERT INTO people VALUES (?, ?, ?, ?)",
		1, "John", "Mc'Doe", Date fromString("05/10/1973", "%m/%d/%Y"))
	c executef("INSERT INTO people VALUES (?, ?, ?, ?)",
		2, "Jane", "Doe", Date fromString("01/19/2004", "%m/%d/%Y"))
	c executef("INSERT INTO people VALUES (?, ?, ?, ?)",
		3, "Joey", "Doe", Date fromString("11/18/1971", "%m/%d/%Y"))
	c execute("COMMIT")
)

dropDummyData := method(c, c execute("DROP TABLE people"))
