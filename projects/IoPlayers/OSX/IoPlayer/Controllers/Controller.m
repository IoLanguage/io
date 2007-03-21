#import "Controller.h"
 
@interface NSString (rangeOfLineNumber)
- (NSRange)rangeOfLineNumber:(int)linenum;
@end

@implementation NSString (rangeOfLineNumber)
- (NSRange)rangeOfLineNumber:(int)linenum
{
  NSCharacterSet *returnChar = [NSCharacterSet characterSetWithCharactersInString:@"\n"];
  int length = [self length];
  NSRange r = NSMakeRange(0, length);
  int lineCount = 0;
  int index = 0;
  if (!length) { return r; }
  if (linenum < 0) { return NSMakeRange(0, 0); }
  while (1)
  {
    r.location = index+1;
    //if (r.location >length) { r.location = NSNotFound; return r; }
    if (r.location > length-1) { r.location = length-1; }
    r.length = length - r.location;
    index = [self rangeOfCharacterFromSet:returnChar options:nil range:r].location;
    if (index == NSNotFound) { return r; }
    if (lineCount >= linenum) { return NSMakeRange(r.location, index - r.location); }
    lineCount++;
  }
  return r;
}
@end


@interface NSUserDefaults (Color)
- (void)setColor:(NSColor *)color forKey:key;
- (NSColor *)colorForKey:key;
@end

@implementation NSUserDefaults (Color)
- (void)setColor:(NSColor *)color forKey:key
{
  NSMutableDictionary *d = [NSMutableDictionary dictionary];
  float r, g, b, a;
  [color getRed:&r green:&g blue:&b alpha:&a];
  [d setObject:[NSNumber numberWithFloat:r] forKey:@"r"];
  [d setObject:[NSNumber numberWithFloat:g] forKey:@"g"];
  [d setObject:[NSNumber numberWithFloat:b] forKey:@"b"];
  [d setObject:[NSNumber numberWithFloat:a] forKey:@"a"];
  [self setObject:d forKey:key];
}
- (NSColor *)colorForKey:key
{
  NSDictionary *d = [self dictionaryForKey:key];
  if (d)
  {
    float r = [[d objectForKey:@"r"] floatValue];
    float g = [[d objectForKey:@"g"] floatValue];
    float b = [[d objectForKey:@"b"] floatValue];
    float a = [[d objectForKey:@"a"] floatValue];
    return [NSColor colorWithDeviceRed:r green:g blue:b alpha:a];
  }
  return nil;
}
@end


@implementation Controller

- init
{
  [super init];
  
  [[NSNotificationCenter defaultCenter] 
    addObserver:self 
    selector:@selector(appDidFinishLaunching:) 
    name:NSApplicationDidFinishLaunchingNotification 
    object:NSApp];  
  return self;
}

- (void)awakeFromNib
{
  [[NSFontManager sharedFontManager] setDelegate:self];
  //[[LuaManager sharedInstance] setDelegate:self];
  //[LuaObjC addLuaObjCBindings];
  [sourceText setUsesFontPanel:YES];
  [outputText setUsesFontPanel:YES];
  [self loadPrefs];
}

- (void)appDidFinishLaunching:aNotification
{
  //[fileDrawer open:self];
  [[glView openGLContext] clearDrawable];
  [[glView openGLContext] makeCurrentContext];
  if ([self defaultProjectPath]) { [self openFile:[self defaultProjectPath]]; }
}

- (BOOL)openFile:(NSString *)filename
{
  BOOL isDir;
  if ([[filename pathExtension] isEqual:@"luadev"] &&
    [[NSFileManager defaultManager] fileExistsAtPath:filename isDirectory:&isDir] && isDir)
  {
    [self setProjectPath:filename];
    return YES;
  }
  return NO;
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)fileName
{ return [self openFile:fileName]; }

- (BOOL)openFile:(NSPasteboard *)pboard userData:(NSString *)data error:(NSString **)error 
{
  if ([[pboard types] containsObject:NSStringPboardType])
  {
    [self openFile:[pboard stringForType:NSStringPboardType]];
    return YES;
  }
  return NO;
}

- (IBAction)changeProjectName:sender
{
  NSString *name = [projectNameTextField stringValue];
  NSString *newName = [[name stringByDeletingPathExtension] 
    stringByAppendingString:@".luadev"];
  NSString *newPath = [[projectPath stringByDeletingLastPathComponent]
    stringByAppendingPathComponent:newName];
  if (![name length] || 
    [[NSFileManager defaultManager] fileExistsAtPath:newPath])
  { 
    [projectNameTextField setStringValue:[self projectName]];
    return; 
  }
  [[NSFileManager defaultManager] movePath:projectPath toPath:newPath handler:nil];
  [self setProjectPath:newPath];
}

- (NSString *)projectName
{
  return [[projectPath lastPathComponent] stringByDeletingPathExtension];
}

