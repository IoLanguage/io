AddonBuilder clone do(
	if(platform == "darwin",
		dependsOnFramework("CoreAudio")
		dependsOnFramework("AudioToolbox")
	)

	dependsOnLib("sndfile")
	dependsOnHeader("sndfile.h")
	optionallyDependsOnLib("FLAC")

	debs    atPut("sndfile", "libsndfile1-dev")
	ebuilds atPut("sndfile", "libsndfile")
	pkgs    atPut("sndfile", "libsndfile")
	rpms    atPut("sndfile", "libsndfile-devel")
)

