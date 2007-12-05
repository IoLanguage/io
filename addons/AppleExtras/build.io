AddonBuilder clone do(
	supportedOnPlatform := method(platform, platform == "darwin")

	dependsOnFramework("CoreFoundation")
	dependsOnFramework("IOKit")
	dependsOnFramework("Carbon")
)
