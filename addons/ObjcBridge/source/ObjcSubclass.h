/*   Copyright (c) 2003, Steve Dekorte
docLicense("BSD revised")
 */

#include "Objc2Io.h"
#include "IoObject.h"

@interface ObjcSubclass : Objc2Io
{
}

+ (void)mark;
+ (Class)newClassNamed:(IoSymbol *)ioName proto:(IoObject *)proto;
+ alloc;
- (void)setProto;

@end
