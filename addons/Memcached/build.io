AddonBuilder clone do(
	dependsOnLib("memcached")

	pkgs atPut("memcached", "libmemcached")
)
