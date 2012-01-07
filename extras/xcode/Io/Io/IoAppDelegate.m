//
//  IoAppDelegate.m
//  Io
//
//  Created by Steve Dekorte on 20120104.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "IoAppDelegate.h"

@implementation IoAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Insert code here to initialize your application
	NSFont *font = [NSFont fontWithName:@"Times New Roman" size:14.0];
	NSDictionary *d = [NSDictionary dictionaryWithObject:font forKey:NSFontAttributeName];
//	[[textView textStorage] setFont:f];
	
	NSAttributedString *s = [[[NSAttributedString alloc] initWithString:@">" attributes:d] autorelease];
	[[textView textStorage] appendAttributedString:s];
	[textView setTextContainerInset:NSMakeSize(20, 20)];
	
}

@end
