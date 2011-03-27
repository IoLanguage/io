
numDrivers := 0
e := try( numDrivers = DBI init ) 
e catch(
	dbdPath := System ioPath .. "/../dbd"
	e = try( numDrivers = DBI initWithDriversPath(dbdPath) )
	e catch(
		e error println
		System exit
	)
)

