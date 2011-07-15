ObjcBridge debugOn

b := Object clone
a := NSMutableArray alloc init 
a addObject:(b)
a addObject:(123)
a count println
a objectAtIndex:(1) intValue println
