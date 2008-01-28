AddonBuilder clone do(
	if(list("cygwin", "mingw") contains(platform),
		nil
	)

	if(platform == "windows",
		dependsOnSysLib("Iphlpapi")
	)

	if(platform == "darwin",
		dependsOnFramework("IoKit")
		dependsOnFramework("Carbon")
		dependsOnFramework("Foundation")
	)
)