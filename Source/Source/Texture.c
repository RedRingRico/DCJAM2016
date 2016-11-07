#include <Texture.h>
#include <FileSystem.h>
#include <Log.h>

typedef struct _tagGLOBAL_INDEX
{
	char	ID[ 4 ];
	Uint32	Size;
}GLOBAL_INDEX, *PGLOBAL_INDEX;

typedef struct _tagPVRT_HEADER
{
	char	ID[ 4 ];
	Uint32	Size;
	Uint32	Flags;
	Uint16	Width;
	Uint16	Height;
}PVRT_HEADER, *PPVRT_HEADER;

Sint32 TEX_CreateTextureFromFile( PTEXTURE p_pTexture, const char *p_pFileName,
	PMEMORY_BLOCK p_pMemoryBlock )
{
	GDFS FileHandle;
	long FileBlocks;
	PKMDWORD pTexture;
	GLOBAL_INDEX GlobalIndex;
	PVRT_HEADER PVRTHeader;
	Sint32 TextureOffset = 0, FileSize = 0, DataOffset = 0;
	char MemoryName[ 64 ];

	if( p_pTexture->ReferenceCount != 0 )
	{
		++p_pTexture->ReferenceCount;

		return TEX_OK;
	}

	if( !( FileHandle = FS_OpenFile( p_pFileName ) ) )
	{
		LOG_Debug( "Failed to load texture: \"%s\"", p_pFileName );

		return TEX_LOADERROR;
	}

	gdFsGetFileSize( FileHandle, &FileSize );
	gdFsGetFileSctSize( FileHandle, &FileBlocks );

#if defined ( DCJAM_BUILD_DEBUG )
	sprintf( MemoryName, "[TEXTURE] %s", p_pFileName );
#endif /* DCJAM_BUILD_DEBUG */

	pTexture = MEM_AllocateFromMemoryBlock( p_pMemoryBlock, FileBlocks * 2048,
		p_pFileName );

	gdFsReqRd32( FileHandle, FileBlocks, pTexture );

	/* It would be really cool if this didn't block */
	while( gdFsGetStat( FileHandle ) != GDD_STAT_COMPLETE )
	{
	}

	gdFsClose( FileHandle );

	FileSize /= 4;

	while( TextureOffset < FileSize )
	{
		/* GBIX */
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

	kmCreateTextureSurface( &p_pTexture->SurfaceDescription,
		p_pTexture->Width, p_pTexture->Height, p_pTexture->Flags );

	kmLoadTexture( &p_pTexture->SurfaceDescription, pTexture + DataOffset );

	while( kmQueryFinishLastTextureDMA( ) != KMSTATUS_SUCCESS )
	{
	}

	MEM_FreeFromMemoryBlock( p_pMemoryBlock, pTexture );

	++p_pTexture->ReferenceCount;

	return TEX_OK;
}

void TEX_DeleteTexture( PTEXTURE p_pTexture )
{
	--p_pTexture->ReferenceCount;

	if( p_pTexture->ReferenceCount == 0 )
	{
		kmFreeTexture( &p_pTexture->SurfaceDescription );
	}
}

