AddonBuilder clone do(
	dependsOnLib("readline")
	dependsOnLib("history")
	dependsOnHeader("readline/readline.h")
	dependsOnHeader("readline/history.h")
	dependsOnHeader("locale.h")
)
