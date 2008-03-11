//
//  main.m
//  IPhoneIoPlayer
//
//  Created by Steve Dekorte on 20080310.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

int main(int argc, char *argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
    int retVal = UIApplicationMain(argc, argv, nil, @"IPhoneIoPlayerAppDelegate");
    [pool release];
    return retVal;
}
