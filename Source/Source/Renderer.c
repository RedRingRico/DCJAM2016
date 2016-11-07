#include <Renderer.h>
#include <Memory.h>
#include <Log.h>

/* Using the macro interface seems to make things worse
 * The recommended L5 is slower than L4 and L3 (this may improve with multi-
   triangle strips
 */

#define _KM_USE_VERTEX_MACRO_
#define _KM_USE_VERTEX_MACRO_L3_
#include <kamui2.h>
#include <kamuix.h>

static void *g_pTextureWorkArea = NULL;
static void *g_pVertexBufferWorkArea = NULL;
static KMVERTEXBUFFDESC g_VertexBufferDescription;

static KMSYSTEMCONFIGSTRUCT g_Kamui2Configuration;
static KMSTRIPCONTEXT g_DefaultStripContext =
{
	0x90, /* Size */
	{
		KM_OPAQUE_POLYGON,		/* List type */
		KM_USERCLIP_DISABLE,	/* User clip mode */
		KM_NORMAL_POLYGON,		/* Shadow mode */
		KM_FLOATINGCOLOR,		/* Intensity mode */
		KM_FALSE,				/* Offset */
		KM_TRUE					/* Gouraud */
	}, /* KMSTRIPCONTROL */
	{
		KM_GREATER,				/* Depth compare */
		KM_NOCULLING,			/* Culling mode */
		KM_FALSE,				/* Z write disable */
		KM_FALSE,				/* D calculation control (precision/no
								 * precision) */
		KM_MODIFIER_NORMAL_POLY	/* Modifier instruction */
	}, /* KMOBJECTCONTROL */
	{
		{
			0.0f,
			0.0f
		} /* intensity */
	}, /* type */
	{
		KM_ONE,						/* Source blending mode */
		KM_ZERO,					/* Destination blending mode */
		KM_FALSE,					/* Source select */
		KM_FALSE,					/* Destination select */
		KM_NOFOG,					/* Fog mode */
		KM_FALSE,					/* Colour clamp */
		KM_FALSE,					/* Alpha enable */
		KM_FALSE,					/* Ignore texture alpha */
		KM_NOFLIP,					/* Flip UV */
		KM_NOCLAMP,					/* Clamp UV */
		KM_BILINEAR,				/* Filter mode */
		KM_FALSE,					/* Super sample mode */
		KM_MIPMAP_D_ADJUST_1_00,	/* Mip map adjust */
		KM_MODULATE,				/* Texture shading mode */
		0,							/* Palette bank */
		0							/* Texture surface description */
	} /* KMIMAGECONTROL */
};

static KMSTRIPHEAD g_StripHead01;
static KMSTRIPHEAD g_StripHead16;

