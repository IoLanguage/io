AddonBuilder clone do(
	dependsOnLib("tokyocabinet")

	debs	atPut("tokyocabinet", "libtokyocabinet-dev")
	pkgs    atPut("tokyocabinet", "tokyocabinet")
)
