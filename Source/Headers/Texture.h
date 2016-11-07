#ifndef __DCJAM_TEXTURE_H__
#define __DCJAM_TEXTURE_H__

#include <Memory.h>
#include <shinobi.h>
#include <kamui2.h>

#define TEX_OK			0
#define TEX_ERROR		1
#define TEX_LOADERROR	10
#define TEX_FATALERROR	-1

typedef struct _tagTEXTURE
{
	KMINT32			Width;
	KMINT32			Height;
	KMTEXTURETYPE	Flags;
	KMSURFACEDESC	SurfaceDescription;
	Uint32			ReferenceCount;
}TEXTURE, *PTEXTURE;

Sint32 TEX_CreateTextureFromFile( PTEXTURE p_pTexture, const char *p_pFileName,
	PMEMORY_BLOCK p_pMemoryBlock );
void TEX_DeleteTexture( PTEXTURE p_pTexture );

#endif /* __DCJAM_TEXTURE_H__ */

