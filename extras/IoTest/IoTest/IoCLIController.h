
#import <Cocoa/Cocoa.h>
#import <IoLanguageKit/IoLanguageKit.h>

@interface IoCLIController : NSObject <NSApplicationDelegate, NSTextViewDelegate> 
{
	NSWindow *window;
	IBOutlet NSTextView *inputTextView;
	IBOutlet NSTextView *outputTextView;
}

@property (assign) IBOutlet NSWindow *window;

@end
