#include <Hardware.h>
#include <Memory.h>
#include <Log.h>
#include <Renderer.h>
#include <Peripheral.h>
#include <GitVersion.h>
#include <Text.h>
#include <Texture.h>
#include <GameStateManager.h>
#include <GameStates/AspectRatioSelect.h>
#include <GameStates/MainMenu.h>

typedef struct _tagLOAD_ARGS
{
	float	Progress;
}LOAD_ARGS, *PLOAD_ARGS;

typedef void ( *LOAD_CALLBACK )( PLOAD_ARGS p_pLoadArgs );

void ProcessFiles( void );
Sint32 LoadAsync( const char *p_pFileName, PMEMORY_BLOCK p_pMemoryBlock,
	LOAD_CALLBACK p_pCallback );

void main( void )
{
	/* Chunks of memory to allocate from in different sub-systems */
	size_t SystemMemorySize, GraphicsMemorySize, AudioMemorySize;
	void *pSystemMemory = NULL, *pGraphicsMemory = NULL, *pAudioMemory = NULL;
	MEMORY_BLOCK SystemMemoryBlock, GraphicsMemoryBlock, AudioMemoryBlock;
	NATIVE_MEMORY_FREESTAT MemoryFree;
	DREAMCAST_RENDERERCONFIGURATION RendererConfiguration;
	RENDERER Renderer;
	PKMSURFACEDESC Framebuffer[ 2 ];
	KMSURFACEDESC FrontBuffer, BackBuffer;
	SYE_CBL AVCable;
	bool Run = true;
	GLYPHSET WhiteRabbitGlyphSet;
	TEXTURE WhiteRabbitTexture;
	KMPACKEDARGB TextColour;
	float TextLength;
	HW_CONSOLEID ConsoleID;
	GAMESTATE_MANAGER GameStateManager;
	GAMESTATE_MEMORY_BLOCKS GameStateMemoryBlocks;

	if( HW_Initialise( KM_DSPBPP_RGB888, &AVCable, &MemoryFree ) != HW_OK )
	{
		goto MainCleanup;
	}

	LOG_Initialise( NULL );
	LOG_Debug( "DCJAM 2016" );
	LOG_Debug( "Version: %s", GIT_VERSION );

	SystemMemorySize = MEM_MIB( 4 );
	GraphicsMemorySize = MEM_MIB( 4 );
	AudioMemorySize = MEM_MIB( 1 );

	if( ( pSystemMemory = syMalloc( SystemMemorySize ) ) == NULL )
	{
		LOG_Debug( "Could not allocate system memory of size: %ld",
			SystemMemorySize );

		goto MainCleanup;
	}

	if( ( pGraphicsMemory = syMalloc( GraphicsMemorySize ) ) == NULL )
	{
		LOG_Debug( "Could not allocate graphics memory of size: %ld",
			GraphicsMemorySize );

		goto MainCleanup;
	}

	if( ( pAudioMemory = syMalloc( AudioMemorySize ) ) == NULL )
	{
		LOG_Debug( "Could not allocate audio memory of size: %ld",
			AudioMemorySize );

		goto MainCleanup;
	}

	if( MEM_InitialiseMemoryBlock( &SystemMemoryBlock, pSystemMemory,
		SystemMemorySize, 32, "System Memory" ) != MEM_OK )
	{
		LOG_Debug( "Failed to initialise the system memory block of size: %ld",
			SystemMemorySize );

		goto MainCleanup;
	}

	if( MEM_InitialiseMemoryBlock( &GraphicsMemoryBlock, pGraphicsMemory,
		GraphicsMemorySize, 32, "Graphics Memory" ) != MEM_OK )
	{
		LOG_Debug( "Failed to initialise the graphics memory block of size: "
			"%ld", GraphicsMemorySize );

		goto MainCleanup;
	}

	if( MEM_InitialiseMemoryBlock( &AudioMemoryBlock, pAudioMemory,
		AudioMemorySize, 32, "Audio Memory" ) != MEM_OK )
	{
		LOG_Debug( "Failed to initialise the audio memory block of size: %ld",
			AudioMemorySize );

		goto MainCleanup;
	}

	memset( &RendererConfiguration, 0, sizeof( RendererConfiguration ) );

	Framebuffer[ 0 ] = &FrontBuffer;
	Framebuffer[ 1 ] = &BackBuffer;

	RendererConfiguration.ppSurfaceDescription = Framebuffer;
	RendererConfiguration.FramebufferCount = 2;
	RendererConfiguration.TextureMemorySize = MEM_MIB( 5 );
	RendererConfiguration.MaximumTextureCount = 4096;
	RendererConfiguration.MaximumSmallVQTextureCount = 0;
	RendererConfiguration.VertexBufferWorkAreaSize = MEM_MIB( 1 );
	RendererConfiguration.PassCount = 1;

	RendererConfiguration.PassInfo[ 0 ].dwRegionArrayFlag =
		KM_PASSINFO_AUTOSORT;
	RendererConfiguration.PassInfo[ 0 ].nDirectTransferList =
		KM_OPAQUE_POLYGON;
	RendererConfiguration.PassInfo[ 0 ].fBufferSize[ 0 ] = 0.0f;
	RendererConfiguration.PassInfo[ 0 ].fBufferSize[ 1 ] = 0.0f;
	RendererConfiguration.PassInfo[ 0 ].fBufferSize[ 2 ] = 50.0f;
	RendererConfiguration.PassInfo[ 0 ].fBufferSize[ 3 ] = 0.0f;
	RendererConfiguration.PassInfo[ 0 ].fBufferSize[ 4 ] = 50.0f;

	RendererConfiguration.pMemoryBlock = &GraphicsMemoryBlock;

	if( REN_Initialise( &Renderer, &RendererConfiguration ) != REN_OK )
	{
		LOG_Debug( "Failed to initialise the renderer" );

		goto MainCleanup;
	}

	if( TXT_Initialise( ) != TXT_OK )
	{
		LOG_Debug( "Failed to initialise the text system" );

		goto MainCleanup;
	}

	if( TXT_CreateGlyphSetFromFile( "/FONTS/WHITERABBIT.FNT",
		&WhiteRabbitGlyphSet, &SystemMemoryBlock ) != TXT_OK )
	{
		LOG_Debug( "Failed to load the glyph descriptions" );

		goto MainCleanup;
	}

	if( TXT_SetTextureForGlyphSet( "/FONTS/WHITERABBIT.PVR",
		&WhiteRabbitGlyphSet, &GraphicsMemoryBlock ) != TXT_OK )
	{
		LOG_Debug( "Failed to load the glyph texture" );

		goto MainCleanup;
	}

	REN_SetClearColour( 0.0f, 17.0f / 255.0f, 43.0f / 255.0f );
	TextColour.dwPacked = 0xFFFFFFFF;

	if( HW_GetConsoleID( &ConsoleID ) != HW_OK )
	{
		LOG_Debug( "Failed to get the console ID" );

		goto MainCleanup;
	}

	GameStateMemoryBlocks.pSystemMemoryBlock = &SystemMemoryBlock;
	GameStateMemoryBlocks.pGraphicsMemoryBlock = &GraphicsMemoryBlock;
	GameStateMemoryBlocks.pAudioMemoryBlock = &AudioMemoryBlock;

	if( GSM_Initialise( &GameStateManager, &GameStateMemoryBlocks ) != 0 )
	{
		LOG_Debug( "Failed to initialise the game state manager\n" );

		goto MainCleanup;
	}

	GSM_RegisterGlyphSet( &GameStateManager, GSM_GLYPH_SET_DEBUG,
		&WhiteRabbitGlyphSet );
	GSM_RegisterGlyphSet( &GameStateManager, GSM_GLYPH_SET_GUI_1,
		&WhiteRabbitGlyphSet );

	GS_ARS_RegisterWithGameStateManager( &GameStateManager );
	GS_MM_RegisterWithGameStateManager( &GameStateManager );

#if defined ( DCJAM_BUILD_DEBUG )
	GSM_ChangeGameState( &GameStateManager, GAME_STATE_MAINMENU, NULL, NULL );
#else
	if( AVCable == SYE_CBL_PAL )
	{
	}
	else
	{
		GSM_ChangeGameState( &GameStateManager, GAME_STATE_ASPECTRATIOSELECT,
			NULL, NULL );
	}
#endif /* DCJAM_BUILD_DEBUG */

	g_Peripherals[ 0 ].press = 0;

	while( GSM_IsRunning( &GameStateManager ) == true )
	{
		GSM_Run( &GameStateManager );
	}

	/*while( Run )
	{
		float TextLength = TXT_MeasureString( &WhiteRabbitGlyphSet,
			"Red Ring Rico's DCJAM 2016 Entry" );

		if( g_Peripherals[ 0 ].on & PDD_DGT_ST )
		{
			Run = false;
		}

		REN_Clear( );

		TXT_RenderString( &WhiteRabbitGlyphSet, TextColour,
			320.0f - TextLength * 0.5f,
			480.0f - WhiteRabbitGlyphSet.LineHeight * 3.0f,
			"Red Ring Rico's DCJAM 2016 Entry" );

		TextLength = TXT_MeasureString( &WhiteRabbitGlyphSet,
			ConsoleID.ConsoleIDString );
		TXT_RenderString( &WhiteRabbitGlyphSet, TextColour,
			320.0f - TextLength * 0.5f,
			480.0f - WhiteRabbitGlyphSet.LineHeight * 2.0f,
			ConsoleID.ConsoleIDString );

		REN_SwapBuffers( );
	}*/

MainCleanup:
	GSM_Terminate( &GameStateManager );

	REN_Terminate( &Renderer );

	/* Useful debug information */
	if( pAudioMemory != NULL )
	{
		MEM_GarbageCollectMemoryBlock( &AudioMemoryBlock );
		MEM_ListMemoryBlocks( &AudioMemoryBlock );
		syFree( pAudioMemory );
	}
	if( pGraphicsMemory != NULL )
	{
		MEM_GarbageCollectMemoryBlock( &GraphicsMemoryBlock );
		MEM_ListMemoryBlocks( &GraphicsMemoryBlock );
		syFree( pGraphicsMemory );
	}
	if( pSystemMemory != NULL )
	{
		MEM_GarbageCollectMemoryBlock( &SystemMemoryBlock );
		MEM_ListMemoryBlocks( &SystemMemoryBlock );
		syFree( pSystemMemory );
	}

	LOG_Terminate( );

	HW_Terminate( );
	HW_Reboot( );
}

