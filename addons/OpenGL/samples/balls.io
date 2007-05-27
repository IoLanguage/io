#!/usr/bin/env io

// glballs.io - A simple program
// (C) 2002 Mike Austin

GameObject := Object clone do(
  appendProto(OpenGL)
  // Initialize pos and angles
  init := method( x, y, z,
    write( "GameObject>init\n" )
    self x := if( x, x, 0 )
    self y := if( y, y, 0 )
    self z := if( z, z, 1 )
    self xvel := .01; self yvel := .01; self zvel := 0
    self zangle := 0; self zavel := -1
    self
  )

  // Advance the game object
  advance := method(
    zvel = zvel - .001

    x = x + xvel
    if( x >  1.3, xvel = (0 - xvel); x =  1.3 )
    if( x < -1.3, xvel = (0 - xvel); x = -1.3 )

    y = y + yvel
    if( y >  1.3, yvel = (0 - yvel); y =  1.3 )
    if( y < -1.3, yvel = (0 - yvel); y = -1.3 )

    z = z + zvel
    if( z < -1, zvel = (0 - zvel); z = -1 )

    zangle = zangle + zavel
  )

  // Transform and render object
  render := method(
    glPushMatrix
      glTranslated( x, z, y )
      glRotated(zangle, 0, 1, 0)
      renderObject
    glPopMatrix
  )
)

// A ball game object
Ball := GameObject clone do(
  appendProto(OpenGL)
  renderObject := method(
    glutSolidSphere( .2, 24, 32 )
  )
)

// A cube game object
Cube := GameObject clone do(
 appendProto(OpenGL)
 renderObject := method(
    glutSolidCube( .3 )
  )
)

// Define application object
GLBallsApp := Object clone do(
   appendProto(OpenGL)
 // Define glut events object
  Screen := Object clone do(
    // Create a couple models
    objs := List clone append(
      Ball clone,
      Ball clone( .5, -1, 2 ),
      Cube clone( -1, .5, 3 )
    )
    angleX := 22; angleY := 0
    lastX := 0; lastY := 0

    //Define reshape callback
    reshape := method( w, h,
      self width := w; self height := h

      glMatrixMode( GL_PROJECTION );
      glLoadIdentity();
      gluPerspective( 60, w / h, .5, 10.0 );
      glMatrixMode( GL_MODELVIEW );

      glViewport( 0, 0, w, h )
    )

    // Define display callback
    display := method(
      glClear( GL_COLOR_BUFFER_BIT | (GL_DEPTH_BUFFER_BIT) )

      glLoadIdentity
      glTranslated( 0, 0, -5 )
      glRotated( angleX, 1, 0, 0 )
      glRotated( angleY, 0, 1, 0 )

      // Iterate through the object list
      objs foreach( i, obj,
        obj render
      )

      glDisable( GL_LIGHTING )
      glutWireCube( 3 )
      glEnable( GL_LIGHTING )

      glFlush
      glutSwapBuffers
    )

    // Define timer callback
    timer := method( arg,
      objs foreach( i, obj,
        obj advance
      )
      glutPostRedisplay
      glutTimerFunc( arg, arg )
    )
  )

  // Mouse methods category
  Screen do(
   appendProto(OpenGL)
   // Define mouse buttons callback
    mouse := method( button, state, x, y,
      lastX = x; lastY = y
    )

    // Define mouse motion callback
    motion := method( x, y,
      angleX = angleX + (y - lastY)
      angleY = angleY + (x - lastX)
      lastX = x; lastY = y

      glutPostRedisplay
    )
  )

  // Define application run block
  run := method(
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize( 512, 512 )
    glutInit
    glutCreateWindow("Bouncing Balls")
    glutEventTarget( Screen )
    glutReshapeFunc; glutDisplayFunc
    glutMouseFunc; glutMotionFunc
    glutTimerFunc( 0, 10 )

    glClearColor( 0, 0, 0, 1 )
    glEnable( GL_DEPTH_TEST )
    glEnable( GL_LIGHTING ); glEnable( GL_LIGHT0 )

    glEnable( GL_BLEND )
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable( GL_LINE_SMOOTH ); glLineWidth( 1.5 )

    glutMainLoop
  )
)

// Create an GLBallsApp and run it
GLBallsApp run


