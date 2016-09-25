#include <Hardware.h>
#include <Memory.h>

KMVOID PALExtCallback( PKMVOID p_pArgs );

Sint32 HW_Initialise( KMBPPMODE p_BPP, SYE_CBL *p_pCableType,
	PNATIVE_MEMORY_FREESTAT p_pMemoryFreeStat )
{
	KMDISPLAYMODE DisplayMode;

	switch( syCblCheck( ) )
	{
		case SYE_CBL_NTSC:
		{
			DisplayMode = KM_DSPMODE_NTSCNI640x480;

			break;
		}
		case SYE_CBL_PAL:
		{
			if( p_pCableType )
			{
				( *p_pCableType ) = SYE_CBL_PAL;
			}

			DisplayMode = KM_DSPMODE_PALNI640x480EXT;

			break;
		}
		case SYE_CBL_VGA:
		{
			DisplayMode = KM_DSPMODE_VGA;

			break;
		}
		default:
		{
			HW_Reboot( );
		}
	}

	set_imask( 15 );

	/* Initialise the CPU, G1 bus, interrupt controller, cache, and timer */
	syHwInit( );
	MEM_Initialise( p_pMemoryFreeStat );
	syStartGlobalConstructor( );
	kmInitDevice( KM_DREAMCAST );

	/* No antialiasing, dither if 16-BPP is enabled */
	kmSetDisplayMode( DisplayMode, p_BPP, KM_DITHER, KM_AAMODE );
	kmSetWaitVsyncCount( 1 );
	
	/* Initialise the interrupt controller and libraries required for after the
	 * graphics library is initialised */
	syHwInit2( );

	PER_Initialise( );

	/* Start the real-time clock */
	syRtcInit( );

	set_imask( 0 );

	if( syCblCheck( ) == SYE_CBL_PAL )
	{
		kmSetPALEXTCallback( PALExtCallback, NULL );
		kmSetDisplayMode( DisplayMode, p_BPP, KM_DITHER, KM_AAMODE );
	}

	return HW_OK;
}

void HW_Terminate( void )
{
	syRtcFinish( );
	PER_Terminate( );
	kmUnloadDevice( );
	syStartGlobalDestructor( );
	MEM_Terminate( );
	syHwFinish( );
	set_imask( 15 );
}

void HW_Reboot( void )
{
	syBtExit( );
}

KMVOID PALExtCallback( PKMVOID p_pArgs )
{
	PKMPALEXTINFO pPALInfo;

	pPALInfo = ( PKMPALEXTINFO )p_pArgs;
	pPALInfo->nPALExtMode = KM_PALEXT_HEIGHT_RATIO_1_166;
}

