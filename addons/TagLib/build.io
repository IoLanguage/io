AddonBuilder clone do(
	dependsOnLib("tag_c")
	dependsOnHeader("taglib/tag_c.h")

	debs    atPut("tag_c", "libtagc0-dev")
	ebuilds atPut("tag_c", "taglib")
	pkgs    atPut("tag_c", "taglib")
	rpms    atPut("tag_c", "taglib-devel")
	kegs    atPut("tag_c", "taglib")
)

