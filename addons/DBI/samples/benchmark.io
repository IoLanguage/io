#!/usr/bin/env io

doRelativeFile("dbi_init.io")

recCount := if(System args at(1), System args at(1) asNumber, 50000)

c := DBI with("sqlite3")
c optionPut("dbname", "test.db")
c optionPut("sqlite3_dbdir", ".")
c connect

("      Setup: Creating table/inserting " .. recCount .. " rows") println

c execute("BEGIN TRANSACTION")
c execute("""CREATE TABLE people (first varchar(15),
	last varchar(15), dob date)""")
for(a, 1, recCount,
	c execute("INSERT INTO people VALUES ('John', 'Doe', '1972-10-21')")
	c execute("INSERT INTO people VALUES ('Jane', 'Doe', '1933-08-12')")
	c execute("INSERT INTO people VALUES ('Joe',  'Doe', '2002-03-17')")
)
c execute("COMMIT")

"Benchmark 1: Old fashioned DBI access: " print
time1 := Date cpuSecondsToRun(
	a := 1
	r := c query("SELECT * FROM people")
	r foreach(row, 
		fullName := row at(1) .. " " .. row at(2)
		age := Duration fromNumber(row at(3) secondsSinceNow) years
		
		// Ensure things are working correctly
		if (a == 1, ("   " .. fullName .. " " .. age) println)
		a := a + 1
	)
	r done
)

"Benchmark 2: New OO DBI access:        " print
time2 := Date cpuSecondsToRun(
	a := 1
	r := c query("SELECT * FROM people")
	r foreach(Person, p,
		p fullName
		p age
		
		// Ensure things are working correctly
		if (a == 1, ("   " .. p fullName .. " " .. p age) println)
		a := a + 1
	)
	r done
)

"" println

("Old fashioned DBI access: " .. time1) println
("       New OO DBI access: " .. time2) println

if(time2 > time1,
	diff := time2 - time1
	("    OO slower by seconds: " .. diff) println
	("             per request: " .. (diff / recCount)) println,
	
	diff := time1 - time2
	("   Old slower by seconds: " .. diff) println
	("             per request: " .. (diff / recCount)) println
)

"" println

c execute("DROP TABLE people")
c close
