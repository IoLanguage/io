//metadoc PDB copyright Steve Dekorte 2008
//metadoc PDB license BSD revised
//metadoc PDB proto Obsidian
//metadoc PDB credits Steve Dekorte & Rich Collins
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
If the object was already in the database, only its updated slots will be written.

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
	objectsToPersist ::= Map clone
	ppidMap := Map clone
	_root := nil
	
	clone := method(self)
	
	//doc PDB newId Generate a new PDB id number for use as a persistent object's ppid.
	newId := method(
		UUID uuidTime asMutable replaceSeq("-", "") asSymbol
	)
	
	//doc PDB reopen Sync, close and reopen the PDB store.
	repoen := method(deprecatedWarning("reopen"); reopen)
	reopen := method(
		sync
		close
		open
	)
	
	/*doc PDB sync
Immediately persist data for all objects marked dirty by Collector whose 
shouldPersistByDefault is true, or that have specifically requested to be
persisted since the last sync via addObjectToPersist.
	*/
	sync := method(
		//writeln("\n\nPDB sync ----------")
		Collector collect
		dirty := Collector dirtyObjects 
		dirty foreach(obj,
			if(getSlot("obj") shouldPersistByDefault == true,
				//writeln(getSlot("obj") type, "_", getSlot("obj") uniqueId, " shouldPersistByDefault ")
				objectsToPersist atPut(getSlot("obj") ppid, getSlot("obj"))
			)
		)
		//writeln("PDB: persisting ", objectsToPersist size, " of ", dirty size, " dirty objects")
	
		while(objectsToPersist size > 0,
			keys := objectsToPersist keys
			keys foreach(k, 
				o := objectsToPersist at(k)
				//"About to persist #{o type}_#{o uniqueHexId}" interpolate println
				o persist
				objectsToPersist removeAt(k)
			)
		)
		Collector cleanAllObjects
		self
	)
	
	//doc PDB objectAtPpid Return the object associated in the database with a ppid.
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
		ppidMap atPut(ppid, unpersistedObj := obj unpersist)
    unpersistedObj
	)
	
	//doc PDB addObjectToPersist Register an object to be persisted in the next PDB sync.
	addObjectToPersist := method(o,
		objectsToPersist atIfAbsentPut(o ppid, o)
	)
	
	//doc PDB close Close the persistence database.
	close := method(
		resend
		_root = nil
		ppidMap = Map clone
		self
	)
	
	//doc PDB root Return the root PMap object used to store and retrieve persistent objects and their slots.
	root := method(
		if(_root, return _root)
		_root = self objectAtPpid("root")
		if(_root == nil, _root = PMap clone setPpid("root"))
		_root
	)
	
	
	//doc PDB show Print to standard output a listing of all objects and IDs stored in PDB.  
	show := method(
		//writeln("PDB ", db path, ":")
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
	
	//doc PDB collectGarbage Remove from PDB all objects not accessible via the root object.
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
				v := c value
				if(k beginsWithSeq("_") == false and walked at(k) == nil,
					toWalk append(k)
					if(walked at(v) == nil, toWalk append(v))
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
