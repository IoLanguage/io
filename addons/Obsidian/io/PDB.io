//metadoc PDB copyright Steve Dekorte 2008
//metadoc PDB license BSD revised
//metadoc PDB proto Obsidian
//metadoc PDB credits In collaboration with Rich Collins
//metadoc PDB category Databases
/*metadoc PDB description 
An arbitrary graph database with support for on-disk garbage collection. Example use:

<h4>Setup</h4>

<pre>
PDB open
PDB root atPut("users", PMap clone)
PDB sync
PDB close
</pre>

PMap is a Map/Dictionary whose keys are lazily loaded from the database. 
PDB root is the root PMap in the database and the root object used for PDB's garbage collector. 
PDB sync needs to be called to write any changes to the database.

<h4>Defining a Persistent Object</h4>

<pre>
User := Object clone pSlots(name, email)
</pre>

The pSlots(), declares which slots on the object should be persisted. 
The List, Date, Sequence and Number primitives already know how to persist themselves.

<h4>Inserting a Persistent Object</h4>

<pre>
PDB open 
user := User clone setName("steve") setEmail("steve@foo.com")
PDB root users atPut("steve", user)
PDB sync
PDB close
</pre>

<h4>Accessing a Persistent Object</h4>

<pre>
user := PDB root users at("steve")
writeln("user name = ", user name, " email = ", user email)
</pre>

<h4>Updating a Persistent Object</h4>

<pre>
user setEmail("steve@newDomain.com")
PDB sync 
</pre>

PDB sync will scan all persistent objects in the vm and save any with changes to their persistent slots. 
If the object was already in the database, only it's updated slots will be written.

<h4>Removing an entry in a PMap</h4>

<pre>
PDB root users removeAt("steve")
</pre>

<h4>Removing a persistent object</h4>

This is never done explicitly, instead calling:

<pre>
PDB collectGarbage
</pre>

Will remove all objects unreachable by the reference graph from the root PMap.

<p>
Notes: Currently, PDB is a singleton.
*/

PDB := Obsidian clone do(
	objectsToPersist ::= List clone
	ppidMap := Map clone
	_root := nil
	
	clone := method(self)
	
	newId := method(
		UUID uuidTime asMutable replaceSeq("-", "") asSymbol
	)
	
	reopen := method(
		sync
		close
		open
	)
	
	sync := method(
		Collector collect
		Collector dirtyObjects foreach(obj,
			if(getSlot("obj") shouldPersistByDefault == true,
				//writeln(getSlot("obj") type, "_", getSlot("obj") uniqueId, " shouldPersistByDefault ")
				objectsToPersist appendIfAbsent(getSlot("obj"))
			)
		)
		objectsToPersist foreach(persist)
		objectsToPersist removeAll
		Collector cleanAllObjects
		self
	)
	
	objectAtPpid := method(ppid,
		if(ppid == nil ppid, return(nil))
		if(ppid == false ppid, return(false))
		if(ppid == true ppid, return(true))
		obj := ppidMap at(ppid)
		if(obj, return obj)
		objType := self onAt(ppid, "_type")
		//writeln(objType, " = db onAt('", ppid, "/_type)")
		if(objType == nil, return nil)
		obj := Lobby doString(objType) clone setPpid(ppid)
		ppidMap atPut(ppid, obj)
		obj unpersist
	)
	
	addObjectToPersist := method(o,
		objectsToPersist appendIfAbsent(o)
	)
	
	close := method(
		resend
		_root = nil
		ppidMap = Map clone
		self
	)
	
	root := method(
		if(_root, return _root)
		_root = self objectAtPpid("root")
		if(_root == nil, _root = PMap clone setPpid("root"))
		_root
	)
	
	show := method(
		writeln("PDB ", db path, ":")
		c := db cursor
		c first
		while(c key,
			k := c key
			id := k beforeSeq("/")
			slotName := k afterSeq("/")
			if(slotName beginsWithSeq("_"),
				writeln("  ", k, " = '", c value, "'")
			,
				writeln("  ", k, " = ", c value)
			)
			c next
		)
		c close
			
	)
	
	noteReference := method(v,
		nil
	)
	
	collectGarbage := method(
		// plan to make this incremental and distributed later
		// walk objects from root, recording ids found
		self walked := Map clone
		self toWalk := List clone append("root")
		c := self sharedPrefixCursor
		while(id := toWalk pop,
			walked atPut(id, true)
			c setPrefix(id)
			c first
			while(c key,
				k := c key
				if(k beginsWithSeq("_") == false and walked at(k) == nil,
					toWalk append(k)
				)
				c next
			)
		)
		
		self toWalk := nil

		// now remove all non-walked ids
		db begin
		c := db cursor
		c first
		while(k := c key,
			if(walked at(k beforeSeq("/")) == nil, c remove, c next)
		)
		c close
		db commit
		
		self walked := nil
	)

	/*
	stringSymbols := method(
		self stringSymbols := self at("stringSymbols")
	)
	
	numberSymbols := method(
		self numberSymbols := self at("numberSymbols")
	)
	
	ppidForStringSymbol := method(s,
		stringSymbols atPut(s, nil)
	)
	
	ppidForNumberSymbol := method(n,
		numberSymbols atPut(n asString, nil)
	)
	*/	
)
