#import "GLIoView.h"
#import "IoGL.h"
#import "IoState.h"
#import "IoCFunction.h"

#import <OpenGL/gl.h>
#import <GLUT/glut.h>
#include <unistd.h>

static id sharedInstance = nil; 
static BOOL needsDisplay = YES; 

// --- Register for event notification calls ---

void GLIoView_postRedisplay(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    //[[GLIoView sharedInstance] postRedisplay];
    needsDisplay = YES;
}

void GLIoView_glutReshapeFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    [[GLIoView sharedInstance] setReshape:YES]; 
}

void GLIoView_glutDisplayFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    [[GLIoView sharedInstance] setDisplay:YES]; 
}

void GLIoView_glutEntryFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    [[GLIoView sharedInstance] setEntry:YES]; 
}

void GLIoView_glutMouseFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    [[GLIoView sharedInstance] setMouse:YES]; 
}

void GLIoView_glutMotionFunc(IoGL *self, IoObject *locals, IoMessage *m)
{
    [[GLIoView sharedInstance] setMotion:YES]; 
}

void GLIoView_glutKeyboardFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    [[GLIoView sharedInstance] setKeyboard:YES]; 
}

void GLIoView_glutPassiveMotionFunc(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    [[GLIoView sharedInstance] setPassiveMotion:YES]; 
}

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

// --- setter calls ---

void GLIoView_glutInitWindowSize(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    NSWindow *window = [[GLIoView sharedInstance] window];
    int width = IoMessage_locals_intArgAt_(m, locals, 0);
    int height = IoMessage_locals_intArgAt_(m, locals, 1);
    NSRect f = [window frame];
    
    f.size.width = width;
    f.size.height = height;
    printf("set window %p size %i %i\n", window, width, height);
    [window setFrame:f display:YES];
    [[GLIoView sharedInstance] reshape];
}

void GLIoView_glutInitWindowPosition(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    NSWindow *window = [[GLIoView sharedInstance] window];
    int x = IoMessage_locals_intArgAt_(m, locals, 0);
    int y = IoMessage_locals_intArgAt_(m, locals, 1);
    NSRect f = [window frame];
    
    f.origin.x = x;
    f.origin.y = y;
    printf("set window %p position %i %i\n", window, x, y);
    [window setFrame:f display:YES];
    //[[GLIoView sharedInstance] reshape];
}

// -----------------

void GLIoView_mainLoop(IoGL *self, IoObject *locals, IoMessage *m)
{ 
    [[GLIoView sharedInstance] mainLoop]; 
}

void GLIoView_noop(IoGL *self, IoObject *locals, IoMessage *m) {}

void IoDesktopInit(IoState *self, IoObject *context);

@implementation GLIoView

// ---------------------------------

+ (NSOpenGLPixelFormat *)defaultPixelFormat
{
    NSOpenGLPixelFormatAttribute attributes[] = {0};
    /*
     NSOpenGLPixelFormatAttribute attributes [] = 
     {
	 NSOpenGLPFAWindow,
	 //NSOpenGLPFADoubleBuffer, // double buffered
	 //NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)16, // 16 bit depth buffer
	 //NSOpenGLPFAAlphaSize, (NSOpenGLPixelFormatAttribute)8, // 8 bit alpha 
	 (NSOpenGLPixelFormatAttribute)nil
     };
     */
    return [[[NSOpenGLPixelFormat alloc] initWithAttributes:attributes] autorelease];
}

- initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)pf
{
    printf("--------------------- IoGLView initWithFrame:pixelFormat:\n");
    self = [super initWithFrame:frameRect pixelFormat:pf];
    return self;
}

- initWithFrame:(NSRect)frameRect
{
    printf("--------------------- IoGLView initWithFrame:\n");
    self = [super initWithFrame:frameRect 
		    pixelFormat:[GLIoView defaultPixelFormat]];
    return self;
}

- initWithCoder:(NSCoder *)c
{
    id r = [super initWithCoder:c];
    printf("--------------------- IoGLView initWithCoder:\n");
    timers = [[NSMutableDictionary alloc] init];
    [self runMain];
    return r;
}

