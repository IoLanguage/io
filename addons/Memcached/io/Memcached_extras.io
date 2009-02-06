Memcached do(

	/*doc Memcached with(servers)
	Returns a new instance of Memcached.
	Accepts a Sequence or a List of server addresses.
	*/
	with := method(servers,
		m := self clone
		list(servers) flatten foreach(s, m addServer(s))
		m
	)

	/*doc Memcached at(key)
	Asks memcached to retrieve data corresponding to the key.
	Returns nil if the data is not there (or if the data *is* nil).
	*/
	at := method(key,
		e := try(
			value := get(key)
		) 

		e catch(Exception,
			if(e error == "NOT FOUND", value := nil, e pass)		
		)

		value
	)

	/*doc Memcached atPut(key, value)
	Asks memcached to store the value identified by the key.
	Returns self.
	*/
	atPut := method(key, value,
		set(key, value)
		self
	)

	/*doc Memcached removeAt(key)
	Asks memcached to remove value with a given key. Returns self.
	*/
	removeAt := method(key,
		delete(key)
		self
	)

)
