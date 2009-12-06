AddonBuilder clone do(
	dependsOnHeader("uuid/uuid.h")

	if(platform != "darwin",
		dependsOnLib("uuid")
	)

	debs atPut("uuid", "uuid-dev")
)
