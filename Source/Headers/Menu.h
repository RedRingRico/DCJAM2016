#ifndef __DCJAM_MENU_H__
#define __DCJAM_MENU_H__

#include <Memory.h>
#include <Text.h>
#include <shinobi.h>
#include <kamui2.h>

#define MNU_OK 0
#define MNU_ERROR 1
#define MNU_FATALERROR -1
#define MNU_UNKNOWNHIGHLIGHT -100

typedef Sint32 ( *MenuFunction )( void * );

typedef enum
{
	SELECTION_HIGHLIGHT_TYPE_BOX,
	SELECTION_HIGHLIGHT_TYPE_STRING
}SELECTION_HIGHLIGHT_TYPE;

typedef enum
{
	SELECTION_HIGHLIGHT_STRING_POSITION_LEFT,
	SELECTION_HIGHLIGHT_STRING_POSITION_RIGHT,
	SELECTION_HIGHLIGHT_STRING_POSITION_TOP,
	SELECTION_HIGHLIGHT_STRING_POSITION_BOTTOM
}SELECTION_HIGHLIGHT_STRING_POSITION;

typedef enum
{
	MENU_ITEM_ALIGNMENT_LEFT,
	MENU_ITEM_ALIGNMENT_RIGHT,
	MENU_ITEM_ALIGNMENT_CENTRE
}MENU_ITEM_ALIGNMENT;

typedef struct _tagMENU_ITEM
{
	char					*pName;
	MenuFunction			Function;
	struct _tagMENU_ITEM	*pNext;
}MENU_ITEM, *PMENU_ITEM;

typedef struct _tagSELECTION_HIGHLIGHT
{
	SELECTION_HIGHLIGHT_TYPE	Type;
	Uint32						Flags;
	KMPACKEDARGB				HighlightColour;
	/* Pulse rate in microseconds */
	Uint32						TextPulseRate;
	Uint32						TextHighlightPulseRate;
	Uint32						TextPulseRateElapsed;
	Uint32						TextHighlightPulseRateElapsed;
	Uint32						LastTime;
}SELECTION_HIGHLIGHT, *PSELECTION_HIGHLIGHT;

typedef struct _tagSELECTION_HIGHLIGHT_BOX
{
	SELECTION_HIGHLIGHT	Base;
	KMPACKEDARGB		Colour;
}SELECTION_HIGHLIGHT_BOX, *PSELECTION_HIGHLIGHT_BOX;

typedef struct _tagSELECTION_HIGHLIGHT_STRING
{
	SELECTION_HIGHLIGHT					Base;
	char								*pString;
	SELECTION_HIGHLIGHT_STRING_POSITION	Position;
}SELECTION_HIGHLIGHT_STRING, *PSELECTION_HIGHLIGHT_STRING;

typedef struct _tagMENU
{
	PMENU_ITEM				pMenuItems;
	PMEMORY_BLOCK			pMemoryBlock;
	size_t					MenuItemCount;
	size_t					SelectedMenuItem;
	PSELECTION_HIGHLIGHT	pSelectionHighlight;
	PGLYPHSET				pGlyphSet;
	KMPACKEDARGB			TextColour;
	MENU_ITEM_ALIGNMENT		MenuItemAlignment;
}MENU, *PMENU;

Sint32 MNU_Initialise( PMENU p_pMenu, PMENU_ITEM p_pMenuItems,
	size_t p_MenuItemCount, PSELECTION_HIGHLIGHT p_pSelectionHighlight,
	PGLYPHSET p_pGlyphSet, KMPACKEDARGB p_TextColour,
	MENU_ITEM_ALIGNMENT p_MenuItemAlignment, PMEMORY_BLOCK p_pMemoryBlock );
void MNU_Terminate( PMENU p_pMenu );

void MNU_SelectNextMenuItem( PMENU p_pMenu );
void MNU_SelectPreviousMenuItem( PMENU p_pMenu );
PMENU_ITEM MNU_GetSelectedMenuItem( PMENU p_pMenu );

void MNU_Render( PMENU p_pMenu, float p_Spacing, float p_X, float p_Y );

#endif /* __DCJAM_MENU_H__ */

