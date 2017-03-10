#include "QUIX.h"

Renderer::Renderer( void* window )
{
	// Store the handle to the parent window.
	m_window = (HWND)window;
//m_bLostDevice = false;
}

Renderer::~Renderer()
{
	DeInit();
}

void Renderer::DeInit()
{
	SAFE_RELEASE( m_sprite );
	SAFE_RELEASE( m_device );
	SAFE_RELEASE( m_d3d );

	g_engine->SetRendererValid( false );
}

bool Renderer::Init()
{
	m_d3d = Direct3DCreate9( D3D_SDK_VERSION );
	if (m_d3d == NULL)
	{
		return false;
	}

	//get system desktop color depth
	D3DDISPLAYMODE dm;
	m_d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( D3DPRESENT_PARAMETERS ) );
	
	d3dpp.Windowed = (!g_engine->GetFullScreen());
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = FindDepthStencilFormat( D3DADAPTER_DEFAULT, dm, D3DDEVTYPE_HAL );
	d3dpp.BackBufferFormat = dm.Format;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = g_engine->GetScreenWidth();
	d3dpp.BackBufferHeight = g_engine->GetScreenHeight();
	d3dpp.hDeviceWindow = m_window;
	//d3dpp.FullScreen_RefreshRateInHz = dm.RefreshRate;

D3DCAPS9 caps;
// Set Creation Flags

unsigned long ulFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
// Check if Hardware T&L is available
ZeroMemory(&caps, sizeof(D3DCAPS9));
m_d3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );

if ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	ulFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

// Attempt to create a HAL device
if( FAILED( m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, ulFlags, &d3dpp, &m_device ) ) )
{
	// Find REF depth buffer format
	d3dpp.AutoDepthStencilFormat = FindDepthStencilFormat( D3DADAPTER_DEFAULT, dm, D3DDEVTYPE_REF );

	// Check if Hardware T&L is available
	ZeroMemory(&caps , sizeof(D3DCAPS9));
	ulFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	m_d3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, &caps );
	
	if ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		ulFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	// Attempt to create a REF device
	if( FAILED( m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_window, ulFlags, &d3dpp, &m_device ) ) )
	{
		// Failed
		return false;
	} // End if Failure (REF)

} // End if Failure (HAL)



/*
// REPLACE THIS PART WITH SOMETHING BETTER

// Check for vertex shader version 2.0 support.
if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) )
	MessageBox(0, _T("NO V_SHADER 2.0"), _T("Error"), MB_OK);
// Check for pixel shader version 2.0 support.
if( caps.PixelShaderVersion < D3DPS_VERSION(2, 0) )
	MessageBox(0, _T("NO P_SHADER 2.0"), _T("Error"), MB_OK);
*/


// Store the present parameters
m_d3dpp = d3dpp;

	/*
	// Create the Direct3D device with hardware vertex processing. 
	if( FAILED( m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_device ) ) )
	{ 
		// Create the Direct3D device with mixed vertex processing. 
		if( FAILED( m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &m_device ) ) ) 
		{ 
			// Create the Direct3D device with software vertex processing. 
			if( FAILED( m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_device ) ) ) 
				return false; 
		} 
	}
	*/

	SetupRenderStates();

	// Store the display mode details.
	m_displayMode.Width = d3dpp.BackBufferWidth;
	m_displayMode.Height = d3dpp.BackBufferHeight;
	m_displayMode.RefreshRate = d3dpp.FullScreen_RefreshRateInHz;
	m_displayMode.Format = d3dpp.BackBufferFormat;

	HRESULT result = D3DXCreateSprite(m_device, &m_sprite);
	if (result != D3D_OK)
		return false;

	g_engine->SetRendererValid( true );

	return true;
}

void Renderer::SetupRenderStates()
{
	m_device->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	m_device->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	m_device->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
	m_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	m_device->SetRenderState( D3DRS_LIGHTING, FALSE );

	// Set the texture filters to use anisotropic texture filtering.
	m_device->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
	m_device->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	m_device->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	// Set the projection matrix.
	//D3DXMATRIX projMatrix;
	//D3DXMatrixPerspectiveFovLH( &projMatrix, D3DXToRadian( 60.0f ), (float)m_d3dpp.BackBufferWidth / (float)m_d3dpp.BackBufferHeight, 0.1f / g_engine->GetScale(), 1000.0f / g_engine->GetScale() );
	//m_device->SetTransform( D3DTS_PROJECTION, &projMatrix );
}

