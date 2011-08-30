/*   Copyright (c) 2003, Steve Dekorte
//metadoc license BSD revised
 *
 *   A bridge between Io and Objective-C
 *   This object is a singleton that tracks
 *   the lists of proxies on each side.
 */

#ifndef IOOBJCBRIDGE_DEFINED
#define IOOBJCBRIDGE_DEFINED 1

#include "IoState.h"
#include "CHash.h"
#include "Runtime.h"

#define ISOBJCBRIDGE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoObjcBridge_rawClone)

typedef IoObject IoObjcBridge;

typedef struct
{
	CHash *io2objcs;
	CHash *objc2ios;

	union cValue_tag
	{
		id o;
		Class class;
		SEL sel;
		char c;
		unsigned char C;
		short s;
		unsigned short S;
		int i;
		unsigned int I;
		long l;
		unsigned long L;
		unsigned long long LL;
		float f;
		double d;
		// bitfield
		void *v;
		char *cp;
		unsigned char *ucp;
		NSPoint point;
		NSSize size;
		NSRect rect;
		CGPoint cgPoint;
		CGSize cgSize;
		CGRect cgRect;
	} cValue;

	char *methodNameBuffer;
	int methodNameBufferSize;
	BOOL debug;
	List *allClasses;
} IoObjcBridgeData;

IoObjcBridge *IoObjcBridge_sharedBridge(void);
List *IoObjcBridge_allClasses(IoObjcBridge *self);

IoObjcBridge *IoObjcBridge_rawClone(IoObjcBridge *self);
IoObjcBridge *IoObjcBridge_proto(void *state);
IoObjcBridge *IoObjcBridge_new(void *state);

void IoObjcBridge_free(IoObjcBridge *self);
void IoObjcBridge_mark(IoObjcBridge *self);

BOOL IoObjcBridge_rawDebugOn(IoObjcBridge *self);

IoObject *IoObjcBridge_clone(IoObjcBridge *self, IoObject *locals, IoMessage *m);

IoObject *IoObjcBridge_autoLookupClassNamesOn(IoObjcBridge *self, IoObject *locals, IoMessage *m);
IoObject *IoObjcBridge_autoLookupClassNamesOff(IoObjcBridge *self, IoObject *locals, IoMessage *m);

IoObject *IoObjcBridge_debugOn(IoObjcBridge *self, IoObject *locals, IoMessage *m);
IoObject *IoObjcBridge_debugOff(IoObjcBridge *self, IoObject *locals, IoMessage *m);

//IoObject *IoObjcBridge_NSSelectorFromString(IoObjcBridge *self, IoObject *locals, IoMessage *m);
//IoObject *IoObjcBridge_NSStringFromSelector(IoObjcBridge *self, IoObject *locals, IoMessage *m);

IoObject *IoObjcBridge_main(IoObjcBridge *self, IoObject *locals, IoMessage *m);

/* ----------------------------------------------------------------- */
IoObject *IoObjcBridge_classNamed(IoObjcBridge *self, IoObject *locals, IoMessage *m);

void *IoObjcBridge_proxyForId_(IoObjcBridge *self, id obj);
void *IoObjcBridge_proxyForIoObject_(IoObjcBridge *self, IoObject *v);

IoMessage *IoObjcBridge_ioMessageForNSInvocation_(IoObjcBridge *self, NSInvocation *invocation);

void IoObjcBridge_removeId_(IoObjcBridge *self, id obj);
void IoObjcBridge_removeValue_(IoObjcBridge *self, IoObject *v);
void IoObjcBridge_addValue_(IoObjcBridge *self, IoObject *v, id obj);

const char *IoObjcBridge_selectorEncoding(IoObjcBridge *self, SEL selector);

/* ----------------------------------------------------------------- */
IoObject *IoObjcBridge_ioValueForCValue_ofType_error_(IoObjcBridge *self, void *cValue, const char *cType, char **error);
void *IoObjcBridge_cValueForIoObject_ofType_error_(IoObjcBridge *self, IoObject *value, const char *cType, char **error);

/* --- method name buffer ----------------------------------- */
void IoObjcBridge_setMethodBuffer_(IoObjcBridge *self, char *name);
char *IoObjcBridge_ioMethodFor_(IoObjcBridge *self, char *name);
char *IoObjcBridge_objcMethodFor_(IoObjcBridge *self, char *name);

/* --- new classes -------------------------------------------- */
IoObject *IoObjcBridge_newClassNamed_withProto_(IoObjcBridge *self, IoObject *locals, IoMessage *m);

char *IoObjcBridge_nameForTypeChar_(IoObjcBridge *self, char type);

// We need this so the runtime will know the correct method signatures

@protocol AddressBook
//ABActionDelegate
- (NSString *)actionProperty;
- (void)performActionForPerson:(id)person identifier:(NSString *)identifier;
- (BOOL)shouldEnableActionForPerson:(id)person identifier:(NSString *)identifier;
- (NSString *)titleForPerson:(id)person identifier:(NSString *)identifier;

//ABImageClient
- (void)consumeImageData:(NSData *)data forTag:(int)tag;
@end

@protocol AppKit
//NSAccessibility
- (NSString *)accessibilityActionDescription:(NSString *)action;
- (NSArray *)accessibilityActionNames;
- (NSArray *)accessibilityAttributeNames;
- (id)accessibilityAttributeValue:(NSString *)attribute;
- (id)accessibilityAttributeValue:(NSString *)attribute forParameter:(id)parameter;
- (id)accessibilityFocusedUIElement;
- (id)accessibilityHitTest:(NSPoint)point;
- (BOOL)accessibilityIsAttributeSettable:(NSString *)attribute;
- (BOOL)accessibilityIsIgnored;
- (NSArray *)accessibilityParameterizedAttributeNames;
- (void)accessibilityPerformAction:(NSString *)action;
- (void)accessibilitySetValue:(id)value forAttribute:(NSString *)attribute;

//NSAccessibilityAdditions
- (BOOL)accessibilitySetOverrideValue:(id)value forAttribute:(NSString *)attribute;

//NSAlertDelegate
- (BOOL)alertShowHelp:(id)alert;

//NSAnimationDelegate
- (void)animation:(id)animation didReachProgressMark:(float)progress;
- (float)animation:(id)animation valueForProgress:(float)progress;
- (void)animationDidEnd:(id)animation;
- (void)animationDidStop:(id)animation;
- (BOOL)animationShouldStart:(id)animation;

//NSApplicationDelegate
- (BOOL)application:(NSApplication *)sender openFile:(NSString *)filename;
- (BOOL)application:(id)sender openFileWithoutUI:(NSString *)filename;
- (void)application:(NSApplication *)sender openFiles:(NSArray *)filenames;
- (BOOL)application:(NSApplication *)sender openTempFile:(NSString *)filename;
- (BOOL)application:(NSApplication *)sender printFile:(NSString *)filename;
- (void)application:(NSApplication *)sender printFiles:(NSArray *)filenames;
- (int)application:(NSApplication *)application printFiles:(NSArray *)fileNames withSettings:(NSDictionary *)printSettings showPrintPanels:(BOOL)showPrintPanels;
- (id)application:(NSApplication *)application willPresentError:(id)error;
- (NSMenu *)applicationDockMenu:(NSApplication *)sender;
- (BOOL)applicationOpenUntitledFile:(NSApplication *)sender;
- (BOOL)applicationShouldHandleReopen:(NSApplication *)sender hasVisibleWindows:(BOOL)flag;
- (BOOL)applicationShouldOpenUntitledFile:(NSApplication *)sender;
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender;

