// NeHe Tutorial #10
// See original source and C based tutorial at:
// http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=10
//
// Io port by Steve Dekorte, 2001
// 2004-10-03 Updated by Doc O'Leary
 
/*
    As with Lesson 9, this is essentially "from scratch" code.  Most of
    the concepts have been covered in previous lessons, though, and those
    that have not are more extensively documented.
*/

// A filter method for the world data that discards comments and blank lines.
File readstr := method(
    result := self readLine
    
    while(result == "" or result beginsWithSeq("//"),
        result = self readLine
    )

    return result
)

// Rather than peppering the piover180 constant everywhere, lets just
// add a conversion method to Number itself.
Number degreesToRadians := method(
    return self * .0174532925
)

// Define the object that represents a vertex.  Since a triangle is just
// a list of 3 vertices and a sector is just a list of triangles, we won't
// bother creating special objects for those.
Vertex := Object clone do(
    x := 0
    y := 0
    z := 0
    u := 0
    v := 0
)

Demo := Object clone
Demo appendProto(OpenGL)

// We need to set up an entirely new environment.
Demo do(
    blend := 0
    xpos := 0
    zpos := 0
    yrot := 0
    walkbias := 0
    walkbiasangle := 0
    lookupdown := 0
    
    filter := 0		
    texture := list(1, 2, 3)
    
    sector := List clone
)

// Here's where we read in the data file.  Io doesn't have sscanf(), but
// similar parsing with String is just as clean, if not cleaner.
Demo SetupWorld := method(
    worldFile := File clone openForReading(Path with(System launchPath, "World.txt"))
    //worldFile readLines selectInPlace(i, v, v != "" and v beginsWithSeq("//") == nil)
    numtriangles := worldFile readstr removePrefix("NUMPOLLIES ") asNumber
    
    for(triloop, 0, numtriangles - 1,
        triangle := list(Vertex clone, Vertex clone, Vertex clone)
        triangle foreach(i, vertex,
            worldValues := worldFile readstr splitNoEmpties(" ")
            
            vertex x = worldValues at(0) asNumber
            vertex y = worldValues at(1) asNumber
            vertex z = worldValues at(2) asNumber
            vertex u = worldValues at(3) asNumber
            vertex v = worldValues at(4) asNumber
        )
        sector append(triangle)
    )
    
    worldFile close
)

Demo reshape := method(w, h,
    glViewport(0, 0, w, h)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity
    gluPerspective(45, w/h, 0.1, 100)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity
)

// The only thing changed here is the use of the mud texture.
Demo LoadGLTextures := method(
    image := Image clone open(Path with(System launchPath, "mud.jpg"))
    
    glGenTextures(3, texture)
    
    glBindTexture(GL_TEXTURE_2D, texture at(0))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image width, image height, 0, GL_RGB, GL_UNSIGNED_BYTE, image data)
    
    glBindTexture(GL_TEXTURE_2D, texture at(1))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image width, image height, 0, GL_RGB, GL_UNSIGNED_BYTE, image data)
    
    glBindTexture(GL_TEXTURE_2D, texture at(2))
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST)
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image width, image height, GL_RGB, GL_UNSIGNED_BYTE, image data)
)

// Add support for reading in the world data.
Demo InitGL := method(
    self LoadGLTextures
    
    glEnable(GL_TEXTURE_2D)       
    glBlendFunc(GL_SRC_ALPHA, GL_ONE)	
    glClearColor(0, 0, 0, 0)
    glClearDepth(1)
    glDepthFunc(GL_LESS)
    glEnable(GL_DEPTH_TEST)
    glShadeModel(GL_SMOOTH)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)
    
    self SetupWorld
)

// Added support for wasd keys for looking, and ec for moving
Demo keyboard := method(key, x, y, 
    key := key asCharacter
    
    if(key == "f",  filter = filter + 1; if (filter > 2, filter = 0))
    
    if(key == "b", 
        blend = blend toggle
        if(blend == 0,
            glDisable(GL_BLEND); glEnable(GL_DEPTH_TEST)
        ,
            glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST)
        )
    )

    if(key == "d", yrot = yrot - 1.5)
    if(key == "a", yrot = yrot + 1.5)
    
    if(key == "w", lookupdown = lookupdown - 1)
    if(key == "s", lookupdown = lookupdown + 1)
            
    if(key == "e",
        xpos = xpos - (yrot degreesToRadians sin * .05) 
        zpos = zpos - (yrot degreesToRadians cos * .05)
        if (walkbiasangle >= 359,
            walkbiasangle = 0
        ,
            walkbiasangle = walkbiasangle + 10
        )
        walkbias = walkbiasangle degreesToRadians sin / 20
    )

    if(key == "c",
        xpos = xpos + (yrot degreesToRadians sin * .05)  
        zpos = zpos + (yrot degreesToRadians cos * .05)  
        if (walkbiasangle <= 1,
            walkbiasangle = 359
        ,
            walkbiasangle = walkbiasangle - 10
        )
        walkbias = walkbiasangle degreesToRadians sin / 20
    )
)

Demo display := method(    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity
    
    xtrans := -xpos
    ztrans := -zpos
    ytrans := -walkbias - 0.25
    sceneroty := 360 - yrot
    
    glRotated(lookupdown, 1, 0, 0)
    glRotated(sceneroty, 0, 1, 0)
    
    glTranslated(xtrans, ytrans, ztrans)
    glBindTexture(GL_TEXTURE_2D, texture at(filter))

    sector foreach(i, triangle, 
        glBegin(GL_TRIANGLES)
            glNormal3d(0, 0, 1)

            // first vertex of triangle 
            vertex := triangle at(0) 
            glTexCoord2d(vertex u, vertex v)
            glVertex3d(vertex x, vertex y, vertex z)

            // second vertex of triangle 
            vertex := triangle at(1) 
            glTexCoord2d(vertex u, vertex v)
            glVertex3d(vertex x, vertex y, vertex z)

            // third vertex of triangle 
            vertex := triangle at(2) 
            glTexCoord2d(vertex u, vertex v)
            glVertex3d(vertex x, vertex y, vertex z)
        glEnd
    )
    
    glutSwapBuffers
)

Demo timer := method(v, 
  self display
  glutTimerFunc(1, 0)
)

Demo main := method(
    glutInit
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(640, 480)
    glutCreateWindow("Jeff Molofee's GL Code Tutorial ... NeHe '99")
    glutEventTarget(self)
    
    self InitGL
    
    glutDisplayFunc
    glutReshapeFunc
    glutTimerFunc(10, 0)
    glutKeyboardFunc
    
    glutMainLoop 
)


Demo main
