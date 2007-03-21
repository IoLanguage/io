#import <AppKit/AppKit.h>
#import <ScreenSaver/ScreenSaver.h>

@interface IoSaverView : ScreenSaverView 
{
    NSOpenGLView *_view;
    BOOL _initedGL;
            
    // outlets
    NSWindow *configureSheet;
    NSButton *allScreensCheckbox;
}

- (void)initDemo;
- (void)drawDemo;

// preferences methods
- (void)loadPrefs;
- (void)savePrefs;
- (void)setPrefs;

// configure sheet actions
- (IBAction)save:(id)sender;
- (IBAction)cancel:(id)sender;
- (IBAction)visitSite:sender;

@end
