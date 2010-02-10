AddonBuilder clone do(
	if(platform == "darwin",
		dependsOnFramework("CoreAudio")
		dependsOnFramework("AudioToolbox")
	)

	dependsOnLib("portaudio")
	dependsOnHeader("portaudio.h")

	debs    atPut("portaudio", "libportaudio-dev")
	ebuilds atPut("portaudio", "portaudio")
	pkgs    atPut("portaudio", "portaudio")
	rpms    atPut("portaudio", "portaudio-devel")
)

