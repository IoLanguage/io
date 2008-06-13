AddonBuilder clone do(
	// For some reason, using the includes and headers in the default search
	// paths on Darwin leads to unresolved externals. Seems to be something
	// related to /usr/include/openssl/bio.h declaring BIO_method_type, 
	// among others, as macros instead of real functions.
	// This forces it to use the openssl port versions, which work.
	if(platform == "darwin",
		appendHeaderSearchPath("/opt/local/include")
		appendLibSearchPath("/opt/local")
		headerSearchPaths := headerSearchPaths reverse //reverse so that the user headers are used instead of the system headers
		libSearchPaths := libSearchPaths reverse //reverse so that the user libs are used instead of the system libs
		pkgs atPut("ssl", "openssl")
	)
	dependsOnHeader("openssl/ssl.h")
	dependsOnBinding("Socket")
	dependsOnLib("crypto")
	dependsOnLib("ssl")
)
