#include <Text.h>
#include <FileSystem.h>
#include <Log.h>
#include <string.h>

KMSTRIPHEAD g_TXT_StripHead;
KMVERTEX_16 g_TXT_TextBuffer[ 128 ];

#define BMF_ID_INFO		1
#define BMF_ID_COMMON	2
#define BMF_ID_PAGES	3
#define BMF_ID_CHARS	4
#define BMF_ID_KERNING	5

#define BMF_COMMON_PACKED_SIZE	15

typedef struct _tagBMF_CHUNK
{
	Uint8	ID;
	Uint8	Size[ 4 ];
}BMF_CHUNK, *PBMF_CHUNK;

typedef struct _tagBMF_COMMON
{
	Uint16	LineHeight;
	Uint16	BaseLine;
	Uint16	Width;
	Uint16	Height;
	Uint16	Pages;
	Uint8	BitField;
	Uint8	Alpha;
	Uint8	Red;
	Uint8	Green;
	Uint8	Blue;
}BMF_COMMON, *PBMF_COMMON;

typedef struct _tagBMF_CHAR
{
	Uint32	ID;
	Uint16	X;
	Uint16	Y;
	Uint16	Width;
	Uint16	Height;
	Sint16	XOffset;
	Sint16	YOffset;
	Sint16	XAdvance;
	Uint8	Page;
	Uint8	Channel;
}BMF_CHAR, *PBMF_CHAR;

Sint32 TXT_Initialise( void )
{
	KMPACKEDARGB BaseColour;
	KMSTRIPCONTEXT TextContext;

	memset( &TextContext, 0, sizeof( KMSTRIPCONTEXT ) );

	TextContext.nSize = sizeof( TextContext );
	TextContext.StripControl.nListType = KM_TRANS_POLYGON;
	TextContext.StripControl.nUserClipMode = KM_USERCLIP_DISABLE;
	TextContext.StripControl.nShadowMode = KM_NORMAL_POLYGON;
	TextContext.StripControl.bOffset = KM_FALSE;
	TextContext.StripControl.bGouraud = KM_TRUE;
	TextContext.ObjectControl.nDepthCompare = KM_ALWAYS;
	TextContext.ObjectControl.nCullingMode = KM_NOCULLING;
	TextContext.ObjectControl.bZWriteDisable = KM_FALSE;
	TextContext.ObjectControl.bDCalcControl = KM_FALSE;
	BaseColour.dwPacked = 0xFFFFFFFF;
	TextContext.type.splite.Base = BaseColour;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].nSRCBlendingMode = KM_SRCALPHA;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].nDSTBlendingMode =
		KM_INVSRCALPHA;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].bSRCSelect = KM_FALSE;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].bDSTSelect = KM_FALSE;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].nFogMode = KM_NOFOG;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].bColorClamp = KM_FALSE;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].bUseAlpha = KM_TRUE;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].bIgnoreTextureAlpha = KM_FALSE;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].nFlipUV = KM_NOFLIP;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].nClampUV = KM_CLAMP_UV;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].nFilterMode = KM_BILINEAR;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].bSuperSampleMode = KM_FALSE;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].dwMipmapAdjust = 
		KM_MIPMAP_D_ADJUST_1_00;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].nTextureShadingMode =
		KM_MODULATE_ALPHA;
	TextContext.ImageControl[ KM_IMAGE_PARAM1 ].pTextureSurfaceDesc = NULL;

	kmGenerateStripHead16( &g_TXT_StripHead, &TextContext );

	return TXT_OK;
}