/*Sint32 LoadAsync( const char *p_pFileName, PMEMORY_BLOCK p_pMemoryBlock,
	LOAD_CALLBACK p_pCallback )
{
	GDFS FileHandle;
	long FileBlocks;
	PKMDWORD pFile
	Sint32 FileSize = 0, DataOffset = 0;
	char MemoryName[ 64 ];

	if( !( FileHandle = FS_OpenFile( p_pFileName ) ) )
	{
		LOG_Debug( "Failed to load texture: \"%s\"", p_pFileName );

		return TEX_LOADERROR;
	}

	gdFsGetFileSize( FileHandle, &FileSize );
	gdFsGetFileSctSize( FileHandle, &FileBlocks );

#if defined ( DCJAM_BUILD_DEBUG )
	sprintf( MemoryName, "[FILE] %s", p_pFileName );
#endif * DCJAM_BUILD_DEBUG *

	pFile = MEM_AllocateFromMemoryBlock( p_pMemoryBlock, FileBlocks * 2048,
		p_pFileName );

	gdFsReqRd32( FileHandle, FileBlocks, pFile );

	* It would be really cool if this didn't block *
	while( gdFsGetStat( FileHandle ) != GDD_STAT_COMPLETE )
	{
	}

	gdFsClose( FileHandle );

	FileSize /= 4;

	while( TextureOffset < FileSize )
	{
		if( pTexture[ TextureOffset ] == 0x48494247 )
		{
			memcpy( &GlobalIndex, &pTexture[ TextureOffset / 4 ],
				sizeof( GlobalIndex ) );
			DataOffset += ( sizeof( GLOBAL_INDEX ) + GlobalIndex.Size ) / 4;
			TextureOffset += ( sizeof( GLOBAL_INDEX ) + GlobalIndex.Size ) / 4;
		}
		
		if( pTexture[ TextureOffset ] == 0x54525650 )
		{
			memcpy( &PVRTHeader, &pTexture[ TextureOffset / 4 ],
				sizeof( PVRTHeader ) );

			DataOffset += sizeof( PVRT_HEADER ) / 4;
			TextureOffset +=
				( ( sizeof( Uint32 ) * 2 ) + PVRTHeader.Size ) / 4;

			p_pTexture->Width = PVRTHeader.Width;
			p_pTexture->Height = PVRTHeader.Height;
			p_pTexture->Flags = PVRTHeader.Flags;
		}
	}
}*/

