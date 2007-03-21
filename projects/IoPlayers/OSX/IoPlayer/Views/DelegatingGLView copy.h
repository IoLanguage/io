#import <Cocoa/Cocoa.h>
#import <OpenGL/gl.h>

@interface DelegatingGLView : NSOpenGLView
{
  id delegate;
}

- setDelegate:anObject;
- delegate;

@end
