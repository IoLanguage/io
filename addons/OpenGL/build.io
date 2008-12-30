AddonBuilder clone do(
	if(list("cygwin", "mingw") contains(platform),
		dependsOnLib("winmm")
		dependsOnLib("glu32")
		dependsOnLib("glut32")
		dependsOnLib("opengl32")
	)

	if(platform == "windows",
		dependsOnSysLib("glut32")
		dependsOnSysLib("opengl32")
	)

	if(platform == "darwin",
		dependsOnFramework("GLUT")
		dependsOnFramework("OpenGL")
		dependsOnFramework("Carbon")
		dependsOnFramework("Foundation")
	)

	if(list("linux", "netbsd") contains(platform),
		dependsOnLib("GL")
		dependsOnLib("GLU")
		dependsOnLib("glut")
	)

	if (platform != "darwin" and platform != "windows",
		dependsOnHeader("GL/gl.h")
		dependsOnHeader("GL/glu.h")
		dependsOnHeader("GL/glut.h")
	)

	dependsOnBinding("Box")
	dependsOnBinding("Image")
	addDefine("GLEXPORT")

	debs    atPut("GL", "libgl1-mesa-dev")
	ebuilds atPut("GL", "mesa")
	rpms    atPut("GL", "Mesa-devel")

	debs    atPut("GLU", "libglu1-mesa-dev")
	ebuilds atPut("GLU", "mesa")
	rpms    atPut("GLU", "Mesa-devel")

	debs    atPut("glut", "freeglut3-dev")
	ebuilds atPut("glut", "media-libs/glut")
	pkgs    atPut("glut", "glut")
	rpms    atPut("glut", "Mesa-devel")
)
