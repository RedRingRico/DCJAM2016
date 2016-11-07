#ifndef __DCJAM_VECTOR3_H__
#define __DCJAM_VECTOR3_H__

typedef struct _tagVECTOR3
{
	float	X;
	float	Y;
	float	Z;
}VECTOR3, *PVECTOR3;

void VEC3_Add( PVECTOR3 p_pOut, const PVECTOR3 p_pLeft,
	const PVECTOR3 p_pRight );
void VEC3_Subtract( PVECTOR3 p_pOut, const PVECTOR3 p_pLeft,
	const PVECTOR3 p_pRight );
void VEC3_MultiplyV( PVECTOR3 p_pOut, const PVECTOR3 p_pLeft,
	const PVECTOR3 p_pRight );
void VEC3_MultiplyF( PVECTOR3 p_pOut, const PVECTOR3 p_pVectore,
	const float p_Scalar );
void VEC3_Divide( PVECTOR3 p_pOut, const PVECTOR3 p_pVector3,
	const float p_Scalar );
float VEC3_Magnitude( const PVECTOR3 p_pVector );
float VEC3_MagnitudeSquared( const PVECTOR3 p_pVector );
float VEC3_Distance( const PVECTOR3 p_pLeft, const PVECTOR3 p_pRight );
float VEC3_DistanceSquared( const PVECTOR3 p_pLeft, const PVECTOR3 p_pRight );
void VEC3_Normalise( PVECTOR3 p_pVector );
float VEC3_Dot( const PVECTOR3 p_pLeft, const PVECTOR3 p_pRight );
void VEC3_Cross( PVECTOR3 p_pOut, const PVECTOR3 p_pLeft,
	const PVECTOR3 p_pRight );
void VEC3_Clean( PVECTOR3 p_pVector );
void VEC3_Zero( PVECTOR3 p_pVector );

#endif /* __DCJAM_VECTOR3_H__ */

