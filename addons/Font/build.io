AddonBuilder clone do(
	dependsOnLib("freetype")
	dependsOnHeader("ft2build.h")
	dependsOnHeader("freetype/config/ftheader.h")

	if(list("cygwin", "mingw") contains(platform),
		dependsOnLib("opengl32")
	)

	if(platform == "windows",
		dependsOnSysLib("opengl32")
	)

	if(platform == "darwin",
		dependsOnFramework("OpenGL")
	)

	if(platform == "linux",
		dependsOnLib("GL")
	)

	if(platform != "darwin",
		dependsOnHeader("GL/gl.h")
	)

	// add /usr/X11 to the search paths under darwin, because Snow Leopard provides freetype2 there
	if (platform == "darwin",
		appendHeaderSearchPath("/usr/X11/include")
		appendLibSearchPath("/usr/X11/lib")
	)
	headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/freetype2"))

	debs    atPut("freetype", "libfreetype6-dev")
	ebuilds atPut("freetype", "freetype")
	pkgs    atPut("freetype", "freetype")
	rpms    atPut("freetype", "freetype-devel")
)
