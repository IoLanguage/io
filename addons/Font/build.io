Binding clone do(
	dependsOnLib("freetype")
	dependsOnHeader("ft2build.h")
	dependsOnHeader("freetype/config/ftheader.h")

	if(list("cygwin", "mingw", "windows") contains(platform),
		dependsOnLib("opengl32")
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

	headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/freetype2"))

	debs    atPut("freetype", "libfreetype6-dev")
	ebuilds atPut("freetype", "freetype")
	pkgs    atPut("freetype", "freetype")
	rpms    atPut("freetype", "freetype-devel")
)
