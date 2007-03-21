
SQLite3 do(
	exists := method(File clone setPath(path) exists)
	delete := method(File clone setPath(path) remove)
)
