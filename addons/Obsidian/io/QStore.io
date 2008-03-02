#!/usr/local/bin/io

PMap := Object clone do(
	qfolder ::= nil
	
	withQFolder := method(f,
		self qfolder = f
	)

	writeToQFolder := method(f,
		f atPut("_type", "PMap")
		self pid := f pid
		self
	)
		
	at := method(k, 
		qfolder objectAt(k)
	)
	
	atPut := method(k, obj,
		qfolder atPutObject(k, obj)
		self
	)
)

nil do(
	withQFolder := method(f,
		nil
	)

	writeToQFolder := method(f,
		f atPut("_type", "nil")
		self pid := f pid
		self
	)
)

Number do(
	withQFolder := method(f,
		f at("_data") asNumber
	)

	writeToQFolder := method(f,
		f atPut("_type", "Number")
		f atPut("_data", self asString)
		self pid := f pid
		self
	)
)

Sequence do(
	withQFolder := method(f,
		f atPut("_type", "Sequence")
		f at("_data")
	)

	writeToQFolder := method(f,
		f atPut("_data", self)
		self pid := f pid
		self
	)
)

Object do(
	withQFolder := method(f,
		obj := self clone
		#obj pid := f pid
		obj qfolder := f
		c := f cursor
		c firstSlot
		while(c key,
			//if(c key != "pid",
				obj setSlot(c key, c object)
			//)
			c nextSlot
		)
		obj
	)
	
	writeToQFolder := method(f,
		self pid := f pid
		f atPut("_type", self type)
		self foreachSlot(k, v, 
			f atPutObject(k, self getSlot(v))
		)
		self
	)
	
	writeToQStore := method(
		nil
	)
)

// ----------------------------

QStoreObject := Object clone do(
    pathSeparator := "/" 
)

QFolderCursor := QStoreObject clone do(
    with := method(store, pid,
        self store := store
        self pid := pid
        self cursor := store db cursor
        first
    )
    
    pid ::= nil
    cursor ::= nil
    currentKey ::= nil
    
    getCurrentKey := method(
        k := cursor key
        if(k == nil, currentKey = nil; return nil)
        parts := k split(pathSeparator)
        self currentKey := if(parts first != pid, nil, parts at(1))
    )
    
    first := method(
        cursor first
        cursor jumpForward(pid)
        getCurrentKey
        self
    )
    
    last  := method(
        cursor last
        //cursor jumpBackward(pid)
        cursor jumpBackward(pid .. "/" .. 127 asCharacter)
        getCurrentKey
        self
    )
    
    lastSlot := method(last; previousUntilSlot)
    firstSlot := method(first; nextUntilSlot)
    
    jumpForward  := method(k,
        fk := list(pid, k) join(pathSeparator)
        cursor jumpForward(fk)
        getCurrentKey
        self
    )
    
    jumpBackward := method(k, 
        fk := list(pid, k) join(pathSeparator)
        cursor jumpBackward(fk)
        getCurrentKey
        self
    )
    
    next := method(
        if(currentKey, cursor next; getCurrentKey)
        self
    )

    nextUntilSlot := method(
        while(currentKey and currentKey beginsWithSeq("_"), next)
    )
        
    nextSlot := method(next; nextUntilSlot; self)
    
    previous := method(
        if(currentKey, cursor previous; getCurrentKey)
        self
    )
    
    previousUntilSlot := method(
        while(currentKey and currentKey beginsWithSeq("_"), previous)
    )
    
    previousSlot := method(previous; previousUntilSlot; self)
    
    key := method(currentKey)
    
    value := method(
        v := cursor value
        if(v == nil, return nil)
        if(key beginsWithSeq("_"), v, QFolder with(store, v))
    )
    
    object := method(
    	QFolder with(store, pid) objectAt(key)
    )
    	    
    previousCount := method(max,
        results := List clone
        //writeln("key = ", key, " max = ", max)
        while(self key,
            if((max = max - 1) < 0, break)
            results append(self value)
            self previousSlot
        )
        results
    )
    
    nextCount := method(max,
        results := List clone
        while(self key and (max = max - 1) < 0,
            results append(self value)
            self nextSlot
        )
        results
    )     
    
    slotsBeforeCount := method(k, max,
	    lastSlot
	    if(k, jumpBackward(k))
	    previousCount(max)
    )

	slotsAfterCount := method(k, max,
	    firstSlot
	    if(k, jumpForward(k))
	    previousCount(max)
    )
)

