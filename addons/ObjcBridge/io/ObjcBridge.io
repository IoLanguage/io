
NSMakePoint := method(x, y, Point clone set(x, y))
NSMakeSize := method(w, h, Point clone set(w, h))
NSMakeRect := method(x, y, w, h, Box clone set(NSMakePoint(x, y), NSMakeSize(w, h)))

// AddressBook -----------------------------------

if(Addon platform != "darwin",
	ABAddressBook       := method(ObjcBridge classNamed("ADAddressBook"))
	ABGroup             := method(ObjcBridge classNamed("ADGroup"))
	ABMultiValue        := method(ObjcBridge classNamed("ADMultiValue"))
	ABMutableMultiValue := method(ObjcBridge classNamed("ADMutableMultiValue"))
	ABPerson            := method(ObjcBridge classNamed("ADPerson"))
	ABRecord            := method(ObjcBridge classNamed("ADRecord"))
	ABSearchElement     := method(ObjcBridge classNamed("ADSearchElement"))
)

//NSBox -----------------------

NSBoxPrimary   := 0
NSBoxSecondary := 1
NSBoxSeparator := 2
NSBoxOldStyle  := 3
NSBoxCustom    := 4

NSNoTitle     := 0
NSAboveTop    := 1
NSAtTop       := 2
NSBelowTop    := 3
NSAboveBottom := 4
NSAtBottom    := 5
NSBelowBottom := 6

//NSToolbarDisplayMode
NSToolbarDisplayModeDefault      := 0
NSToolbarDisplayModeIconAndLabel := 1
NSToolbarDisplayModeIconOnly     := 2
NSToolbarDisplayModeLabelOnly    := 3

//NSToolbarSizeMode
NSToolbarSizeModeDefault := 0
NSToolbarSizeModeRegular := 1
NSToolbarSizeModeSmall   := 2

YES := 1
NO  := 0

Object io2ObjcType := "[NOT an Io2Objc object]"

//ObjcBridge autoLookupClassNamesOn

Lobby forward := method(
	m := call message name
	v := ObjcBridge classNamed(m)
	if(v, return v)
	Importer import(call)
)

//writeln("adding search path: ", NSBundle mainBundle resourcePath)
Importer addSearchPath(NSBundle mainBundle resourcePath) // so we'll try to find protos in the App's resource folder

