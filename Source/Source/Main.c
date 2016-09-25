#include <Hardware.h>
#include <Memory.h>
#include <Log.h>
#include <Renderer.h>
#include <Peripheral.h>
#include <GitVersion.h>

void main( void )
{
	/* Chunks of memory to allocate from in different sub-systems */
	size_t SystemMemorySize, GraphicsMemorySize, AudioMemorySize;
	void *pSystemMemory, *pGraphicsMemory, *pAudioMemory;
	MEMORY_BLOCK SystemMemoryBlock, GraphicsMemoryBlock, AudioMemoryBlock;
	NATIVE_MEMORY_FREESTAT MemoryFree;
	DREAMCAST_RENDERERCONFIGURATION RendererConfiguration;
	RENDERER Renderer;
	PKMSURFACEDESC Framebuffer[ 2 ];
	KMSURFACEDESC FrontBuffer, BackBuffer;
	SYE_CBL AVCable;
	bool Run = true;

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
		goto MainCleanup;
	}

	REN_SetClearColour( 0.0f, 1.0f, 0.0f );

	g_Peripherals[ 0 ].press = 0;

	while( Run )
	{
		if( g_Peripherals[ 0 ].on & PDD_DGT_ST )
		{
			Run = false;
		}

		REN_Clear( );
		REN_SwapBuffers( );
	}

MainCleanup:
	LOG_Terminate( );
	HW_Terminate( );
	HW_Reboot( );
}

