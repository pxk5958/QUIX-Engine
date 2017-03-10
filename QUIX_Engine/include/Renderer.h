#ifndef RENDERER_H
#define RENDERER_H

#include <d3d9.h>
#include <d3dx9.h>

class Mesh;
class Material;

class Renderer
{
public:
	Renderer( void* window );
	virtual ~Renderer();

	void DeInit();
	bool Init();
	//void Release();
	
//bool CheckRenderDeviceLost();
	void SetupRenderStates();
	bool BeginScene(unsigned long flags);
	bool EndScene();
	bool Begin2D();
	bool End2D();

	D3DFORMAT FindDepthStencilFormat( ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType );
	
	IDirect3DDevice9 *GetDevice();
	D3DDISPLAYMODE *GetDisplayMode();
	ID3DXSprite *GetSpriteHandler();
	D3DPRESENT_PARAMETERS *GetPresentationParams();

private:
	HWND m_window; // Handle of the parent window.
	
	LPDIRECT3DDEVICE9 m_device; // Direct3D device interface.
	D3DDISPLAYMODE m_displayMode; // Display mode of the current Direct3D device.
	D3DPRESENT_PARAMETERS m_d3dpp;
	LPD3DXSPRITE m_sprite; // Sprite interface for rendering 2D textured primitives.
	LPDIRECT3D9 m_d3d;
	//LPDIRECT3DSURFACE9 p_backbuffer;

//bool m_bLostDevice;
};

#endif