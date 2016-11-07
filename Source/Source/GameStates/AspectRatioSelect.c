#include <GameStates/AspectRatioSelect.h>
#include <GameState.h>
#include <Vector3.h>
#include <Matrix4x4.h>
#include <Camera.h>
#include <kamui2.h>
#include <Peripheral.h>
#include <GameStates/MainMenu.h>

typedef struct _tagASPECTRATIOSELECT_GAMESTATE
{
	GAMESTATE		Base;
	float			AspectRatio;
	bool			SelectAspect;
	bool			FourThree;
	KMBYTE			Alpha;
	KMSTRIPHEAD		SquareStripHead;
	CAMERA			Camera;
	VECTOR3			SquareVerts[ 4 ];
	MATRIX4X4		WorldMatrix;
	MATRIX4X4		ViewProjectionMatrix;
	KMVERTEX_01		Square[ 4 ];
}ASPECTRATIOSELECT_GAMESTATE, *PASPECTRATIOSELECT_GAMESTATE;

static ASPECTRATIOSELECT_GAMESTATE AspectRatioSelectState;

static Sint32 Load( void *p_pArgs )
{
	KMSTRIPCONTEXT SquareContext;
	KMPACKEDARGB BaseColour;
	VECTOR3 Translate = { 0.0f, 0.0f, 100.0f };

	memset( &SquareContext, 0, sizeof( KMSTRIPCONTEXT ) );
	BaseColour.dwPacked = 0xFFFFFFFF;

	SquareContext.nSize = sizeof( SquareContext );
	SquareContext.StripControl.nListType = KM_TRANS_POLYGON;
	SquareContext.StripControl.nUserClipMode = KM_USERCLIP_DISABLE;
	SquareContext.StripControl.nShadowMode = KM_NORMAL_POLYGON;
	SquareContext.StripControl.bOffset = KM_FALSE;
	SquareContext.StripControl.bGouraud = KM_TRUE;
	SquareContext.ObjectControl.nDepthCompare = KM_ALWAYS;
	SquareContext.ObjectControl.nCullingMode = KM_NOCULLING;
	SquareContext.ObjectControl.bZWriteDisable = KM_FALSE;
	SquareContext.ObjectControl.bDCalcControl = KM_FALSE;
	SquareContext.type.splite.Base = BaseColour;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].nSRCBlendingMode =
		KM_SRCALPHA;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].nDSTBlendingMode =
		KM_INVSRCALPHA;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].bSRCSelect = KM_FALSE;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].bDSTSelect = KM_FALSE;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].nFogMode = KM_NOFOG;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].bColorClamp = KM_FALSE;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].bUseAlpha = KM_TRUE;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].bIgnoreTextureAlpha =
		KM_FALSE;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].nFlipUV = KM_NOFLIP;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].nClampUV = KM_CLAMP_UV;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].nFilterMode = KM_BILINEAR;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].bSuperSampleMode = KM_FALSE;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].dwMipmapAdjust = 
		KM_MIPMAP_D_ADJUST_1_00;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].nTextureShadingMode =
		KM_MODULATE_ALPHA;
	SquareContext.ImageControl[ KM_IMAGE_PARAM1 ].pTextureSurfaceDesc = NULL;

	kmGenerateStripHead01( &AspectRatioSelectState.SquareStripHead,
		&SquareContext );

	AspectRatioSelectState.SquareVerts[ 0 ].X = -10.0f;
	AspectRatioSelectState.SquareVerts[ 0 ].Y = -10.0f;
	AspectRatioSelectState.SquareVerts[ 0 ].Z = -10.0f;

	AspectRatioSelectState.SquareVerts[ 1 ].X = -10.0f;
	AspectRatioSelectState.SquareVerts[ 1 ].Y = 10.0f;
	AspectRatioSelectState.SquareVerts[ 1 ].Z = -10.0f;

	AspectRatioSelectState.SquareVerts[ 2 ].X = 10.0f;
	AspectRatioSelectState.SquareVerts[ 2 ].Y = -10.0f;
	AspectRatioSelectState.SquareVerts[ 2 ].Z = -10.0f;

	AspectRatioSelectState.SquareVerts[ 3 ].X = 10.0f;
	AspectRatioSelectState.SquareVerts[ 3 ].Y = 10.0f;
	AspectRatioSelectState.SquareVerts[ 3 ].Z = -10.0f;

	MAT44_SetIdentity( &AspectRatioSelectState.WorldMatrix );
	MAT44_Translate( &AspectRatioSelectState.WorldMatrix, &Translate );

	AspectRatioSelectState.Camera.Position.X = 0.0f;
	AspectRatioSelectState.Camera.Position.Y = 0.0f;
	AspectRatioSelectState.Camera.Position.Z = 0.0f;

	AspectRatioSelectState.Camera.LookAt.X = 0.0f;
	AspectRatioSelectState.Camera.LookAt.Y = 0.0f;
	AspectRatioSelectState.Camera.LookAt.Z = 1.0f;

	AspectRatioSelectState.Camera.WorldUp.X = 0.0f;
	AspectRatioSelectState.Camera.WorldUp.Y = 1.0f;
	AspectRatioSelectState.Camera.WorldUp.Z = 0.0f;

	AspectRatioSelectState.Camera.GateWidth = 640.0f;
	AspectRatioSelectState.Camera.GateHeight = 480.0f;
	
	AspectRatioSelectState.Camera.FieldOfView = ( 3.141592654f / 4.0f );
	AspectRatioSelectState.Camera.NearPlane = 1.0f;
	AspectRatioSelectState.Camera.FarPlane = 100000.0f;

	return GSM_OK;	
}

