PostgreSQL := SqlDatabase clone do(

	DbError := SqlDatabase DbError clone do(
		newSlot("pgsqlMessage")
		raise := method(message, pgsqlMessage,
			setPgsqlMessage(pgsqlMessage)
			super(raise(message))
		)
	)

	ConnectionError := DbError clone
	DatabaseError := DbError clone

	// Is the following compatible with Windows?
	libc := method(  // only needed for malloc and free, is there another way?
		PostgreSQL libc := DynLib clone setPath("libc." .. Addon dllSuffix) open
	)
	libpq := method(
		PostgreSQL libpq := DynLib clone setPath("libpq." .. Addon dllSuffix) open
	)

	libpqConsts := Object clone do(  // taken from PostgreSQL source file 'libpq-fe.h'
		CONNECTION_OK  := 0
		CONNECTION_BAD := 1
		PGRES_EMPTY_QUERY    := 0
		PGRES_COMMAND_OK     := 1
		PGRES_TUPLES_OK      := 2
		PGRES_COPY_OUT       := 3
		PGRES_COPY_IN        := 4
		PGRES_RAD_RESPONSE   := 5
		PGRES_NONFATAL_ERROR := 6  // not returned directly
		PGRES_FATAL_ERROR    := 7
	)

	oids := Object clone do(
		int2        := 21
		int4        := 23
		int8        := 20
		text        := 25
		varchar     := 1043
		numeric     := 1700
		float4      := 700
		float8      := 701
		date        := 1082
		timestamp   := 1114
		timestamptz := 1184
	)

	Connection := clone appendProto(SqlDatabase Connection) do(

		newSlot("host")
		newSlot("port", "5432")
		newSlot("database")
		newSlot("user")
		newSlot("password")
		newSlot("options")

		newSlot("internalConnection")

		internalCall := method(
			opened ifFalse(Exception raise("Database connection has not been opened."))
			args := call evalArgs
			functionName := "PQ" .. args removeFirst
			libpq performWithArgList("call",
				list(functionName, internalConnection) appendSeq(args)
			)
		)
		internalCallStr := method(
			libpq returnsString(performWithArgList("internalCall", call evalArgs))
		)

		errorMessage := method(internalCallStr("errorMessage"))

		opened := method(
			if(internalConnection == nil, false, true)
		)

		open := method(
			opened ifTrue(close)
			excpt := try(
				setInternalConnection(
					libpq call("PQsetdbLogin",
						if(host, host asString, ""),
						if(port, port asString, ""),
						if(options, options asString, ""),
						"",
						if(database, database asString, ""),
						if(user, user asString, ""),
						if(password, password asString, "")
					)
				)
				(internalCall("status") == libpqConsts CONNECTION_OK) ifFalse(
					ConnectionError raise(
						"Database connection could not be opened.",
						errorMessage
					)
				)
			)
			excpt catch(Exception, close)
			excpt pass
			self
		)

		close := method(
			opened ifTrue(
				internalCall("finish")
				setInternalConnection(nil)
			)
			self
		)

		escapeLiteral := method(string,
			string := string asString
			// Is there any nicier way to allocate temporary memory?
			buffer := libc call("malloc", string size * 2 + 1)
			(buffer == 0) ifTrue(Exception raise("Out of memory."))
			internalCall("escapeStringConn", buffer, string, string size, nil)
			result := libpq returnsString(buffer)
			libc call("free", buffer)
			result
		)

		// Method "quoteLiteral" is implemented in "SqlDatabase Connection"

		sqlFromTemplate := method(template, context,
			context isNil ifTrue(context = call sender)
			encapsulationContext := Object clone
			encapsulationContext doString := block(string,
				quoteLiteral(context doString(string))
			) setIsActivatable(true)
			template interpolate(encapsulationContext)
		)

		query := method(sql, careForResult,
			careForResult isNil ifTrue(careForResult := true)
			internalResult := internalCall("exec", sql)
			resultCall := block(
				args := call evalArgs
				functionName := "PQ" .. args removeFirst
				libpq performWithArgList("call",
					list(functionName, internalResult) appendSeq(args)
				)
			) setIsActivatable(true)
			resultCallStr := block(
				libpq returnsString(performWithArgList("resultCall", call evalArgs))
			) setIsActivatable(true)
			excpt := try(
				status := resultCall("resultStatus")
				// a segfauls happens between here
				list(libpqConsts PGRES_COMMAND_OK, libpqConsts PGRES_TUPLES_OK) contains(status) ifFalse(
					// and here
					DatabaseError raise(
						if(careForResult,
							"Database failed to execute requested query: #{sql}",
							"Database failed to execute requested command: #{sql}"
						) interpolate,
						errorMessage
					)
				)
				careForResult ifTrue(
					columnNames := List clone
					rawRows := List clone
					columnNum := resultCall("nfields")
					rowNum := resultCall("ntuples")
					rowNum repeat(rawRows append(List clone))
					columnNum repeat(columnIdx,
						columnNames append(resultCallStr("fname", columnIdx))
						columnType := resultCall("ftype", columnIdx)
						conversion := method(v, v)
						list(oids timestamptz) contains(columnType) ifTrue(
							// TODO: honour time zone
							// TODO: don't ignore microseconds
							conversion := method(v, v Date clone fromString(v, "%Y-%m-%d %H:%M:%S"))
						)
						list(oids timestamp) contains(columnType) ifTrue(
							// TODO: don't ignore microseconds
							conversion := method(v, v Date clone fromString(v, "%Y-%m-%d %H:%M:%S"))
						)
						list(oids date) contains(columnType) ifTrue(
							conversion := method(v, Date clone fromString(v, "%Y-%m-%d"))
						)
						list(oids numeric, oids float4, oids float8) contains(columnType) ifTrue(
							conversion := method(v, v asNumber)
						)
						list(oids int2, oids int4, oids int8) contains(columnType) ifTrue(
							conversion := method(v, v asNumber)
						)
						rowNum repeat(rowIdx,
							rawRows at(rowIdx) append(
								if(resultCall("getisnull", rowIdx, columnIdx) > 0, nil,
									conversion(resultCallStr("getvalue", rowIdx, columnIdx))
								)
							)
						)
					)
				)
			)
			resultCall("clear")
			excpt pass
			if(careForResult, DbResult with(columnNames, rawRows), self)
		)

		execute := method(sql,
			query(sql, false)
		)

	)

)
