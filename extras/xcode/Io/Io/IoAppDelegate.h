//
//  IoAppDelegate.h
//  Io
//
//  Created by Steve Dekorte on 20120104.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface IoAppDelegate : NSObject <NSApplicationDelegate> 
{
	NSWindow *window;
	IBOutlet NSTextView *textView;
}

@property (assign) IBOutlet NSWindow *window;

@end
