AddonBuilder clone do(
	dependsOnLib("samplerate")
	dependsOnHeader("samplerate.h")

//	debs    atPut("samplerate", "libsamplerate-dev")
	ebuilds atPut("samplerate", "libsamplerate")
//	pkgs    atPut("samplerate", "libsamplerate")
	rpms    atPut("samplerate", "libsamplerate-devel")
)

