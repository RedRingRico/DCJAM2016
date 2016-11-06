#include <Matrix4x4.h>
#include <machine.h>
#include <SHC/private.h>
#include <mathf.h>
#include <Arithmetic.h>
#include <Log.h>

static float g_SH4Vector[ 4 ];
static float g_ResultVector[ 4 ];

void MAT44_SetIdentity( PMATRIX4X4 p_pMatrix )
{
	p_pMatrix->M00 = 1.0f;
	p_pMatrix->M01 = p_pMatrix->M02 = p_pMatrix->M03 = p_pMatrix->M10 = 0.0f;
	p_pMatrix->M11 = 1.0f;
	p_pMatrix->M12 = p_pMatrix->M13 = p_pMatrix->M20 = p_pMatrix->M21 = 0.0f;
	p_pMatrix->M22 = 1.0f;
	p_pMatrix->M23 = p_pMatrix->M30 = p_pMatrix->M31 = p_pMatrix->M32 = 0.0f;
	p_pMatrix->M33 = 1.0f;
}

void MAT44_Multiply( PMATRIX4X4 p_pOut, const PMATRIX4X4 p_pLeft,
	const PMATRIX4X4 p_pRight )
{
	/* Load the matrix on the right to be multiplied with */
	ld_ext( ( void * )p_pRight );

	mtrx4mul( ( void * )p_pLeft, ( void * )p_pOut );
}

void MAT44_Translate( PMATRIX4X4 p_pMatrix, const PVECTOR3 p_pTranslate )
{
	p_pMatrix->M30 = p_pTranslate->X;
	p_pMatrix->M31 = p_pTranslate->Y;
	p_pMatrix->M32 = p_pTranslate->Z;
}

void MAT44_SetAsScaleV3( PMATRIX4X4 p_pMatrix, const PVECTOR3 p_pScale )
{
	p_pMatrix->M00 = p_pScale->X;
	p_pMatrix->M11 = p_pScale->Y;
	p_pMatrix->M22 = p_pScale->Z;
	p_pMatrix->M33 = 1.0f;
}

void MAT44_SetAsScaleF( PMATRIX4X4 p_pMatrix, const float p_Scale )
{
	p_pMatrix->M00 = p_Scale;
	p_pMatrix->M11 = p_Scale;
	p_pMatrix->M22 = p_Scale;
	p_pMatrix->M33 = 1.0f;
}

void MAT44_RotateAxisAngle( PMATRIX4X4 p_pMatrix, const PVECTOR3 p_pAxis,
	const float p_Angle )
{
	MATRIX4X4 Rotation;
	float Sine, Cosine;
	Uint16 Radian;
	float TwoPi = 2.0f * 3.1415926f;

	/* X */
	Radian = ( Uint16 )( ( ( p_pAxis->X * p_Angle ) * 65536.0f ) / TwoPi );
	fsca( Radian, &Sine, &Cosine );

	Rotation.M00 = 1.0f;
	Rotation.M01 = 0.0f;
	Rotation.M02 = 0.0f;
	Rotation.M03 = 0.0f;

	Rotation.M10 = 0.0f;
	Rotation.M11 = Cosine;
	Rotation.M12 = Sine;
	Rotation.M13 = 0.0f;

	Rotation.M20 = 0.0f;
	Rotation.M21 = -Sine;
	Rotation.M22 = Cosine;
	Rotation.M23 = 0.0f;

	Rotation.M30 = 0.0f;
	Rotation.M31 = 0.0f;
	Rotation.M32 = 0.0f;
	Rotation.M33 = 1.0f;

	MAT44_Multiply( p_pMatrix, p_pMatrix, &Rotation );

	/* Y */
	Radian = ( Uint16 )( ( ( p_pAxis->Y * p_Angle ) * 65536.0f ) / TwoPi );
	fsca( Radian, &Sine, &Cosine );

	Rotation.M00 = Cosine;
	Rotation.M01 = 0.0f;
	Rotation.M02 = -Sine;
	Rotation.M03 = 0.0f;

	Rotation.M10 = 0.0f;
	Rotation.M11 = 1.0f;
	Rotation.M12 = 0.0f;
	Rotation.M13 = 0.0f;

	Rotation.M20 = Sine;
	Rotation.M21 = 0.0f;
	Rotation.M22 = Cosine;
	Rotation.M23 = 0.0f;

	Rotation.M30 = 0.0f;
	Rotation.M31 = 0.0f;
	Rotation.M32 = 0.0f;
	Rotation.M33 = 1.0f;

	MAT44_Multiply( p_pMatrix, p_pMatrix, &Rotation );

	/* Z */
	Radian = ( Uint16 )( ( ( p_pAxis->Z * p_Angle ) * 65536.0f ) / TwoPi );
	fsca( Radian, &Sine, &Cosine );

	Rotation.M00 = Cosine;
	Rotation.M01 = Sine;
	Rotation.M02 = 0.0f;
	Rotation.M03 = 0.0f;

	Rotation.M10 = -Sine;
	Rotation.M11 = Cosine;
	Rotation.M12 = 0.0f;
	Rotation.M13 = 0.0f;

	Rotation.M20 = 0.0f;
	Rotation.M21 = 0.0f;
	Rotation.M22 = 1.0f;
	Rotation.M23 = 0.0f;

	Rotation.M30 = 0.0f;
	Rotation.M31 = 0.0f;
	Rotation.M32 = 0.0f;
	Rotation.M33 = 1.0f;

	MAT44_Multiply( p_pMatrix, p_pMatrix, &Rotation );
}

