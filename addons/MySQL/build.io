AddonBuilder clone do(
	dependsOnHeader("mysql.h")

	dependsOnFrameworkOrLib("MySQL", "mysqlclient")
	headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/mysql"))

	ebuilds atPut("mysql5.0", "mysql")
)
