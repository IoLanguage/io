#!/usr/bin/env io

doRelativeFile("dbi_init.io")
doRelativeFile("dummydata.io")

c := DBI with("sqlite3")
c optionPut("dbname", "test.db")
c optionPut("sqlite3_dbdir", ".")
c connect

createDummyData(c)

r := c queryOne("SELECT * FROM people")
r at("first") println
r done

// See Person.io for Person object definition
p := c queryOne(Person, "SELECT * FROM people WHERE id=?", 2)
p fullName println

dropDummyData(c)

c close
