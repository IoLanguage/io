#!/usr/bin/env io

ObjcBridge autoLookupClassNamesOn
ObjcBridge debugOn

IoConverter := Object clone
IoConverter setInput:  := method(v, self input := v)
IoConverter setOutput: := method(v, self output := v)
IoConverter convert:   := method(sender,
  output setIntValue:(input intValue * 2)
)

ObjcBridge newClassWithNameAndProto("Converter", IoConverter)


FooBar := Object clone
FooBar setConverter:  := method(v, self converter := v)
FooBar setText: := method(v, self text := v)
FooBar doSomething:   := method(sender,
  text setString:("Hello World!")
)

ObjcBridge newClassWithNameAndProto("Foobar", FooBar)

/*
ObjcObject := Object clone
ObjcObject init := method(name, 
  obj := Object clone
  ObjcBridge newClassWithNameAndProto(name, obj)
  return obj
)
*/