Sint32 REN_Initialise( PRENDERER p_pRenderer,
	const PDREAMCAST_RENDERERCONFIGURATION p_pRendererConfiguration )
{
	KMUINT32 PassIndex;
	KMSTATUS ReturnStatus;
	size_t TextureWorkAreaSize;

	p_pRenderer->pMemoryBlock = p_pRendererConfiguration->pMemoryBlock;

	TextureWorkAreaSize =
		( p_pRendererConfiguration->MaximumTextureCount * 24 ) +
		( p_pRendererConfiguration->MaximumSmallVQTextureCount * 76 );
	
	g_pTextureWorkArea = MEM_AllocateFromMemoryBlock(
		p_pRendererConfiguration->pMemoryBlock, TextureWorkAreaSize,
		"Texture Work Area" );

	if( g_pTextureWorkArea == NULL )
	{
		LOG_Debug( "Failed to allocate %ld bytes of memory for the texture "
			"work area", TextureWorkAreaSize );

		return REN_TEXTUREALLOCATIONERROR;
	}

	g_pVertexBufferWorkArea = MEM_AllocateFromMemoryBlock(
		p_pRendererConfiguration->pMemoryBlock,
		p_pRendererConfiguration->VertexBufferWorkAreaSize,
		"Vertex Buffer Work Area" );

	if( g_pVertexBufferWorkArea == NULL )
	{
		LOG_Debug( "Failed to allocate %ld bytes of memory for the vertex "
			"buffer work area",
			p_pRendererConfiguration->VertexBufferWorkAreaSize );

		return REN_VERTEXBUFFERALLOCATIONERROR;
	}

	g_Kamui2Configuration.dwSize = sizeof( g_Kamui2Configuration );

	g_Kamui2Configuration.flags =
		KM_CONFIGFLAG_ENABLE_CLEAR_FRAMEBUFFER |
		KM_CONFIGFLAG_NOWAITVSYNC |
		KM_CONFIGFLAG_ENABLE_2V_LATENCY |
		KM_CONFIGFLAG_NOWAIT_FINISH_TEXTUREDMA;
	g_Kamui2Configuration.ppSurfaceDescArray =
		p_pRendererConfiguration->ppSurfaceDescription;
	g_Kamui2Configuration.fb.nNumOfFrameBuffer =
		p_pRendererConfiguration->FramebufferCount;
	g_Kamui2Configuration.nTextureMemorySize =
		p_pRendererConfiguration->TextureMemorySize;
	g_Kamui2Configuration.nNumOfTextureStruct =
		p_pRendererConfiguration->MaximumTextureCount;
	g_Kamui2Configuration.nNumOfSmallVQStruct =
		p_pRendererConfiguration->MaximumSmallVQTextureCount;
	g_Kamui2Configuration.pTextureWork = g_pTextureWorkArea;
	g_Kamui2Configuration.pVertexBuffer =
		( PKMDWORD )MEM_SH4_P2NonCachedMemory( g_pVertexBufferWorkArea );
	g_Kamui2Configuration.pBufferDesc = &g_VertexBufferDescription;
	g_Kamui2Configuration.nVertexBufferSize =
		p_pRendererConfiguration->VertexBufferWorkAreaSize;
	g_Kamui2Configuration.nNumOfVertexBank = 1;
	g_Kamui2Configuration.nPassDepth = p_pRendererConfiguration->PassCount;

	for( PassIndex = 0; PassIndex < p_pRendererConfiguration->PassCount;
		++PassIndex )
	{
		int BufferIndex = 0;

		for( ; BufferIndex < KM_MAX_DISPLAY_LIST; ++BufferIndex )
		{
			g_Kamui2Configuration.Pass[ PassIndex ].
				fBufferSize[ BufferIndex ] =
					p_pRendererConfiguration->PassInfo[ PassIndex ].
						fBufferSize[ BufferIndex ];

			g_Kamui2Configuration.Pass[ PassIndex ].dwOPBSize[ BufferIndex ] =
				p_pRendererConfiguration->PassInfo[ PassIndex ].
					dwOPBSize[ BufferIndex ];
		}

		g_Kamui2Configuration.Pass[ PassIndex ].nDirectTransferList =
			p_pRendererConfiguration->
				PassInfo[ PassIndex ].nDirectTransferList;

		g_Kamui2Configuration.Pass[ PassIndex ].dwRegionArrayFlag =
			p_pRendererConfiguration->PassInfo[ PassIndex ].dwRegionArrayFlag;
	}

	ReturnStatus = kmSetSystemConfiguration( &g_Kamui2Configuration );
	
	if( ReturnStatus != KMSTATUS_SUCCESS )
	{
		switch( ReturnStatus )
		{
			case KMSTATUS_NOT_ENOUGH_MEMORY:
			{
				LOG_Debug( "Failed to set Kamui configuration [Not enough "
					"memory]" );

				return REN_KAMUIMEMORYERROR;
			}
			default:
			{
				LOG_Debug( "Failed to set Kamui configuration [Unknown "
					"reason]" );

				return REN_FATALERROR;
			}
		}
	}

	memset( &g_StripHead16, 0, sizeof( g_StripHead16 ) );
	kmGenerateStripHead16( &g_StripHead16, &g_DefaultStripContext );

	memset( &g_StripHead01, 0, sizeof( g_StripHead01 ) );
	kmGenerateStripHead01( &g_StripHead01, &g_DefaultStripContext );

	return REN_OK;
}

