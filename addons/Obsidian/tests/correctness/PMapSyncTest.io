PMapSyncTest := UnitTest clone do (
	User := Object clone do(
		username ::= "Unnamed"
	)
	
	createUser := method(name,
		u := User clone
		u pSlots(username)
		u setUsername(name)
		u
	)

	setUp := method(
		writeln("setup ---------------")
		PDB setPath("test.tc") open
		
		// create our own PMap
		pm := PMap clone
		PDB root atPut("userLibrary", pm)

		// add a second user and sync
		pm atPut("tom", createUser("Tom"))
		self tomPpid := pm at("tom") ppid
		PDB sync
					
		// add a user and sync
		pm atPut("bob", createUser("Bob"))
		self bobPpid := pm at("bob") ppid
		PDB sync
		usernamePpid := PDB onAt(bobPpid, "username")
				
		PDB close
	)

	// shows that both users actually exist in the database with valid data
	testObjectPersistence := method(	
		writeln("testObjectPersistence -----------------")	
		PDB setPath("test.tc") open
		
		// find the Sequence object holding Bob's username and verify it
		usernamePpid := PDB onAt(bobPpid, "username")
		assertEquals(PDB onAt(usernamePpid, "_data"), "Bob")

		// find the Sequence object holding Tom's username and verify it		
		usernamePpid := PDB onAt(tomPpid, "username")
		assertEquals(PDB onAt(usernamePpid, "_data"), "Tom")
		
		PDB close
	)
	
	// shows that though the two users exist, the PMap only holds a reference to the first.
	testPMapPersistence := method(		
		writeln("testPMapPersistence -----------------")	
		PDB setPath("test.tc") open

		userLibrary := PDB root at("userLibrary")
		
		bob := userLibrary at("bob")
		
		writeln("userLibrary at(\"bob\") = ", bob)
		assertNotNil(bob)
		assertEquals(bob username, "Bob")
		
		// the bug causes a failure at the line below. Item "tom" can't be found
		tom := userLibrary at("tom")
		writeln("userLibrary at(\"tom\") = ", tom)
		//assert(tom)
		assertEquals(tom username, "Tom")
		
		PDB close
	)

	cleanUp := method(
		writeln("cleanUp ----------------")
		PDB delete
	)
)
