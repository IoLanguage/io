#import "IoCLIController.h"


@implementation IoCLIController

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{	
	[inputTextView setDelegate:self];
	[inputTextView setFont:[NSFont fontWithName:@"Courier" size:14.0]];
	[outputTextView setFont:[NSFont fontWithName:@"Courier" size:14.0]];
	[[IoLanguage shared] setDelegate:self];
}

- (void)printCallback:(NSString *)s
{
	[outputTextView	setEditable:YES];
	[outputTextView insertText:[NSString stringWithFormat:@"%@", s]];
}

- (BOOL)textView:(NSTextView *)aTextView 
	shouldChangeTextInRange:(NSRange)affectedCharRange 
	replacementString:(NSString *)replacementString
{
	//if(isPrinting) return YES;
	
	if([replacementString isEqualToString:@"\n"])
	{
		id result = [[IoLanguage shared] doString:[[inputTextView textStorage] string]];
		//NSLog(@"result = %@", result);
		[self printCallback:@"io> "];
		[self printCallback:[[inputTextView textStorage] string]];
		[self printCallback:@"\n"];
		[self printCallback:[NSString stringWithFormat:@"==> %@\n\n", result]];
		[[inputTextView textStorage] deleteCharactersInRange:
			NSMakeRange(0, [[[inputTextView textStorage] string] length])];
		[inputTextView display];
		[inputTextView setEditable:YES];
		//[textView insertText:@"io>"];
		return NO;
	}
	
	return YES;
}

/*
- (NSRange)textView:(NSTextView *)aTextView 
 willChangeSelectionFromCharacterRange:(NSRange)oldSelectedCharRange 
 toCharacterRange:(NSRange)newSelectedCharRange
{
	//([textView textStorage] length]
	return NSMakeRange(0, 0);
}
*/


@end
