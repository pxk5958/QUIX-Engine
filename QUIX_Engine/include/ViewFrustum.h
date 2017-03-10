#ifndef VIEW_FRUSTUM_H
#define VIEW_FRUSTUM_H


class ViewFrustum
{
public:
	void Update( D3DXMATRIX *view );

	void SetProjectionMatrix( D3DXMATRIX projection );

	int IsBoxInside( D3DXVECTOR3 min, D3DXVECTOR3 max );
	int IsBoxInside( D3DXVECTOR3 translation, D3DXVECTOR3 min, D3DXVECTOR3 max );
	int IsSphereInside( D3DXVECTOR3 translation, float radius );

private:
	D3DXMATRIX m_projection; // Pointer to a projection matrix.
	D3DXPLANE m_planes[6]; // Six planes of the view frustum
};

#endif