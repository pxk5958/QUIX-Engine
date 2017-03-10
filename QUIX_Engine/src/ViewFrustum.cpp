#include "QUIX.h"


void ViewFrustum::Update( D3DXMATRIX *view )
{
	// Calculate the field of view.
	D3DXMATRIX fov;
	D3DXMatrixMultiply( &fov, view, &m_projection );

	// Calculate the right plane.
	m_planes[0].a = fov._14 - fov._11;
	m_planes[0].b = fov._24 - fov._21;
	m_planes[0].c = fov._34 - fov._31;
	m_planes[0].d = fov._44 - fov._41;

	// Calculate the left plane.
	m_planes[1].a = fov._14 + fov._11;
	m_planes[1].b = fov._24 + fov._21;
	m_planes[1].c = fov._34 + fov._31;
	m_planes[1].d = fov._44 + fov._41;

	// Calculate the top plane.
	m_planes[2].a = fov._14 - fov._12;
	m_planes[2].b = fov._24 - fov._22;
	m_planes[2].c = fov._34 - fov._32;
	m_planes[2].d = fov._44 - fov._42;

	// Calculate the bottom plane.
	m_planes[3].a = fov._14 + fov._12;
	m_planes[3].b = fov._24 + fov._22;
	m_planes[3].c = fov._34 + fov._32;
	m_planes[3].d = fov._44 + fov._42;

	// Calculate the far plane.
	m_planes[4].a = fov._14 - fov._13;
	m_planes[4].b = fov._24 - fov._23;
	m_planes[4].c = fov._34 - fov._33;
	m_planes[4].d = fov._44 - fov._43;

	// Calculate the near plane.
	m_planes[5].a = fov._13;
	m_planes[5].b = fov._23;
	m_planes[5].c = fov._33;
	m_planes[5].d = fov._43;

	// Normalize the planes.
	D3DXPlaneNormalize( &m_planes[0], &m_planes[0] );
	D3DXPlaneNormalize( &m_planes[1], &m_planes[1] );
	D3DXPlaneNormalize( &m_planes[2], &m_planes[2] );
	D3DXPlaneNormalize( &m_planes[3], &m_planes[3] );
	D3DXPlaneNormalize( &m_planes[4], &m_planes[4] );
}

//-----------------------------------------------------------------------------
// Set's the view frustum's internal projection matrix.
//-----------------------------------------------------------------------------
void ViewFrustum::SetProjectionMatrix( D3DXMATRIX projection )
{
	m_projection = projection;
}

//-----------------------------------------------------------------------------
// Returns 1 if the given box is inside the view frustum, -1 if outside, and 0 if intersects
//-----------------------------------------------------------------------------
int ViewFrustum::IsBoxInside( D3DXVECTOR3 min, D3DXVECTOR3 max )
{
	/*
	//Old method
	int total_in = 0;

	for( int p = 0; p < 6; p++ )
	{
		int in_count = 8;
		int point_in = 1;

		if( D3DXPlaneDotCoord( &m_planes[p], &D3DXVECTOR3( min.x, min.y, min.z ) ) < 0.0f )
		{
			point_in = 0;
			--in_count;
		}
		if( D3DXPlaneDotCoord( &m_planes[p], &D3DXVECTOR3( max.x, min.y, min.z ) ) < 0.0f )
		{
			point_in = 0;
			--in_count;
		}
		if( D3DXPlaneDotCoord( &m_planes[p], &D3DXVECTOR3( min.x, max.y, min.z ) ) < 0.0f )
		{
			point_in = 0;
			--in_count;
		}
		if( D3DXPlaneDotCoord( &m_planes[p], &D3DXVECTOR3( max.x, max.y, min.z ) ) < 0.0f )
		{
			point_in = 0;
			--in_count;
		}
		if( D3DXPlaneDotCoord( &m_planes[p], &D3DXVECTOR3( min.x, min.y, max.z ) ) < 0.0f )
		{
			point_in = 0;
			--in_count;
		}
		if( D3DXPlaneDotCoord( &m_planes[p], &D3DXVECTOR3( max.x, min.y, max.z ) ) < 0.0f )
		{
			point_in = 0;
			--in_count;
		}
		if( D3DXPlaneDotCoord( &m_planes[p], &D3DXVECTOR3( min.x, max.y, max.z ) ) < 0.0f )
		{
			point_in = 0;
			--in_count;
		}
		if( D3DXPlaneDotCoord( &m_planes[p], &D3DXVECTOR3( max.x, max.y, max.z ) ) < 0.0f )
		{
			point_in = 0;
			--in_count;
		}

		if (in_count == 0)
			return -1;

		total_in += point_in;
	}

	if( total_in == 6 )
		return 1;

	return 0;
	*/


	//New method
	bool intersect = false;
	D3DXVECTOR3 minExtreme,maxExtreme;

	for( int i = 0; i < 6; i++ )
	{
		if (m_planes[i].a >= 0)
		{
			minExtreme.x = min.x;
			maxExtreme.x = max.x;
		}
		else
		{
			minExtreme.x = max.x;
			maxExtreme.x = min.x;
		}

		if (m_planes[i].b >= 0)
		{
			minExtreme.y = min.y;
			maxExtreme.y = max.y;
		}
		else
		{
			minExtreme.y = max.y;
			maxExtreme.y = min.y;
		}

		if (m_planes[i].c >= 0)
		{
			minExtreme.z = min.z;
			maxExtreme.z = max.z;
		}
		else
		{
			minExtreme.z = max.z;
			maxExtreme.z = min.z;
		}

		if ( D3DXPlaneDotCoord( &m_planes[i], &maxExtreme ) < 0 )
			return -1;

		if ( D3DXPlaneDotCoord( &m_planes[i], &minExtreme ) < 0 )
			intersect = true;
	}

	if(intersect)
		return 0;

	return 1;
}

//-----------------------------------------------------------------------------
// Returns 1 if the given box is inside the view frustum, -1 if outside, and 0 if intersects
//-----------------------------------------------------------------------------
int ViewFrustum::IsBoxInside( D3DXVECTOR3 translation, D3DXVECTOR3 min, D3DXVECTOR3 max )
{
	return IsBoxInside( min + translation, max + translation );
}

//-----------------------------------------------------------------------------
// Returns 1 if the given sphere is inside the view frustum, -1 if outside, and 0 if intersects
//-----------------------------------------------------------------------------
int ViewFrustum::IsSphereInside( D3DXVECTOR3 translation, float radius )
{
	for( int p = 0; p < 6; p++ )
	{
		if( D3DXPlaneDotCoord( &m_planes[p], &translation ) < -radius )
			return -1;
		else if( fabs(D3DXPlaneDotCoord( &m_planes[p], &translation )) < radius )
			return 0;
	}

	return 1;
}