//NSApplicationNotifications
- (void)applicationDidBecomeActive:(NSNotification *)notification;
- (void)applicationDidChangeScreenParameters:(NSNotification *)notification;
- (void)applicationDidFinishLaunching:(NSNotification *)notification;
- (void)applicationDidHide:(NSNotification *)notification;
- (void)applicationDidResignActive:(NSNotification *)notification;
- (void)applicationDidUnhide:(NSNotification *)notification;
- (void)applicationDidUpdate:(NSNotification *)notification;
- (void)applicationWillBecomeActive:(NSNotification *)notification;
- (void)applicationWillFinishLaunching:(NSNotification *)notification;
- (void)applicationWillHide:(NSNotification *)notification;
- (void)applicationWillResignActive:(NSNotification *)notification;
- (void)applicationWillTerminate:(NSNotification *)notification;
- (void)applicationWillUnhide:(NSNotification *)notification;
- (void)applicationWillUpdate:(NSNotification *)notification;

//NSApplicationScriptingDelegation
- (BOOL)application:(NSApplication *)sender delegateHandlesKey:(NSString *)key;

//NSBrowserDelegate
- (void)browser:(NSBrowser *)sender createRowsForColumn:(int)column inMatrix:(NSMatrix *)matrix;
- (BOOL)browser:(NSBrowser *)sender isColumnValid:(int)column;
- (int)browser:(NSBrowser *)sender numberOfRowsInColumn:(int)column;
- (BOOL)browser:(NSBrowser *)sender selectCellWithString:(NSString *)title inColumn:(int)column;
- (BOOL)browser:(NSBrowser *)sender selectRow:(int)row inColumn:(int)column;
- (float)browser:(NSBrowser *)browser shouldSizeColumn:(int)columnIndex forUserResize:(BOOL)forUserResize toWidth:(float)suggestedWidth;
- (float)browser:(NSBrowser *)browser sizeToFitWidthOfColumn:(int)columnIndex;
- (NSString *)browser:(NSBrowser *)sender titleOfColumn:(int)column;
- (void)browser:(NSBrowser *)sender willDisplayCell:(id)cell atRow:(int)row column:(int)column;
- (void)browserColumnConfigurationDidChange:(NSNotification *)notification;
- (void)browserDidScroll:(NSBrowser *)sender;
- (void)browserWillScroll:(NSBrowser *)sender;

//NSChangeSpelling
- (void)changeSpelling:(id)sender;

//NSColorPanelResponderMethod
- (void)changeColor:(id)sender;

//NSColorPickingCustom
- (int)currentMode;
- (NSView *)provideNewView:(BOOL)initialRequest;
- (void)setColor:(NSColor *)newColor;
- (BOOL)supportsMode:(int)mode;

//NSColorPickingDefault
- (void)alphaControlAddedOrRemoved:(id)sender;
- (void)attachColorList:(NSColorList *)colorList;
- (void)detachColorList:(NSColorList *)colorList;
- (id)initWithPickerMask:(int)mask colorPanel:(NSColorPanel *)owningColorPanel;
- (void)insertNewButtonImage:(NSImage *)newButtonImage in:(NSButtonCell *)buttonCell;
- (NSImage *)provideNewButtonImage;
- (void)setMode:(int)mode;
- (void)viewSizeChanged:(id)sender;

//NSComboBoxCellDataSource
- (NSString *)comboBoxCell:(NSComboBoxCell *)aComboBoxCell completedString:(NSString *)uncompletedString;
- (unsigned int)comboBoxCell:(NSComboBoxCell *)aComboBoxCell indexOfItemWithStringValue:(NSString *)string;
- (id)comboBoxCell:(NSComboBoxCell *)aComboBoxCell objectValueForItemAtIndex:(int)index;
- (int)numberOfItemsInComboBoxCell:(NSComboBoxCell *)comboBoxCell;

//NSComboBoxDataSource
- (NSString *)comboBox:(NSComboBox *)aComboBox completedString:(NSString *)string;
- (unsigned int)comboBox:(NSComboBox *)aComboBox indexOfItemWithStringValue:(NSString *)string;
- (id)comboBox:(NSComboBox *)aComboBox objectValueForItemAtIndex:(int)index;
- (int)numberOfItemsInComboBox:(NSComboBox *)aComboBox;

//NSComboBoxNotifications
- (void)comboBoxSelectionDidChange:(NSNotification *)notification;
- (void)comboBoxSelectionIsChanging:(NSNotification *)notification;
- (void)comboBoxWillDismiss:(NSNotification *)notification;
- (void)comboBoxWillPopUp:(NSNotification *)notification;

//NSControlSubclassDelegate
- (BOOL)control:(NSControl *)control didFailToFormatString:(NSString *)string errorDescription:(NSString *)error;
- (void)control:(NSControl *)control didFailToValidatePartialString:(NSString *)string errorDescription:(NSString *)error;
- (BOOL)control:(NSControl *)control isValidObject:(id)obj;
- (BOOL)control:(NSControl *)control textShouldBeginEditing:(NSText *)fieldEditor;
- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor;
- (NSArray *)control:(NSControl *)control textView:(NSTextView *)textView completions:(NSArray *)words forPartialWordRange:(NSRange)charRange indexOfSelectedItem:(int *)index;
- (BOOL)control:(NSControl *)control textView:(NSTextView *)textView doCommandBySelector:(SEL)commandSelector;

//NSControlSubclassNotifications
- (void)controlTextDidBeginEditing:(NSNotification *)obj;
- (void)controlTextDidChange:(NSNotification *)obj;
- (void)controlTextDidEndEditing:(NSNotification *)obj;

//NSDatePickerCellDelegate
- (void)datePickerCell:(id)aDatePickerCell validateProposedDateValue:(NSDate **)proposedDateValue timeInterval:(NSTimeInterval *)proposedTimeInterval;

//NSDraggingDestination
- (void)concludeDragOperation:(id <NSDraggingInfo>)sender;
- (void)draggingEnded:(id <NSDraggingInfo>)sender;
- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender;
- (void)draggingExited:(id <NSDraggingInfo>)sender;
- (NSDragOperation)draggingUpdated:(id <NSDraggingInfo>)sender;
- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender;
- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender;
- (BOOL)wantsPeriodicDraggingUpdates;

//NSDraggingInfo
- (NSImage *)draggedImage;
- (NSPoint)draggedImageLocation;
- (NSWindow *)draggingDestinationWindow;
- (NSPoint)draggingLocation;
- (NSPasteboard *)draggingPasteboard;
- (int)draggingSequenceNumber;
- (id)draggingSource;
- (NSDragOperation)draggingSourceOperationMask;
- (NSArray *)namesOfPromisedFilesDroppedAtDestination:(NSURL *)dropDestination;
- (void)slideDraggedImageTo:(NSPoint)screenPoint;

//NSDraggingSource
- (void)draggedImage:(NSImage *)image beganAt:(NSPoint)screenPoint;
- (void)draggedImage:(NSImage *)image endedAt:(NSPoint)screenPoint deposited:(BOOL)flag;
- (void)draggedImage:(NSImage *)image endedAt:(NSPoint)screenPoint operation:(NSDragOperation)operation;
- (void)draggedImage:(NSImage *)image movedTo:(NSPoint)screenPoint;
- (NSDragOperation)draggingSourceOperationMaskForLocal:(BOOL)flag;
- (BOOL)ignoreModifierKeysWhileDragging;
- (NSArray *)namesOfPromisedFilesDroppedAtDestination:(NSURL *)dropDestination;

//NSDrawerDelegate
- (BOOL)drawerShouldClose:(NSDrawer *)sender;
- (BOOL)drawerShouldOpen:(NSDrawer *)sender;
- (NSSize)drawerWillResizeContents:(NSDrawer *)sender toSize:(NSSize)contentSize;

//NSDrawerNotifications
- (void)drawerDidClose:(NSNotification *)notification;
- (void)drawerDidOpen:(NSNotification *)notification;
- (void)drawerWillClose:(NSNotification *)notification;
- (void)drawerWillOpen:(NSNotification *)notification;

//NSEditor
- (BOOL)commitEditing;
- (void)commitEditingWithDelegate:(id)delegate didCommitSelector:(SEL)didCommitSelector contextInfo:(void *)contextInfo;
- (void)discardEditing;

//NSEditorRegistration
- (void)objectDidBeginEditing:(id)editor;
- (void)objectDidEndEditing:(id)editor;

