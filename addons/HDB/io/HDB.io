#!/usr/local/bin/io

HDBModule := Object clone do(
    pathSeparator := "/" //0 asCharacter
    keyFromPathList := method(pathList, pathList join(pathSeparator))
)

HDBCursor := HDBModule clone do(
    node ::= nil
    cursor ::= nil
    path := method(node path)
    pathList := method(node pathList)
    inBounds ::= true
    
    checkPath := method(
        k := cursor key
        
        if(k,
            kc := k split(pathSeparator)
            
            if(k beginsWithSeq(path) and kc size == pathList size + 1,
                setInBounds(true)
                return self
            )
        )
        
        setInBounds(false)
        nil
    )
    
    next     := method(cursor next;     checkPath)
    previous := method(cursor previous; checkPath)
    
    key   := method(if(inBounds, cursor key,   nil))
    //data := method(if(inBounds, cursor value, nil))
    value := method(if(inBounds, 
            HDBNode clone setPathList(pathList clone append(key)) setDb(db)
    , nil))
)

HDBNode := HDBModule clone do(
    pathList ::= nil
    path := method(pathList join(pathSeparator))
    db ::= nil
    
    setDb := method(db,
        //writeln("setDb(", db type, ")")
        //if(db type != "HDB", Exception raise("wrong type"))
        self db := db
        self
    )
    
    cursor := method(
        c := HDBCursor clone setNode(self)
        cc := db qdbm cursor
        cc first
        cc jumpForward(path)
        c setCursor(cc)
        if(c checkPath == nil, c next)
        c
    )
    
    name := method(path lastPathComponent)

    setData := method(v,
        db qdbm transactionalAtPut(path, v)
        self
    )
    
    data := method(db qdbm at(path))
        
    createIfAbsent := method(if(data == nil, setData("")); self)
    
    remove := method(
        c := self cursor
        q := db qdbm
        q begin
        while(c key, r removeAt(c key); c next)
        q removeAt(path)
        q commit
        self
    )
    
    size := method(
        c := cursor
        count := 0
        while(c key, count = count + 1; c next)
        count
    )
    
    at := method(name, 
        HDBNode clone setPathList(pathList clone append(name)) setDb(db)
    )
    
    atCreateIfAbsent := method(name, self at(name) createIfAbsent)
    _at := getSlot("atCreateIfAbsent")
    
    exists := method(
        db qdbm at(path) != nil
    )

    foreach := method(
        c := cursor
        while(c key, 
            if(call argCount == 1) then(
                c value doMessage(call argAt(1))
            ) elseif(call argCount == 2) then(
                call sender setSlot(call argAt(0) name, c value)
                call sender doMessage(call argAt(1))
           ) else(
                call sender setSlot(call argAt(0) name, c key)
                call sender setSlot(call argAt(1) name, c value)
                call sender doMessage(call argAt(2))
           )
            c next
        )
    )
    
    proxy := method(HDBNodeProxy clone setNode(self))
)

HDB := HDBModule clone do(
    init := method(
        self qdbm := QDBM clone
        s := self
        self root := HDBNode clone setPathList(list()) setDb(s)
    )
    
    setPath := method(p, qdbm setPath(p); self)
    path    := method(qdbm path)
    open    := method(if(qdbm open, self, nil))
    close   := method(qdbm close; self)
    delete  := method(qdbm delete; self)
    
    size := method(qdbm size)
    
    show := method(
        c := qdbm cursor
        c first
        while(c key, 
            writeln(c key asMutable replaceSeq(pathSeparator, "/"))
            c next
        )
    )
)

HDBNodeProxy := Object clone do(
    setNode := method(n,
        self _node := n
        self forward := self getSlot("_forward")
        self updateSlot := self getSlot("_updateSlot")
        self setSlot := self getSlot("_setSlot")
        self Exception := self getSlot("Exception")
        //self removeAllProtos
        self
    )
    
    _forward := method(
        writeln("forwading ")
        writeln("forwading ", call message name)
        n := _node at(call message name)
        if(n exists not, Exception raise("no node at " .. n path))
        self
    )
    
    _updateSlot := method(k, v,
        if(_node exists,
            self setSlot(k, v)
        ,
            Exception raise("updateSlot error: node " .. node path .. " doesn't exist yet")
        )
        self
    )

    _setSlot := method(k, v,
        if(v isKindOf(Sequence) or v isKindOf(Number),
            _node at(k) setData(v asString)
        ,
            Exception raise("can't set node value to a " .. v type)
        )        
        self
    )
    
    -> := method( 
        _node _at(call message argAt(0) name)
    
    )
)

