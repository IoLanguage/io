SqlDatabase := Object clone do(
	//metadoc SqlDatabase category Databases
	DbError := Exception clone

	ConnectionError := DbError clone
	DatabaseError := DbError clone

	Connection := clone do(
		//doc Connection escapeLiteral has to be implemented in sub-classes, which performs connection or database specific escaping of literals (e.g. converting an object to a string and adding backslashes before certain characters). Notice: According to http://www.postgresql.org/docs/8.2/static/runtime-config-compatible.html#GUC-STANDARD-CONFORMING-STRINGS using backslashes for escaping is not standard conform, and will be changed in future for PostgreSQL. Thus implementing a generic escaping function is a bad idea.

		//doc Connection quoteLiteral must be overwritten in sub-classes, in case other quotes than single-quotes are used to quote literals, or another string than "NULL" is used for NULL-values on a connection.
		quoteLiteral := method(obj,
			if(obj isNil, "NULL", "'" .. escapeLiteral(obj) .. "'")
		)

		sqlFromTemplate := method(template, context,
			context isNil ifTrue(context = call sender)
			encapsulationContext := Object clone
			encapsulationContext doString := block(string,
				quoteLiteral(context doString(string))
			) setIsActivatable(true)
			template interpolate(encapsulationContext)
		)

		//doc Connection query has to be implemented in sub-classes, which executes a query, returning a DbResult.

		queryTemplate := method(template, context,
			context isNil ifTrue(context = call sender)
			query(sqlFromTemplate(template, context))
		)

		//doc Connection execute(sql) Execute the given query. Returns self.
		execute := method(sql,
			query(sql)
			self
		)

		executeTemplate := method(template, context,
			context isNil ifTrue(context = call sender)
			execute(sqlFromTemplate(template, context))
		)

	)

	DbResult := List clone appendProto(thisContext) do(  
		// is "thisContext" a correct way to refer to "SqlDatabase" here?

		with := method(columnNames, rawRows,
			self clone setColumnNamesAndRawRows(columnNames, rawRows)
		)

		setColumnNamesAndRawRows := method(columnNames, rawRows,
			self columnNames := columnNames
			self rawRows := rawRows
			empty
			rawRows foreach(rawRow,
				row := Map clone
				columnNames foreach(columnIndex, columnName,
					row atIfAbsentPut(columnName, rawRow at(columnIndex))
				)
				append(row)
			)
			self
		)

	)

)
