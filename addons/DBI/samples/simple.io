#!/usr/bin/env io

doRelativeFile("dbi_init.io")
doRelativeFile("dummydata.io")

c := DBI with("sqlite3")            // pgsql, mysql, sqlite, sqlite3, etc
//c setOption("host", "localhost")  // MySQL, PostgreSQL, mSQL, FreeTDS, etc
//c setOption("username", "user")   // ditto above
c optionPut("dbname", "test.db")    // test or whatever for other SQL engines
c optionPut("sqlite3_dbdir", ".")   // only for sqlite3
c connect

createDummyData(c)

r := c queryf("SELECT * FROM people WHERE id < ?", 1000	)
("  Rows: " .. r size) println
("Fields: " .. r fields size) println
"" println

r foreach(row, 
	row fields foreach(name,
		("   " .. name alignLeft(6) .. " = " \
			.. row at(name) asString alignLeft(25) \
			.. row at(name) type) println
	)
	"" println
)
r done

dropDummyData(c)

c close
