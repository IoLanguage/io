AddonBuilder clone do(
	dependsOnLib("edit")
	dependsOnHeader("histedit.h")
	
	debs    atPut("edit", "libedit-dev")
	
	/*
	// is this correct?
	ebuilds atPut("png", "libedit")
	pkgs    atPut("png", "libedit")
	rpms    atPut("png", "libedit")
	*/
)
