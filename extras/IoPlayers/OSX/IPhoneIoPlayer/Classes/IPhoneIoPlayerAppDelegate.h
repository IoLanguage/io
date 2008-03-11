//
//  IPhoneIoPlayerAppDelegate.h
//  IPhoneIoPlayer
//
//  Created by Steve Dekorte on 20080310.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "IoState.h"

@class MyView;

@interface IPhoneIoPlayerAppDelegate : NSObject <UITextFieldDelegate>
{
	UITextView *output;
	UITextField *input;
    UIWindow *window;
    MyView *contentView;
	IoState *ioState;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) MyView *contentView;

@end
