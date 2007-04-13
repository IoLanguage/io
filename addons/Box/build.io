Binding clone do(
	if(platform == "darwin",
		dependsOnFramework("Accelerate")
		dependsOnLinkOption("-faltivec")
	)
)

