
#import <UIKit/UIKit.h>
#import "IoState.h"

@class MyView;

@interface iPhoneIoREPL : NSObject 
{
	UITextView *output;
	UITextField *input;
	UIWindow *window;
	MyView *contentView;
	IoState *ioState;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) MyView *contentView;

- (BOOL)textFieldShouldReturn:(UITextField *)textField;

- (void)setupInput;
- (void)setupOutput;
- (void)setupIo;

@end
