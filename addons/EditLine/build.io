AddonBuilder clone do(
	dependsOnLib("edit")
	dependsOnHeader("histedit.h")
	
	/*
	// is this correct?
	debs    atPut("png", "libedit")
	ebuilds atPut("png", "libedit")
	pkgs    atPut("png", "libedit")
	rpms    atPut("png", "libedit")
	*/
)
