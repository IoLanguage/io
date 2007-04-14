AddonBuilder clone do(
	dependsOnLib("event")
	dependsOnHeader("event.h")

	if(list("cygwin", "mingw") contains(platform),
		dependsOnLib("iphlpapi")
		dependsOnLib("ws2_32")
	)

        if(platform == "windows",
		dependsOnSysLib("iphlpapi")
		dependsOnSysLib("ws2_32")
	)


	if(platform == "sunos",
		dependsOnLib("nsl")
		dependsOnLib("socket")
	)

	debs    atPut("event", "libevent-dev")
	ebuilds atPut("event", "libevent")
	pkgs    atPut("event", "libevent")
	rpms    atPut("event", "libevent-devel")
)