//NSFontManagerDelegate
- (BOOL)fontManager:(id)sender willIncludeFont:(NSString *)fontName;

//NSFontManagerResponderMethod
- (void)changeFont:(id)sender;

//NSFontPanelValidationAdditions
- (unsigned int)validModesForFontPanel:(NSFontPanel *)fontPanel;

//NSGlyphStorage
- (NSAttributedString *)attributedString;
- (void)insertGlyphs:(const NSGlyph *)glyphs length:(unsigned int)length forStartingGlyphAtIndex:(unsigned int)glyphIndex characterIndex:(unsigned int)charIndex;
- (unsigned int)layoutOptions;
- (void)setIntAttribute:(int)attributeTag value:(int)val forGlyphAtIndex:(unsigned)glyphIndex;

//NSIgnoreMisspelledWords
- (void)ignoreSpelling:(id)sender;

//NSImageDelegate
- (void)image:(NSImage *)image didLoadPartOfRepresentation:(NSImageRep *)rep withValidRows:(int)rows;
- (void)image:(NSImage *)image didLoadRepresentation:(NSImageRep *)rep withStatus:(int)status;
- (void)image:(NSImage *)image didLoadRepresentationHeader:(NSImageRep *)rep;
- (void)image:(NSImage *)image willLoadRepresentation:(NSImageRep *)rep;
- (NSImage *)imageDidNotDraw:(id)sender inRect:(NSRect)aRect;

//NSInputServerMouseTracker
- (BOOL)mouseDownOnCharacterIndex:(unsigned)theIndex atCoordinate:(NSPoint)thePoint withModifier:(unsigned int)theFlags client:(id)sender;
- (BOOL)mouseDraggedOnCharacterIndex:(unsigned)theIndex atCoordinate:(NSPoint)thePoint withModifier:(unsigned int)theFlags client:(id)sender;
- (void)mouseUpOnCharacterIndex:(unsigned)theIndex atCoordinate:(NSPoint)thePoint withModifier:(unsigned int)theFlags client:(id)sender;

//NSInputServiceProvider
- (void)activeConversationChanged:(id)sender toNewConversation:(long)newConversation;
- (void)activeConversationWillChange:(id)sender fromOldConversation:(long)oldConversation;
- (BOOL)canBeDisabled;
- (void)doCommandBySelector:(SEL)aSelector client:(id)sender;
- (void)inputClientBecomeActive:(id)sender;
- (void)inputClientDisabled:(id)sender;
- (void)inputClientEnabled:(id)sender;
- (void)inputClientResignActive:(id)sender;
- (void)insertText:(id)aString client:(id)sender;
- (void)markedTextAbandoned:(id)sender;
- (void)markedTextSelectionChanged:(NSRange)newSel client:(id)sender;
- (void)terminate:(id)sender;
- (BOOL)wantsToDelayTextChangeNotifications;
- (BOOL)wantsToHandleMouseEvents;
- (BOOL)wantsToInterpretAllKeystrokes;

//NSKeyValueBindingCreation
- (void)bind:(NSString *)binding toObject:(id)observable withKeyPath:(NSString *)keyPath options:(NSDictionary *)options;
- (void)exposeBinding:(NSString *)binding;
- (NSArray *)exposedBindings;
- (NSDictionary *)infoForBinding:(NSString *)binding;
- (void)unbind:(NSString *)binding;
- (Class)valueClassForBinding:(NSString *)binding;

//NSLayoutManagerDelegate
- (void)layoutManager:(NSLayoutManager *)layoutManager didCompleteLayoutForTextContainer:(NSTextContainer *)textContainer atEnd:(BOOL)layoutFinishedFlag;
- (void)layoutManagerDidInvalidateLayout:(NSLayoutManager *)sender;

//NSMenuDelegate
//- (BOOL)menu:(NSMenu *)menu updateItem:(NSMenuItem *)item atIndex:(int)index shouldCancel:(BOOL)shouldCancel;
- (BOOL)menuHasKeyEquivalent:(NSMenu *)menu forEvent:(NSEvent *)event target:(id *)target action:(SEL*)action;
- (void)menuNeedsUpdate:(NSMenu *)menu;
- (int)numberOfItemsInMenu:(NSMenu *)menu;

/*
//NSMenuItem
- (SEL)action;
- (NSAttributedString *)attributedTitle;
- (BOOL)hasSubmenu;
- (NSImage *)image;
- (int)indentationLevel;
- (id)initWithTitle:(NSString *)aString action:(SEL)aSelector keyEquivalent:(NSString *)charCode;
- (BOOL)isAlternate;
- (BOOL)isEnabled;
- (BOOL)isSeparatorItem;
- (NSString *)keyEquivalent;
- (unsigned int)keyEquivalentModifierMask;
- (NSMenu *)menu;
- (NSImage *)mixedStateImage;
- (NSString *)mnemonic;
- (unsigned)mnemonicLocation;
- (NSImage *)offStateImage;
- (NSImage *)onStateImage;
- (id)representedObject;
- (id <NSMenuItem>)separatorItem;
- (void)setAction:(SEL)aSelector;
- (void)setAlternate:(BOOL)isAlternate;
- (void)setAttributedTitle:(NSAttributedString *)string;
- (void)setEnabled:(BOOL)flag;
- (void)setImage:(NSImage *)menuImage;
- (void)setIndentationLevel:(int)indentationLevel;
- (void)setKeyEquivalent:(NSString *)aKeyEquivalent;
- (void)setKeyEquivalentModifierMask:(unsigned int)mask;
- (void)setMenu:(NSMenu *)menu;
- (void)setMixedStateImage:(NSImage *)image;
- (void)setMnemonicLocation:(unsigned)location;
- (void)setOffStateImage:(NSImage *)image;
- (void)setOnStateImage:(NSImage *)image;
- (void)setRepresentedObject:(id)anObject;
- (void)setState:(int)state;
- (void)setSubmenu:(NSMenu *)submenu;
- (void)setTag:(int)anInt;
- (void)setTarget:(id)anObject;
- (void)setTitle:(NSString *)aString;
- (void)setTitleWithMnemonic:(NSString *)stringWithAmpersand;
- (void)setToolTip:(NSString *)toolTip;
- (void)setUsesUserKeyEquivalents:(BOOL)flag;
- (int)state;
- (NSMenu *)submenu;
- (int)tag;
- (id)target;
- (NSString *)title;
- (NSString *)toolTip;
- (NSString *)userKeyEquivalent;
- (unsigned int)userKeyEquivalentModifierMask;
- (BOOL)usesUserKeyEquivalents;

//NSMenuValidation
- (BOOL)validateMenuItem:(id <NSMenuItem>)menuItem;
*/
//NSNibAwaking
- (void)awakeFromNib;

//NSOutlineViewDataSource
- (BOOL)outlineView:(NSOutlineView *)olv acceptDrop:(id <NSDraggingInfo>)info item:(id)item childIndex:(int)index;
- (id)outlineView:(NSOutlineView *)outlineView child:(int)index ofItem:(id)item;
- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item;
- (id)outlineView:(NSOutlineView *)outlineView itemForPersistentObject:(id)object;
- (NSArray *)outlineView:(NSOutlineView *)olv namesOfPromisedFilesDroppedAtDestination:(NSURL *)dropDestination forDraggedItems:(NSArray *)items;
- (int)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item;
- (id)outlineView:(NSOutlineView *)outlineView objectValueForTableColumn:(NSTableColumn *)tableColumn byItem:(id)item;
- (id)outlineView:(NSOutlineView *)outlineView persistentObjectForItem:(id)item;
- (void)outlineView:(NSOutlineView *)outlineView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn byItem:(id)item;
- (void)outlineView:(NSOutlineView *)outlineView sortDescriptorsDidChange:(NSArray *)oldDescriptors;
- (NSDragOperation)outlineView:(NSOutlineView *)olv validateDrop:(id <NSDraggingInfo>)info proposedItem:(id)item proposedChildIndex:(int)index;
- (BOOL)outlineView:(NSOutlineView *)olv writeItems:(NSArray *)items toPasteboard:(NSPasteboard *)pboard;

