#import <OpenGL/glu.h>
#import "IoSaverView.h"
#import "GLIoView.h"
 
static NSString *MODULE_NAME = @"com.dekorte.IoSaver";

@implementation IoSaverView 

- (id)initWithFrame:(NSRect)frameRect isPreview:(BOOL) preview
{
  printf("---------------- IoSaverView init\n");
  if (!configureSheet) 
  { [NSBundle loadNibNamed:@"IoSaverConfigureSheet" owner:self]; }
  
  //[[NSColorPanel sharedColorPanel] setShowsAlpha:YES];
  _initedGL = NO;
  if (self = [super initWithFrame:frameRect isPreview:preview]) 
  {
    NSOpenGLPixelFormatAttribute attribs[] =
    {
      NSOpenGLPFAAccelerated,
      NSOpenGLPFAColorSize, 32,
      NSOpenGLPFADepthSize, 24,
      NSOpenGLPFAAlphaSize, 8,
      //NSOpenGLPFAStencilSize, 0,
      //NSOpenGLPFAAccumSize, 0,
      NSOpenGLPFAMinimumPolicy,
      NSOpenGLPFAClosestPolicy,
      0
    };
    NSOpenGLPixelFormat *format = 
      [[[NSOpenGLPixelFormat alloc] initWithAttributes:attribs] autorelease]; 
    _view = [[[GLIoView alloc] initWithFrame:NSZeroRect pixelFormat:format] autorelease];

    if ([_view respondsToSelector:@selector(awakeFromNib)])
    { [_view awakeFromNib]; }
    
    [self addSubview:_view];
    [[_view openGLContext] makeCurrentContext];

            
    if (!_initedGL) 
    {
      [self loadPrefs];
      [self initDemo];
      [self setPrefs];
      _initedGL = YES;
    }

    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
  }
  return self;
}

- (void)dealloc
{
  [_view removeFromSuperview];
  [super dealloc];
}

- (void)setFrameSize:(NSSize)newSize
{
  //int w = (int)[self frame].size.width;
  //int h = (int)[self frame].size.height;
        
  [super setFrameSize:newSize];
  [_view setFrameSize:newSize];
  [[_view openGLContext] makeCurrentContext];
  _initedGL = NO;
}

- (void)animateOneFrame
{
  if ([allScreensCheckbox intValue] || [[self window] screen]==[NSScreen mainScreen]) 
  {
    [[_view openGLContext] makeCurrentContext];
    if (!_initedGL) 
    {
      [self loadPrefs];
      [self initDemo];
      [self setPrefs];
      _initedGL = YES;
    }
    [self drawDemo];
  }
  else 
  {
    // secondary screen; remove OpenGL view and fill with black (only once)
    if (_view) 
    {
      [_view removeFromSuperview];
      _view = nil;
      [[NSColor blackColor] set];
      NSRectFill([self frame]);
    }
  }
}



- (void)stopAnimation
{
  // Do your animation termination here
  _initedGL = NO;
  [super stopAnimation];
}

// ===================================

- (void)initDemo
{
  //int w = (int)[self frame].size.width;
  //int h = (int)[self frame].size.height;
  
  _initedGL = YES;
}

- (void)drawDemo
{
  [[_view openGLContext] makeCurrentContext];
  [_view lockFocus];
  [_view drawRect:[_view frame]];
  [_view unlockFocus];
}

// ===================================

- (BOOL)hasConfigureSheet { return YES; }

- (NSWindow*)configureSheet
{
  [self loadPrefs];
  return configureSheet;
}

-(void)ok:(id)sender { [self save:nil]; }

-(void)save:(id)sender 
{
  [self savePrefs];
  [NSApp endSheet:configureSheet];
}

-(void)cancel:(id)sender { [NSApp endSheet:configureSheet]; }

// =======================================

-(void)loadPrefs 
{
    /*
  NSUserDefaults *defaults = [ScreenSaverDefaults 
    defaultsForModuleWithName:MODULE_NAME];

  [allScreensCheckbox setIntValue:
    intValueWithDefault([defaults objectForKey:@"useAllScreens"], 0)];
    
  if ([defaults colorForKey:@"IoSaverColor"])
  { [IoSaverColor setColor:[defaults colorForKey:@"IoSaverColor"]]; }
  
  if ([defaults colorForKey:@"backgroundColor"])
  { [backgroundColor setColor:[defaults colorForKey:@"backgroundColor"]]; }
  */
  [self setPrefs];
}

- (void)setPrefs
{

}
 
-(void)savePrefs 
{
  NSUserDefaults *defaults = [ScreenSaverDefaults 
    defaultsForModuleWithName:MODULE_NAME];

    /* 
  [defaults setInteger:[allScreensCheckbox intValue]
     forKey:@"useAllScreens"];

  [defaults setInteger:1 forKey:@"hasDefaults"];
  if ([IoSaverColor color])
  { [defaults setColor:[IoSaverColor color] forKey:@"IoSaverColor"]; }
  
  if ([backgroundColor color])
  { [defaults setColor:[backgroundColor color] forKey:@"backgroundColor"]; }
  */
    
  [defaults synchronize];
  [self setPrefs];
}

- (IBAction)visitSite:sender
{
  NSString *url = @"http://www.dekorte.com/downloads.html";
  NSURL *nsUrl = [NSURL URLWithString:url];
  [[NSWorkspace sharedWorkspace] openURL:nsUrl];
}

@end