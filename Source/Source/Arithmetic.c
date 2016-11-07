#include <Arithmetic.h>
#include <mathf.h>

bool ARI_IsZero( const float p_Value )
{
	if( fabsf( p_Value ) < ARI_EPSILON )
	{
		return true;
	}

	return false;
}