- (void)setProjectPath:(NSString *)filename
{
  [projectPath autorelease];
  projectPath = [filename retain];
  if (projectPath)
  {
    {
      DirectoryItem *item = [[[DirectoryItem alloc] init] autorelease];
      [item setPath:projectPath];
      [outlineViewController rootItem:item];
    }
    [[LuaManager sharedInstance] setRootPath:projectPath];
    [self setEditorPath:[projectPath stringByAppendingPathComponent:@"main.lua"]];
    [projectNameTextField setStringValue:[[projectPath lastPathComponent] stringByDeletingPathExtension]];
    [[sourceText window] orderFront:self];
    [fileDrawer open:self];
    [self setDefaultProjectPath:projectPath];
  }
  else
  { [self setEditorPath:nil]; }
}


- (void)setEditorPath:(NSString *)path
{
  BOOL isDir;
  [self save:nil];
  [editorPath autorelease];
  editorPath = [path retain];
  if (![[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir])
  { [sourceText setString:@""]; }
  else
  {
    [sourceText setString:[NSString stringWithContentsOfFile:path]];
    [outlineViewController selectItem:[outlineViewController itemAtPath:path]]; 
  }  
}

- (NSString *)editorPath { return editorPath; }

- (void)setDefaultProjectPath:(NSString *)path
{ 
  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
  [defaults setObject:path forKey:@"projectPath"];
  [defaults synchronize];
}

- (NSString *)defaultProjectPath 
{ 
  return [[NSUserDefaults standardUserDefaults] objectForKey:@"projectPath"];
}

- (IBAction)new:(id)sender
{
  int result;
  NSOpenPanel *oPanel = [NSOpenPanel openPanel];
  [oPanel setAllowsMultipleSelection:NO];
  [oPanel setCanChooseFiles:NO];
  [oPanel setCanChooseDirectories:YES];
  result = [oPanel 
    runModalForDirectory:[[self defaultProjectPath] stringByDeletingLastPathComponent]
    file:nil types:nil];
  if (result == NSOKButton) 
  {
    NSString *dir = [[oPanel filenames] objectAtIndex:0];
    NSString *newName = @"untitled.luadev";
    NSString *newPath = [dir stringByAppendingPathComponent:newName];
    int num = 1;
    while ([[NSFileManager defaultManager] fileExistsAtPath:newPath])
    {
      newName = [NSString stringWithFormat:@"untitled%i.luadev", num];
      newPath = [dir stringByAppendingPathComponent:newName];
    }
    [[NSFileManager defaultManager] createDirectoryAtPath:newPath attributes:nil];
    [@"-- main" writeToFile:[newPath stringByAppendingPathComponent:@"main.lua"]
      atomically:YES];
    [self close:nil];
    [self setProjectPath:newPath];
  }
}

- (IBAction)open:(id)sender
{
  int result;
  NSArray *fileTypes = [NSArray arrayWithObject:@"luadev"];
  NSOpenPanel *oPanel = [NSOpenPanel openPanel];
  
  [oPanel setAllowsMultipleSelection:NO];
  [oPanel setCanChooseFiles:YES];
  result = [oPanel runModalForDirectory:[self defaultProjectPath] 
    file:nil types:fileTypes];
  if (result == NSOKButton) 
  {
    NSString *fileName = [[oPanel filenames] objectAtIndex:0];
    [self openFile:fileName];
  }
}

- (IBAction)close:(id)sender
{
  [[[browserController browser] window] orderOut:self];
  [[glView window]  orderOut:self];
  [[outputText window] orderOut:self];
  [fileDrawer close:self];
  [[sourceText window] orderOut:self];
  [self setProjectPath:nil];
}

- (IBAction)save:(id)sender
{
  if ([editorPath length] &&
   [[NSFileManager defaultManager] fileExistsAtPath:editorPath])
  { [[sourceText string] writeToFile:editorPath atomically:NO]; }
}


- (IBAction)run:(id)sender
{
  [self save:nil];
  [[LuaManager sharedInstance] reset];

  [[[browserController browser] window] orderOut:self];
  [browserController rootItems:[NSArray array]];
  
  [glView stopTimers:nil];
  [[outputText window] orderOut:self];
  [[glView window] orderFront:self];
  [[glView openGLContext] makeCurrentContext];
  
  [outputText setString:@""];
  {
  NSMutableArray *array = [[LuaManager sharedInstance] doFile:@"main.lua"];
  if ([array count])
  {
    [self appendOutput:@"returned: "];
    [self appendOutput:[array description]];
    [self appendOutput:@"\n"];
  }
  }
  [glView reshape];
  [glView setNeedsDisplay:YES];
  //[[glView openGLContext] flushBuffer];
}

- (void)luaManager:luaManager error:msg
{
  NSArray *stack = [luaManager stackForBrowser];
  //[self appendOutput:@"error: "];
  //[self appendOutput:msg];
  //[self appendOutput:@"\nstack:\n"];
  //[self appendOutput:[stack description]];
  //[self appendOutput:@"\n"];
  [errorTextField setString:msg];
  [browserController rootItems:stack];
  //[[[browserController browser] window] setTitle:msg];
  [[[browserController browser] window] orderFront:self];
  [[browserController browser] selectRow:[stack count]-1 inColumn:0];
  [glView stopTimers:nil];
}

- (void)luaManager:luaManager print:msg
{
  [self appendOutput:msg];
}

- (void)appendOutput:(NSString *)msg
{
  [outputText setString:[[outputText string] stringByAppendingString:msg]];
  [outputText scrollRangeToVisible:NSMakeRange([[outputText string] length], 0)];
  [[outputText window] orderFront:self];
}

// --- folders & files ---

- (void)addFolder:sender
{
  id newItem;
  id selected = [outlineViewController selectedItem];
  [self save:nil];
  if ([selected isFile]) { selected = [selected parent]; }
  newItem = [selected addDirectoryNamed:@"untitled"];
  [[outlineViewController outlineView] reloadData];
  [[outlineViewController outlineView] expandItem:[newItem parent]];
  [outlineViewController selectItem:newItem];
}

- (void)addFile:sender
{
  id newItem;
  id selected = [outlineViewController selectedItem];
  [self save:nil];
  if ([selected isFile]) { selected = [selected parent]; }
  newItem = [selected addFileNamed:@"untitled.lua"];
  [[outlineViewController outlineView] reloadData];
  [[outlineViewController outlineView] expandItem:[newItem parent]];
  [outlineViewController selectItem:newItem];
  [self setEditorPath:[newItem path]];
}

// -- delegate methods ---

- (void)browserController:sender selected:anObject row:(int)r column:(int)c
{
  NSString *fileName = [anObject objectForKey:@"fileName"];
  int currentLine = [[anObject objectForKey:@"currentLine"] intValue];
  if (fileName && ![fileName isEqual:@"(C)"])
  {
    NSString *fullPath = [projectPath stringByAppendingPathComponent:fileName];
    [self setEditorPath:fullPath];
    {
    NSRange lineRange = [[sourceText string] rangeOfLineNumber:currentLine-1];
    printf("stack selected fileName:%s\n", [fileName cString]);
    //[outlineViewController selectItem:[outlineViewController itemAtPath:fullPath]]; 
    [sourceText setSelectedRange:lineRange];
    [[sourceText window] makeKeyAndOrderFront:nil];
    [sourceText scrollRangeToVisible:lineRange];
    }
  }
}

- (BOOL)outlineViewController:sender shouldSelect:anItem
{
  if ([anItem isFile]) { [self setEditorPath:[anItem path]]; }
  return YES;
}

- (BOOL)outlineViewController:sender shouldRename:anItem
{
  return YES;
}

- (BOOL)outlineViewController:sender shouldRemove:anItem
{
  return YES;
}

- (void)loadPrefs
{
  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
  if ([defaults colorForKey:@"sourceBackgroundColor"])
  {
    [sourceBackgroundColor setColor:[defaults colorForKey:@"sourceBackgroundColor"]];
    [sourceText setBackgroundColor:[sourceBackgroundColor color]];

    [sourceTextColor setColor:[defaults colorForKey:@"sourceTextColor"]];
    [sourceText setTextColor:[sourceTextColor color]];
    
    [outputBackgroundColor setColor:[defaults colorForKey:@"outputBackgroundColor"]];
    [outputText setBackgroundColor:[outputBackgroundColor color]];

    [outputTextColor setColor:[defaults colorForKey:@"outputTextColor"]];
    [outputText setTextColor:[outputTextColor color]];
  }
}

- (IBAction)changedPrefs:sender
{
  NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
  [defaults setColor:[sourceBackgroundColor color] forKey:@"sourceBackgroundColor"];
  [sourceText setBackgroundColor:[sourceBackgroundColor color]];

  [defaults setColor:[sourceTextColor color] forKey:@"sourceTextColor"];
  [sourceText setTextColor:[sourceTextColor color]];
  
  [defaults setColor:[outputBackgroundColor color] forKey:@"outputBackgroundColor"];
  [outputText setBackgroundColor:[outputBackgroundColor color]];

  [defaults setColor:[outputTextColor color] forKey:@"outputTextColor"];
  [outputText setTextColor:[outputTextColor color]];

  [defaults synchronize]; 
}

- (IBAction)setFont:sender
{
  [[NSFontPanel sharedFontPanel] makeKeyAndOrderFront:nil];
}

- (void)changeFont:(id)fontManager
{
  //NSFont *oldFont = [outputText font];
  //NSFont *newFont = [fontManager convertFont:oldFont];
  //[self setSelectionFont:newFont];
  return;
}

@end