Sint32 TXT_CreateGlyphSetFromFile( char *p_pFileName, PGLYPHSET p_pGlyphSet,
	PMEMORY_BLOCK p_pMemoryBlock )
{
	GDFS FileHandle;
	long FileBlocks;
	char *pFileContents, *pLine, *pLineChar;
	char Token[ 256 ];
	size_t FilePosition = 0;
	BMF_CHUNK Chunk;
	Sint32 FileSize;

	if( !( FileHandle = FS_OpenFile( p_pFileName ) ) )
	{
		LOG_Debug( "Failed to open glyph file: \"%s\"", p_pFileName );

		return TXT_GLYPHFILEERROR;
	}

	gdFsGetFileSize( FileHandle, &FileSize );
	gdFsGetFileSctSize( FileHandle, &FileBlocks );
	
	pFileContents = MEM_AllocateFromMemoryBlock( p_pMemoryBlock,
		FileBlocks * 2048, p_pFileName );

	if( gdFsReqRd32( FileHandle, FileBlocks, pFileContents ) < 0 )
	{
		LOG_Debug( "Could not load the glyph file into memory" );

		return TXT_GLYPHMEMORYERROR;
	}

	while( gdFsGetStat( FileHandle ) != GDD_STAT_COMPLETE )
	{
	}

	gdFsClose( FileHandle );

	if( ( pFileContents[ 0 ] != 'B' ) ||
		( pFileContents[ 1 ] != 'M' ) ||
		( pFileContents[ 2 ] != 'F' ) ||
		( pFileContents[ 3 ] != 0x03 ) )
	{
		MEM_FreeFromMemoryBlock( p_pMemoryBlock, pFileContents );

		LOG_Debug( "Font file is not a binary file" );

		return TXT_BMFNOTBINARY;
	}

	FilePosition += 4;

	while( FilePosition != FileSize )
	{
		size_t TestSize = sizeof( BMF_CHAR );
		Sint32 Size = 0;

		memcpy( &Chunk, &pFileContents[ FilePosition ], sizeof( BMF_CHUNK ) );

		Size = ( Chunk.Size[ 0 ] );
		Size |= ( Chunk.Size[ 1 ] << 8 );
		Size |= ( ( Chunk.Size[ 2 ] ) << 16 );
		Size |= ( ( Chunk.Size[ 3 ] ) << 24 );

		FilePosition += sizeof( BMF_CHUNK );

		switch( Chunk.ID )
		{
			case BMF_ID_COMMON:
			{
				BMF_COMMON Common;

				memcpy( &Common, &pFileContents[ FilePosition ],
					BMF_COMMON_PACKED_SIZE );

				p_pGlyphSet->LineHeight = ( float )Common.LineHeight;
				p_pGlyphSet->BaseLine = ( float )Common.BaseLine;
				p_pGlyphSet->Width = ( float )Common.Width;
				p_pGlyphSet->Height = ( float )Common.Height;

				FilePosition += BMF_COMMON_PACKED_SIZE;

				break;
			}
			case BMF_ID_CHARS:
			{
				size_t CharCount = Size / sizeof( BMF_CHAR );
				size_t CharIndex;

				for( CharIndex = 0; CharIndex < CharCount; ++CharIndex )
				{
					BMF_CHAR Char;

					memcpy( &Char, &pFileContents[ FilePosition ],
						sizeof( BMF_CHAR ) );

					p_pGlyphSet->Glyphs[ Char.ID ].X = ( float )Char.X;
					p_pGlyphSet->Glyphs[ Char.ID ].Y = ( float )Char.Y;
					p_pGlyphSet->Glyphs[ Char.ID ].Width = ( float )Char.Width;
					p_pGlyphSet->Glyphs[ Char.ID ].Height =
						( float )Char.Height;
					p_pGlyphSet->Glyphs[ Char.ID ].XOffset =
						( float )Char.XOffset;
					p_pGlyphSet->Glyphs[ Char.ID ].YOffset =
						( float )Char.YOffset;
					p_pGlyphSet->Glyphs[ Char.ID ].XAdvance =
						( float )Char.XAdvance;

					FilePosition += sizeof( BMF_CHAR );
				}
				break;
			}
			default:
			{
				FilePosition += Size;

				break;
			}
		}
	}

	MEM_FreeFromMemoryBlock( p_pMemoryBlock, pFileContents );
	MEM_GarbageCollectMemoryBlock( p_pMemoryBlock );

	p_pGlyphSet->Texture.ReferenceCount = 0;

	return TXT_OK;
}

Sint32 TXT_SetTextureForGlyphSet( char *p_pFileName, GLYPHSET *p_pGlyphSet,
	PMEMORY_BLOCK p_pMemoryBlock )
{
	return TEX_CreateTextureFromFile( &p_pGlyphSet->Texture, p_pFileName,
		p_pMemoryBlock );
}

