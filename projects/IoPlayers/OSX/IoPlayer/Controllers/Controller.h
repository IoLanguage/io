#import <Cocoa/Cocoa.h>
#import "OutlineViewController.h"
#import "NSBrowserController.h"
#import "GLIoView.h"

@interface Controller : NSObject 
{
    NSString *projectPath;
    NSString *editorPath;
    IBOutlet NSDrawer *fileDrawer;
    IBOutlet NSText *sourceText;
    IBOutlet NSText *outputText;
    IBOutlet OutlineViewController *outlineViewController;
    IBOutlet GLIoView *glView;
    IBOutlet NSBrowserController *browserController;
    IBOutlet NSTextField *projectNameTextField;
    IBOutlet NSText *errorTextField;
    
    IBOutlet NSColorWell *sourceBackgroundColor;
    IBOutlet NSColorWell *sourceTextColor;
    
    IBOutlet NSColorWell *outputBackgroundColor;
    IBOutlet NSColorWell *outputTextColor;
}

- (void)appDidFinishLaunching:aNotification;

- (BOOL)openFile:(NSString *)filename;
- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)fileName;
- (BOOL)openFile:(NSPasteboard *)pboard userData:(NSString *)data error:(NSString **)error;

- (IBAction)changeProjectName:sender;
- (NSString *)projectName;
- (void)setProjectPath:(NSString *)filename;

- (void)setEditorPath:(NSString *)path;
- (NSString *)editorPath;

- (void)setDefaultProjectPath:(NSString *)path;
- (NSString *)defaultProjectPath;

- (IBAction)new:(id)sender;
- (IBAction)open:(id)sender;
- (IBAction)close:(id)sender;
- (IBAction)save:(id)sender;
- (IBAction)run:(id)sender;

- (void)addFolder:sender;
- (void)addFile:sender;

- (void)appendOutput:(NSString *)msg;

- (void)loadPrefs;
- (IBAction)changedPrefs:sender;

- (IBAction)setFont:sender;
- (void)changeFont:(id)fontManager;

@end

