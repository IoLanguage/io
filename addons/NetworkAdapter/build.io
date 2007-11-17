AddonBuilder clone do(
	if(list("cygwin", "mingw") contains(platform),
		nil
	)

	if(platform == "windows",
		nil
	)

	if(platform == "darwin",
		dependsOnFramework("IOKit")
		dependsOnFramework("Carbon")
		dependsOnFramework("Foundation")
	)
)