void REN_Terminate( PRENDERER p_pRenderer )
{
	if( g_pVertexBufferWorkArea != NULL )
	{
		MEM_FreeFromMemoryBlock( p_pRenderer->pMemoryBlock,
			g_pVertexBufferWorkArea );
		g_pVertexBufferWorkArea = NULL;
	}

	if( g_pTextureWorkArea != NULL )
	{
		MEM_FreeFromMemoryBlock( p_pRenderer->pMemoryBlock,
			g_pTextureWorkArea );
		g_pTextureWorkArea = NULL;
	}

	/* Print out some useful memory statistics */
#if defined ( DCJAM_BUILD_DEBUG )
	{
		KMUINT32 AvailableTextureMemory, MaxBlockSizeOfTexture;

		kmGetFreeTextureMem( &AvailableTextureMemory, &MaxBlockSizeOfTexture );

		LOG_Debug( "KAMUI2 Texture Memory Available: %ld",
			AvailableTextureMemory );
	}
#endif /* DCJAM_BUILD_DEBUG */
}

void REN_SetClearColour( float p_Red, float p_Green, float p_Blue )
{
	KMSTRIPHEAD BackgroundStripHead;
	KMVERTEX_01 BackgroundClear[ 3 ];
	KMPACKEDARGB BorderColour;

	memset( &BackgroundStripHead, 0, sizeof( BackgroundStripHead ) );
	kmGenerateStripHead01( &BackgroundStripHead, &g_DefaultStripContext );

	BackgroundClear[ 0 ].ParamControlWord = KM_VERTEXPARAM_NORMAL;
	BackgroundClear[ 0 ].fX = 0.0f;
	BackgroundClear[ 0 ].fY = 479.0f;
	BackgroundClear[ 0 ].u.fZ = 0.0001f;
	BackgroundClear[ 0 ].fBaseAlpha = 1.0f;
	BackgroundClear[ 0 ].fBaseRed = p_Red;
	BackgroundClear[ 0 ].fBaseBlue = p_Blue;
	BackgroundClear[ 0 ].fBaseGreen = p_Green;

	BackgroundClear[ 1 ].ParamControlWord = KM_VERTEXPARAM_NORMAL;
	BackgroundClear[ 1 ].fX = 639.0f;
	BackgroundClear[ 1 ].fY = 0.0f;
	BackgroundClear[ 1 ].u.fZ = 0.0001f;
	BackgroundClear[ 1 ].fBaseAlpha = 1.0f;
	BackgroundClear[ 1 ].fBaseRed = p_Red;
	BackgroundClear[ 1 ].fBaseBlue = p_Blue;
	BackgroundClear[ 1 ].fBaseGreen = p_Green;

	BackgroundClear[ 2 ].ParamControlWord = KM_VERTEXPARAM_ENDOFSTRIP;
	BackgroundClear[ 2 ].fX = 639.0f;
	BackgroundClear[ 2 ].fY = 479.0f;
	BackgroundClear[ 2 ].u.fZ = 0.0001f;
	BackgroundClear[ 2 ].fBaseAlpha = 1.0f;
	BackgroundClear[ 2 ].fBaseRed = p_Red;
	BackgroundClear[ 2 ].fBaseBlue = p_Blue;
	BackgroundClear[ 2 ].fBaseGreen = p_Green;

	kmSetBackGround( &BackgroundStripHead, KM_VERTEXTYPE_01,
		&BackgroundClear[ 0 ], &BackgroundClear[ 1 ], &BackgroundClear[ 2 ] );

	BorderColour.dwPacked = 0;

	kmSetBorderColor( BorderColour );
}