void TXT_RenderString( PGLYPHSET p_pGlyphSet, KMPACKEDARGB p_Colour,
	float p_X, float p_Y, char *p_pString )
{
	size_t StringLength, Char, IndexChar;
	float XPosition = p_X, YPosition = p_Y;
	KMDWORD ParamControl = KM_VERTEXPARAM_NORMAL;
	union
	{
		KMFLOAT	F[ 2 ];
		KMWORD	F16[ 4 ];
	}F;

	if( p_pString == NULL )
	{
		return;
	}

	StringLength = strlen( p_pString );

	if( StringLength == 0 )
	{
		return;
	}

	kmChangeStripTextureSurface( &g_TXT_StripHead, KM_IMAGE_PARAM1,
		&p_pGlyphSet->Texture.SurfaceDescription );

	kmChangeStripSpriteBaseColor( &g_TXT_StripHead, p_Colour );

	for( Char = 0; Char < StringLength; ++Char )
	{
		if( Char == StringLength - 1 )
		{
			ParamControl = KM_VERTEXPARAM_ENDOFSTRIP;
		}

		IndexChar = p_pString[ Char ] < 0 ?
			( p_pString[ Char ] & 0x6F ) + 128 : p_pString[ Char ];

		g_TXT_TextBuffer[ Char ].ParamControlWord = ParamControl;

		g_TXT_TextBuffer[ Char ].fAX =
			XPosition + p_pGlyphSet->Glyphs[ IndexChar ].XOffset;
		g_TXT_TextBuffer[ Char ].fAY =
			YPosition + p_pGlyphSet->Glyphs[ IndexChar ].YOffset;
		g_TXT_TextBuffer[ Char ].uA.fAZ = 256.0f;

		g_TXT_TextBuffer[ Char ].fBX = XPosition +
			p_pGlyphSet->Glyphs[ IndexChar ].Width +
			p_pGlyphSet->Glyphs[ IndexChar ].XOffset;
		g_TXT_TextBuffer[ Char ].fBY =
			YPosition + p_pGlyphSet->Glyphs[ IndexChar ].YOffset;
		g_TXT_TextBuffer[ Char ].uB.fBZ = 256.0f;

		g_TXT_TextBuffer[ Char ].fCX = XPosition +
			p_pGlyphSet->Glyphs[ IndexChar ].Width +
			p_pGlyphSet->Glyphs[ IndexChar ].XOffset;
		g_TXT_TextBuffer[ Char ].fCY = YPosition +
			p_pGlyphSet->Glyphs[ IndexChar ].Height +
			p_pGlyphSet->Glyphs[ IndexChar ].YOffset;
		g_TXT_TextBuffer[ Char ].uC.fCZ = 256.0f;

		g_TXT_TextBuffer[ Char ].fDX = XPosition +
			p_pGlyphSet->Glyphs[ IndexChar ].XOffset;
		g_TXT_TextBuffer[ Char ].fDY = YPosition +
			p_pGlyphSet->Glyphs[ IndexChar ].Height +
			p_pGlyphSet->Glyphs[ IndexChar ].YOffset;

		F.F[ 0 ] = p_pGlyphSet->Glyphs[ IndexChar ].Y / p_pGlyphSet->Height;
		F.F[ 1 ] = p_pGlyphSet->Glyphs[ IndexChar ].X / p_pGlyphSet->Width;
		g_TXT_TextBuffer[ Char ].dwUVA = ( ( KMDWORD )F.F16[ 3 ] << 16 ) |
			( KMDWORD )F.F16[ 1 ];

		F.F[ 0 ] = p_pGlyphSet->Glyphs[ IndexChar ].Y / p_pGlyphSet->Height;
		F.F[ 1 ] = ( p_pGlyphSet->Glyphs[ IndexChar ].X +
			p_pGlyphSet->Glyphs[ IndexChar ].Width ) / p_pGlyphSet->Width;
		g_TXT_TextBuffer[ Char ].dwUVB = ( ( KMDWORD )F.F16[ 3 ] << 16 ) |
			( KMDWORD )F.F16[ 1 ];

		F.F[ 0 ] = ( p_pGlyphSet->Glyphs[ IndexChar ].Y +
			p_pGlyphSet->Glyphs[ IndexChar ].Height ) / p_pGlyphSet->Height;
		F.F[ 1 ] = ( p_pGlyphSet->Glyphs[ IndexChar ].X +
			p_pGlyphSet->Glyphs[ IndexChar ].Width ) / p_pGlyphSet->Width;
		g_TXT_TextBuffer[ Char ].dwUVC = ( ( KMDWORD )F.F16[ 3 ] << 16 ) |
			( KMDWORD )F.F16[ 1 ];

		XPosition += p_pGlyphSet->Glyphs[ IndexChar ].XAdvance;
	}

	REN_DrawPrimitives16( &g_TXT_StripHead, g_TXT_TextBuffer, StringLength );
}

float TXT_MeasureString( PGLYPHSET p_pGlyphSet, char *p_pString )
{
	size_t StringLength, Char;
	float Width = 0.0f;

	StringLength = strlen( p_pString );

	for( Char = StringLength; Char > 0; --Char )
	{
		size_t IndexChar = p_pString[ Char ] < 0 ? p_pString[ Char ] + 128 :
			p_pString[ Char ];

		Width += ( float )p_pGlyphSet->Glyphs[ IndexChar ].XAdvance;
	}

	return Width;
}

