Binding clone do(
	if(list("cygwin", "mingw", "windows") contains(platform) not,
		dependsOnHeader("pthread.h")
	)

	if(list("freebsd", "linux", "netbsd") contains(platform),
		dependsOnLib("pthread")
	)

	setIsServerBinding(true)
)
