
#import "IoTestAppDelegate.h"
#import <IoLanguageKit/IoObjcBridge.h>
#import <IoLanguageKit/Objc2Io.h>
#import <IoLanguageKit/IoState.h>

@implementation IoTestAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{	
	[inputTextView setDelegate:self];
	[inputTextView setFont:[NSFont fontWithName:@"Courier" size:14.0]];
	[outputTextView setFont:[NSFont fontWithName:@"Courier" size:14.0]];
	//[outputTextView setDelegate:self];
	iolang = [[IoLanguage alloc] init];
	[iolang setDelegate:self];
	//id result = [iolang doString:@"1+1"];
	//NSLog(@"result = %@", result);
	//[textView insertText:@"io>"];
}

- (void)printCallback:(NSString *)s
{
	//isPrinting = YES;
	//NSLog(@"insert %@", [NSString stringWithFormat:@"%@", s]);
	[outputTextView	setEditable:YES];
	[outputTextView insertText:[NSString stringWithFormat:@"%@", s]];
	//[outputTextView	setEditable:NO];
	//[outputTextView display];
	//isPrinting = NO;
}

/*
- (void)markLastIndex
{
	lastIndex = [[[inputTextView textStorage] string] length];
}

- (NSString *)stringSinceLastIndex
{
	return @"";
}
*/

- (BOOL)textView:(NSTextView *)aTextView 
	shouldChangeTextInRange:(NSRange)affectedCharRange 
	replacementString:(NSString *)replacementString
{
	//if(isPrinting) return YES;
	
	if([replacementString isEqualToString:@"\n"])
	{
		
		id result = [iolang doString:[[inputTextView textStorage] string]];
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
	//NSLog(@"replacementString '%@'", replacementString);
	
	return YES;
}

/*
- (NSRange)textView:(NSTextView *)aTextView willChangeSelectionFromCharacterRange:(NSRange)oldSelectedCharRange toCharacterRange:(NSRange)newSelectedCharRange
{
	//([textView textStorage] length]
	return NSMakeRange(0, 0);
}
*/

@end
