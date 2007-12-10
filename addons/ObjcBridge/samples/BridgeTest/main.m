//
//  main.m
//  BridgeTest
// 
//  Created by Steve Dekorte on Sun Feb 16 2003.
//  Copyright (c) 2003 Steve Dekorte. All rights reserved.
//

#import <Cocoa/Cocoa.h> 

@interface IoLanguageInit : NSObject {}
- (int)main:(int)argc :(const char **)argv;
@end

int main(int argc, const char *argv[])  
{
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  NSBundle *appBundle = [NSBundle mainBundle];
  NSArray *bundlePaths = [appBundle pathsForResourcesOfType:@"bundle" inDirectory:nil];
  NSEnumerator *enm = [bundlePaths objectEnumerator];
  NSString *path;
  NSBundle *bundle;
  
  while (path = [enm nextObject])
  { 
    bundle = [NSBundle bundleWithPath:path];
    if ([[bundle bundleIdentifier] isEqual:@"org.iolanguage.IoLanguage"])
    {
      Class ioClass = [bundle classNamed:@"IoLanguageInit"];
      if (!ioClass) { printf("Unable to load IoLanguageInit class\n"); return -1; }
      return [[[ioClass alloc] init] main:argc :argv];
    }
  }
  if (!bundle) { printf("Unable to find IoLanguage bundle\n"); return -1; }
  [pool release];
  return -1;
}