void MAT44_Inverse( PMATRIX4X4 p_pMatrix )
{
	MATRIX4X4 TempMatrix;
	float Determinate;
	float Positive, Negative, Temp;

	Positive = Negative = 0.0f;

	Temp = p_pMatrix->M00 * p_pMatrix->M11 * p_pMatrix->M22;

	if( Temp >= ARI_EPSILON )
	{
		Positive += Temp;
	}
	else
	{
		Negative += Temp;
	}

	Temp = p_pMatrix->M01 * p_pMatrix->M12 * p_pMatrix->M20;

	if( Temp >= ARI_EPSILON )
	{
		Positive += Temp;
	}
	else
	{
		Negative += Temp;
	}

	Temp = p_pMatrix->M02 * p_pMatrix->M13 * p_pMatrix->M21;

	if( Temp >= ARI_EPSILON )
	{
		Positive += Temp;
	}
	else
	{
		Negative += Temp;
	}

	Temp = -p_pMatrix->M02 * p_pMatrix->M11 * p_pMatrix->M20;

	if( Temp >= ARI_EPSILON )
	{
		Positive += Temp;
	}
	else
	{
		Negative += Temp;
	}

	Temp = -p_pMatrix->M01 * p_pMatrix->M10 * p_pMatrix->M22;

	if( Temp >= ARI_EPSILON )
	{
		Positive += Temp;
	}
	else
	{
		Negative += Temp;
	}

	Temp = -p_pMatrix->M00 * p_pMatrix->M12 * p_pMatrix->M21;

	if( Temp >= ARI_EPSILON )
	{
		Positive += Temp;
	}
	else
	{
		Negative += Temp;
	}

	Determinate = Positive + Negative;

	if( ARI_IsZero( Determinate ) ||
		ARI_IsZero( Determinate / ( Positive - Negative ) ) )
	{
		LOG_Debug( "Could not find an inverse for the matrix\n" );

		return;
	}

	Determinate = 1.0f / Determinate;

	TempMatrix.M00 = ( ( p_pMatrix->M11 * p_pMatrix->M22 ) -
		( p_pMatrix->M12 * p_pMatrix->M21 ) ) * Determinate;
	TempMatrix.M10 = -( ( p_pMatrix->M10 * p_pMatrix->M22 ) -
		( p_pMatrix->M12 * p_pMatrix->M20 ) ) * Determinate;
	TempMatrix.M20 = ( ( p_pMatrix->M10 * p_pMatrix->M21 ) -
		( p_pMatrix->M11 * p_pMatrix->M20 ) ) * Determinate;

	TempMatrix.M01 = -( ( p_pMatrix->M01 * p_pMatrix->M22 ) -
		( p_pMatrix->M22 * p_pMatrix->M21 ) ) * Determinate;
	TempMatrix.M11 = ( ( p_pMatrix->M00 * p_pMatrix->M22 ) -
		( p_pMatrix->M02 * p_pMatrix->M20 ) ) * Determinate;
	TempMatrix.M21 = -( ( p_pMatrix->M00 * p_pMatrix->M21 ) -
		( p_pMatrix->M01 * p_pMatrix->M20 ) ) * Determinate;
	
	TempMatrix.M02 = ( ( p_pMatrix->M01 * p_pMatrix->M12 ) -
		( p_pMatrix->M02 * p_pMatrix->M11 ) ) * Determinate;
	TempMatrix.M12 = -( ( p_pMatrix->M00 * p_pMatrix->M12 ) -
		( p_pMatrix->M02 * p_pMatrix->M10 ) ) * Determinate;
	TempMatrix.M22 = ( ( p_pMatrix->M00 * p_pMatrix->M11 ) -
		( p_pMatrix->M01 * p_pMatrix->M10 ) ) * Determinate;

	TempMatrix.M30 = -( p_pMatrix->M30 * TempMatrix.M00 +
		p_pMatrix->M31 * TempMatrix.M10 +
		p_pMatrix->M32 * TempMatrix.M20 );
	TempMatrix.M31 = -( p_pMatrix->M30 * TempMatrix.M01 +
		p_pMatrix->M31 * TempMatrix.M11 +
		p_pMatrix->M32 * TempMatrix.M21 );
	TempMatrix.M32 = -( p_pMatrix->M30 * TempMatrix.M02 +
		p_pMatrix->M31 * TempMatrix.M12 +
		p_pMatrix->M32 * TempMatrix.M22 );

	TempMatrix.M03 = TempMatrix.M13 = TempMatrix.M23 = 0.0f;
	TempMatrix.M33 = 1.0f;
	
	MAT44_Copy( p_pMatrix, &TempMatrix );
}

