//
//  IoLanguage.h
//  IoLanguageKit
//
//  Created by Stephen Dekorte on 20110720.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IoState.h"
#import "Objc2Io.h"

@interface IoLanguage : NSObject 
{
    IoState *state;
	id delegate;
}

+ shared;

- (void)setDelegate:anObject;
- delegate;

- (Objc2Io *)lobby;

- doString:(NSString *)s;

- runIoResource:(NSString *)name;
- runMain;

@end
