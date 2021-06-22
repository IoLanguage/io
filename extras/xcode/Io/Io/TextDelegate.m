//
//  TextDelegate.m
//  Io
//
//  Created by Steve Dekorte on 20120104.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "TextDelegate.h"

@implementation TextDelegate

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

- (NSRange)textView:(NSTextView *)aTextView willChangeSelectionFromCharacterRange:(NSRange)oldSelectedCharRange toCharacterRange:(NSRange)newSelectedCharRange
{
	//NSLog(@"will change selection");
	return newSelectedCharRange;
}

- (BOOL)textView:(NSTextView *)aTextView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString *)replacementString
{
	NSLog(@"will change text '%@'", replacementString);
	return YES;
}

@end
