
DBIConn do (
	valueForSQL := method(arg,
		if (arg isKindOf(Sequence)) then(
			return self quote(arg asString)
		) elseif (arg isKindOf(Number),
			return arg asString
		) elseif (arg isKindOf(Date)) then(
			if (arg hour + arg minute + arg second == 0) then(
				return arg asString("'%Y-%m-%d'")
			) elseif (arg year + arg month + arg day == 1901) then(
				return arg asString("'%H:%M:%s'")
			) else (
				return arg asString("'%Y-%m-%d %H:%M:%S'")
			)
		) elseif (arg isKindOf(List)) then(
			l := List clone
			arg foreach(a, l append(self valueForSQL(a)))
			return l join(",")
		)
	)

	queryf := method(
		sql := call evalArgAt(0) asMutable

		for (a, 1, (call argCount) - 1,
			sql replaceFirstSeq("?", self valueForSQL(call evalArgAt(a)))
		)

		return self query(sql asString)
	)

	executef := method(
		sql := call evalArgAt(0) asMutable

		for (a, 1, (call argCount) - 1,
			sql replaceFirstSeq("?", self valueForSQL(call evalArgAt(a)))
		)

		return self execute(sql asString)
	)

	/*doc DBIConn queryOne([object], sql) 
	Perform a query and return the first
	result.

	If [object] is supplied, the object is populated with the contents of the
	result, the result is marked done and the object is returned.
	*/
		
	queryOne := method(
		first := call evalArgAt(0)
		obj := if (first isKindOf(Sequence) == false, first, nil)
		sql := if (obj isNil, first asMutable, call evalArgAt(1) asMutable)

		for (a, if (obj isNil, 1, 2), (call argCount) - 1,
			sql replaceFirstSeq("?", self valueForSQL(call evalArgAt(a)))
		)

		r := self query(sql .. " LIMIT 1")
		if (r size == 0, r done; return nil)

		r first
		if (obj, o := r populate(obj); r done; return o)
		return r
	)
)
