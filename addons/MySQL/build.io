AddonBuilder clone do(
	if(platform == "darwin", 
		appendHeaderSearchPath("/opt/local/include/mysql5/mysql")
		appendHeaderSearchPath("/usr/local/mysql/include")
		//appendLibSearchPath("/usr/local/mysql/lib")
		appendLibSearchPath("/opt/local/lib/mysql5/mysql")
	,

		headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/mysql"))
	)

	dependsOnHeader("mysql.h")
	dependsOnFrameworkOrLib("MySQL", "mysqlclient")

	ebuilds atPut("mysql5.0", "mysql")
	pkgs    atPut("mysqlclient", "mysql5")

)
