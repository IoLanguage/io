#!/usr/bin/env io

App := Object clone do(
	display := method(
		args := System args
		e := try(
			inFile  := args at(1)
			outFile := args at(2)
			w := args at(3) asNumber
			h := args at(4) asNumber
		)
		e catch(Exception,
			writeln("Usage: io script.io infile outfile width height")
			System exit
		)
		//Image clone open(inFile) resizedTo(w, h) save(outFile)
		img := Image clone open(inFile) 
		
		//w = (img width) //floor 
		//h = (img height) //floor

		img resizedTo(w, h) save(outFile) 

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