QFolder := QStoreObject clone do(
    with := method(store, pid,
        obj := self clone
        obj store := store
        obj pid := pid
        obj
    )
    
    rawAt := method(k,
        fk := list(pid, k) join(store pathSeparator)
        store db at(fk)    
    )
    
    at := method(k,
        v := rawAt(k)
        if(k beginsWithSeq("_"), v, if(v, QFolder with(store, v), return nil)) //Exception raise(k .. " folder not found")))
    )

	removeAt := method(key,
		k := pid .. store pathSeparator .. key
		//writeln(k, " ", if(store db at(k) == nil, "nil", "found"))
		store db begin
		store db removeAt(k)
		store db commit
		self
	)
    
    objectAt := method(k,
    	self at(k) object
    )
       
    object := method(
    	t := self at("_type")
	if(t == nil, return nil)
     	Lobby getSlot(t) withQFolder(self)    	
    )
    
    atPutObject := method(k, obj,
    	if(obj getSlot("pid") == nil, 
    		f := self createIfAbsentAt(k)
    		obj writeToQFolder(f)
 			self atPut(k, f)
   		,
        	store transactionalAtPut(k, obj pid)
    	)
    	self
    )
    
    createIfAbsentAt := method(k,
        if(k beginsWithSeq("_"), Exception raise("error with '" .. k .. "': folder keys must not begin with a _"))
        v := rawAt(k)
        if(v == nil, 
            f := store newFolder
            f atPut("_type", "PMap")
            self atPut(k, f)
            return f
        )
        at(k)
    )
        
    atPutString := method(k, s,
    	stringFolder := self createIfAbsentAt(k)
    	stringFolder atPut("_type", "Sequence")
    	stringFolder atPut("_data", s)
    	self
    )

	atPutNumber := method(k, n,
        stringFolder := self createIfAbsentAt(k)
        stringFolder atPut("_type", "Number")
        stringFolder atPut("_data", n asString)
        self  
	)
    
    atPut := method(k, v,
        fk := list(pid, k) join(store pathSeparator)
        if(v type == "QFolder", 
            if(k beginsWithSeq("_"), Exception raise("error with '" .. k .. "': folder keys must not begin with _"))
            v = v pid
        ,
            if(k beginsWithSeq("_") not, Exception raise("error with '" .. k .. "': non folder keys must begin with a _"))
        ) 
        store transactionalAtPut(fk, v)
        self
    )
    
    show := method(
        c := store db cursor
        c first
        c jumpForward(pid)
        writeln(self type, pid)
        while(c key and c key split(store pathSeparator) first == pid,
            writeln("  ", c key, ": ", c value)
            c next
        )
    )
    
    startCursor := method(
        c := store db cursor
        c first 
        c jumpForward(pid)
    )
  
    foreach := method(
        keyName   := call argAt(0) name
        valueName := call argAt(1) name
        m := call argAt(2)
        
        c := startCursor
        while(c key and c key split(store pathSeparator) first == pid,
            k := c key split(store pathSeparator) at(1)
            call sender setSlot(keyName, k)
            v := c value
            if(k beginsWithSeq("_") not, v = QFolder with(store, v))
            call sender setSlot(valueName, v)
            call sender doMessage(m)
            c next
        )
        self
    )
      
    foreachSlot := method(
        keyName   := call argAt(0) name
        valueName := call argAt(1) name
        m := call argAt(2)
        
        c := startCursor
        while(c key and c key split(store pathSeparator) first == pid,
            k := c key split(store pathSeparator) at(1)
            if(k beginsWithSeq("_") not,
                call sender setSlot(keyName, k)
                v := c value
                if(k beginsWithSeq("_") not, v = QFolder with(store, v))
                call sender setSlot(valueName, v)
                call sender doMessage(m)
            )
            c next
        )
    )
    
    keyNames := method(
        names := List clone
        c := cursor
        while(c key, names append(c key); c next)
        names
    )
    
    delete := method(
        q := store db
        q begin
        self foreach(k, v, q removeAt(k))
        q commit
        self
    )
    
    size := method(
        count := 0
        self foreach(k, v, count = count + 1)
        count   
    )
    
    exists := method(
        k := startCursor key
        k and k split(store pathSeparator) first == pid
    )
    
    cursor := method(
        QFolderCursor clone with(store, pid)
    )
    
    asObject := method(
        t := self at("_type")
        t ifNil(return nil)
        Lobby getSlot(t) fromQStoreFolder(self)
    )

    slotsBeforeCount := method(k, max, cursor slotsBeforeCount(k, max))
    slotsAfterCount  := method(k, max, cursor slotsAfterCount(k, max))
)

