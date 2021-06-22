#import "IoBrowserController.h"

@implementation IoBrowserController

@synthesize window;
@synthesize columns;

- (id)init
{
	[super init];
	[self setColumns:[NSMutableArray array]];
	[columns addObject:[[IoLanguage shared] lobby]];
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(applicationDidFinishLaunching:) 
											 name:NSApplicationDidFinishLaunchingNotification
											 object:nil];
	return self;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	[browser setDelegate:self];
	[browser loadColumnZero];
}

/*
- (id)rootItemForBrowser:(NSBrowser *)browser 
{
    return nil;
}

- (BOOL)browser:(NSBrowser *)sender isColumnValid:(NSInteger)column
{
	return NO;
}

- (NSIndexSet *)browser:(NSBrowser *)browser 
	selectionIndexesForProposedSelection:(NSIndexSet *)proposedSelectionIndexes 
	inColumn:(NSInteger)column
{
	printf("---------------proposedSelectionIndexes:\n");

	return proposedSelectionIndexes;
}

- (BOOL)browser:(NSBrowser *)sender selectCellWithString:(NSString *)title inColumn:(NSInteger)column
{
	printf("---------------selectCellWithString:\n");

	return YES;
}

- (BOOL)browser:(NSBrowser *)sender selectRow:(NSInteger)row inColumn:(NSInteger)column
{
	printf("---------------selectRow:\n");
	while ([columns count] >= column) [columns removeLastObject];
	
	Objc2Io *obj = [columns lastObject];
	NSString *slotName = [[obj _rawSlotNames] objectAtIndex:row];	
	[columns addObject:[obj _rawGetSlot:slotName]];
	return YES;
}

- (NSInteger)browser:(NSBrowser *)browser numberOfChildrenOfItem:(id)item
{
	return 1;
}
*/
- (NSInteger)browser:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column
{
	if(column == 0) return [[columns objectAtIndex:column] _rawSlotCount];
	
	/*
	NSBrowserCell *cell = [browser selectedCellInColumn:column-1];
	Objc2Io *obj = [cell objectValue];
	return [obj _rawSlotCount];
	*/
	return 0;
}

/*
- (id)browser:(NSBrowser *)browser objectValueForItem:(id)item
{
	return nil;
}

- (NSString *)browser:(NSBrowser *)sender titleOfColumn:(NSInteger)column
{
	if(column == 0) return @"root";
	//NSUInteger index = [[browser selectedRowIndexesInColumn:column-1] firstIndex];
	return @"";
}
*/

- (void)browser:(NSBrowser *)sender 
	willDisplayCell:(id)cell 
	atRow:(NSInteger)row 
	column:(NSInteger)column
{
	Objc2Io *obj = [columns objectAtIndex:column];
	NSString *slotName = [[obj _rawSlotNames] objectAtIndex:row];
	//[cell setObjectValue:obj];
	[cell setTitle:slotName];
}	

/*
- (void)browser:(NSBrowser *)sender createRowsForColumn:(NSInteger)column inMatrix:(NSMatrix *)matrix
{
	printf("---------------createRowsForColumn:\n");
}
*/

@end
