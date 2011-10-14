#import <Cocoa/Cocoa.h>
#import <IoLanguageKit/IoLanguageKit.h>

@interface IoBrowserController : NSObject <NSBrowserDelegate> 
{
	IBOutlet NSBrowser *browser;
	NSMutableArray *columns;
}

@property (assign) IBOutlet NSWindow *window;
@property (nonatomic, retain) NSMutableArray *columns;

/*
- (NSIndexSet *)browser:(NSBrowser *)browser 
selectionIndexesForProposedSelection:(NSIndexSet *)proposedSelectionIndexes 
			   inColumn:(NSInteger)column;
*/			   
//- (BOOL)browser:(NSBrowser *)sender selectRow:(NSInteger)row inColumn:(NSInteger)column;
//- (NSInteger)browser:(NSBrowser *)browser numberOfChildrenOfItem:(id)item;

- (NSInteger)browser:(NSBrowser *)sender numberOfRowsInColumn:(NSInteger)column;

//- (id)browser:(NSBrowser *)browser objectValueForItem:(id)item;

//- (NSString *)browser:(NSBrowser *)sender titleOfColumn:(NSInteger)column;

- (void)browser:(NSBrowser *)sender 
willDisplayCell:(id)cell 
		  atRow:(NSInteger)row 
		 column:(NSInteger)column;

//- (void)browser:(NSBrowser *)sender createRowsForColumn:(NSInteger)column inMatrix:(NSMatrix *)matrix;

@end
