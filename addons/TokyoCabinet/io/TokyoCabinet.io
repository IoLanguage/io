
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

TokyoCabinetPrefixCursor prefix ::= nil