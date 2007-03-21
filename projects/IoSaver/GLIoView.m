#import "GLIoView.h"
#import "IoGL.h"
#import "IoState.h"
#import "IoCFunction.h"

#import <OpenGL/gl.h>
#import <GLUT/glut.h>

static id sharedInstance = nil; 

// --- overload some of the IoGL methods ---

void GLIoView_postRedisplay(IoGL *self, IoObject *locals, IoMessage *m)
{ [[GLIoView sharedInstance] setNeedsDisplay:YES]; }

void GLIoView_glutReshapeFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ [[GLIoView sharedInstance] setReshape:YES]; }

void GLIoView_glutDisplayFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ [[GLIoView sharedInstance] setDisplay:YES]; }

void GLIoView_glutEntryFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ [[GLIoView sharedInstance] setEntry:YES]; }

void GLIoView_glutMouseFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ [[GLIoView sharedInstance] setMouse:YES]; }

void GLIoView_glutMotionFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ [[GLIoView sharedInstance] setMotion:YES]; }

void GLIoView_glutKeyboardFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ [[GLIoView sharedInstance] setKeyboard:YES]; }

void GLIoView_glutPassiveMotionFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ [[GLIoView sharedInstance] setPassiveMotion:YES]; }

void GLIoView_registerTimer(IoGL *self, IoObject *locals, IoMessage *m)
{ 
  int millisecs = IoMessage_locals_intArgAt_(m, locals, 0);
  int tid = IoMessage_locals_intArgAt_(m, locals, 1);
  id timerId = [NSNumber numberWithInt:tid];
  NSMutableDictionary *info = [NSMutableDictionary dictionary];
  id aTimer;
  
  [info setObject:timerId forKey:@"id"];
  
  //[NSTimer scheduledTimerWithTimeInterval:(NSTimeInterval)((float)millisecs)/1000.0
  aTimer = [NSTimer timerWithTimeInterval:(NSTimeInterval)((float)millisecs)/1000.0
    target:[GLIoView sharedInstance]
    selector:@selector(doTimer:)
    userInfo:info
    repeats:NO];
  [info setObject:[NSNumber numberWithInt:(int)[aTimer hash]] forKey:@"hash"];
  [[GLIoView sharedInstance] addTimer:aTimer];
}

// -----------------

void GLIoView_mainLoop(IoGL *self, IoObject *locals, IoMessage *m)
{ [[GLIoView sharedInstance] mainLoop]; }

void GLIoView_noop(IoGL *self, IoObject *locals, IoMessage *m) {}

void IoDesktopInit(IoState *self, IoObject *context);

@implementation GLIoView

- (id)initWithFrame:(NSRect)frameRect isPreview:(BOOL) preview
{
   int *a = 0x0;
   *a = 1;
   return self;
}

+ sharedInstance { return sharedInstance; }

- (void)runMain
{
  NSString *path = [[NSBundle bundleWithIdentifier:@"com.dekorte.IoSaver"] 
      pathForResource:@"main"
      ofType:@"io"];
      
  //NSString *path = [[[NSBundle mainBundle] resourcePath] 
  //  stringByAppendingPathComponent:@"main.io"];
  printf("path = %s\n", [path cString]);
  ioState = IoState_new();
  IoDesktopInit(ioState, ioState->lobby);
  [self overrideIoGLMethods];
  
  IoState_doFile_(ioState, (char *)[path cString]);
  didInit = YES;
}

