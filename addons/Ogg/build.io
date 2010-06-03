AddonBuilder clone do(
	dependsOnLib("ogg")
	dependsOnHeader("ogg/ogg.h")
	
	kegs atPut("ogg", "libogg")
)
