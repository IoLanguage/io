
#import <UIKit/UIKit.h>
#import "IoState.h"

@interface iPhoneIoREPL : NSObject 
{
	UIWindow *window;
	UITextView *output;
	UITextField *input;
	IoState *ioState;
}

@property (nonatomic, retain) UIWindow *window;

- (BOOL)textFieldShouldReturn:(UITextField *)textField;

- (void)setupInput;
- (void)setupOutput;
- (void)setupIo;

@end
