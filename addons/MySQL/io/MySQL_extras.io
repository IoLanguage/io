
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
		tableNames map(name, SqlTable clone setDb(self) setName(name))
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

	columnNames := method(
		db query("SHOW columns FROM " .. name) map(first) flatten
	)

	show := method(
		writeln(list(name, columnNames))
	)
)
