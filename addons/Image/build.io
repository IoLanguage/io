AddonBuilder clone do(
	if(list("cygwin", "mingw") contains(platform),
		dependsOnLib("glu32")
		dependsOnLib("opengl32")
	)

	if(platform == "windows",
		dependsOnSysLib("glu32")
		dependsOnSysLib("opengl32")
	)

	if(platform == "darwin",
		dependsOnFramework("OpenGL")
	)

	if(platform == "linux",
		dependsOnLib("GL")
		dependsOnLib("GLU")
	)

	if (platform != "darwin",
		dependsOnHeader("zlib.h")
        )
	if (platform != "darwin" and platform != "windows",
		dependsOnHeader("GL/gl.h")
		dependsOnHeader("GL/glu.h")
	)

	dependsOnLib("z")
	dependsOnLib("png")
	dependsOnLib("tiff")
	dependsOnLib("jpeg")

	dependsOnHeader("png.h")
	dependsOnHeader("tiff.h")
	dependsOnHeader("jpeglib.h")

	debs    atPut("png", "libpng12-dev")
	ebuilds atPut("png", "libpng")
	pkgs    atPut("png", "libpng")
	rpms    atPut("png", "libpng-devel")

	debs    atPut("tiff", "libtiff4-dev")
	ebuilds atPut("tiff", "tiff")
	pkgs    atPut("tiff", "tiff")
	rpms    atPut("tiff", "libtiff-devel")

	debs    atPut("jpeg", "libjpeg62-dev")
	ebuilds atPut("jpeg", "jpeg")
	pkgs    atPut("jpeg", "jpeg")
	rpms    atPut("jpeg", "libjpeg-devel")
)
