Sequence do(
	glScale := method(OpenGL glScaled(x ifNilEval(1), y ifNilEval(1), z ifNilEval(1)))
	glTranslate := method(OpenGL glTranslated(x ifNilEval(0), y ifNilEval(0), z ifNilEval(0)))
)