//NSOutlineViewDelegate
- (void)outlineView:(NSOutlineView *)outlineView didClickTableColumn:(NSTableColumn *)tableColumn;
- (void)outlineView:(NSOutlineView *)outlineView didDragTableColumn:(NSTableColumn *)tableColumn;
- (float)outlineView:(NSOutlineView *)outlineView heightOfRowByItem:(id)item;
- (void)outlineView:(NSOutlineView *)outlineView mouseDownInHeaderOfTableColumn:(NSTableColumn *)tableColumn;
- (BOOL)outlineView:(NSOutlineView *)outlineView shouldCollapseItem:(id)item;
- (BOOL)outlineView:(NSOutlineView *)outlineView shouldEditTableColumn:(NSTableColumn *)tableColumn item:(id)item;
- (BOOL)outlineView:(NSOutlineView *)outlineView shouldExpandItem:(id)item;
- (BOOL)outlineView:(NSOutlineView *)outlineView shouldSelectItem:(id)item;
- (BOOL)outlineView:(NSOutlineView *)outlineView shouldSelectTableColumn:(NSTableColumn *)tableColumn;
- (NSString *)outlineView:(NSOutlineView *)ov toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tc item:(id)item mouseLocation:(NSPoint)mouseLocation;
- (void)outlineView:(NSOutlineView *)outlineView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item;
- (void)outlineView:(NSOutlineView *)outlineView willDisplayOutlineCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn item:(id)item;
- (BOOL)selectionShouldChangeInOutlineView:(NSOutlineView *)outlineView;

//NSOutlineViewNotifications
- (void)outlineViewColumnDidMove:(NSNotification *)notification;
- (void)outlineViewColumnDidResize:(NSNotification *)notification;
- (void)outlineViewItemDidCollapse:(NSNotification *)notification;
- (void)outlineViewItemDidExpand:(NSNotification *)notification;
- (void)outlineViewItemWillCollapse:(NSNotification *)notification;
- (void)outlineViewItemWillExpand:(NSNotification *)notification;
- (void)outlineViewSelectionDidChange:(NSNotification *)notification;
- (void)outlineViewSelectionIsChanging:(NSNotification *)notification;

//NSPasteboardOwner
- (void)pasteboard:(NSPasteboard *)sender provideDataForType:(NSString *)type;
- (void)pasteboardChangedOwner:(NSPasteboard *)sender;

//NSPlaceholders
- (id)defaultPlaceholderForMarker:(id)marker withBinding:(NSString *)binding;
- (void)setDefaultPlaceholder:(id)placeholder forMarker:(id)marker withBinding:(NSString *)binding;

//NSSavePanelDelegate
- (NSComparisonResult)panel:(id)sender compareFilename:(NSString *)name1 with:(NSString *)name2 caseSensitive:(BOOL)caseSensitive;
- (void)panel:(id)sender directoryDidChange:(NSString *)path;
- (BOOL)panel:(id)sender isValidFilename:(NSString *)filename;
- (BOOL)panel:(id)sender shouldShowFilename:(NSString *)filename;
- (NSString *)panel:(id)sender userEnteredFilename:(NSString *)filename confirmed:(BOOL)okFlag;
- (void)panel:(id)sender willExpand:(BOOL)expanding;
- (void)panelSelectionDidChange:(id)sender;

//NSServicesRequests
- (BOOL)readSelectionFromPasteboard:(NSPasteboard *)pboard;
- (BOOL)writeSelectionToPasteboard:(NSPasteboard *)pboard types:(NSArray *)types;

//NSSoundDelegateMethods
- (void)sound:(NSSound *)sound didFinishPlaying:(BOOL)aBool;

//NSSpeechRecognizerDelegate
- (void)speechRecognizer:(id)sender didRecognizeCommand:(id)command;

//NSSpeechSynthesizerDelegate
- (void)speechSynthesizer:(id)sender didFinishSpeaking:(BOOL)finishedSpeaking;
- (void)speechSynthesizer:(id)sender willSpeakPhoneme:(short)phonemeOpcode;
- (void)speechSynthesizer:(id)sender willSpeakWord:(NSRange)characterRange ofString:(NSString *)string;

//NSSplitViewDelegate
- (BOOL)splitView:(NSSplitView *)sender canCollapseSubview:(NSView *)subview;
- (float)splitView:(NSSplitView *)sender constrainMaxCoordinate:(float)proposedCoord ofSubviewAt:(int)offset;
- (float)splitView:(NSSplitView *)sender constrainMinCoordinate:(float)proposedCoord ofSubviewAt:(int)offset;
- (float)splitView:(NSSplitView *)splitView constrainSplitPosition:(float)proposedPosition ofSubviewAt:(int)index;
- (void)splitView:(NSSplitView *)sender resizeSubviewsWithOldSize:(NSSize)oldSize;
- (void)splitViewDidResizeSubviews:(NSNotification *)notification;
- (void)splitViewWillResizeSubviews:(NSNotification *)notification;

//NSTabViewDelegate
- (void)tabView:(NSTabView *)tabView didSelectTabViewItem:(NSTabViewItem *)tabViewItem;
- (BOOL)tabView:(NSTabView *)tabView shouldSelectTabViewItem:(NSTabViewItem *)tabViewItem;
- (void)tabView:(NSTabView *)tabView willSelectTabViewItem:(NSTabViewItem *)tabViewItem;
- (void)tabViewDidChangeNumberOfTabViewItems:(NSTabView *)TabView;

//NSTableDataSource
- (int)numberOfRowsInTableView:(NSTableView *)tableView;
- (BOOL)tableView:(NSTableView *)tv acceptDrop:(id <NSDraggingInfo>)info row:(int)row dropOperation:(NSTableViewDropOperation)op;
- (NSArray *)tableView:(NSTableView *)tv namesOfPromisedFilesDroppedAtDestination:(NSURL *)dropDestination forDraggedRowsWithIndexes:(id)indexSet;
- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)row;
- (void)tableView:(NSTableView *)tableView setObjectValue:(id)object forTableColumn:(NSTableColumn *)tableColumn row:(int)row;
- (void)tableView:(NSTableView *)tableView sortDescriptorsDidChange:(NSArray *)oldDescriptors;
- (NSDragOperation)tableView:(NSTableView *)tv validateDrop:(id <NSDraggingInfo>)info proposedRow:(int)row proposedDropOperation:(NSTableViewDropOperation)op;
- (BOOL)tableView:(NSTableView *)tv writeRows:(NSArray *)rows toPasteboard:(NSPasteboard *)pboard;
- (BOOL)tableView:(NSTableView *)tv writeRowsWithIndexes:(id)rowIndexes toPasteboard:(NSPasteboard *)pboard;

//NSTableViewDelegate
- (BOOL)selectionShouldChangeInTableView:(NSTableView *)aTableView;
- (void)tableView:(NSTableView *)tableView didClickTableColumn:(NSTableColumn *)tableColumn;
- (void)tableView:(NSTableView *)tableView didDragTableColumn:(NSTableColumn *)tableColumn;
- (float)tableView:(NSTableView *)tableView heightOfRow:(int)row;
- (void)tableView:(NSTableView *)tableView mouseDownInHeaderOfTableColumn:(NSTableColumn *)tableColumn;
- (BOOL)tableView:(NSTableView *)tableView shouldEditTableColumn:(NSTableColumn *)tableColumn row:(int)row;
- (BOOL)tableView:(NSTableView *)tableView shouldSelectRow:(int)row;
- (BOOL)tableView:(NSTableView *)tableView shouldSelectTableColumn:(NSTableColumn *)tableColumn;
- (NSString *)tableView:(NSTableView *)tv toolTipForCell:(NSCell *)cell rect:(NSRectPointer)rect tableColumn:(NSTableColumn *)tc row:(int)row mouseLocation:(NSPoint)mouseLocation;
- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(int)row;

