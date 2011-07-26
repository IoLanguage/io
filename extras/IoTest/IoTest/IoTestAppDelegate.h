
#import <Cocoa/Cocoa.h>
#import <IoLanguageKit/IoLanguage.h>

@interface IoTestAppDelegate : NSObject <NSApplicationDelegate, NSTextViewDelegate> 
{
	NSWindow *window;
	IoLanguage *iolang;
	IBOutlet NSTextView *inputTextView;
	IBOutlet NSTextView *outputTextView;
	BOOL isPrinting;
	size_t lastIndex;
}

@property (assign) IBOutlet NSWindow *window;

@end
