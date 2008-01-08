
DBM := Object clone do(
	type := "DBM"
	docDescription("A simple DBM style key/value database.")
	docCategory("Databases")

	newSlot("sqliteProto", SQLite)

	init := method(
		self db := sqliteProto clone
		//db debugOn
	)

	setPath := method(path,
		db setPath(path)
	)
	path := method(db path)

	open := method(name,
		if(File clone setPath(db path) exists) then(
			db open
		) else(
			db open
			db exec("CREATE TABLE DBM (key, value)")
			db exec("CREATE INDEX DBMIndex ON DBM (key)")
		)
		self
	)

	isOpen := method(db isOpen)
	close := method(db close; self)

	atPut := method(key, value,
		// add code to escape single quotes
		if(self at(key)) then(
			cmd := "UPDATE DBM SET value := '" .. value .."' WHERE key='" .. key .. "'"
		) else(
			cmd := "INSERT INTO DBM (key, value) VALUES ('" .. key .. "', '" .. value .."')"
		)
		db exec(cmd)
		self
	)

	at := method(key,
		cmd := "SELECT value FROM DBM WHERE key='" .. key .. "'"
		//write("cmd := ", cmd, "\n")
		results := db exec(cmd)
		//write("[size := ", results size, "]")
		if (results size == 0, return nil)
		results at(0) at("value")
	)
)