//NSTableViewNotifications
- (void)tableViewColumnDidMove:(NSNotification *)notification;
- (void)tableViewColumnDidResize:(NSNotification *)notification;
- (void)tableViewSelectionDidChange:(NSNotification *)notification;
- (void)tableViewSelectionIsChanging:(NSNotification *)notification;

//NSTextAttachmentCell
- (NSTextAttachment *)attachment;
- (NSPoint)cellBaselineOffset;
- (NSRect)cellFrameForTextContainer:(NSTextContainer *)textContainer proposedLineFragment:(NSRect)lineFrag glyphPosition:(NSPoint)position characterIndex:(unsigned)charIndex;
- (NSSize)cellSize;
- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView;
- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView characterIndex:(unsigned)charIndex;
- (void)drawWithFrame:(NSRect)cellFrame inView:(NSView *)controlView characterIndex:(unsigned)charIndex layoutManager:(NSLayoutManager *)layoutManager;
- (void)highlight:(BOOL)flag withFrame:(NSRect)cellFrame inView:(NSView *)controlView;
- (void)setAttachment:(NSTextAttachment *)anObject;
- (BOOL)trackMouse:(NSEvent *)theEvent inRect:(NSRect)cellFrame ofView:(NSView *)controlView atCharacterIndex:(unsigned)charIndex untilMouseUp:(BOOL)flag;
- (BOOL)trackMouse:(NSEvent *)theEvent inRect:(NSRect)cellFrame ofView:(NSView *)controlView untilMouseUp:(BOOL)flag;
- (BOOL)wantsToTrackMouse;
- (BOOL)wantsToTrackMouseForEvent:(NSEvent *)theEvent inRect:(NSRect)cellFrame ofView:(NSView *)controlView atCharacterIndex:(unsigned)charIndex;

//NSTextDelegate
- (void)textDidBeginEditing:(NSNotification *)notification;
- (void)textDidChange:(NSNotification *)notification;
- (void)textDidEndEditing:(NSNotification *)notification;
- (BOOL)textShouldBeginEditing:(NSText *)textObject;
- (BOOL)textShouldEndEditing:(NSText *)textObject;

//NSTextInput
- (NSAttributedString *)attributedSubstringFromRange:(NSRange)theRange;
- (unsigned int)characterIndexForPoint:(NSPoint)thePoint;
- (long)conversationIdentifier;
- (void)doCommandBySelector:(SEL)aSelector;
- (NSRect)firstRectForCharacterRange:(NSRange)theRange;
- (BOOL)hasMarkedText;
- (void)insertText:(id)aString;
- (NSRange)markedRange;
- (NSRange)selectedRange;
- (void)setMarkedText:(id)aString selectedRange:(NSRange)selRange;
- (void)unmarkText;
- (NSArray *)validAttributesForMarkedText;

//NSTextStorageDelegate
- (void)textStorageDidProcessEditing:(NSNotification *)notification;
- (void)textStorageWillProcessEditing:(NSNotification *)notification;

//NSTextViewDelegate
- (void)textView:(NSTextView *)textView clickedOnCell:(id <NSTextAttachmentCell>)cell inRect:(NSRect)cellFrame;
- (void)textView:(NSTextView *)textView clickedOnCell:(id <NSTextAttachmentCell>)cell inRect:(NSRect)cellFrame atIndex:(unsigned)charIndex;
- (BOOL)textView:(NSTextView *)textView clickedOnLink:(id)link;
- (BOOL)textView:(NSTextView *)textView clickedOnLink:(id)link atIndex:(unsigned)charIndex;
- (NSArray *)textView:(NSTextView *)textView completions:(NSArray *)words forPartialWordRange:(NSRange)charRange indexOfSelectedItem:(int *)index;
- (BOOL)textView:(NSTextView *)textView doCommandBySelector:(SEL)commandSelector;
- (void)textView:(NSTextView *)textView doubleClickedOnCell:(id <NSTextAttachmentCell>)cell inRect:(NSRect)cellFrame;
- (void)textView:(NSTextView *)textView doubleClickedOnCell:(id <NSTextAttachmentCell>)cell inRect:(NSRect)cellFrame atIndex:(unsigned)charIndex;
- (void)textView:(NSTextView *)view draggedCell:(id <NSTextAttachmentCell>)cell inRect:(NSRect)rect event:(NSEvent *)event;
- (void)textView:(NSTextView *)view draggedCell:(id <NSTextAttachmentCell>)cell inRect:(NSRect)rect event:(NSEvent *)event atIndex:(unsigned)charIndex;
- (BOOL)textView:(NSTextView *)textView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString *)replacementString;
- (BOOL)textView:(NSTextView *)textView shouldChangeTextInRanges:(NSArray *)affectedRanges replacementStrings:(NSArray *)replacementStrings;
- (NSDictionary *)textView:(NSTextView *)textView shouldChangeTypingAttributes:(NSDictionary *)oldTypingAttributes toAttributes:(NSDictionary *)newTypingAttributes;
- (NSRange)textView:(NSTextView *)textView willChangeSelectionFromCharacterRange:(NSRange)oldSelectedCharRange toCharacterRange:(NSRange)newSelectedCharRange;
- (NSArray *)textView:(NSTextView *)textView willChangeSelectionFromCharacterRanges:(NSArray *)oldSelectedCharRanges toCharacterRanges:(NSArray *)newSelectedCharRanges;
- (NSString *)textView:(NSTextView *)textView willDisplayToolTip:(NSString *)tooltip forCharacterAtIndex:(unsigned)characterIndex;
- (NSArray *)textView:(NSTextView *)view writablePasteboardTypesForCell:(id <NSTextAttachmentCell>)cell atIndex:(unsigned)charIndex;
- (BOOL)textView:(NSTextView *)view writeCell:(id <NSTextAttachmentCell>)cell atIndex:(unsigned)charIndex toPasteboard:(NSPasteboard *)pboard type:(NSString *)type;
- (void)textViewDidChangeSelection:(NSNotification *)notification;
- (void)textViewDidChangeTypingAttributes:(NSNotification *)notification;
- (NSUndoManager *)undoManagerForTextView:(NSTextView *)view;

//NSTokenFieldCellDelegate
- (NSArray *)tokenFieldCell:(id)tokenFieldCell completionsForSubstring:(NSString *)substring indexOfToken:(int)tokenIndex indexOfSelectedItem:(int *)selectedIndex;
- (NSString *)tokenFieldCell:(id)tokenFieldCell displayStringForRepresentedObject:(id)representedObject;
- (NSString *)tokenFieldCell:(id)tokenFieldCell editingStringForRepresentedObject:(id)representedObject;
- (BOOL)tokenFieldCell:(id)tokenFieldCell hasMenuForRepresentedObject:(id)representedObject;
- (NSMenu *)tokenFieldCell:(id)tokenFieldCell menuForRepresentedObject:(id)representedObject;
- (NSArray *)tokenFieldCell:(id)tokenFieldCell readFromPasteboard:(NSPasteboard *)pboard;
- (id)tokenFieldCell:(id)tokenFieldCell representedObjectForEditingString:(NSString *)editingString;
- (NSArray *)tokenFieldCell:(id)tokenFieldCell shouldAddObjects:(NSArray *)tokens atIndex:(unsigned)index;
- (int)tokenFieldCell:(id)tokenFieldCell styleForRepresentedObject:(id)representedObject;
- (BOOL)tokenFieldCell:(id)tokenFieldCell writeRepresentedObjects:(NSArray *)objects toPasteboard:(NSPasteboard *)pboard;

