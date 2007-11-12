// A simple ObjcBridge script that let you run other Io scripts.
// This script may replace the main.io that's in /Projects/IoApp
// Recommended IoDesktop version: 20050818
// Written by Jon Kleiser, 18-Aug-2005

// This is a quick way to replace the one in the build:
// cp ./Projects/IoApp/main.io ./Projects/IoApp/build/IoApp.app/Contents/Resources/main.io

ObjcBridge autoLookupClassNamesOn

//ObjcBridge debugOn

nsApp := NSApplication sharedApplication
currentTarget := Object clone
currentActor := nil
stepBtn := nil

getCurrentActor := method(
	//writeln("getCurrentActor: ", currentActor)
	currentActor
)

setCurrentActor := method(a,
	//writeln("setCurrentActor: ", a)
	Lobby currentActor = a	// Lobby required! Why?
)

getSelectedFilepath := method(extension,
	// Opens a dialog where user can select a file
	fileTypes := if(extension, NSArray arrayWithObject:(extension), nil)
	panel := NSOpenPanel openPanel
	result := panel runModalForTypes:(fileTypes)
	return if(result == 1, panel filename, nil)
)

AppDelegate := Object clone
AppDelegate applicationDidFinishLaunching: := method(aNotification,
	writeln("applicationDidFinishLaunching\n")
)

AppDelegate doSelectedFile: := method(sender,
	path := getSelectedFilepath("io")
	writeln("doSelectedFile: ", path)
	if (path,
		e := try (
			result := currentTarget doFile(path)
			writeln("doFile result: ", result)
		)
		e catch (Exception,
			writeln("*** doSelectedFile: ", e name, ": ", e description)
		)
	)
)

AppDelegate yield: := method(sender,
	writeln("yield:")
	yield
)

AppDelegate step: := method(sender,
	writeln("step:")
	getCurrentActor ?resume
	setCurrentActor(nil)
	stepBtn setEnabled:(0)
	yield
)

AppDelegate controlTextShouldBeginEditing: := method(control, fieldEditor,
	writeln("controlTextShouldBeginEditing")
	1
)

WinDelegate := Object clone

frame := method(x, y, w, h,
	List clone append(Point clone set(x, y), Point clone set(w, h))
)

windowWidth := method(nsWin, nsWin contentView frame width)
btnHeight := 32

main := method(
	appDel := AppDelegate clone
	nsApp setDelegate:(appDel)

	win := NSWindow alloc
	win initWithContentRect:styleMask:backing:defer:(frame(200, 500, 420, 150), 15, 2, 0)
	win setTitle:("IoApp main.io v. 0.9.1")
	//win setLevel:(3)	// NSFloatingWindowLevel
	winDel := WinDelegate clone do(
	
		doText: := method(sender,
			str := self doField stringValue
			writeln("doText: ", str)
			e := try (
				result := currentTarget doString(str)
				writeln("doString result: ", result)
			)
			e catch (Exception,
				writeln("*** doText: ", e name, ": ", e description)
			)
		)
		
		windowDidResize: := method(notif,
			//writeln("windowDidResize ", windowWidth(notif object))
			doField setFrameSize:(Point clone set(windowWidth(notif object) - 30, 90))
		)
	)

	winDel windowWillClose: := method(notif,
		//writeln("windowWillClose")
		nsApp terminate:
	)
	
	win setDelegate:(winDel)

	txt := NSTextField alloc initWithFrame:(frame(15, 50, windowWidth(win) - 30, 90))
	win contentView addSubview:(txt)
	winDel doField := txt
	txt setStringValue:("writeln(11, 22)")
	//txt setEditable:(1)
	//writeln("txt isEditable = ", txt isEditable)

	doTextBtn := NSButton alloc initWithFrame:(frame(10, 10, 76, btnHeight))
	win contentView addSubview:(doTextBtn)
	doTextBtn setBezelStyle:(4)
	doTextBtn setTitle:("doText")
	doTextBtn setTarget:(winDel)
	doTextBtn setAction:("doText:")

	doFileBtn := NSButton alloc initWithFrame:(frame(90, 10, 76, btnHeight))
	win contentView addSubview:(doFileBtn)
	doFileBtn setBezelStyle:(4)
	doFileBtn setTitle:("doFile...")
	doFileBtn setTarget:(appDel)
	doFileBtn setAction:("doSelectedFile:")

	yieldBtn := NSButton alloc initWithFrame:(frame(170, 10, 76, btnHeight))
	win contentView addSubview:(yieldBtn)
	yieldBtn setBezelStyle:(4)
	yieldBtn setTarget:(appDel)
	yieldBtn setAction:("yield:")
	yieldBtn setTitle:("Yield")

	stepBtn = NSButton alloc initWithFrame:(frame(250, 10, 76, btnHeight))
	win contentView addSubview:(stepBtn)
	stepBtn setBezelStyle:(4)
	stepBtn setTarget:(appDel)
	stepBtn setAction:("step:")
	stepBtn setTitle:("Step")
	stepBtn setEnabled:(0)

	quitBtn := NSButton alloc initWithFrame:(frame(330, 10, 76, btnHeight))
	win contentView addSubview:(quitBtn)
	quitBtn setBezelStyle:(4)
	quitBtn setTarget:(nsApp)
	quitBtn setAction:("terminate:")
	quitBtn setTitle:("Quit")

	basso := NSSound alloc initWithContentsOfFile:byReference:("/System/Library/Sounds/Basso.aiff", 1)
	quitBtn setSound:(basso)

	win display
	win orderFrontRegardless

	nsApp run
)

Debugger vmWillSendMessage = method(actor, msg,
	writeln("vmWillSendMessage, actor uniqueId ", actor uniqueId, ": ", msg asString)
	setCurrentActor(actor)
	stepBtn setEnabled:(1)
)

main
