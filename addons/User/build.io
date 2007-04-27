AddonBuilder clone do(
	if(platform == "windows",
		dependsOnSysLib("advapi32")
		dependsOnSysLib("shell32")
	)
)
