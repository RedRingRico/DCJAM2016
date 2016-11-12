#include <GameStates/MainMenu.h>
#include <GameState.h>
#include <Menu.h>
#include <Log.h>
#include <Peripheral.h>

typedef struct _tagMAINMENU_GAMESTATE
{
	GAMESTATE	Base;
	MENU		Menu;
}MAINMENU_GAMESTATE, *PMAINMENU_GAMESTATE;

static MAINMENU_GAMESTATE MainMenuState;

static Sint32 BootROM( void *p_pArgs );

static Sint32 Load( void *p_pArgs )
{
	KMPACKEDARGB TextColour, HighlightColour;
	MENU_ITEM MenuItems[ 3 ];
	SELECTION_HIGHLIGHT_STRING SelectionHighlight;

	TextColour.dwPacked = 0xFFFFFFFF;
	HighlightColour.dwPacked = 0xFF00FF00;

	MenuItems[ 0 ].pName = "Arcade";
	MenuItems[ 0 ].Function = NULL;

	MenuItems[ 1 ].pName = "Options";
	MenuItems[ 1 ].Function = NULL;

	MenuItems[ 2 ].pName = "Exit to BootROM";
	MenuItems[ 2 ].Function = &BootROM;

	SelectionHighlight.Base.Type = SELECTION_HIGHLIGHT_TYPE_STRING;
	SelectionHighlight.Base.HighlightColour = HighlightColour;
	SelectionHighlight.pString = "";

	if( MNU_Initialise( &MainMenuState.Menu, MenuItems,
		sizeof( MenuItems ) / sizeof( MenuItems[ 0 ] ), &SelectionHighlight,
		GSM_GetGlyphSet( MainMenuState.Base.pGameStateManager,
			GSM_GLYPH_SET_GUI_1 ), TextColour, MENU_ITEM_ALIGNMENT_CENTRE,
		MainMenuState.Base.pGameStateManager->MemoryBlocks.pSystemMemoryBlock )
			!= GSM_OK )
	{
		LOG_Debug( "GS_MM_Load <ERROR> Failed to initialise the menu\n" );

		return GSM_FATALERROR;
	}

	return GSM_OK;
}

static Sint32 Initialise( void *p_pArgs )
{
	return GSM_OK;
}

static Sint32 Update( void *p_pArgs )
{
	if( MainMenuState.Base.Paused == false )
	{
		if( g_Peripherals[ 0 ].press & PDD_DGT_KU )
		{
			MNU_SelectPreviousMenuItem( &MainMenuState.Menu );
		}

		if( g_Peripherals[ 0 ].press & PDD_DGT_KD )
		{
			MNU_SelectNextMenuItem( &MainMenuState.Menu );
		}

		if( g_Peripherals[ 0 ].press & PDD_DGT_TA )
		{
			PMENU_ITEM pMenuItem;

			pMenuItem = MNU_GetSelectedMenuItem( &MainMenuState.Menu );

			if( pMenuItem != NULL )
			{
				if( pMenuItem->Function != NULL )
				{
					pMenuItem->Function( NULL );
				}
			}
		}
	}

	return GSM_OK;
}

static Sint32 Render( void *p_pArgs )
{
	KMPACKEDARGB TextColour;
	float TextLength;

	if( MainMenuState.Base.Paused == false )
	{
		PGLYPHSET pGlyphSet;
		pGlyphSet = GSM_GetGlyphSet( MainMenuState.Base.pGameStateManager,
			GSM_GLYPH_SET_GUI_1 );

		TextColour.dwPacked = 0xFFFFFFFF;

		REN_Clear( );

		TextLength = TXT_MeasureString( pGlyphSet,
			"Red Ring Rico's DC JAM 2016 Entry" );
		TXT_RenderString( pGlyphSet, TextColour,
			320.0f - ( TextLength * 0.5f ), 32.0f,
			"Red Ring Rico's DC JAM 2016 Entry" );

		TextLength = TXT_MeasureString( pGlyphSet, "[A] Select" );
		TXT_RenderString( pGlyphSet, TextColour,
			640.0f - 64.0f - TextLength,
			480.0f - ( 32.0f + pGlyphSet->LineHeight ), "[A] Select" );

		MNU_Render( &MainMenuState.Menu, 1.5f, 320.0f, 240.0f );

		REN_SwapBuffers( );
	}

	return GSM_OK;
}

static Sint32 Terminate( void *p_pArgs )
{
	return GSM_OK;
}

static Sint32 Unload( void *p_pArgs )
{
	MNU_Terminate( &MainMenuState.Menu );

	return GSM_OK;
}

Sint32 GS_MM_RegisterWithGameStateManager(
	PGAMESTATE_MANAGER p_pGameStateManager )
{
	MainMenuState.Base.Load = &Load;
	MainMenuState.Base.Initialise = &Initialise;
	MainMenuState.Base.Update = &Update;
	MainMenuState.Base.Render = &Render;
	MainMenuState.Base.Terminate = &Terminate;
	MainMenuState.Base.Unload = &Unload;
	MainMenuState.Base.pGameStateManager = p_pGameStateManager;

	return GSM_RegisterGameState( p_pGameStateManager, GAME_STATE_MAINMENU,
		( GAMESTATE * )&MainMenuState );
}

static Sint32 BootROM( void *p_pArgs )
{
	GSM_Quit( MainMenuState.Base.pGameStateManager );

	return GSM_OK;
}

