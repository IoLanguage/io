#import "DelegatingGLView.h"

@implementation DelegatingGLView

- (void)drawRect: (NSRect)rect 
{
    //Begin code from "OpenGL: Programming Guide" third edition, pg 6.
    //Comments are mine
    
    //Set the background to black
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    //Set the drawing color to white
    glColor3f(1.0, 1.0, 1.0);
    //Set the grid to use for drawing coordinates
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
    //draw a OpenGL polygon
    glBegin(GL_POLYGON);
        glVertex3f(0.25, 0.25, 0.0);
        glVertex3f(0.75, 0.25, 0.0);
        glVertex3f(0.75, 0.75, 0.0);
        glVertex3f(0.25, 0.75, 0.0);
    glEnd();
    //Flush the drawing commands, so it actually gets drawn
    glFlush();
    //End code from "OpenGL: Programming Guide" third edition, pg 6.
}

@end
