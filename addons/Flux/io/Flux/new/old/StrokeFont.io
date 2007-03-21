
Font = Object clone
Font parent = OpenGL
Font scale = 1
Font draw = block(aString, aSize, offsetX, offsetY, 
  scale copy(aSize)
  scale *= .006
  glPushMatrix
  glTranslated(offsetX, offsetY, 0)
  glScaled(scale, scale, 0)
  glutStrokeString(0, aString)
  glPopMatrix
)

