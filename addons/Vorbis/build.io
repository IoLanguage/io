AddonBuilder clone do(
	dependsOnBinding("Ogg")
	dependsOnLib("vorbis")
	dependsOnHeader("vorbis/codec.h")
	
	kegs atPut("vorbis", "libvorbis")
)
