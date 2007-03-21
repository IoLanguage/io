#import <Cocoa/Cocoa.h>
#import "IoState.h"
#import "IoGLUT.h"

@interface GLIoView : NSOpenGLView
{
    IoState *ioState;
    NSTrackingRectTag trackingRectTag;
    NSMutableDictionary *timers;
    BOOL didInit;

    BOOL doesReshape;
    BOOL doesDisplay;
    BOOL doesMouse;
    BOOL doesMotion;
    BOOL doesKeyboard;
    BOOL doesEntry;
    BOOL doesPassiveMotion;

    BOOL canDrop;
    BOOL canDrag;

    //BOOL needsDisplay;

    BOOL FullScreenOn;
    NSWindow *FullScreenWindow;
    NSWindow *StartingWindow;

    IBOutlet NSMenuItem *mainMenu;
    IBOutlet NSMenuItem *aboutMenu;
    IBOutlet NSMenuItem *hideMenu;
    IBOutlet NSMenuItem *quitMenu;
}

+ sharedInstance;

- (void)drawRect:(NSRect)rect;
- (void)reshape;
- (void)setTrackingRect;
- (void)postRedisplay;

- (void)setReshape:(BOOL)b;
- (void)setDisplay:(BOOL)b;
- (void)setPassiveMotion:(BOOL)b;
- (void)setEntry:(BOOL)b;
- (void)setMouse:(BOOL)b;
- (void)setMotion:(BOOL)b;
- (void)setKeyboard:(BOOL)b;

- (void)runMain;

// Events 
- (void)mouseDown:(NSEvent *)event;
- (void)mouseUp:(NSEvent *)event;
- (void)keyDown:(NSEvent *)event;
- (void)keyUp:(NSEvent *)event;

// Other
- (void)mainLoop; 
- (void)doTimer:(id)timerId;
- (void)addTimer:(NSTimer *)aTimer;
- (IBAction)stopTimers:(id)sender;
- (IBAction)redisplay:(id)sender;

- (void)overrideIoGLMethods;

- (IBAction)toggleFullScreen:(id)sender;

@end
