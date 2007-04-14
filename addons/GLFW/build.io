AddonBuilder clone do(
	dependsOnLib("glfw")
	dependsOnHeader("GL/glfw.h")
	if(platform == "darwin", dependsOnFramework("AGL"))
	dependsOnBinding("OpenGL")
	dependsOnBinding("Vector")

//	debs    atPut("glfw", "libglfw-dev")
	ebuilds atPut("glfw", "glfw")
//	pkgs    atPut("glfw", "glfw")
	rpms    atPut("glfw", "glfw-devel")
)
