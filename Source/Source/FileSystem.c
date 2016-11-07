#include <FileSystem.h>
#include <Log.h>
#include <Hardware.h>
#include <string.h>

#define GDROM_MAX_OPEN_FILES	10
#define GDROM_BUFFER_COUNT		512
#define GDROM_RETRY_COUNT		10

static Uint8 g_GDFSWork[ GDFS_WORK_SIZE( GDROM_MAX_OPEN_FILES ) + 32 ];
static Uint8 g_GDFSHandleTable[ GDFS_DIRREC_SIZE( GDROM_BUFFER_COUNT ) + 32 ];
static GDFS_DIRREC g_RootDirectory;

void GDFSErrorCallback( void *p_pObject, long p_Error );

Sint32 FS_Initialise( void )
{
	Uint8 *pWork, *pDirectory;
	Sint32 Error, Itr;
	Uint32 RootDirectoryBuffer[ GDFS_DIRREC_SIZE( 64 ) ];

	pWork =
		( Uint8 * )( ( ( Uint32 )g_GDFSWork & 0xFFFFFFE0 ) + 0x20 );
	pDirectory =
		( Uint8 * )( ( ( Uint32 )g_GDFSHandleTable & 0xFFFFFFE0 ) + 0x20 );
	
	for( Itr = GDROM_RETRY_COUNT; Itr > 0; --Itr )
	{
		Error = gdFsInit( GDROM_MAX_OPEN_FILES, pWork, GDROM_BUFFER_COUNT,
			pDirectory );

		if( ( Error == GDD_ERR_TRAYOPEND ) ||
			( Error == GDD_ERR_UNITATTENT ) )
		{
			/* Debug builds are allowed to have their tray open */
#if defined ( DCJAM_BUILD_DEBUG )
			LOG_Debug( "Warning - GD ROM drive door open" );
			break;
#else
			return FS_FATALERROR;
#endif /* DCJAM_BUILD_DEBUG */
		}
		else if( Error == GDD_ERR_OK )
		{
			break;
		}
		else if( Error == GDD_ERR_NOTREADY )
		{
			LOG_Debug( "Warning - GD ROM drive not ready" );
#if defined ( DCJAM_BUILD_DEBUG )
			break;
#endif
		}
		else
		{
			LOG_Debug( "GD ROM Error code: %d", Error );
		}
	}

	if( Itr == 0 )
	{
		return FS_DRIVEERROR;
	}

	gdFsEntryErrFuncAll( GDFSErrorCallback, NULL );
	
	g_RootDirectory = gdFsCreateDirhn( RootDirectoryBuffer, 64 );
	gdFsLoadDir( ".", g_RootDirectory );

#if defined ( DCJAM_BUILD_DEBUG )
	LOG_Debug( "Initialised GD-ROM | Maximum open files: %d | Open buffer "
		"count: %d", GDROM_MAX_OPEN_FILES, GDROM_BUFFER_COUNT );
	LOG_Debug( "%s", GDD_VERSION_STR );
#endif /* DCJAM_BUILD_DEBUG */

	return FS_OK;
}

void FS_Terminate( void )
{
	gdFsFinish( );
}

GDFS FS_OpenFile( char *p_pFilePath )
{
	char *pFileName;
	char FileName[ GDD_FS_FNAMESIZE ];
	char FilePath[ 256 ];
	GDFS_DIRREC Directory;
	static Uint32 DirectoryBuffer[ GDFS_DIRREC_SIZE( 64 ) ];
	GDFS File;

	/* Save the direcotry used before entering */
	Directory = gdFsCreateDirhn( DirectoryBuffer, 64 );
	gdFsLoadDir( ".", Directory );

	/* For an absolute path, begin with the root directory */
	if( p_pFilePath[ 0 ] == '/' )
	{
		gdFsSetDir( g_RootDirectory );
	}

	/* Don't modify the input file path */
	strncpy( FilePath, p_pFilePath, strlen( p_pFilePath ) );
	FilePath[ strlen( p_pFilePath ) ] = '\0';

	/* Break the path down for GDFS to consume */
	pFileName = strtok( FilePath, "/" );

	File = NULL;

	while( pFileName != NULL )
	{
		GDFS_DIRINFO FileInformation;
		strcpy( FileName, pFileName );

		if( gdFsGetDirInfo( pFileName, &FileInformation ) != GDD_ERR_OK )
		{
			LOG_Debug( "FILE: \"%s\" invalid", FileName );

			goto OpenFileEnd;
		}

		if( FileInformation.flag & GDD_FF_DIRECTORY )
		{
			gdFsChangeDir( FileName );
		}

		pFileName = strtok( NULL, "/" );
	}

	File = gdFsOpen( FileName, NULL );

OpenFileEnd:
	/* Restore the directory used before entering the function */
	gdFsSetDir( Directory );

	return File;
}

void GDFSErrorCallback( void *p_pObject, long p_Error )
{
	LOG_Debug( "OK" );
	if( ( p_Error == GDD_ERR_TRAYOPEND ) ||
		( p_Error == GDD_ERR_UNITATTENT ) )
	{
#if defined ( DCJAM_BUILD_DEBUG )
		return;
#else
		/* Hard reset.  Maybe this callback should be set from main? */
		HW_Terminate( );
		HW_Reboot( );
#endif /* DCJAM_BUILD_DEBUG */
	}
}