- (void)awakeFromNib
{
    //[super initWithCoder:c];
    printf("--------------------- IoGLView awakeFromNib\n");
    //timers = [[NSMutableDictionary alloc] init];
    sharedInstance = self;
    [self setTrackingRect];
    [[self window] setDelegate:self];
    canDrag = NO;
    canDrop = YES;
    [self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    //[self runMain];
    
    //[self setAppName:@"Foo"];
    
    /*
     [[GLIoView sharedInstance] 
      performSelector:@selector(setAppName:) 
	   withObject:@"Foo" 
	   afterDelay:0.0001]; 
     */
}

- (BOOL)acceptsFirstResponder 
{ 
    return YES; 
}

- (BOOL)becomeFirstResponder  
{ 
    return YES; 
}

- (BOOL)resignFirstResponder  
{ 
    return YES; 
}

+ sharedInstance 
{
    return sharedInstance; 
}

void *DoIoFile(IoState *ioState, char *path, void *unused)
{
    return IoState_doFile_(ioState, path);
}

void *DoIoString(IoState *ioState, char *s, void *unused)
{
    return IoState_doCString_(ioState, s);
}

- (void)runMain
{ 
    char lp[256];
    NSString *path = [[NSBundle mainBundle] resourcePath];
    chdir([path cString]);
    if (didInit) return;
    didInit = YES;
    
    printf("path = %s\n", [path cString]);
    ioState = IoState_new();
    IoDesktopInit(ioState, ioState->lobby);
    [self overrideIoGLMethods];
    
    sprintf(lp, "launchPath := \"%s\"", [path cString]);
    printf("lp = [%s]\n", lp);
    //IoState_doCString_(ioState, lp);
    
    IoState_pushRetainPool(ioState);
    {
	IoException *e;
	IoCatch *catch = IoCatch_new(ioState);
	IoCatch_tryFunc_(catch, (IoCatchCallback *)DoIoString, (void *)ioState, (void *)lp, (void *)0x0, &e);
	IoCatch_free(catch);
	if (e) IoException_printBackTrace(e); 
    }
    
    path = [path stringByAppendingPathComponent:@"main.io"];
    //IoState_doFile_(ioState, (char *)[path cString]);
    
    {
	IoException *e;
	IoCatch *catch = IoCatch_new(ioState);
	IoCatch_tryFunc_(catch, (IoCatchCallback *)DoIoFile, (void *)ioState, (void *)[path cString], (void *)0x0, &e);
	IoCatch_free(catch);
	if (e) IoException_printBackTrace(e);
    }
    IoState_popRetainPool(ioState);
}

- (void)setAppName:(NSString *)appName
{
    [[NSApp mainMenu] setTitle:appName];
    [aboutMenu setTitle:[NSString stringWithFormat:@"About %@", appName]];
    [hideMenu setTitle:[NSString stringWithFormat:@"Hide %@", appName]];
    [quitMenu setTitle:[NSString stringWithFormat:@"Quit %@", appName]];
}

- (void)dealloc
{
    [timers autorelease];
    if (ioState) IoState_free(ioState);
    [super dealloc];
}

- (void)drawRect:(NSRect)rect 
{
    //[self setAppName:@"Foo"];
    //int w, h;
    //glViewport(0, 0, w, h);
    if (!didInit) [self runMain];
    //w = [super frame].size.width;
    //h = [super frame].size.height;
    /*
     if (!firstDisplayDone)
     {
	 firstDisplayDone = YES;
	 [self reshape];
     }
     */
    
    IoGlutDisplayFunc();
    glFlush();
}

- (float)height
{ return [super frame].size.height; }

- (void)reshape
{
    int w, h;
    //glViewport(0, 0, w, h);
    if (!didInit) [self runMain];
    w = [super frame].size.width;
    h = [super frame].size.height;
    
    //printf("reshape %i %i\n", w, h);
    IoGlutReshapeFunc(w, h);  
    
    [self setTrackingRect];
    //[super reshape];
}

- (void)setTrackingRect
{
    if (trackingRectTag) 
    { 
	[self removeTrackingRect:trackingRectTag]; 
    }
    
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

- (void)postRedisplay
{
    needsDisplay = YES;
}

- (void)updateDisplayIfNeeded
{
    if (needsDisplay) [self setNeedsDisplay:YES]; 
    needsDisplay = NO;
}

- (void)mouseDown:(NSEvent *)event
{
    if (doesMouse)
    {
	int x = [event locationInWindow].x;
	int y = [self height] - [event locationInWindow].y;
	IoGlutMouseFunc(0, 0, x, y);
	[self updateDisplayIfNeeded]; 
    }
    [[self window] makeFirstResponder:self];
}

- (void)mouseUp:(NSEvent *)event
{
    if (doesMouse)
    {
	int x = [event locationInWindow].x;
	int y = [self height] - [event locationInWindow].y;
	IoGlutMouseFunc(0, 1, x, y);
	if (needsDisplay) [self setNeedsDisplay:YES]; 
    }
}

- (void)mouseMoved:(NSEvent *)event
{
    if (doesPassiveMotion) 
    {
	int x = [event locationInWindow].x;
	int y = [self height] - [event locationInWindow].y;
	IoGlutPassiveMotionFunc(x, y);
	if (needsDisplay) [self setNeedsDisplay:YES]; 
    }
}

- (void)mouseEntered:(NSEvent *)event
{
    [[self window] makeFirstResponder:self];
    if (doesEntry) IoGlutEntryFunc(GLUT_ENTERED);
    if (needsDisplay) [self setNeedsDisplay:YES]; 
}

- (void)mouseExited:(NSEvent *)event
{
    if (doesEntry) IoGlutEntryFunc(GLUT_LEFT);
    if (needsDisplay) [self setNeedsDisplay:YES]; 
}

- (void)mouseDragged:(NSEvent *)event
{
    if (doesMotion)
    {
	int x = [event locationInWindow].x;
	int y = [self height] - [event locationInWindow].y;
	IoGlutMotionFunc(x, y);
	
	if (needsDisplay) 
	{
	    [self setNeedsDisplay:YES]; 
	}
    }
}

- (void)keyDown:(NSEvent *)event
{
    if (doesKeyboard)
    {
	NSString *k = [event characters];
	unsigned char key = [k cString][0];
	IoGlutKeyboardFunc(key, 0, 0);
	
	if (needsDisplay) 
	{
	    [self setNeedsDisplay:YES]; 
	}
    }
}

- (void)keyUp:(NSEvent *)event
{
    if (doesKeyboard)
    {
	NSString *k = [event characters];
	unsigned char key = [k cString][0];
	
	IoGlutKeyboardUpFunc(key, 0, 0);
	
	if (needsDisplay) 
	{
	    [self setNeedsDisplay:YES]; 
	}
    }
}

// ----------------------------------------------------------

- (void)doTimer:(id)timerId 
{
    int n = [[[timerId userInfo] objectForKey:@"id"] intValue];
    
    [timers removeObjectForKey:[[timerId userInfo] objectForKey:@"hash"]];
    IoGlutTimerFunc(n);
    
    if (needsDisplay) 
    {
	[self setNeedsDisplay:YES]; 
    }
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
    
    while (timer = [enm nextObject]) 
    { 
	[timer invalidate]; 
    }
    
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
    
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutDisplayFunc"),
			 IOCFUNCTION_GL(GLIoView_glutDisplayFunc));
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutReshapeFunc"), 
			 IOCFUNCTION_GL(GLIoView_glutReshapeFunc));
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutKeyboardFunc"), 
			 IOCFUNCTION_GL(GLIoView_glutKeyboardFunc));
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutKeyboardUpFunc"), 
			 IOCFUNCTION_GL(GLIoView_glutKeyboardFunc));
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutMouseFunc"),
			 IOCFUNCTION_GL(GLIoView_glutMouseFunc));
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutMotionFunc"), 
			 IOCFUNCTION_GL(GLIoView_glutMotionFunc));
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutPassiveMotionFunc"), 
			 IOCFUNCTION_GL(GLIoView_glutPassiveMotionFunc));
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutEntryFunc"), 
			 IOCFUNCTION_GL(GLIoView_glutEntryFunc));
    
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutVisibilityFunc"), noopfunc);
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutTimerFunc"), 
			 IOCFUNCTION_GL(GLIoView_registerTimer));
    
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutIdleFunc"), noopfunc);
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutPostRedisplay"), 
			 IOCFUNCTION_GL(GLIoView_postRedisplay));
    
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInit"), noopfunc);
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutMainLoop"), noopfunc);
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInitDisplayMode"), noopfunc);
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInitWindowSize"), 
			 IOCFUNCTION_GL(GLIoView_glutInitWindowSize));
    
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutCreateWindow"), noopfunc);
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInitWindowPosition"), 
			 IOCFUNCTION_GL(GLIoView_glutInitWindowPosition));
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutReshapeWindow"), 
			 IOCFUNCTION_GL(GLIoView_glutInitWindowPosition));
    
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutEventTarget"), 
			 IOCFUNCTION_GL(IoGLUT_glutEventTarget_));  
    
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutInitDisplayMode"), noopfunc);
    IoObject_setSlot_to_(cxt, IoState_symbolWithCString_(ioState, "glutSwapBuffers"), 
			 IOCFUNCTION_GL(GLIoView_postRedisplay));
}

