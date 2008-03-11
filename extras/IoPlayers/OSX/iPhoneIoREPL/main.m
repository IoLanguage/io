//
//  main.m
//  iPhoneIoREPL
//
//  Created by Steve Dekorte on 20080311.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

int main(int argc, char *argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, @"iPhoneIoREPL");
    [pool release];
    return retVal;
}