//NSTokenFieldDelegate
- (NSArray *)tokenField:(id)tokenField completionsForSubstring:(NSString *)substring indexOfToken:(int)tokenIndex indexOfSelectedItem:(int *)selectedIndex;
- (NSString *)tokenField:(id)tokenField displayStringForRepresentedObject:(id)representedObject;
- (NSString *)tokenField:(id)tokenField editingStringForRepresentedObject:(id)representedObject;
- (BOOL)tokenField:(id)tokenField hasMenuForRepresentedObject:(id)representedObject;
- (NSMenu *)tokenField:(id)tokenField menuForRepresentedObject:(id)representedObject;
- (NSArray *)tokenField:(id)tokenField readFromPasteboard:(NSPasteboard *)pboard;
- (id)tokenField:(id)tokenField representedObjectForEditingString:(NSString *)editingString;
- (NSArray *)tokenField:(id)tokenField shouldAddObjects:(NSArray *)tokens atIndex:(unsigned)index;
- (int)tokenField:(id)tokenField styleForRepresentedObject:(id)representedObject;
- (BOOL)tokenField:(id)tokenField writeRepresentedObjects:(NSArray *)objects toPasteboard:(NSPasteboard *)pboard;

//NSToolTipOwner
- (NSString *)view:(NSView *)view stringForToolTip:(NSToolTipTag)tag point:(NSPoint)point userData:(void *)data;

//NSToolbarDelegate
- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag;
- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar;
- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar;
- (NSArray *)toolbarSelectableItemIdentifiers:(NSToolbar *)toolbar;

//NSToolbarItemValidation
- (BOOL)validateToolbarItem:(NSToolbarItem *)theItem;

//NSToolbarNotifications
- (void)toolbarDidRemoveItem:(NSNotification *)notification;
- (void)toolbarWillAddItem:(NSNotification *)notification;

//NSUserInterfaceValidations
- (BOOL)validateUserInterfaceItem:(id <NSValidatedUserInterfaceItem>)anItem;

//NSValidatedUserInterfaceItem
- (SEL)action;
- (int)tag;

//NSWindowDelegate
- (NSRect)window:(NSWindow *)window willPositionSheet:(NSWindow *)sheet usingRect:(NSRect)rect;
- (BOOL)windowShouldClose:(id)sender;
- (BOOL)windowShouldZoom:(NSWindow *)window toFrame:(NSRect)newFrame;
- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)frameSize;
- (id)windowWillReturnFieldEditor:(NSWindow *)sender toObject:(id)client;
- (NSUndoManager *)windowWillReturnUndoManager:(NSWindow *)window;
- (NSRect)windowWillUseStandardFrame:(NSWindow *)window defaultFrame:(NSRect)newFrame;

//NSWindowNotifications
- (void)windowDidBecomeKey:(NSNotification *)notification;
- (void)windowDidBecomeMain:(NSNotification *)notification;
- (void)windowDidChangeScreen:(NSNotification *)notification;
- (void)windowDidChangeScreenProfile:(NSNotification *)notification;
- (void)windowDidDeminiaturize:(NSNotification *)notification;
- (void)windowDidEndSheet:(NSNotification *)notification;
- (void)windowDidExpose:(NSNotification *)notification;
- (void)windowDidMiniaturize:(NSNotification *)notification;
- (void)windowDidMove:(NSNotification *)notification;
- (void)windowDidResignKey:(NSNotification *)notification;
- (void)windowDidResignMain:(NSNotification *)notification;
- (void)windowDidResize:(NSNotification *)notification;
- (void)windowDidUpdate:(NSNotification *)notification;
- (void)windowWillBeginSheet:(NSNotification *)notification;
- (void)windowWillClose:(NSNotification *)notification;
- (void)windowWillMiniaturize:(NSNotification *)notification;
- (void)windowWillMove:(NSNotification *)notification;
@end

@protocol Foundation
//NSArchiverCallback
- (Class)classForArchiver;
- (id)replacementObjectForArchiver: (NSArchiver *)archiver;

//NSClassDescriptionPrimitives
- (NSArray *)attributeKeys;
- (NSClassDescription *)classDescription;
- (NSString *)inverseForRelationshipKey:(NSString *)relationshipKey;
- (NSArray *)toManyRelationshipKeys;
- (NSArray *)toOneRelationshipKeys;

//NSCoding
- (void)encodeWithCoder:(NSCoder *)aCoder;
- (id)initWithCoder:(NSCoder *)aDecoder;

//NSComparisonMethods
- (BOOL)doesContain:(id)object;
- (BOOL)isCaseInsensitiveLike:(NSString *)object;
- (BOOL)isEqualTo:(id)object;
- (BOOL)isGreaterThan:(id)object;
- (BOOL)isGreaterThanOrEqualTo:(id)object;
- (BOOL)isLessThan:(id)object;
- (BOOL)isLessThanOrEqualTo:(id)object;
- (BOOL)isLike:(NSString *)object;
- (BOOL)isNotEqualTo:(id)object;

//NSConnectionDelegateMethods
- (BOOL)authenticateComponents:(NSArray *)components withData:(NSData *)signature;
- (NSData *)authenticationDataForComponents:(NSArray *)components;
- (BOOL)connection:(NSConnection *)ancestor shouldMakeNewConnection:(NSConnection *)conn;
- (id)createConversationForConnection:(NSConnection *)conn;
- (BOOL)makeNewConnection:(NSConnection *)conn sender:(NSConnection *)ancestor;

//NSCopyLinkMoveHandler
- (BOOL)fileManager:(NSFileManager *)fm shouldProceedAfterError:(NSDictionary *)errorInfo;
- (void)fileManager:(NSFileManager *)fm willProcessPath:(NSString *)path;

//NSCopying
- (id)copyWithZone:(NSZone *)zone;

//NSDecimalNumberBehaviors
- (NSDecimalNumber *)exceptionDuringOperation:(SEL)operation error:(NSCalculationError)error leftOperand:(NSDecimalNumber *)leftOperand rightOperand:(NSDecimalNumber *)rightOperand;
- (NSRoundingMode)roundingMode;
- (short)scale;

//NSDelayedPerforming
- (void)cancelPreviousPerformRequestsWithTarget:(id)aTarget;
- (void)cancelPreviousPerformRequestsWithTarget:(id)aTarget selector:(SEL)aSelector object:(id)anArgument;
- (void)performSelector:(SEL)aSelector withObject:(id)anArgument afterDelay:(NSTimeInterval)delay;
- (void)performSelector:(SEL)aSelector withObject:(id)anArgument afterDelay:(NSTimeInterval)delay inModes:(NSArray *)modes;

//NSDeprecatedKeyValueCoding
- (id)handleQueryWithUnboundKey:(NSString *)key;
- (void)handleTakeValue:(id)value forUnboundKey:(NSString *)key;
- (id)storedValueForKey:(NSString *)key;
- (void)takeStoredValue:(id)value forKey:(NSString *)key;
- (void)takeValue:(id)value forKey:(NSString *)key;
- (void)takeValue:(id)value forKeyPath:(NSString *)keyPath;
- (void)takeValuesFromDictionary:(NSDictionary *)properties;
- (void)unableToSetNilForKey:(NSString *)key;
- (BOOL)useStoredAccessor;
- (NSDictionary *)valuesForKeys:(NSArray *)keys;

//NSDistantObjectRequestMethods
- (BOOL)connection:(NSConnection *)connection handleRequest:(id)doreq;

//NSDistributedObjects
- (Class)classForPortCoder;
- (id)replacementObjectForPortCoder:(NSPortCoder *)coder;

//NSErrorRecoveryAttempting
- (BOOL)attemptRecoveryFromError:(id)error optionIndex:(unsigned int)recoveryOptionIndex;
- (void)attemptRecoveryFromError:(id)error optionIndex:(unsigned int)recoveryOptionIndex delegate:(id)delegate didRecoverSelector:(SEL)didRecoverSelector contextInfo:(void *)contextInfo;

