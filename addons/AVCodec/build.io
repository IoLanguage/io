AddonBuilder clone do(
	if(platform == "darwin",
		dependsOnFramework("CoreAudio")
		dependsOnFramework("AudioToolbox")
	)

	dependsOnLib("avformat")
	dependsOnLib("avcodec")
	dependsOnLib("avutil")
        dependsOnLib("swscale")

	dependsOnHeader("ffmpeg/avcodec.h")
	dependsOnLinkOption("-fno-common")

	debs    atPut("avcodec", "libavcodec-dev")
	ebuilds atPut("avcodec", "ffmpeg")
	pkgs    atPut("avcodec", "subversion ffmpeg")
	rpms    atPut("avcodec", "ffmpeg-devel")
)
