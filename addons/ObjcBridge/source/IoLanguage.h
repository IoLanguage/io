//
//  IoLanguage.h
//  IoLanguageKit
//
//  Created by Stephen Dekorte on 20110720.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "IoState.h"

@interface IoLanguage : NSObject 
{
    IoState *state;
	id delegate;
}

- (void)setDelegate:anObject;
- delegate;

- doString:(NSString *)s;

@end
