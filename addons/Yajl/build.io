AddonBuilder clone do(
	dependsOnLib("yajl")
	dependsOnHeader("yajl/yajl_common.h")
	dependsOnHeader("yajl/yajl_gen.h")
	dependsOnHeader("yajl/yajl_parse.h")

	pkgs atPut("yajl", "yajl")
)
