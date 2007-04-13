Binding clone do(
	if(list("cygwin", "mingw", "windows") contains(platform),
		dependsOnLib("curses")
		dependsOnHeader("curses.h")
	)

	if(list("darwin", "linux", "netbsd") contains(platform),
		dependsOnLib("ncurses")
		dependsOnHeader("ncurses.h")
	)

	debs    atPut("ncurses", "libncurses5-dev")
	ebuilds atPut("ncurses", "ncurses")
	pkgs    atPut("ncurses", "ncurses")
	rpms    atPut("ncurses", "ncurses-devel")
)
