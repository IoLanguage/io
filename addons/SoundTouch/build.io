AddonBuilder clone do(
	//dependsOnLib("stdc++")
	dependsOnLib("stdc++" .. if(platform == "darwin", "-static",""))
	dependsOnLib("SoundTouch")
	dependsOnHeader("soundtouch/SoundTouch.h")

	debs    atPut("stdc++", "libstdc++6-dev")
	debs	atPut("SoundTouch", "libsoundtouch1-dev")
	ebuilds atPut("SoundTouch", "soundtouch")
	pkgs    atPut("SoundTouch", "soundtouch")
	rpms    atPut("SoundTouch", "soundtouch-devel")
)

