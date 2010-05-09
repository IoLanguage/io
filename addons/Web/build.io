# This clone is _required_ for the build process to function correctly.
# However, it doesn't actually have to do anything at all.  Therefore,
# I've commented out the stuff that you'd normally see, since it really
# doesn't depend on anything not already provided in the NullAddon
# software.

AddonBuilder clone do(
/*
 	if(list("cygwin", "mingw", "windows") contains(platform),
 		dependsOnLib("C-library-name-here")
 		dependsOnHeader("C-header-file-here.h")
 	)
 
 	if(list("darwin", "linux", "netbsd") contains(platform),
 		dependsOnLib("C-library-name-here")
 		dependsOnHeader("C-header-file-here.h")
 	)
 
 	debs    atPut("package-name-here", "DistroPackageNameHere")
 	ebuilds atPut("package-name-here", "DistroPackageNameHere")
 	pkgs    atPut("package-name-here", "DistroPackageNameHere")
 	rpms    atPut("package-name-here", "DistroPackageNameHere")
*/
)