/*
bool Renderer::CheckRenderDeviceLost()
{
	// Recover lost device if required
	if ( m_bLostDevice )
	{
		// Can we reset the device yet ?
		HRESULT hRet = m_device->TestCooperativeLevel();
		if ( hRet == D3DERR_DEVICENOTRESET )
		{
			g_engine->GetSceneManager()->DestroyScene();
			SAFE_RELEASE( m_sprite );

			// Restore the device
			m_device->Reset( &m_d3dpp );

			SetupRenderStates();
			D3DXCreateSprite(m_device, &m_sprite);
			g_engine->GetSceneManager()->RestoreScene();

			m_bLostDevice = false;
			//MessageBox(0, "FALSE", "m_bLostDevice",0);
			return false;
		} // End if Can Reset
		else
		{
			// device cannot be reset at this time
			return true;
		}
	} // end if Device Lost
	return false;
}
*/

bool Renderer::BeginScene(unsigned long flags)
{
	m_device->Clear( 0, NULL, flags, 0, 1.0f, 0 );
	if (!m_device)
		return false;
	if (m_device->BeginScene() != D3D_OK)
		return false;
		
	return true;
}

bool Renderer::EndScene()
{
	if (!m_device)
		return false;
	if (m_device->EndScene() != D3D_OK)
		return false;
	//if (m_device->Present(NULL, NULL, NULL, NULL) != D3D_OK)
	//	return false;
if ( FAILED(m_device->Present( NULL, NULL, NULL, NULL )) ) 
{
//m_bLostDevice = true;
	//MessageBox(0, "TRUE", "m_bLostDevice",0);
	return false;
}

	return true;
}

bool Renderer::Begin2D()
{
	if (m_sprite->Begin(D3DXSPRITE_ALPHABLEND/* | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT*/) != D3D_OK)
	{
		return false;
	}
	else
	{
		//m_device->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		D3DXMATRIX identity;
		D3DXMatrixIdentity(&identity);
		m_sprite->SetTransform(&identity);
		return true;
	}
}

bool Renderer::End2D()
{
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	m_sprite->SetTransform(&identity);
	//m_device->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	if (m_sprite->End() != D3D_OK)
		return false;

	return true;
}

D3DFORMAT Renderer::FindDepthStencilFormat( ULONG AdapterOrdinal, D3DDISPLAYMODE Mode, D3DDEVTYPE DevType )
{

    // Test for 24 bith depth buffer
    if (SUCCEEDED( m_d3d->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D32 )))
    {
        if (SUCCEEDED( m_d3d->CheckDepthStencilMatch ( AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D32 ))) return D3DFMT_D32;
    
    } // End if 32bpp Available

    // Test for 24 bit depth buffer
    if (SUCCEEDED( m_d3d->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D24X8 )))
    {
        if (SUCCEEDED( m_d3d->CheckDepthStencilMatch ( AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D24X8 ))) return D3DFMT_D24X8;
    
    } // End if 24bpp Available

    // Test for 16 bit depth buffer
    if (SUCCEEDED( m_d3d->CheckDeviceFormat(AdapterOrdinal, DevType, Mode.Format, D3DUSAGE_DEPTHSTENCIL , D3DRTYPE_SURFACE , D3DFMT_D16 )))
    {
        if (SUCCEEDED( m_d3d->CheckDepthStencilMatch ( AdapterOrdinal, DevType, Mode.Format, Mode.Format, D3DFMT_D16 ))) return D3DFMT_D16;
    
    } // End if 16bpp Available

    // No depth buffer supported
    return D3DFMT_UNKNOWN;

}


IDirect3DDevice9 *Renderer::GetDevice()
{
	return m_device;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the display mode of the current Direct3D device.
//-----------------------------------------------------------------------------
D3DDISPLAYMODE *Renderer::GetDisplayMode()
{
	return &m_displayMode;
}

D3DPRESENT_PARAMETERS *Renderer::GetPresentationParams()
{
	return &m_d3dpp;
}
//-----------------------------------------------------------------------------
// Returns a pointer to the sprite interface.
//-----------------------------------------------------------------------------
ID3DXSprite *Renderer::GetSpriteHandler()
{
	return m_sprite;
}