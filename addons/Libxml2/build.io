AddonBuilder clone do(
	dependsOnLib("xml2")
	dependsOnHeader("libxml/xmlreader.h")
	dependsOnHeader("libxml/xmlwriter.h")
	headerSearchPaths foreach(headerSearchPath, appendHeaderSearchPath(headerSearchPath .. "/libxml2"))

	debs    atPut("xml2", "libxml2-dev")
	ebuilds atPut("xml2", "libxml2")
	pkgs    atPut("xml2", "libxml2")
	rpms    atPut("xml2", "libxml2-devel")
)