Sint32 REN_Clear( void )
{
	KMSTATUS ReturnStatus;

	ReturnStatus = kmBeginScene( &g_Kamui2Configuration );
	ReturnStatus = kmBeginPass( g_Kamui2Configuration.pBufferDesc );

	return REN_OK;
}

Sint32 REN_SwapBuffers( void )
{
	kmEndPass( g_Kamui2Configuration.pBufferDesc );
	kmRender( KM_RENDER_FLIP );
	kmEndScene( &g_Kamui2Configuration );

	return REN_OK;
}

void REN_DrawPrimitives01( PKMSTRIPHEAD p_pStripHead, PKMVERTEX_01 p_pVertices,
	KMUINT32 p_Count )
{
	KMUINT32 VertexIndex;

	kmxxGetCurrentPtr( g_Kamui2Configuration.pBufferDesc );

	if( p_pStripHead )
	{
		kmxxStartStrip( g_Kamui2Configuration.pBufferDesc, p_pStripHead );
	}
	else
	{
		kmxxStartStrip( g_Kamui2Configuration.pBufferDesc, &g_StripHead01 );
	}

	for( VertexIndex = 0; VertexIndex < p_Count; ++VertexIndex )
	{
		kmxxSetVertex_1(
			p_pVertices[ VertexIndex ].ParamControlWord,
			p_pVertices[ VertexIndex ].fX,
			p_pVertices[ VertexIndex ].fY,
			p_pVertices[ VertexIndex ].u.fZ,
			p_pVertices[ VertexIndex ].fBaseAlpha,
			p_pVertices[ VertexIndex ].fBaseRed,
			p_pVertices[ VertexIndex ].fBaseGreen,
			p_pVertices[ VertexIndex ].fBaseBlue );
	}

	kmxxReleaseCurrentPtr( g_Kamui2Configuration.pBufferDesc );

	kmEndStrip( g_Kamui2Configuration.pBufferDesc );
}

void REN_DrawPrimitives16( PKMSTRIPHEAD p_pStripHead, PKMVERTEX_16 p_pVertices,
	KMUINT32 p_Count )
{
	KMUINT32 VertexIndex;

	kmxxGetCurrentPtr( g_Kamui2Configuration.pBufferDesc );

	if( p_pStripHead )
	{
		kmxxStartStrip( g_Kamui2Configuration.pBufferDesc, p_pStripHead );
	}
	else
	{
		kmxxStartStrip( g_Kamui2Configuration.pBufferDesc, &g_StripHead16 );
	}

	for( VertexIndex = 0; VertexIndex < p_Count; ++VertexIndex )
	{
		kmxxSetVertex_16(
			p_pVertices[ VertexIndex ].ParamControlWord,
			p_pVertices[ VertexIndex ].fAX,
			p_pVertices[ VertexIndex ].fAY,
			p_pVertices[ VertexIndex ].uA.fAZ,
			p_pVertices[ VertexIndex ].fBX,
			p_pVertices[ VertexIndex ].fBY,
			p_pVertices[ VertexIndex ].uB.fBZ,
			p_pVertices[ VertexIndex ].fCX,
			p_pVertices[ VertexIndex ].fCY,
			p_pVertices[ VertexIndex ].uC.fCZ,
			p_pVertices[ VertexIndex ].fDX,
			p_pVertices[ VertexIndex ].fDY,
			p_pVertices[ VertexIndex ].dwUVA,
			p_pVertices[ VertexIndex ].dwUVB,
			p_pVertices[ VertexIndex ].dwUVC );
	}

	kmxxReleaseCurrentPtr( g_Kamui2Configuration.pBufferDesc );

	kmEndStrip( g_Kamui2Configuration.pBufferDesc );
}