static Sint32 Initialise( void *p_pArgs )
{
	MATRIX4X4 ScreenMatrix;
	MATRIX4X4 ProjectionMatrix;
	MATRIX4X4 ViewMatrix;
	VECTOR3 SquareVertsT[ 4 ];
	int i;

	AspectRatioSelectState.AspectRatio = 4.0f / 3.0f;
	AspectRatioSelectState.SelectAspect = true;
	AspectRatioSelectState.FourThree = true;

	AspectRatioSelectState.Camera.AspectRatio =
		AspectRatioSelectState.AspectRatio;

	CAM_CalculateProjectionMatrix( &ProjectionMatrix,
		&AspectRatioSelectState.Camera );
	CAM_CalculateScreenMatrix( &ScreenMatrix, &AspectRatioSelectState.Camera );
	CAM_CalculateViewMatrix( &ViewMatrix, &AspectRatioSelectState.Camera );

	MAT44_Multiply( &AspectRatioSelectState.ViewProjectionMatrix,
		&AspectRatioSelectState.WorldMatrix, &ViewMatrix );
	MAT44_Multiply( &AspectRatioSelectState.ViewProjectionMatrix,
		&AspectRatioSelectState.ViewProjectionMatrix, &ProjectionMatrix );
	MAT44_Multiply( &AspectRatioSelectState.ViewProjectionMatrix,
		&AspectRatioSelectState.ViewProjectionMatrix, &ScreenMatrix );

	MAT44_TransformVertices( ( float * )SquareVertsT,
		( float * )AspectRatioSelectState.SquareVerts, 4,
		sizeof( SquareVertsT[ 0 ] ),
		sizeof( AspectRatioSelectState.SquareVerts[ 0 ] ),
		&AspectRatioSelectState.ViewProjectionMatrix, true );

	for( i = 0; i < 4; ++i )
	{
		AspectRatioSelectState.Square[ i ].ParamControlWord =
			KM_VERTEXPARAM_NORMAL;
		AspectRatioSelectState.Square[ i ].fX =
			SquareVertsT[ i ].X;
		AspectRatioSelectState.Square[ i ].fY =
			SquareVertsT[ i ].Y;
		AspectRatioSelectState.Square[ i ].u.fInvW =
			SquareVertsT[ i ].Z;
		AspectRatioSelectState.Square[ i ].fBaseAlpha = 0.7f;
		AspectRatioSelectState.Square[ i ].fBaseRed =
			83.0f / 255.0f;
		AspectRatioSelectState.Square[ i ].fBaseGreen =
			254.0f / 255.0f;
		AspectRatioSelectState.Square[ i ].fBaseBlue = 1.0f;
	}

	AspectRatioSelectState.Square[ 3 ].ParamControlWord =
		KM_VERTEXPARAM_ENDOFSTRIP;

	AspectRatioSelectState.Camera.AspectRatio =
		AspectRatioSelectState.AspectRatio;

	AspectRatioSelectState.Alpha = 255;

	return GSM_OK;
}

