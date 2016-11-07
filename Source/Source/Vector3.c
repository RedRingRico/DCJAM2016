#include <Vector3.h>
#include <mathf.h>
#include <Arithmetic.h>
#include <SHC/umachine.h>
#include <SHC/private.h>

void VEC3_Add( PVECTOR3 p_pOut, const PVECTOR3 p_pLeft,
	const PVECTOR3 p_pRight )
{
	p_pOut->X = p_pLeft->X + p_pRight->X;
	p_pOut->Y = p_pLeft->Y + p_pRight->Y;
	p_pOut->Z = p_pLeft->Z + p_pRight->Z;
}

void VEC3_Subtract( PVECTOR3 p_pOut, const PVECTOR3 p_pLeft,
	const PVECTOR3 p_pRight )
{
	p_pOut->X = p_pLeft->X - p_pRight->X;
	p_pOut->Y = p_pLeft->Y - p_pRight->Y;
	p_pOut->Z = p_pLeft->Z - p_pRight->Z;
}

void VEC3_MultiplyV( PVECTOR3 p_pOut, const PVECTOR3 p_pLeft,
	const PVECTOR3 p_pRight )
{
	p_pOut->X = p_pLeft->X * p_pRight->X;
	p_pOut->Y = p_pLeft->Y * p_pRight->Y;
	p_pOut->Z = p_pLeft->Z * p_pRight->Z;
}

void VEC3_MultiplyF( PVECTOR3 p_pOut, const PVECTOR3 p_pVector,
	const float p_Scalar )
{
	p_pOut->X = p_pVector->X * p_Scalar;
	p_pOut->Y = p_pVector->Y * p_Scalar;
	p_pOut->Z = p_pVector->Z * p_Scalar;
}

void VEC3_Divide( PVECTOR3 p_pOut, const PVECTOR3 p_pVector,
	const float p_Scalar )
{
	p_pOut->X = p_pVector->X / p_Scalar;
	p_pOut->Y = p_pVector->Y / p_Scalar;
	p_pOut->Z = p_pVector->Z / p_Scalar;
}

float VEC3_Magnitude( const PVECTOR3 p_pVector )
{
	return sqrtf( ( p_pVector->X * p_pVector->X ) +
		( p_pVector->Y * p_pVector->Y ) +
		( p_pVector->Z * p_pVector->Z ) );
}

float VEC3_MagnitudeSquared( const PVECTOR3 p_pVector )
{
	return ( ( p_pVector->X * p_pVector->X ) +
		( p_pVector->Y * p_pVector->Y ) +
		( p_pVector->Z * p_pVector->Z ) );
}

float VEC3_Distance( const PVECTOR3 p_pLeft, const PVECTOR3 p_pRight )
{
	float X, Y, Z;

	X = p_pLeft->X - p_pRight->X;
	Y = p_pLeft->Y - p_pRight->Y;
	Z = p_pLeft->Z - p_pRight->Z;

	return sqrtf( ( X * X ) + ( Y * Y ) + ( Z * Z ) );
}

float VEC3_DistanceSquared( const PVECTOR3 p_pLeft, const PVECTOR3 p_pRight )
{
	float X, Y, Z;

	X = p_pLeft->X - p_pRight->X;
	Y = p_pLeft->Y - p_pRight->Y;
	Z = p_pLeft->Z - p_pRight->Z;

	return ( ( X * X ) + ( Y * Y ) + ( Z * Z ) );
}

void VEC3_Normalise( PVECTOR3 p_pVector )
{
	float Factor;
	float Squared;

	Squared = ( p_pVector->X * p_pVector->X ) +
		( p_pVector->Y * p_pVector->Y ) +
		( p_pVector->Z * p_pVector->Z );

	if( ARI_IsZero( Squared ) )
	{
		return;
	}

	Factor = ( float )fsrra( Squared );

	p_pVector->X *= Factor;
	p_pVector->Y *= Factor;
	p_pVector->Z *= Factor;
}

float VEC3_Dot( const PVECTOR3 p_pLeft, const PVECTOR3 p_pRight )
{
	float Vector0[ 4 ], Vector1[ 4 ];

	Vector0[ 0 ] = p_pLeft->X;
	Vector0[ 1 ] = p_pLeft->Y;
	Vector0[ 2 ] = p_pLeft->Z;
	Vector0[ 3 ] = 0.0f;

	Vector1[ 0 ] = p_pRight->X;
	Vector1[ 1 ] = p_pRight->Y;
	Vector1[ 2 ] = p_pRight->Z;
	Vector1[ 3 ] = 0.0f;

	return ( fipr( Vector0, Vector1 ) );
}

void VEC3_Cross( PVECTOR3 p_pOut, const PVECTOR3 p_pLeft,
	const PVECTOR3 p_pRight )
{
	p_pOut->X = ( p_pLeft->Y * p_pRight->Z ) - ( p_pLeft->Z * p_pRight->Y );
	p_pOut->Y = ( p_pLeft->Z * p_pRight->X ) - ( p_pLeft->X * p_pRight->Z );
	p_pOut->Z = ( p_pLeft->X * p_pRight->Y ) - ( p_pLeft->Y * p_pRight->X );
}

void VEC3_Clean( PVECTOR3 p_pVector )
{
	if( ARI_IsZero( p_pVector->X ) )
	{
		p_pVector->X = 0.0f;
	}
	if( ARI_IsZero( p_pVector->Y ) )
	{
		p_pVector->Y = 0.0f;
	}
	if( ARI_IsZero( p_pVector->Z ) )
	{
		p_pVector->Z = 0.0f;
	}
}

void VEC3_Zero( PVECTOR3 p_pVector )
{
	p_pVector->X = p_pVector->Y = p_pVector->Z = 0.0f;
}

