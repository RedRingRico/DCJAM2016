#ifndef __DCJAM_RENDERER_H__
#define __DCJAM_RENDERER_H__

#include <Memory.h>
#include <kamui2.h>
#include <shinobi.h>

#define REN_OK							0
#define REN_ERROR						1
#define REN_FATALERROR					-1
#define REN_TEXTUREALLOCATIONERROR		-10
#define REN_VERTEXBUFFERALLOCATIONERROR	-11
#define REN_KAMUIMEMORYERROR			-100

typedef struct _tagDREAMCAST_RENDERERCONFIGURATION
{
	PPKMSURFACEDESC		ppSurfaceDescription;
	PMEMORY_BLOCK		pMemoryBlock;
	KMUINT32			FramebufferCount;
	KMUINT32			TextureMemorySize;
	KMUINT32			MaximumTextureCount;
	KMUINT32			MaximumSmallVQTextureCount;
	size_t				VertexBufferWorkAreaSize;
	KMUINT32			PassCount;
	KMPASSINFO			PassInfo[ KM_MAX_DISPLAY_LIST_PASS ];
}DREAMCAST_RENDERERCONFIGURATION, *PDREAMCAST_RENDERERCONFIGURATION;

typedef struct _tagRENDERER
{
	Uint32			VisiblePolygons;
	Uint32			CulledPolygons;
	Uint32			GeneratedPolygons;
	Uint32			StripCount;
	PKMVERTEX_05	pVertices05;
	PMEMORY_BLOCK	pMemoryBlock;
}RENDERER, *PRENDERER;

Sint32 REN_Initialise( PRENDERER p_pRenderer,
	const PDREAMCAST_RENDERERCONFIGURATION p_pRendererConfiguration );
void REN_Terminate( PRENDERER p_pRenderer );

void REN_SetClearColour( float p_Red, float p_Green, float p_Blue );

Sint32 REN_Clear( void );
Sint32 REN_SwapBuffers( void );

#endif /* __DCJAM_RENDERER_H__ */

