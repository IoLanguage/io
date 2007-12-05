AddonBuilder clone do(
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
