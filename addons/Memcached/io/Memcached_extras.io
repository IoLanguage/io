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

	/*doc Memcached at(key, optionalDefaultValue)
	Asks memcached to retrieve data corresponding to the key.
	Returns nil if the data is not there (or if the data *is* nil).
	*/
	at := method(key, optionalDefaultValue,
		e := try(
			value := get(key)
		)

		e catch(Exception,
			if(e error == "NOT FOUND",
				value := if(optionalDefaultValue, optionalDefaultValue, nil),
				e pass
			)
		)

		value
	)

	/*doc Memcached atIfAbsentPut(key, value[, expiration])
	If a value is present at the specified key, its value is returned.
	Otherwise, inserts the new value and returns it.
	*/
	atIfAbsentPut := method(key, value, expiration,
		e := try(
			currValue := get(key)
		)

		e catch(Exception,
			if(e error == "NOT FOUND",
				set(key, value, if(expiration == nil, 0, expiration))
				currValue := value,
				e pass
			)
		)

		currValue
	)

	/*doc Memcached atPut(key, value[, expiration])
	Asks memcached to store the value identified by the key.
	Same as Memcached set, but returns self.
	*/
	atPut := method(key, value, expiration,
		set(key, value, if(expiration == nil, 0, expiration))
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