- (void)awakeFromNib
{
  printf("--------------------- IoGLView awakeFromNib\n");
  timers = [[NSMutableDictionary alloc] init];
  sharedInstance = self;
  [self setTrackingRect];
  [[self window] setDelegate:self];
  canDrag = NO;
  canDrop = YES;
  [self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
}

- (void)dealloc
{
  [timers autorelease];
  if (ioState) IoState_free(ioState);
  [super dealloc];
}

- (void)drawRect:(NSRect)rect 
{
  //int w = [super frame].size.width;
  //int h = [super frame].size.height;
  //glViewport(0, 0, w, h);
  if (!didInit) [self runMain];
  IoGlutDisplayFunc();
  glFlush();
}

- (void)reshape
{
  int w = [super frame].size.width;
  int h = [super frame].size.height;
  glViewport(0, 0, w, h);
  if (!didInit) [self runMain];
  IoGlutReshapeFunc(w, h);  
  [self setTrackingRect];
  [super reshape];
}

- (void)setTrackingRect
{
  if (trackingRectTag) { [self removeTrackingRect:trackingRectTag]; }
  trackingRectTag = [self addTrackingRect:[super frame] 
    owner:self userData:nil assumeInside:NO];
}

- (void)setReshape:(BOOL)b       { doesReshape = b; }
- (void)setDisplay:(BOOL)b       { doesDisplay = b; }
- (void)setMouse:(BOOL)b         { doesMouse = b; }
- (void)setMotion:(BOOL)b        { doesMotion = b; }
- (void)setKeyboard:(BOOL)b      { doesKeyboard = b; }
- (void)setPassiveMotion:(BOOL)b { doesPassiveMotion = b; }
- (void)setEntry:(BOOL)b         { doesEntry = b; }

// --- Events -----------------------------------------

- (void)windowDidExpose:(NSNotification *)aNotification
{
    //IoGlutVisibilityFunc(GLUT_VISIBLE);
}

- (void)windowDidMiniaturize:(NSNotification *)aNotification
{
    //IoGlutVisibilityFunc(GLUT_NOT_VISIBLE);
}

- (void)mouseDown:(NSEvent *)event
{
  if (doesMouse)
  {
    int x = [event locationInWindow].x;
    int y = [event locationInWindow].y;
    IoGlutMouseFunc(1, 1, x, y);
  }
  [[self window] makeFirstResponder:self];
}

- (void)mouseUp:(NSEvent *)event
{
  if (doesMouse)
  {
    int x = [event locationInWindow].x;
    int y = [event locationInWindow].y;
    IoGlutMouseFunc(1, 0, x, y);
  }
}

- (void)mouseMoved:(NSEvent *)event
{
  if (doesPassiveMotion) 
  {
    int x = [event locationInWindow].x;
    int y = [event locationInWindow].y;
    IoGlutPassiveMotionFunc(x, y);
  }
}

- (void)mouseEntered:(NSEvent *)event
{
  [[self window] makeFirstResponder:self];
  if (doesEntry) IoGlutEntryFunc(GLUT_ENTERED);
}

- (void)mouseExited:(NSEvent *)event
{
  if (doesEntry) IoGlutEntryFunc(GLUT_LEFT);
}

- (void)mouseDragged:(NSEvent *)event
{
  if (doesMotion)
  {
    int x = [event locationInWindow].x;
    int y = [event locationInWindow].y;
    IoGlutMotionFunc(x, y);
  }
}

- (void)keyDown:(NSEvent *)event
{
  if (doesKeyboard)
  {
    NSString *k = [event characters];
    unsigned char key = [k cString][0];
    IoGlutKeyboardFunc(key, 0, 0);
  }
}

- (void)keyUp:(NSEvent *)event
{
  if (doesKeyboard)
  {
    NSString *k = [event characters];
    unsigned char key = [k cString][0];
    IoGlutKeyboardUpFunc(key, 0, 0);
  }
}

// ----------------------------------------------------------

- (void)doTimer:(id)timerId 
{
  int n = [[[timerId userInfo] objectForKey:@"id"] intValue];
  [timers removeObjectForKey:[[timerId userInfo] objectForKey:@"hash"]];
  IoGlutTimerFunc(n);
}

- (void)addTimer:(NSTimer *)aTimer
{
  [timers setObject:aTimer forKey:[NSNumber numberWithInt:(int)[aTimer hash]]];
  [[NSRunLoop currentRunLoop] addTimer:aTimer forMode:NSDefaultRunLoopMode];
}

- (IBAction)stopTimers:(id)sender
{
  id enm = [timers objectEnumerator];
  id timer;
  while (timer = [enm nextObject]) { [timer invalidate]; }
  [timers removeAllObjects];
}

- (IBAction)redisplay:(id)sender
{
  [self setNeedsDisplay:YES];
}

- (void)mainLoop
{
  [self reshape];
  [self setNeedsDisplay:YES];
}

#define IOCFUNCTION_GL(func) IoCFunction_newWithFunctionPointer_tag_(ioState, (void *)func, NULL)

- (void)overrideIoGLMethods  
{
  IoObject *cxt = IoState_doCString_(ioState, "GLUT");
  IoCFunction *noopfunc = IOCFUNCTION_GL(GLIoView_noop);
  
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutDisplayFunc"),
    IOCFUNCTION_GL(GLIoView_glutDisplayFunc));
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutReshapeFunc"), 
    IOCFUNCTION_GL(GLIoView_glutReshapeFunc));
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutKeyboardFunc"), 
    IOCFUNCTION_GL(GLIoView_glutKeyboardFunc));
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutKeyboardUpFunc"), 
    IOCFUNCTION_GL(GLIoView_glutKeyboardFunc));
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutMouseFunc"),
    IOCFUNCTION_GL(GLIoView_glutMouseFunc));
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutMotionFunc"), 
    IOCFUNCTION_GL(GLIoView_glutMotionFunc));
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutPassiveMotionFunc"), 
    IOCFUNCTION_GL(GLIoView_glutPassiveMotionFunc));
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutEntryFunc"), 
    IOCFUNCTION_GL(GLIoView_glutEntryFunc));
    
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutVisibilityFunc"), noopfunc);
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutTimerFunc"), 
    IOCFUNCTION_GL(GLIoView_registerTimer));
    
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutIdleFunc"), noopfunc);
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutPostRedisplay"), 
    IOCFUNCTION_GL(GLIoView_postRedisplay));

  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutInit"), noopfunc);
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutMainLoop"), noopfunc);
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutInitDisplayMode"), noopfunc);
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutInitWindowSize"), noopfunc);
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutCreateWindow"), noopfunc);
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutInitWindowPosition"), noopfunc);
  
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutEventTarget"), 
    IOCFUNCTION_GL(IoGLUT_glutEventTarget_));  
    
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutInitDisplayMode"), noopfunc);
  IoObject_setSlot_to_(cxt, IoState_stringWithCString_(ioState, "glutSwapBuffers"), 
    IOCFUNCTION_GL(GLIoView_postRedisplay));
}

