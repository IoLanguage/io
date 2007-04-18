AddonBuilder clone do(
	if(platform != "darwin",
		GNUstepSystemRoot := System getenv("GNUSTEP_SYSTEM_ROOT") ifNilEval("/usr/GNUstep/System")
		GNUstepLocalRoot := System getenv("GNUSTEP_LOCAL_ROOT") ifNilEval("/usr/GNUstep/Local")
		appendHeaderSearchPath(GNUstepSystemRoot .. "/Library/Headers")
		appendHeaderSearchPath(GNUstepLocalRoot .. "/Library/Headers")
		appendLibSearchPath(GNUstepSystemRoot .. "/Library/Libraries")
		appendLibSearchPath(GNUstepLocalRoot .. "/Library/Libraries")
		dependsOnInclude("-fconstant-string-class=NSConstantString")
		optionallyDependsOnLib("Addresses")
		dependsOnHeader("AppKit/AppKit.h")
		dependsOnInclude("-DGNUSTEP")
		dependsOnLib("gnustep-gui")
	)

	if(platform == "darwin",
		optionallyDependsOnFramework("AddressBook")
		dependsOnFramework("AppKit")
	)

	dependsOnBinding("Vector")

	debs    atPut("gnustep-gui", "libgnustep-gui-dev")
	ebuilds atPut("gnustep-gui", "gnustep-gui")
	rpms    atPut("gnustep-gui", "libgnustep-gui-devel")
)
