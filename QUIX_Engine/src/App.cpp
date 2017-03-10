#include "QUIX.h"

#include <windowsx.h>
#include <windows.h>


Engine *g_engine;

//-----------------------------------------------------------------------------
// Handles Windows messages.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	switch( msg )
	{
		case WM_ACTIVATEAPP:
		{
			g_engine->SetActiveFlag( wparam );
			return 0;
		}

		case WM_DESTROY:
		{
			PostQuitMessage( 0 );
			return 0;
		}

		default:
			return DefWindowProc( wnd, msg, wparam, lparam );
	}
}

//-----------------------------------------------------------------------------
// Entry point for the application.
//-----------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev, LPSTR cmdLine, int cmdShow )
{
	// Create the engine (using the setup structure), then run it.
	g_engine = new Engine();

	if( game_preload() == false )
		return false;
	
	// Prepare and register the window class.
	WNDCLASSEX wcex;
	wcex.cbSize        = sizeof( WNDCLASSEX );
	wcex.style         = CS_CLASSDC;
	wcex.lpfnWndProc   = WindowProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = instance;
	wcex.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
	wcex.hCursor       = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = _T("WindowClass");
	wcex.hIconSm       = LoadIcon( NULL, IDI_APPLICATION );
	RegisterClassEx( &wcex );

	::OleUninitialize();      // Uninit OLE

	// Initialise the COM using multithreaded concurrency.
	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	// Create the window and retrieve a handle to it.
	g_engine->m_window = CreateWindowA( "WindowClass", 
							g_engine->GetEngineSetup()->name, 
							g_engine->GetEngineSetup()->fullscreen ? WS_POPUP | WS_VISIBLE : WS_OVERLAPPED | WS_VISIBLE, 
							( GetSystemMetrics(SM_CXSCREEN) - g_engine->GetEngineSetup()->screenwidth ) / 2, 
							( GetSystemMetrics(SM_CYSCREEN) - g_engine->GetEngineSetup()->screenheight ) / 2, 
							g_engine->GetEngineSetup()->screenwidth,
							g_engine->GetEngineSetup()->screenheight, NULL, NULL, instance, NULL );

	g_engine->Init();


	// Show the window.
	ShowWindow( g_engine->m_window, SW_NORMAL );

	// Enter the message loop.
	MSG msg;
	ZeroMemory( &msg, sizeof( MSG ) );
	while( msg.message != WM_QUIT && g_engine->IsGameOver() == false )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			g_engine->Update();
		}

	}

	// Destroy the engine.
	SAFE_DELETE( g_engine );


	// Uninitialise the COM.
	CoUninitialize();

	// Unregister the window class.
	UnregisterClass( _T("WindowClass"), instance );

	return true;
}