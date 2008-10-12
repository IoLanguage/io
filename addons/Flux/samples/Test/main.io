#!/usr/bin/env io

Flux

/*
gluPartialDiskSmooth := method(quadric, innerRadius, outerRadius, slices, loops, startAngle, sweepAngle,
    gluQuadricDrawStyle(quadric, GLU_FILL)
    gluPartialDisk(quadric, innerRadius, outerRadius, slices, loops, startAngle, sweepAngle)
    gluQuadricDrawStyle(quadric, GLU_LINE)
    gluPartialDisk(quadric, outerRadius, outerRadius, slices, loops, startAngle, sweepAngle)
)

glRoundedBox := method(w, h, r,
    glRecti(r, r,  w - r, h - r) // middle
    glRecti(0, r,  r, h - r) // left
    glRecti(w - r, r,  w, h - r) // right
    glRecti(r, h-r,  w - r, h) // top
    glRecti(r, 0, w-r, r) // bottom
     
    slices := r/3
    
    glPushMatrix
    glTranslatei(r, r, 0)
    gluPartialDiskSmooth(quadric, 0, r, slices, 1, 180, 90)
    glPopMatrix
    
    glPushMatrix
    glTranslatei(r, h-r, 0)
    gluPartialDiskSmooth(quadric, 0, r, slices, 1, 0, -90)
    glPopMatrix
    
    glPushMatrix
    glTranslatei(w-r, h-r, 0)
    gluPartialDiskSmooth(quadric, 0, r, slices, 1, 0, 90)
    glPopMatrix
    
    glPushMatrix
    glTranslatei(w-r, r, 0)
    gluPartialDiskSmooth(quadric, 0, r, slices, 1, 90, 90)
    glPopMatrix
)
*/

Application clone do(
    appDidStart := method(
        self setTitle("Test")
        b := Button clone
        b setTitle("Foo")
        b setWidth(300)
        mainWindow addSubview(b)
        
        b := View clone
        b setWidth(100) setHeight(100) 
        b position set(100, 200)
        b quadric := gluNewQuadric
        b draw := method(
            c := .25
            glColor4d(c, c, c, 1)
            slices := 100
            gluRoundedBox(quadric, size x, size y, 26, slices)
            glColor4d(0,0,0,.3)
            glColor4d(.1,.1,.1,1)
            glLineWidth(1)
            gluRoundedBoxOutline(quadric, size x, size y, 26, slices)
        )

        mainWindow contentView resizeWithSuperview                   
        mainWindow contentView addSubview(b)
        b resizeWithSuperview
        //mainWindow contentView draw := View getSlot("drawBackground")
        //mainWindow contentView sizeToSuperview
        resend
    )
) run
