DisplayList := Object clone do(
	appendProto(OpenGL)

	id := lazySlot(glGenLists(1))

	begin := method(
		glNewList(id, GL_COMPILE)
		self
	)

	end := method(
		glEndList
		self
	)

	call := method(
		glCallList(id)
		self
	)

	willFree := method(
		glDeleteLists(id, 1)
	)
)
