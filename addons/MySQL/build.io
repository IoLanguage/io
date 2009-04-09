AddonBuilder clone do(
	if(platform == "darwin", 
		appendHeaderSearchPath("/opt/local/include/mysql5/mysql")
		appendLibSearchPath("/opt/local/lib/mysql5/mysql"),

		headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/mysql"))
	)

	dependsOnHeader("mysql.h")
	dependsOnFrameworkOrLib("MySQL", "mysqlclient")

	ebuilds atPut("mysql5.0", "mysql")
	pkgs    atPut("mysqlclient", "mysql5")
	debs	atPut("mysqlclient", "libmysqlclient15-dev")

)