static Sint32 Update( void *p_pArgs )
{
	static float Alpha = 1.0f, AlphaInc = 0.7f;

	if( AspectRatioSelectState.Base.Paused == false )
	{
		if( AspectRatioSelectState.SelectAspect == true )
		{
			if( g_Peripherals[ 0 ].press & PDD_DGT_TA )
			{
				AspectRatioSelectState.SelectAspect = false;
			}

			if( ( g_Peripherals[ 0 ].press & PDD_DGT_KL ) ||
				( g_Peripherals[ 0 ].press & PDD_DGT_KR ) )
			{
				MATRIX4X4 ScreenMatrix;
				MATRIX4X4 ProjectionMatrix;
				MATRIX4X4 ViewMatrix;
				VECTOR3 SquareVertsT[ 4 ];
				int i;

				Alpha = 1.0f;

				AspectRatioSelectState.FourThree =
					!AspectRatioSelectState.FourThree;

				if( AspectRatioSelectState.FourThree == true )
				{
					AspectRatioSelectState.AspectRatio = 4.0f / 3.0f;
				}
				else
				{
					AspectRatioSelectState.AspectRatio = 16.0f / 9.0f;
				}

				AspectRatioSelectState.Camera.AspectRatio =
					AspectRatioSelectState.AspectRatio;

				CAM_CalculateProjectionMatrix( &ProjectionMatrix,
					&AspectRatioSelectState.Camera );
				CAM_CalculateScreenMatrix( &ScreenMatrix,
					&AspectRatioSelectState.Camera );
				CAM_CalculateViewMatrix( &ViewMatrix,
					&AspectRatioSelectState.Camera );

				MAT44_Multiply( &AspectRatioSelectState.ViewProjectionMatrix,
					&AspectRatioSelectState.WorldMatrix, &ViewMatrix );
				MAT44_Multiply( &AspectRatioSelectState.ViewProjectionMatrix,
					&AspectRatioSelectState.ViewProjectionMatrix,
					&ProjectionMatrix );
				MAT44_Multiply( &AspectRatioSelectState.ViewProjectionMatrix,
					&AspectRatioSelectState.ViewProjectionMatrix,
					&ScreenMatrix );

				MAT44_TransformVertices( ( float * )SquareVertsT,
					( float * )AspectRatioSelectState.SquareVerts, 4,
					sizeof( SquareVertsT[ 0 ] ),
					sizeof( AspectRatioSelectState.SquareVerts[ 0 ] ),
					&AspectRatioSelectState.ViewProjectionMatrix, true );

				for( i = 0; i < 4; ++i )
				{
					AspectRatioSelectState.Square[ i ].ParamControlWord =
						KM_VERTEXPARAM_NORMAL;
					AspectRatioSelectState.Square[ i ].fX =
						SquareVertsT[ i ].X;
					AspectRatioSelectState.Square[ i ].fY =
						SquareVertsT[ i ].Y;
					AspectRatioSelectState.Square[ i ].u.fInvW =
						SquareVertsT[ i ].Z;
					AspectRatioSelectState.Square[ i ].fBaseAlpha = 0.7f;
					AspectRatioSelectState.Square[ i ].fBaseRed =
						83.0f / 255.0f;
					AspectRatioSelectState.Square[ i ].fBaseGreen =
						254.0f / 255.0f;
					AspectRatioSelectState.Square[ i ].fBaseBlue = 1.0f;
				}

				AspectRatioSelectState.Square[ 3 ].ParamControlWord =
					KM_VERTEXPARAM_ENDOFSTRIP;
			}
		}
		else
		{
			GSM_ChangeGameState( AspectRatioSelectState.Base.pGameStateManager,
				GAME_STATE_MAINMENU, NULL, NULL );
		}

		Alpha += AlphaInc;

		if( Alpha <= 0.0f )
		{
			AlphaInc = 0.7f;
			Alpha = 0.0f;
			AspectRatioSelectState.Alpha = 0;
		}
		else if( Alpha >= 1.0f )
		{
			AlphaInc = -0.7f;
			Alpha = 1.0f;
			AspectRatioSelectState.Alpha = 255;
		}
		else
		{
			AspectRatioSelectState.Alpha = ( KMBYTE )( Alpha * 255.0f );
		}
	}

	return GSM_OK;
}

