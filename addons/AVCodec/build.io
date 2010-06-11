AddonBuilder clone do(
	if(platform == "darwin",
		dependsOnFramework("CoreAudio")
		dependsOnFramework("AudioToolbox")
		dependsOnHeader("libavcodec/avcodec.h")
		dependsOnHeader("libavformat/avformat.h")
	,
		dependsOnHeader("ffmpeg/avcodec.h")
	)

	dependsOnLib("avformat")
	dependsOnLib("avcodec")
	dependsOnLib("avutil")
        dependsOnLib("swscale")

	dependsOnLinkOption("-fno-common")

	debs    atPut("avcodec", "libavcodec-dev")
	debs    atPut("avformat", "libavformat-dev")
	ebuilds atPut("avcodec", "ffmpeg")
	pkgs    atPut("avcodec", "ffmpeg")
	rpms    atPut("avcodec", "ffmpeg-devel")
	kegs    atPut("avcodec", "ffmpeg")
)
