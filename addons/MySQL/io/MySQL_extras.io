
MySQL do(
	queryThenMap := method(queryString, self query(queryString, true))

	host ::= "localhost"
	user ::= "root"
	password ::= nil
	name ::= nil
	port ::= nil 
	socket ::= "/tmp/mysql.sock"

	tableNames := method(db query("SHOW tables") flatten)

	tables := method(
		self tables := Map clone
		tableNames foreach(name, tables atPut(name, SqlTable clone setDb(self) setName(name)))
	)

	_connect := getSlot("connect")
	connect := method(
		_connect(host, user, password, name, port, socket)
		self
	)
)

SqlTable := Object clone do(
	db ::= nil
	name ::= nil

	fetchColumnNames := method(
		self columnNames := db query("SHOW columns FROM " .. name) map(first) flatten
	)

	columnNames := method(
		self columnNames := fetchColumnNames
	)

	show := method(
		writeln(list(name, columnNames))
	)

	foreach := method(
		start := 0
		increment := 1000
		loop(
			write(" <sql"); File standardOutput flush
			rows := db queryThenMap("SELECT * FROM " .. name .. " LIMIT " .. start .. ", " .. increment)
			write(">"); File standardOutput flush
			if(rows size == 0, break)
			rows foreach(row, 
				call sender setSlot(call message arguments first name, row)
				call sender doMessage(call message arguments second, call sender)
				//b call(row)
			)
			start = start + increment
		)
	)
)