/* 
    drop methods
	1. draggingEntered
	2. draggingUpdated
	3. draggingExited
	4. prepareForDragOperation
	5. performDragOperation
	6. concludeDragOperation
*/


- (unsigned int)draggingEntered:(id <NSDraggingInfo>)sender
{
    printf("draggingEntered\n");
    return NSDragOperationCopy; 

/*
    NSPasteboard *dragPasteboard = [sender draggingPasteboard];
    NSArray *types = [dragPasteboard types];
    //printf("draggingEntered:\n");
    
    if (!canDrop) 
    { 
	return NSDragOperationNone; 
    }
    
    if ([types containsObject:NSFilenamesPboardType])
    {
	 NSArray *paths = [dragPasteboard propertyListForType:NSFilenamesPboardType];
	 [self filePaths:paths];
	 
	 if ([delegate respondsToSelector:@selector(acceptsDrop:)])
	 { 
	     if (![delegate acceptsDrop:self]) { return NSDragOperationNone; } 
	 }
	return NSDragOperationCopy; 
    }
    
    return NSDragOperationNone;
*/
}

- (NSDragOperation)draggingUpdated:(id <NSDraggingInfo>)draggingInfo 
{
    NSPasteboard *dragPasteboard = [draggingInfo draggingPasteboard];
    NSArray *types = [dragPasteboard types];

    int result = 0;
    //printf("draggingUpdated\n");
    
    if ([types containsObject:NSFilenamesPboardType])
    {
	NSArray *paths = [dragPasteboard propertyListForType:NSFilenamesPboardType];
	NSEnumerator *pathEnum = [paths objectEnumerator];
	int x = [draggingInfo draggingLocation].x;
	int y = [draggingInfo draggingLocation].y;
	const char *type = "Filenames";
	ByteArray *ba = ByteArray_new();
	NSString *path;
	
	while (path = [pathEnum nextObject])
	{
	    ByteArray_appendCString_(ba, (char *)[path cString]);
	    ByteArray_appendByte_(ba, '\n');
	}
	
	//printf("calling IoGlutAcceptsDropFunc with: %s\n", ByteArray_bytes(ba));
	
	result = IoGlutAcceptsDropFunc(x, y, type, ByteArray_bytes(ba), ByteArray_size(ba));
	printf("result = %i\n", result);
	ByteArray_free(ba);
    }
    
    return result ? NSDragOperationCopy : NSDragOperationNone; 
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
    printf("draggingExited\n");
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
    printf("prepareForDragOperation\n");
    return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)draggingInfo
{
    NSPasteboard *dragPasteboard = [draggingInfo draggingPasteboard];
    NSArray *types = [dragPasteboard types];

    printf("performDragOperation\n");
    
    //printf("performDragOperation:\n");
    if ([types containsObject:NSFilenamesPboardType])
    {
	NSArray *paths = [dragPasteboard propertyListForType:NSFilenamesPboardType];
	NSEnumerator *pathEnum = [paths objectEnumerator];
	int x = [draggingInfo draggingLocation].x;
	int y = [draggingInfo draggingLocation].y;
	const char *type = "Filenames";
	ByteArray *ba = ByteArray_new();
	NSString *path;
	
	while (path = [pathEnum nextObject])
	{
	    ByteArray_appendCString_(ba, (char *)[path cString]);
	    ByteArray_appendByte_(ba, '\n');
	}
	
	printf("calling IoGlutDropFunc with: %s\n", ByteArray_bytes(ba));
	
	IoGlutDropFunc(x, y, type, ByteArray_bytes(ba), ByteArray_size(ba));
	ByteArray_free(ba);
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

- (BOOL)allowDropFrom:(NSPasteboard *)dragPasteboard
{
    /*
    NSArray *types = [dragPasteboard types];
    NSArray *paths;
    //printf("draggingEntered:\n");

    if (!canDrop) { return NSDragOperationNone; }

    if (![types containsObject:NSFilenamesPboardType])
    { return NSDragOperationNone; }

    paths = [dragPasteboard propertyListForType:NSFilenamesPboardType];

    if ( (!canDropMultiple) && [paths count] !=1 ) 
    { return NSDragOperationNone; }

    if ([delegate respondsToSelector:@selector(acceptsDropPaths:)]
    && ![delegate acceptsDropPaths:paths]) 
    { return NSDragOperationNone; }

    [self setImage:[[NSWorkspace sharedWorkspace] iconForFiles:paths]];
    */
    return NSDragOperationCopy; 
}


- (void)draggedImage:(NSImage *)anImage beganAt:(NSPoint)aPoint
{
    printf("draggedImage:beganAt:\n");
}

- (void)draggedImage:(NSImage *)anImage 
	     endedAt:(NSPoint)aPoint 
	   operation:(NSDragOperation)operation
{
    printf("draggedImage:endedAt:operation:\n");

}

- (void)draggedImage:(NSImage *)draggedImage 
	     movedTo:(NSPoint)screenPoint
{
    printf("draggedImage:movedTo:\n");

}

- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)flag
{
    printf("draggingSourceOperationMaskForLocal:\n");

    if ((!canDrag) || flag) 
    { 
	return NSDragOperationNone; 
    }
    
    return NSDragOperationCopy;
}

- (BOOL)ignoreModifierKeysWhileDragging 
{ 
    printf("ignoreModifierKeysWhileDragging:\n");
    return NO; 
}

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

// ------------------------------------------

- (IBAction)toggleFullScreen:(id)sender
{
    if( FullScreenOn == true ) // we need to go back to non-full screen
    {
        [FullScreenWindow close];
        [StartingWindow setContentView: self];
        [StartingWindow makeKeyAndOrderFront: self];
        [StartingWindow makeFirstResponder: self];
        FullScreenOn = false;
    }
    else // FullScreenOn == false
    {
        unsigned int windowStyle;
        NSRect       contentRect;
	
        StartingWindow = [NSApp keyWindow];
        windowStyle    = NSBorderlessWindowMask; 
        contentRect    = [[NSScreen mainScreen] frame];
	
        FullScreenWindow = [[NSWindow alloc] 
	    initWithContentRect:contentRect
		      styleMask: windowStyle 
			backing:NSBackingStoreBuffered 
			  defer:NO];
	
        if (FullScreenWindow != nil)
        {
            NSLog(@"Window was created");			
            [FullScreenWindow setTitle: @"myWindow"];			
            [FullScreenWindow setReleasedWhenClosed: YES];
            [FullScreenWindow setContentView: self];
            [FullScreenWindow makeKeyAndOrderFront:self ];
            [FullScreenWindow setLevel: NSScreenSaverWindowLevel - 1];
            [FullScreenWindow makeFirstResponder:self];
            FullScreenOn = true;
        }
    }
}


@end
