Binding clone do(
	dependsOnHeader("mysql.h")

	dependsOnFrameworkOrLib("MySQL", "mysqlclient")
	headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/mysql"))

	setIsServerBinding(true)

	ebuilds atPut("mysql5.0", "mysql")
)