//NSKeyValueCoding
- (BOOL)accessInstanceVariablesDirectly;
- (NSDictionary *)dictionaryWithValuesForKeys:(NSArray *)keys;
- (NSMutableArray *)mutableArrayValueForKey:(NSString *)key;
- (NSMutableArray *)mutableArrayValueForKeyPath:(NSString *)keyPath;
- (NSMutableSet *)mutableSetValueForKey:(NSString *)key;
- (NSMutableSet *)mutableSetValueForKeyPath:(NSString *)keyPath;
- (void)setNilValueForKey:(NSString *)key;
- (void)setValue:(id)value forKey:(NSString *)key;
- (void)setValue:(id)value forKeyPath:(NSString *)keyPath;
- (void)setValue:(id)value forUndefinedKey:(NSString *)key;
- (void)setValuesForKeysWithDictionary:(NSDictionary *)keyedValues;
- (BOOL)validateValue:(id *)ioValue forKey:(NSString *)inKey error:(id *)outError;
- (BOOL)validateValue:(id *)ioValue forKeyPath:(NSString *)inKeyPath error:(id *)outError;
- (id)valueForKey:(NSString *)key;
- (id)valueForKeyPath:(NSString *)keyPath;
- (id)valueForUndefinedKey:(NSString *)key;

//NSKeyValueObserverNotification
- (void)didChange:(int)changeKind valuesAtIndexes:(id)indexes forKey:(NSString *)key;
- (void)didChangeValueForKey:(NSString *)key;
- (void)didChangeValueForKey:(NSString *)key withSetMutation:(int)mutationKind usingObjects:(NSSet *)objects;
- (void)willChange:(int)changeKind valuesAtIndexes:(id)indexes forKey:(NSString *)key;
- (void)willChangeValueForKey:(NSString *)key;
- (void)willChangeValueForKey:(NSString *)key withSetMutation:(int)mutationKind usingObjects:(NSSet *)objects;

//NSKeyValueObserverRegistration
- (void)addObserver:(NSObject *)observer forKeyPath:(NSString *)keyPath options:(unsigned int)options context:(void *)context;
- (void)removeObserver:(NSObject *)observer forKeyPath:(NSString *)keyPath;

//NSKeyValueObserving
- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context;

//NSKeyValueObservingCustomization
- (BOOL)automaticallyNotifiesObserversForKey:(NSString *)key;
- (void *)observationInfo;
- (void)setKeys:(NSArray *)keys triggerChangeNotificationsForDependentKey:(NSString *)dependentKey;
- (void)setObservationInfo:(void *)observationInfo;

//NSKeyedArchiverDelegate
- (void)archiver:(NSKeyedArchiver *)archiver didEncodeObject:(id)object;
- (id)archiver:(NSKeyedArchiver *)archiver willEncodeObject:(id)object;
- (void)archiver:(NSKeyedArchiver *)archiver willReplaceObject:(id)object withObject:(id)newObject;
- (void)archiverDidFinish:(NSKeyedArchiver *)archiver;
- (void)archiverWillFinish:(NSKeyedArchiver *)archiver;

//NSKeyedArchiverObjectSubstitution
- (NSArray *)classFallbacksForKeyedArchiver;
- (Class)classForKeyedArchiver;
- (id)replacementObjectForKeyedArchiver:(NSKeyedArchiver *)archiver;

//NSKeyedUnarchiverDelegate
- (Class)unarchiver:(NSKeyedUnarchiver *)unarchiver cannotDecodeObjectOfClassName:(NSString *)name originalClasses:(NSArray *)classNames;
- (id)unarchiver:(NSKeyedUnarchiver *)unarchiver didDecodeObject:(id)object;
- (void)unarchiver:(NSKeyedUnarchiver *)unarchiver willReplaceObject:(id)object withObject:(id)newObject;
- (void)unarchiverDidFinish:(NSKeyedUnarchiver *)unarchiver;
- (void)unarchiverWillFinish:(NSKeyedUnarchiver *)unarchiver;

//NSKeyedUnarchiverObjectSubstitution
- (Class)classForKeyedUnarchiver;

//NSLocking
- (void)lock;
- (void)unlock;

//NSMachPortDelegateMethods
- (void)handleMachMessage:(void *)msg;

//NSMainThreadPerformAdditions
- (void)performSelectorOnMainThread:(SEL)aSelector withObject:(id)arg waitUntilDone:(BOOL)wait;
- (void)performSelectorOnMainThread:(SEL)aSelector withObject:(id)arg waitUntilDone:(BOOL)wait modes:(NSArray *)array;

//NSMetadataQueryDelegate
- (id)metadataQuery:(id)query replacementObjectForResultObject:(id)result;
- (id)metadataQuery:(id)query replacementValueForAttribute:(NSString *)attrName value:(id)attrValue;

//NSMutableCopying
- (id)mutableCopyWithZone:(NSZone *)zone;

//NSNetServiceBrowserDelegateMethods
- (void)netServiceBrowser:(id)aNetServiceBrowser didFindDomain:(NSString *)domainString moreComing:(BOOL)moreComing;
- (void)netServiceBrowser:(id)aNetServiceBrowser didFindService:(id)aNetService moreComing:(BOOL)moreComing;
- (void)netServiceBrowser:(id)aNetServiceBrowser didNotSearch:(NSDictionary *)errorDict;
- (void)netServiceBrowser:(id)aNetServiceBrowser didRemoveDomain:(NSString *)domainString moreComing:(BOOL)moreComing;
- (void)netServiceBrowser:(id)aNetServiceBrowser didRemoveService:(id)aNetService moreComing:(BOOL)moreComing;
- (void)netServiceBrowserDidStopSearch:(id)aNetServiceBrowser;
- (void)netServiceBrowserWillSearch:(id)aNetServiceBrowser;

//NSNetServiceDelegateMethods
- (void)netService:(id)sender didNotPublish:(NSDictionary *)errorDict;
- (void)netService:(id)sender didNotResolve:(NSDictionary *)errorDict;
- (void)netService:(id)sender didUpdateTXTRecordData:(NSData *)data;
- (void)netServiceDidPublish:(id)sender;
- (void)netServiceDidResolveAddress:(id)sender;
- (void)netServiceDidStop:(id)sender;
- (void)netServiceWillPublish:(id)sender;
- (void)netServiceWillResolve:(id)sender;

//NSObjCTypeSerializationCallBack
- (void)deserializeObjectAt:(id *)object ofObjCType:(const char *)type fromData:(NSData *)data atCursor:(unsigned *)cursor;
- (void)serializeObjectAt:(id *)object ofObjCType:(const char *)type intoData:(NSMutableData *)data;

//NSObject
- (id)autorelease;
- (Class)class;
- (BOOL)conformsToProtocol:(Protocol *)aProtocol;
- (NSString *)description;
- (unsigned)hash;
- (BOOL)isEqual:(id)object;
- (BOOL)isKindOfClass:(Class)aClass;
- (BOOL)isMemberOfClass:(Class)aClass;
- (BOOL)isProxy;
- (id)performSelector:(SEL)aSelector;
- (id)performSelector:(SEL)aSelector withObject:(id)object;
- (id)performSelector:(SEL)aSelector withObject:(id)object1 withObject:(id)object2;
- (oneway void)release;
- (BOOL)respondsToSelector:(SEL)aSelector;
- (id)retain;
- (unsigned)retainCount;
- (id)self;
- (Class)superclass;
- (NSZone *)zone;

//NSPortDelegateMethods
- (void)handlePortMessage:(NSPortMessage *)message;

//NSScriptClassDescription
- (unsigned long)classCode;
- (NSString *)className;

//NSScriptKeyValueCoding
- (id)coerceValue:(id)value forKey:(NSString *)key;
- (void)insertValue:(id)value atIndex:(unsigned)index inPropertyWithKey:(NSString *)key;
- (void)insertValue:(id)value inPropertyWithKey:(NSString *)key;
- (void)removeValueAtIndex:(unsigned)index fromPropertyWithKey:(NSString *)key;
- (void)replaceValueAtIndex:(unsigned)index inPropertyWithKey:(NSString *)key withValue:(id)value;
- (id)valueAtIndex:(unsigned)index inPropertyWithKey:(NSString *)key;
- (id)valueWithName:(NSString *)name inPropertyWithKey:(NSString *)key;
- (id)valueWithUniqueID:(id)uniqueID inPropertyWithKey:(NSString *)key;