void MAT44_Copy( PMATRIX4X4 p_pMatrix, const PMATRIX4X4 p_pOriginal )
{
	memcpy( p_pMatrix, p_pOriginal, sizeof( MATRIX4X4 ) );
}

void MAT44_TransformVertices( float *p_pTransformedVertices,
	const float *p_pVertices, const size_t p_VertexCount,
	const size_t p_TransformedStride, const size_t p_UntransformedStride,
	const PMATRIX4X4 p_pMatrix, bool p_RHW )
{
	register float *pDestinationVector;
	size_t Index, UStrideGap, TStrideGap;

#if defined ( DCJAM_BUILD_DEBUG )
	if( ( p_UntransformedStride < ( sizeof( float ) * 3 ) ) ||
		( p_TransformedStride < ( sizeof( float ) * 3 ) ) )
	{
		return;
	}
#endif /* DCJAM_BUILD_DEBUG */

	UStrideGap = ( p_UntransformedStride - ( sizeof( float ) * 3 ) /
		sizeof( float ) );
	TStrideGap = ( p_TransformedStride - ( sizeof( float ) * 3 ) /
		sizeof( float ) );

	ld_ext( ( void * )p_pMatrix );

	pDestinationVector = p_pTransformedVertices;

	g_SH4Vector[ 3 ] = 1.0f;

	for( Index = 0; Index < p_VertexCount; ++Index )
	{
		g_SH4Vector[ 0 ] = *( p_pVertices++ );
		g_SH4Vector[ 1 ] = *( p_pVertices++ );
		g_SH4Vector[ 2 ] = *( p_pVertices++ );

		p_pVertices += UStrideGap;

		ftrv( ( float * )g_SH4Vector, ( float * )g_ResultVector );

		if( p_RHW == true )
		{
			float RHW;

			/* Z Slam (probably the wrong approach) */
			if( g_ResultVector[ 2 ] < ARI_EPSILON )
			{
				g_ResultVector[ 2 ] = ARI_EPSILON;
			}

			RHW = 1.0f / g_ResultVector[ 2 ];

			*( pDestinationVector++ ) = RHW / g_ResultVector[ 0 ];
			*( pDestinationVector++ ) = RHW / g_ResultVector[ 1 ];
			*( pDestinationVector++ ) = RHW;
		}
		else
		{
			*( pDestinationVector++ ) = g_ResultVector[ 0 ];
			*( pDestinationVector++ ) = g_ResultVector[ 1 ];
			*( pDestinationVector++ ) = g_ResultVector[ 2 ];
		}

		pDestinationVector += TStrideGap;
	}
}

