AddonBuilder clone do(
	dependsOnHeader("gmp.h")
	dependsOnFrameworkOrLib("GMP", "gmp")

	debs    atPut("gmp", "libgmp3-dev")
	ebuilds atPut("gmp", "gmp")
	pkgs    atPut("gmp", "gmp")
	rpms    atPut("gmp", "gmp-devel")
)