static Sint32 Render( void *p_pArgs )
{
	float TextLength;
	KMPACKEDARGB TextColour;

	TextColour.byte.bRed = 83;
	TextColour.byte.bGreen = 254;
	TextColour.byte.bBlue = 255;
	TextColour.byte.bAlpha = 140;

	if( AspectRatioSelectState.Base.Paused == false )
	{
		PGLYPHSET pGlyphSet;
		pGlyphSet = GSM_GetGlyphSet(
			AspectRatioSelectState.Base.pGameStateManager,
			GSM_GLYPH_SET_GUI_1 );

		REN_Clear( );

		TextLength = TXT_MeasureString( pGlyphSet, "Select aspect ratio" );
		TXT_RenderString( pGlyphSet, TextColour,
			320.0f - ( TextLength * 0.5f ),
			pGlyphSet->LineHeight * 4.0f, "Select aspect ratio" );

		TextLength = TXT_MeasureString( pGlyphSet,
			"The square should not be distorted" );
		TXT_RenderString( pGlyphSet, TextColour,
			320.0f - ( TextLength * 0.5f ),
			pGlyphSet->LineHeight * 5.5f,
			"The square should not be distorted" );

		TextLength = TXT_MeasureString( pGlyphSet, "Press 'A' to select" );
		TXT_RenderString( pGlyphSet, TextColour,
			320.0f - ( TextLength * 0.5f ),
			( 480.0f - pGlyphSet->LineHeight * 4.0f ),
			"Press 'A' to select" );

		if( AspectRatioSelectState.FourThree == true )
		{
			TextColour.byte.bAlpha = AspectRatioSelectState.Alpha;
		}
		else
		{
			TextColour.byte.bAlpha = 140;
		}

		TextLength = TXT_MeasureString( pGlyphSet, "4:3" );
		TXT_RenderString( pGlyphSet, TextColour,
			270.0f - TextLength, 360.0f, "4:3" );

		if( AspectRatioSelectState.FourThree == false )
		{
			TextColour.byte.bAlpha = AspectRatioSelectState.Alpha;
		}
		else
		{
			TextColour.byte.bAlpha = 140;
		}

		TXT_RenderString( pGlyphSet, TextColour, 370.0f, 360.0f, "16:9" );

		REN_DrawPrimitives01( &AspectRatioSelectState.SquareStripHead,
			AspectRatioSelectState.Square, 4 );

		REN_SwapBuffers( );
	}

	return GSM_OK;
}

static Sint32 Terminate( void *p_pArgs )
{
	return GSM_OK;
}

static Sint32 Unload( void *p_pArgs )
{
	return GSM_OK;
}

Sint32 GS_ARS_RegisterWithGameStateManager(
	PGAMESTATE_MANAGER p_pGameStateManager )
{
	AspectRatioSelectState.Base.Load = &Load;
	AspectRatioSelectState.Base.Initialise = &Initialise;
	AspectRatioSelectState.Base.Update = &Update;
	AspectRatioSelectState.Base.Render = &Render;
	AspectRatioSelectState.Base.Terminate = &Terminate;
	AspectRatioSelectState.Base.Unload = &Unload;
	AspectRatioSelectState.Base.pGameStateManager = p_pGameStateManager;

	return GSM_RegisterGameState( p_pGameStateManager,
		GAME_STATE_ASPECTRATIOSELECT, ( PGAMESTATE )&AspectRatioSelectState );
}

