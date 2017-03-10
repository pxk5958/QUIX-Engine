//NOTE : MSG_TERMINATE_SESSION is not handled in this demo. Thus, if the host disconnects, clients show as if they are still connected


#include "..\..\QUIX_Engine\include\QUIX.h"

//#define KEY_DOWN(vk) ((GetAsyncKeyState(vk) & 0x8000)?1:0)

//-----------------------------------------------------------------------------
// Test State Class
//-----------------------------------------------------------------------------
class TestState : public State
{
public:
	//-------------------------------------------------------------------------
	// Allows the test state to preform any pre-processing construction.
	//-------------------------------------------------------------------------
	virtual void Load()
	{
		//g_engine->GetRenderer()->Init();

		m_font = new Font;
		m_text[0] = 0;
		m_connected = false;

		// Load the settings
		Script *settings = new Script( "MenuSettings.txt" );
		if( settings->GetStringData( "host_address" ) == NULL )
		{
			m_hostIP = new char[strlen("localhost")+1];
			strcpy( m_hostIP, "localhost" );
		}
		else
		{
			m_hostIP = new char[strlen(settings->GetStringData( "host_address" ))+1];
			strcpy( m_hostIP, settings->GetStringData( "host_address" ) );
		}
		SAFE_DELETE( settings );
				
		// Check if the user wants to host a session.
		//if( g_engine->GetInput()->GetKeyPress( DIK_H ) == true )
			//m_connected = g_engine->GetNetwork()->Init( NULL, true );

		// Check if the user wants to enumerate the sessions on the network.
		//else if( g_engine->GetInput()->GetKeyPress( DIK_J ) == true )
			//m_connected = g_engine->GetNetwork()->Init( NULL, false );
	};

	//-------------------------------------------------------------------------
	// Allows the test state to preform any post-processing destruction.
	//-------------------------------------------------------------------------
	virtual void Close()
	{
		SAFE_DELETE( m_font );
		SAFE_DELETE_ARRAY( m_hostIP );
	};

	//-------------------------------------------------------------------------
	// Returns the view setup details for the given frame.
	//-------------------------------------------------------------------------
	virtual void RequestViewer( ViewerSetup *viewer )
	{
		viewer->viewClearFlags = D3DCLEAR_TARGET;
	}

	//-------------------------------------------------------------------------
	// Updates the state.
	//-------------------------------------------------------------------------
	virtual void Update( float elapsed )
	{
		// Check if the user wants to host a session.
		if( g_engine->GetInput()->GetKeyPress( DIK_H ) == true )
		{
			m_connected = g_engine->GetNetwork()->Init( NULL, m_hostIP, true );
		}

		// Check if the user wants to enumerate the sessions on the network.
		else if( g_engine->GetInput()->GetKeyPress( DIK_J ) == true )
		{
			m_connected = g_engine->GetNetwork()->Init( NULL, m_hostIP, false );
		}
		

		// Check if the user wants to quit.
		if( g_engine->GetInput()->GetKeyPress( DIK_Q ) == true )
		{
			g_engine->GetNetwork()->Terminate();
			g_engine->Shutdown();
			//PostQuitMessage( 0 );
		}

		// Create the menu text.
		wcscpy( m_text, _T("\n\n\n H - Host Session\n\nJ - Join Session") );

		// Add the quit option to the menu text.
		wcscat( m_text, _T("\n\n Q - Quit") );

		// Add the connection status text.
		if( m_connected == true )
		{
			if( g_engine->GetNetwork()->IsHost() == true )
				wcscat( m_text, _T("\n\n CONNECTED - HOST") );			
			else
				wcscat( m_text, _T("\n\n CONNECTED - CLIENT") );

			_stprintf(temp,_T("\n\n ID : %u"),g_engine->GetNetwork()->GetID());

			wcscat(temp, _T("\n\n No. Players :"));
			TCHAR no[4];
			unsigned long n = g_engine->GetNetwork()->GetNumberOfPlayers();
			_stprintf(no, _T(" %lu"), n );
			wcscat(temp, no);

			/*
			wcscat(temp, _T("\n\n List of Players :") );

			PlayerInfo * tempFirst = g_engine->GetNetwork()->GetPlayerList()->GetFirst();
			while( tempFirst != NULL )
			{
				TCHAR id[4];
				_stprintf(id, _T(" %u"), tempFirst->id);
				wcscat( temp, id );

				tempFirst = g_engine->GetNetwork()->GetPlayerList()->GetNext(tempFirst);
			}
			*/

			wcscat( m_text, temp );
			
		}
		else
			wcscat( m_text, _T("\n\nNOT CONNECTED") );
	};

	//-------------------------------------------------------------------------
	// Renders the state.
	//-------------------------------------------------------------------------
	virtual void Render()
	{
		m_font->Render( m_text, 0.0f, 0.0f );
	};

private:
	Font *m_font; // Font to display the menu options.

	TCHAR* m_hostIP;

	TCHAR m_text[ MAX_PATH ]; // Text for the menu options.
	bool m_connected; // Indicates if the network object is connected.
	TCHAR  temp[ MAX_PATH ];
};


//-----------------------------------------------------------------------------
// Application specific state setup.
//-----------------------------------------------------------------------------
void StateSetup()
{
	g_engine->AddState( new TestState, true );
}

bool game_preload()
{
	g_engine->GetEngineSetup()->name = _T("Network Test");
	g_engine->GetEngineSetup()->fullscreen = false;
	g_engine->GetEngineSetup()->screenwidth = 800;
	g_engine->GetEngineSetup()->screenheight = 600;
	g_engine->GetEngineSetup()->scale = 0.01f;
	g_engine->GetEngineSetup()->StateSetup = StateSetup;

	return true;
}