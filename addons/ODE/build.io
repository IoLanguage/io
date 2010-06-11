AddonBuilder clone do(
	dependsOnLib("ode")

	dependsOnBinding("OpenGL")

	debs    atPut("ode", "libode0-dev")
	ebuilds atPut("ode", "ode")
	pkgs    atPut("ode", "ode")
	rpms    atPut("ode", "ode-devel")
	kegs    atPut("ode", "ode")
)
