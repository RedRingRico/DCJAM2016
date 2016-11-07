#ifndef __DCJAM_FILESYSTEM_H__
#define __DCJAM_FILESYSTEM_H__

#include <shinobi.h>
#include <Stack.h>
#include <Memory.h>

#define FS_OK			0
#define FS_ERROR		1
#define FS_FATALERROR	-1
#define FS_DRIVEERROR	-10

typedef struct _tagFILE_CHUNK
{
	Uint32 ID;
	Uint32 Size;
}FILE_CHUNK, *PFILE_CHUNK;

typedef struct _tagFILELOADARGS
{
	float	Progress;
}FILELOADARGS, *PFILELOADARGS;

typedef struct _tagASYNC_FS
{
	PSTACK	pStack;
}ASYNC_FS, *PASYNC_FS;

typedef void ( *FS_LOADCALLBACK )( PFILELOADARGS p_pLoadArgs );

Sint32 FS_Initialise( void );
void FS_Terminate( void );

GDFS FS_OpenFile( char *p_pFilePath );

/* Asynchronous API */
Sint32 FSA_Initialise( PASYNC_FS p_pAsyncFS, size_t p_MaxFiles,
	PMEMORY_BLOCK p_pMemoryBlock );
void FSA_ProcessFiles( void );
Sint32 FSA_OpenFile( const char *p_pFileName, PMEMORY_BLOCK p_pMemoryBlock,
	FS_LOADCALLBACK p_pCallback );

#endif /* __DCJAM_FILESYSTEM_H__ */

