GLUQuadric do(
	appendProto(OpenGL)
	
	texture := nil

	setTexture := method(anImage,
		if (anImage isKindOf(Image),
			self texture = anImage
			gluQuadricTexture(self, GL_TRUE)
			,
			self texture = nil
			gluQuadricTexture(self, GL_FALSE)
		)
		self
	)
	
	useFillStyle := method(gluQuadricDrawStyle(self, GLU_FILL); self)
	useLineStyle := method(gluQuadricDrawStyle(self, GLU_LINE); self)
	usePointStyle := method(gluQuadricDrawStyle(self, GLU_POINT); self)
	useSilhoutteStyle := method(gluQuadricDrawStyle(self, GLU_SILHOUETTE); self)
	
	useNoNormals := method(gluQuadricNormals(self, GLU_NONE); self)
	useFlatNormals := method(gluQuadricNormals(self, GLU_FLAT); self)
	useSmoothNormals := method(gluQuadricNormals(self, GLU_SMOOTH); self)
	
	useOutsideOrientation := method(gluQuadricOrientation(self, GLU_OUTSIDE); self)
	useInsideOrientation := method(gluQuadricOrientation(self, GLU_INSIDE); self)
)

