Binding clone do(
	dependsOnLib("stdc++")
	dependsOnLib("SoundTouch")
	dependsOnHeader("soundtouch/SoundTouch.h")

	debs    atPut("SoundTouch", "libsoundtouch1-dev")
	ebuilds atPut("SoundTouch", "soundtouch")
	pkgs    atPut("SoundTouch", "libsoundtouch")
	rpms    atPut("SoundTouch", "soundtouch-devel")
)

