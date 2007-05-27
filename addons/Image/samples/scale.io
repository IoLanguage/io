#!/usr/bin/env io


App := Object clone do(
	display := method(
		e := try(
			inFile  := args at(0)
			outFile := args at(1)
			w := args at(2) asNumber
			h := args at(3) asNumber
		)
		e catch(Exception,
			writeln("Usage: ioDesktop infile outfile width height")
			System exit
		)
		Image clone open(inFile) scaleTo(w, h) save(outFile)
		System exit
	)

	appendProto(OpenGL)

	run := method(
		glutInit
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH)
		glutInitWindowPosition(0, 0)
		glutInitWindowSize(100, 100)
		glutCreateWindow("Image")
		glutEventTarget(self)
		glutDisplayFunc
		glutMainLoop
	)
)

App run
