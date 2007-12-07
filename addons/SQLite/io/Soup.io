// A somewhat NewtonScript-like Soup database

Soup := Object clone do(
	docCategory("Databases")
    newSlot("sqliteProto", SQLite)
    newSlot("debug", false)
    
    init := method(
		self newSlot("db", sqliteProto clone)
    )

	setPath := method(path, db setPath(path); self)
	path := method(db path)
	
    create := method(indexes,
		if (self open, return self)
		self indexes := indexes
		db open
		self exec(Sequence with("CREATE TABLE Soup (", self indexesString, ", _value)"))
		self exec(Sequence with("CREATE INDEX SoupIndex ON Soup (", self indexesString, ")"))
		self
    )

    exec := method(s, 
		if(debug, writeln(" *** ", s, " ***"))
		r := db exec(s)
		if(db error, Exception raise("Soup: " .. db error))
		return r
    )

    indexesString := method( 
		parts := self indexes join("', '")
		Sequence with("'", parts, "'")
    )

    open := method(indexes,		
		db exists ifFalse( 
			if (indexes, return self create(indexes), return nil)
		) 
	
		db open
		self indexes := self getIndexes
		self
    )

	delete := method(db delete)
	isOpen := method(db isOpen)
    close := method(db close)

    getIndexes := method(columnNames remove("_value"))

    columnNames := method(
		list := exec("PRAGMA TABLE_INFO(Soup)")
		//list foreach(i, m, m foreach(k, v, write(k, " := ", v, "\n")); write("\n"))
		names := List clone
		list foreach(i, m, names append(m at("name")))
		return names
    )

    append := method(object,
		value := stringForObject(object)
		values := List clone
		
		indexes foreach(i, index,  
			values append(Sequence with("'", object doString(index), "'"))
		)
	
		parts := values join(", ")
		cmd := Sequence with("INSERT INTO Soup (", indexesString, ", '_value') VALUES (", parts, ", '", value, "')")
		exec(cmd)
		object ROWID := db lastInsertRowId
    )

    update := method(obj,
		if (obj getSlot("ROWID") == nil, 
			Exception raise("Soup: can't update an object that has no ROWID slot")
		)
	
		value := stringForObject(obj)
		values := List clone
		indexes foreach(i, index, values append(String with("'", obj doString(index), "'")))
	
		s := Sequence clone
		indexes foreach(i, index, s appendSeq(index, "=", values at(i), ", "))
		cmd := String with("UPDATE Soup SET ", s, " _value='", value, "' WHERE ROWID='", obj ROWID, "'")
		exec(cmd)
		self
    )

    remove := method(obj,
		if (obj hasSlot("ROWID") == nil, return nil)
		exec(Sequence with("DELETE FROM Soup WHERE ROWID='", obj ROWID, "'"))
		self
    )

    itemsForQuery := method(q, options,
		if (options, q := q .. " " .. options)
	
		results := exec(q)
		objects := List clone
	
		if (results size == 0, return objects)
	
		results foreach(result,	
			obj := objectForString(result at("_value"))
				obj ROWID := result at("ROWID")
			objects append(obj)
		)
		objects
    )

    allItems := method(options,
		q := "SELECT ROWID, _value FROM Soup"
		itemsForQuery(q, options)
    )

    itemWithId := method(id,
		q := String with("SELECT ROWID, _value FROM Soup WHERE ROWID := '", id, "'")
		itemsForQuery(q) at(0)
    )

    itemsWhere := method(k1, v1, options,
		q := String with("SELECT ROWID, _value FROM Soup WHERE \"", k1, "\"='", v1, "'")
		itemsForQuery(q, options)
    )

    firstId := method(
		q := "SELECT ROWID FROM Soup ORDER BY ROWID ASC LIMIT 1"
		r := exec(q) at(0) 
		if (r, r at("ROWID") asNumber, nil)
    )

    lastId := method(
		q := "SELECT ROWID FROM Soup ORDER BY ROWID DESC LIMIT 1"
		r := exec(q) at(0) 
		if (r, r at("ROWID") asNumber, nil)
    )

    objectForString := method(s, 
		obj := Object clone 
		//try( 
		obj doString(s)
		//) catch(Exception, e, writeln("caught ", e name, ":", e description))
		return obj
    )

    stringForObject := method(object,
		if (object type != "Object", 
			Exception raise("Soup: can only accept Objects, not " .. object type)
		)
		
		s := Sequence clone
		object slotNames foreach(i, key,
			slot := object getSlot(key)
			t := getSlot("slot") type
			if(t == "Number", s appendSeq(key, ":=", slot, ";"))
			if(t == "Sequence", s appendSeq(key, ":=\"", slot asMutable escape, "\";"))
		)
	
		// add code to escape single quotes 
		return s asSymbol
    )
)
