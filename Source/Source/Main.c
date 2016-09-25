#include <Hardware.h>
#include <Memory.h>
#include <Log.h>
#include <GitVersion.h>

void main( void )
{
	SYE_CBL AVCable;
	NATIVE_MEMORY_FREESTAT MemoryFree;

	if( HW_Initialise( KM_DSPBPP_RGB888, &AVCable, &MemoryFree ) != HW_OK )
	{
		HW_Terminate( );
		HW_Reboot( );
	}

	LOG_Initialise( NULL );
	LOG_Debug( "DCJAM 2016" );
	LOG_Debug( "Version: %s", GIT_VERSION );

	while( 1 )
	{
	}
}

