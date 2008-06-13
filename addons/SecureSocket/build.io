AddonBuilder clone do(
	appendHeaderSearchPath("/usr/local/ssl/include")
	appendLibSearchPath("/usr/local/ssl/lib")
	headerSearchPaths reverse //reverse so that the user headers are used instead of the system headers
	libSearchPaths reverse //reverse so that the user libs are used instead of the system libs
	dependsOnBinding("Socket")
/*
	dtls1Path := pathForHeader("openssl/dtls1.h")
	if(dtls1Path, //be sure that the newest of the new SSL libs is used
				  //this is to support my patched lib on /usr/local/ssl/,
				  //and to avoid using /opt/local/ssl/ or some other ssl
				  //by mistake.
		libLocation := pathForLib("crypto")
		if(File with(libLocation .. "/libcrypto." .. dllSuffix) exists,
			dependsOnLinkOption(libLocation .. "/libcrypto." .. dllSuffix)
		,
			if(File with(libLocation .. "/libcrypto.a") exists,
				dependsOnLinkOption(libLocation .. "/libcrypto.a")
			,
				if(File with(libLocation .. "/libcrypto.lib") exists,
					dependsOnLinkOption(libLocation .. "/libcrypto.lib")
				)
			)
		)
		libLocation := pathForLib("ssl")
		if(File with(libLocation .. "/libssl." .. dllSuffix) exists,
			dependsOnLinkOption(libLocation .. "/libssl." .. dllSuffix)
		,
			if(File with(libLocation .. "/libssl.a") exists,
				dependsOnLinkOption(libLocation .. "/libssl.a")
			,
				if(File with(libLocation .. "/libssl.lib") exists,
					dependsOnLinkOption(libLocation .. "/libssl.lib")
				)
			)
		)
		cflags := cflags .. " -DDTLS_IMPLEMENTED "
		,
*/
		dependsOnLib("crypto")
		dependsOnLib("ssl")
/*
	)
*/
)