QStoreCursor := QStoreObject clone do(
    store ::= nil
    
    setStore := method(store,
        self store := store
        self cursor := store db cursor
        self
    )
    
    first := method(cursor first; getBucket; self)
    last  := method(cursor last;  getBucket; self)
    
    jumpForward  := method(k, cursor jumpForward(k); getBucket; self)
    jumpBackward := method(k, cursor jumpBackward(k); getBucket; self)
    
    value := method(
        k := cursor key 
        self bucket := if(k, QFolder with(store, k split(store pathSeparator) first), nil)
        bucket
    )
    
    next := method(
        k := cursor key
        k ifNil(return nil)
        nextKey := (k asNumber + 1) asString
        jumpForward(nextKey)
        getBucket
        if(cursor key, self, nil)
    )
    
    previous := method(
        k := cursor key
        k ifNil(return nil)
        nextKey := (k asNumber - 1) asString
        jumpBackward(nextKey)
        getBucket
        if(cursor key, self, nil)
    )
)

QStore := QStoreObject clone do(
    init := method(
        self db := QDBM clone setPath("default.qstore")
    )
    
    setPath := method(p, db setPath(p); self)
    path := method(db path)
    
    lastPid := method(
        c := db cursor; c last
        if(c key, c key split(pathSeparator) first asNumber, 0)
    )
    
    open := method(
        if(db open(db path, "VL_CMPSNM") not, return nil)
        c := db cursor; c last
        self pidCount := lastPid
        //writeln("opening - last pid = ", pidCount)
        self
    )

    close   := method(db close;  self)
    delete  := method(File with(db path) remove; self)

	begin := method(setPausedTransactions(true); db begin)
	commit := method(setPausedTransactions(false); db commit)
	
	pausedTransactions ::= false
	
	transactionalAtPut := method(k, v,
		if(pausedTransactions, 
			db atPut(k, v)
		,
			//writeln("transactionalAtPut")
			db transactionalAtPut(k, v)
		)
	)
	
    show := method(
        c := db cursor
        c first
        while(c key,
            //writeln(c key asMutable replaceSeq(pathSeparator, "."), ": ", c value)
            writeln("  ", c key, ": ", c value)
            c next
        )
    )
    
    folderAt := method(pid, QFolder with(self, pid))
    newFolder := method(QFolder with(self, newPid))
    root := method(folderAt("0"))
    newPid := method(pidCount = pidCount + 1; pidCount asString asSymbol)
    
    cursor := method(QStoreCursor clone setStore(self) first)
    
    collectGarbage := method(
    	marked := Map clone
    	toMark := List clone append(root)
   	
    	while(f := toMark pop,
    		f foreachSlot(k, v,
    			if(marked at(v pid) == nil, toMark append(v))
    		)
    		marked atPut(f pid, 1)
    	)
		
		collectCount := 0
		db begin
		c := db cursor
		while(c key,
			pid := c key split(pathSeparator) first
			if(marked at(pid) == nil,
	    		//writeln("remove '", pid, "'") //: '", c value, "'")
				c remove
				//db removeAt(c key)
				collectCount = collectCount + 1
			) 
			c next
		) 
		db commit
		writeln("marked = ", marked size)
		writeln("collected = ", collectCount)
		collectCount
    )
)