// -- Drag ----------------------------------------

- (unsigned int)draggingEntered:(id <NSDraggingInfo>)sender
{
  NSPasteboard *dragPasteboard = [sender draggingPasteboard];
  NSArray *types = [dragPasteboard types];
  //printf("draggingEntered:\n");
  if (!canDrop) { return NSDragOperationNone; }
  
  if ([types containsObject:NSFilenamesPboardType])
  {
    /*
    NSArray *paths = [dragPasteboard propertyListForType:NSFilenamesPboardType];
    [self filePaths:paths];
          
    if ([delegate respondsToSelector:@selector(acceptsDrop:)])
    { 
        if (![delegate acceptsDrop:self]) { return NSDragOperationNone; } 
    }
    */
    return NSDragOperationCopy; 
  }

  
  return NSDragOperationNone;
}

- (NSDragOperation)draggingUpdated:(id <NSDraggingInfo>)sender 
{
  NSPasteboard *dragPasteboard = [sender draggingPasteboard];
  NSArray *types = [dragPasteboard types];
  if (!canDrop) { return NO; }
  /*
  if ([delegate respondsToSelector:@selector(acceptsDrop:)])
  { 
    if (![delegate acceptsDrop:self]) { return NO; } 
  }
  */
  if ([types containsObject:NSFilenamesPboardType])
  { return NSDragOperationCopy; }
  return NSDragOperationNone; 
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
  if (!canDrop) { return; }
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *dragPasteboard = [sender draggingPasteboard];
    NSArray *types = [dragPasteboard types];
    if (!canDrop) { return NO; }
    //printf("performDragOperation:\n");
    if ([types containsObject:NSFilenamesPboardType])
    {
      /*
      NSArray *paths = [dragPasteboard propertyListForType:NSFilenamesPboardType];
      [self filePaths:paths];
      if ([delegate respondsToSelector:@selector(droppedInWell:)])
      { [delegate droppedInWell:self]; }
      */
      return YES;
    }
    return NO;
}

// --- Drop -------------------------------------------------

/*
- (void)mouseDown:(NSEvent *)theEvent
{
  //NSSize dragOffset = NSMakeSize(0.0, 0.0);
  NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
  NSPoint locationInWindow = [theEvent locationInWindow];
  NSPoint imageLocation;
  id image = [self image];
  [image setSize:[self frame].size];
  if (!canDrag) { return; }
  imageLocation.x = locationInWindow.x - [self frame].origin.x;
  imageLocation.y = locationInWindow.y - [self frame].origin.y;
 
  [pboard declareTypes:[NSArray arrayWithObject:NSFilenamesPboardType] owner:self];
  [pboard setPropertyList:filePaths forType:NSFilenamesPboardType];
  //[pboard setData:[[self image] TIFFRepresentation] forType:NSTIFFPboardType];
  [self dragImage:[self image] at:imageLocation
    offset:NSMakeSize(0, 0) event:theEvent pasteboard:pboard source:self slideBack:YES];
}
*/

- (void)draggedImage:(NSImage *)anImage beganAt:(NSPoint)aPoint
{
}

- (void)draggedImage:(NSImage *)anImage 
  endedAt:(NSPoint)aPoint 
  operation:(NSDragOperation)operation
{
}

- (void)draggedImage:(NSImage *)draggedImage 
  movedTo:(NSPoint)screenPoint
{
}

- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)flag
{
  if ((!canDrag) || flag) { return NSDragOperationNone; }
  return NSDragOperationCopy;
}

- (BOOL)ignoreModifierKeysWhileDragging { return NO; }

// -----------

- (void)copy:sender
{
  /*
    NSPasteboard *pboard = [NSPasteboard pasteboardWithName:NSGeneralPboard];
    [pboard declareTypes:[NSArray arrayWithObject:NSTIFFPboardType] owner:self];
    [pboard setData:[image TIFFRepresentation] forType:NSTIFFPboardType];
  */
}

- (void)paste:sender
{
  printf("paste\n");

}

- (void)delete:sender
{
  printf("delete\n");
}

@end
