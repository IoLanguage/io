AddonBuilder clone do(
	dependsOnLib("glfw")
	dependsOnHeader("GL/glfw.h")
	if(platform == "darwin", dependsOnFramework("AGL"))
	dependsOnBinding("OpenGL")

//	debs    atPut("glfw", "libglfw-dev")
	ebuilds atPut("glfw", "glfw")
//	pkgs    atPut("glfw", "glfw")
	rpms    atPut("glfw", "glfw-devel")
)
