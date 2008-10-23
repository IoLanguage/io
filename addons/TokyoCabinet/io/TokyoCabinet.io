
TokyoCabinet do(
	//doc TokyoCabinet path Returns the path of the database file.
	//doc TokyoCabinet setPath(aPath) Sets the path of the database file. Returns self.
	path ::= nil
	
	//doc TokyoCabinet compareType Returns the compare type function name used.
	/*doc TokyoCabinet setCompareType(name) 
	Sets the compare type function used to compare keys in the database. Valid compare types include:
	"lexical", "decimal", "int32", "int64" and "path". Returns self.
	*/
	compareType ::= "lexical"

	//doc TokyoCabinet transactionalAtPut(key, value) Transactionally insert the given key and value. Returns self.
	transactionalAtPut := method(k, v,
		self begin
		self atPut(k, v)
		self commit
		self
	)

	//doc TokyoCabinet transactionalRemoveAt(key) Transactionally remove the given key. Returns self.
	transactionalRemoveAt := method(k,
		self begin
		self removeAt(k)
		self commit
		self
	)
)

TokyoCabinetPrefixCursor do(
	prefix ::= nil
	db ::= nil
	
	copyFromCursor := method(other,
		other first
		while(other key and other value, 
			to := Path with(self prefix, other key)
			//writeln("copying from:", Path with(other prefix, other key), " -> ", to)
			self db atPut(to, other value)
			other next
		)
		self
	)
	
	copyKeysFromCursor := method(other,
		other first
		while(other key and other value, 
			to := Path with(self prefix, other key)
			//writeln("copying from:", Path with(other prefix, other key), " -> ", to)
			self db atPut(to, "")
			other next
		)
		self
	)
	
	removeAll := method(
		count := 0
		self first
		while(self key, self remove; self next; count = count + 1)
		count
	)
	transactionalRemoveAll := method(k, db begin; r := self removeAll; db commit; r)

	removeTo := method(k,
		count := 0
		self jump(k)
		while(self key, self remove; self previous; count = count + 1)
		count
	)
	transactionalRemoveTo := method(k, db begin; r := self removeTo(k); db commit; r)

	removeAfter := method(k,
		count := 0
		self jump(k)
		self next
		while(self key, self remove; self next; count = count + 1)
		count
	)
	transactionalRemoveAfter := method(k, db begin; r := self removeAfter(k); db commit; r)
	
	pairsAfter := method(k, count,
		if(count == nil, count = 10)
		pairs := list()
		if(k, self jump(k), self first)
		while(self key and count > 0, 
		//	writeln("key =[", self key, "]")
			pairs append(list(self key, self value))
			self next
			count = count - 1
		)
		pairs
	)
	
	calcSize := method(
		count := 0
		self first
		while(self key, count = count + 1; self next)
		count
	)
	
	sizePath := method(Path with(prefix, "_size"))
	
	size := method(
		s := db at(sizePath)
		if(s == nil, 
			s := calcSize
			self setSize(s)
		)
		s
	)
	
	setSize := method(s,
		//db atPut(sizePath, s asString)
		self
	)

	leaves := method(
		leaves := list()
		self first 
		while(self key, leaves append(self key); self next)
		leaves
	)
	
	subpaths := method(
		leaves map(beforeSeq("/")) unique
	)

	subpathCursors := method(
		subpaths map(p, db prefixCursor setPrefix(Path with(prefix, p)) setDb(db))
	)
		
	subpathSizePairs := method(
		subpathCursors map(c, list(c prefix, c size))
	)
		
	nextPath := method(
		if(key == nil, return nil)
		p := keyPath
		self next
		while(self key and self keyPath == p, // or self key beginsWithSeq("_") not), 
			writeln(self key, " subpath: [", self keyPath, "] calcPathSize ")
			self next
		)
		self key
	)
	
	keyPath := method(
		self key beforeSeq("/")
	)
	
	calcPathSize := method(
		db prefixCursor setDb(db) setPrefix(Path with(prefix, keyPath)) size 
	)
	
	pathSize := method(
		s := db at(Path with(prefix, keyPath, "_size"))
		if(s, s asNumber, calcPathSize)
	)
)
