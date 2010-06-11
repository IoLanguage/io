AddonBuilder clone do(
	dependsOnLib("cairo")
	dependsOnHeader("cairo.h")
	
	// add /usr/X11 to the search paths under darwin, because Snow Leopard provides cairo there
	if (platform == "darwin",
		appendHeaderSearchPath("/usr/X11/include")
		appendLibSearchPath("/usr/X11/lib")
	)
	// cairo has its headers in /usr/local/lib/cairo on my system.  Is this not strange?
	headerSearchPaths foreach(path, appendHeaderSearchPath(path .. "/cairo"))

	debs	atPut("cairo", "libcairo2-dev")
	pkgs	atPut("cairo", "cairo-devel")
	// the keg for this one is marked as keg_only because Snow Leopard provides it
	// Assume we're running on Snow Leopard and don't even ask Homebrew for it
)
