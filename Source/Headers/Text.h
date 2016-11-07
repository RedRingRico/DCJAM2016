#ifndef __DCJAM_TEXT_H__
#define __DCJAM_TEXT_H__

#include <Texture.h>
#include <Memory.h>
#include <shinobi.h>
#include <kamui2.h>

#define TXT_OK					0
#define TXT_ERROR				1
#define TXT_FATALERROR			-1
#define TXT_GLYPHFILEERROR		-100
#define TXT_GLYPHMEMORYERROR	-101
#define TXT_BMFNOTBINARY		-102

typedef struct _tagGLYPH
{
	float	X;
	float	Y;
	float	Width;
	float	Height;
	float	XOffset;
	float	YOffset;
	float	XAdvance;
}GLYPH, *PGLYPH;

typedef struct _tagGLYPHSET
{
	TEXTURE		Texture;
	GLYPH		Glyphs[ 256 ];
	float		LineHeight;
	float		BaseLine;
	float		Width;
	float		Height;
}GLYPHSET, *PGLYPHSET;

Sint32 TXT_Initialise( void );

Sint32 TXT_CreateGlyphSetFromFile( char *p_pFileName, PGLYPHSET p_pGlyphSet,
	PMEMORY_BLOCK p_pMemoryBlock );
Sint32 TXT_SetTextureForGlyphSet( char *p_pFileName, GLYPHSET *p_pGlyphSet,
	PMEMORY_BLOCK p_pMemoryBlock );

void TXT_RenderString( PGLYPHSET p_pGlyphSet, KMPACKEDARGB p_Colour,
	float p_X, float p_Y, char *p_pString );

float TXT_MeasureString( PGLYPHSET p_pGlyphSet, char *p_pString );

#endif /* __DCJAM_TEXT_H__ */