//NSScriptObjectSpecifiers
- (NSArray *)indicesOfObjectsByEvaluatingObjectSpecifier:(id)specifier;
- (id)objectSpecifier;

//NSScripting
- (NSDictionary *)scriptingProperties;
- (void)setScriptingProperties:(NSDictionary *)properties;

//NSScriptingComparisonMethods
- (BOOL)scriptingBeginsWith:(id)object;
- (BOOL)scriptingContains:(id)object;
- (BOOL)scriptingEndsWith:(id)object;
- (BOOL)scriptingIsEqualTo:(id)object;
- (BOOL)scriptingIsGreaterThan:(id)object;
- (BOOL)scriptingIsGreaterThanOrEqualTo:(id)object;
- (BOOL)scriptingIsLessThan:(id)object;
- (BOOL)scriptingIsLessThanOrEqualTo:(id)object;

//NSSpellServerDelegate
- (void)spellServer:(NSSpellServer *)sender didForgetWord:(NSString *)word inLanguage:(NSString *)language;
- (void)spellServer:(NSSpellServer *)sender didLearnWord:(NSString *)word inLanguage:(NSString *)language;
- (NSRange)spellServer:(NSSpellServer *)sender findMisspelledWordInString:(NSString *)stringToCheck language:(NSString *)language wordCount:(int *)wordCount countOnly:(BOOL)countOnly;
- (NSArray *)spellServer:(NSSpellServer *)sender suggestCompletionsForPartialWordRange:(NSRange)range inString:(NSString *)string language:(NSString *)language;
- (NSArray *)spellServer:(NSSpellServer *)sender suggestGuessesForWord:(NSString *)word inLanguage:(NSString *)language;

//NSStreamDelegateEventExtensions
- (void)stream:(id)aStream handleEvent:(int)eventCode;

//NSURLAuthenticationChallengeSender
- (void)cancelAuthenticationChallenge:(id)challenge;
- (void)continueWithoutCredentialForAuthenticationChallenge:(id)challenge;
- (void)useCredential:(id)credential forAuthenticationChallenge:(id)challenge;

//NSURLClient
- (void)URL:(NSURL *)sender resourceDataDidBecomeAvailable:(NSData *)newBytes;
- (void)URL:(NSURL *)sender resourceDidFailLoadingWithReason:(NSString *)reason;
- (void)URLResourceDidCancelLoading:(NSURL *)sender;
- (void)URLResourceDidFinishLoading:(NSURL *)sender;

//NSURLConnectionDelegate
- (void)connection:(id)connection didCancelAuthenticationChallenge:(id)challenge;
- (void)connection:(id)connection didFailWithError:(id)error;
- (void)connection:(id)connection didReceiveAuthenticationChallenge:(id)challenge;
- (void)connection:(id)connection didReceiveData:(NSData *)data;
- (void)connection:(id)connection didReceiveResponse:(id)response;
- (id)connection:(id)connection willCacheResponse:(id)cachedResponse;
- (id)connection:(id)connection willSendRequest:(id)request redirectResponse:(id)response;
- (void)connectionDidFinishLoading:(id)connection;

//NSURLDownloadDelegate
- (void)download:(id)download decideDestinationWithSuggestedFilename:(NSString *)filename;
- (void)download:(id)download didCancelAuthenticationChallenge:(id)challenge;
- (void)download:(id)download didCreateDestination:(NSString *)path;
- (void)download:(id)download didFailWithError:(id)error;
- (void)download:(id)download didReceiveAuthenticationChallenge:(id)challenge;
- (void)download:(id)download didReceiveDataOfLength:(unsigned)length;
- (void)download:(id)download didReceiveResponse:(id)response;
- (BOOL)download:(id)download shouldDecodeSourceDataOfMIMEType:(NSString *)encodingType;
- (void)download:(id)download willResumeWithResponse:(id)response fromByte:(long long)startingByte;
- (id)download:(id)download willSendRequest:(id)request redirectResponse:(id)redirectResponse;
- (void)downloadDidBegin:(id)download;
- (void)downloadDidFinish:(id)download;

//NSURLHandleClient
- (void)URLHandle:(NSURLHandle *)sender resourceDataDidBecomeAvailable:(NSData *)newBytes;
- (void)URLHandle:(NSURLHandle *)sender resourceDidFailLoadingWithReason:(NSString *)reason;
- (void)URLHandleResourceDidBeginLoading:(NSURLHandle *)sender;
- (void)URLHandleResourceDidCancelLoading:(NSURLHandle *)sender;
- (void)URLHandleResourceDidFinishLoading:(NSURLHandle *)sender;

//NSURLProtocolClient
- (void)URLProtocol:(id)protocol cachedResponseIsValid:(id)cachedResponse;
- (void)URLProtocol:(id)protocol didCancelAuthenticationChallenge:(id)challenge;
- (void)URLProtocol:(id)protocol didFailWithError:(id)error;
- (void)URLProtocol:(id)protocol didLoadData:(NSData *)data;
- (void)URLProtocol:(id)protocol didReceiveAuthenticationChallenge:(id)challenge;
- (void)URLProtocol:(id)protocol didReceiveResponse:(id)response cacheStoragePolicy:(int)policy;
- (void)URLProtocol:(id)protocol wasRedirectedToRequest:(id)request redirectResponse:(id)redirectResponse;
- (void)URLProtocolDidFinishLoading:(id)protocol;

//NSXMLParserDelegateEventAdditions
- (void)parser:(id)parser didEndElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName;
- (void)parser:(id)parser didEndMappingPrefix:(NSString *)prefix;
- (void)parser:(id)parser didStartElement:(NSString *)elementName namespaceURI:(NSString *)namespaceURI qualifiedName:(NSString *)qName attributes:(NSDictionary *)attributeDict;
- (void)parser:(id)parser didStartMappingPrefix:(NSString *)prefix toURI:(NSString *)namespaceURI;
- (void)parser:(id)parser foundAttributeDeclarationWithName:(NSString *)attributeName forElement:(NSString *)elementName type:(NSString *)type defaultValue:(NSString *)defaultValue;
- (void)parser:(id)parser foundCDATA:(NSData *)CDATABlock;
- (void)parser:(id)parser foundCharacters:(NSString *)string;
- (void)parser:(id)parser foundComment:(NSString *)comment;
- (void)parser:(id)parser foundElementDeclarationWithName:(NSString *)elementName model:(NSString *)model;
- (void)parser:(id)parser foundExternalEntityDeclarationWithName:(NSString *)name publicID:(NSString *)publicID systemID:(NSString *)systemID;
- (void)parser:(id)parser foundIgnorableWhitespace:(NSString *)whitespaceString;
- (void)parser:(id)parser foundInternalEntityDeclarationWithName:(NSString *)name value:(NSString *)value;
- (void)parser:(id)parser foundNotationDeclarationWithName:(NSString *)name publicID:(NSString *)publicID systemID:(NSString *)systemID;
- (void)parser:(id)parser foundProcessingInstructionWithTarget:(NSString *)target data:(NSString *)data;
- (void)parser:(id)parser foundUnparsedEntityDeclarationWithName:(NSString *)name publicID:(NSString *)publicID systemID:(NSString *)systemID notationName:(NSString *)notationName;
- (void)parser:(id)parser parseErrorOccurred:(id)parseError;
- (NSData *)parser:(id)parser resolveExternalEntityName:(NSString *)name systemID:(NSString *)systemID;
- (void)parser:(id)parser validationErrorOccurred:(id)validationError;
- (void)parserDidEndDocument:(id)parser;
- (void)parserDidStartDocument:(id)parser;
@end

@interface NSObject (Deprecated)
- (void)beginSheetForDirectory:dir file:file types:type modalForWindow:window modalDelegate:delegate didEndSelector:(SEL)sel contextInfo:context;
- (void)my_sheetDidEnd:(id)b returnCode:(NSInteger)c contextInfo:(void *)v;
@end

#endif
