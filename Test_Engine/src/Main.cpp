#include "..\..\QUIX_Engine\include\QUIX.h"


//-----------------------------------------------------------------------------
// Test State Class
//-----------------------------------------------------------------------------
class TestState : public State
{
public:

	void Close()
	{
		SAFE_DELETE( m_fpsFont );
		SAFE_DELETE( m_coordFont );
	}

	virtual void Load()
	{
		// Create a scene object that will be used as a camera.
		m_viewer = new SceneObject;
		m_viewer->SetBoundingSphere( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 120.0f, D3DXVECTOR3( 0.5f, 1.0f, 0.5f ) );
		m_viewer->SetBoundingBox( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 120.0f );
		m_viewer->SetTranslation( 0.0f, 200.0f, -500.0f );
		m_viewer->SetFriction( 8.0f );
		m_viewer->SetFlyMode(true);
		g_engine->GetSceneManager()->AddObject( m_viewer );

		g_engine->GetSceneManager()->LoadScene( "Level.txt", g_engine->GetEngineSetup()->scenePath );

		// Create the font to display the frame rate.
		m_fpsFont = new Font();
		
		m_coordFont = new Font( "Arial", 14, FW_BOLD );
	}

	virtual void RequestViewer( ViewerSetup *viewer )
	{
		viewer->viewer = m_viewer;
		viewer->viewClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
	}

	virtual void Update( float elapsed )
	{

		if( g_engine->GetInput()->GetKeyPress( DIK_R ) == true )
		{
			m_viewer->SetTranslation( 0.0f, 200.0f, -500.0f );
		}

		if( g_engine->GetInput()->GetKeyPress( DIK_ESCAPE ) == true )
		{
			g_engine->Shutdown();
		}

		// Allow the camera (scene object) to be moved around.
		if( g_engine->GetInput()->GetKeyPress( DIK_W, true ) == true )
			m_viewer->Drive( 8000.0f * elapsed, !m_viewer->GetFlyMode() );
		if( g_engine->GetInput()->GetKeyPress( DIK_S, true ) == true )
			m_viewer->Drive( -8000.0f * elapsed, !m_viewer->GetFlyMode() );
		if( g_engine->GetInput()->GetKeyPress( DIK_A, true ) == true )
			m_viewer->Strafe( -4000.0f * elapsed, !m_viewer->GetFlyMode() );
		if( g_engine->GetInput()->GetKeyPress( DIK_D, true ) == true )
			m_viewer->Strafe( 4000.0f * elapsed, !m_viewer->GetFlyMode() );

		
		if( m_viewer->GetFlyMode() == false )
		{
			static float jumpInterval = 0.0f;
			jumpInterval += elapsed;
			if( g_engine->GetInput()->GetKeyPress( DIK_SPACE, true ) == true && m_viewer->IsTouchingGround() == true )
				if( jumpInterval > 0.30f )
				{
					m_viewer->Jump( 3100.0f /* * elapsed */ );
					jumpInterval = 0.0f;
				}
		}

		// Adjust the rotation of the camera based on the mouse movement.
		m_viewer->AddRotation( (float)g_engine->GetInput()->GetDeltaY() * elapsed, (float)g_engine->GetInput()->GetDeltaX() * elapsed, 0.0f );
		
		// Calculate the frame rate.
		static float frameTime = 1.0f;
		static int frameCount = 0;
		frameTime += elapsed;
		frameCount++;

		// Update the fps every second.
		if( frameTime > 1.0f )
		{
			sprintf_s( m_fpsText, 16, "%d", frameCount );
			//_tcscat( m_fpsText, _T("\n\n") );

			frameTime = 0.0f;
			frameCount = 0;
		}

		sprintf( m_coordX, "%f", m_viewer->GetTranslation().x );
		strcat( m_coordX, "\n" );

		sprintf( m_coordY, "%f", m_viewer->GetTranslation().y );
		strcat( m_coordY, "\n" );

		sprintf( m_coordZ, "%f", m_viewer->GetTranslation().z );
		strcat( m_coordZ, "\n" );
	}

	void Render()
	{
		// Render the frame rate.
		m_fpsFont->Render( m_fpsText, 0, 0 );

		m_coordFont->Render( m_coordX, 10, 50, 0xFFFF7700 );
		m_coordFont->Render( m_coordY, 10, 70, 0xFFFF7700 );
		m_coordFont->Render( m_coordZ, 10, 90, 0xFFFF7700 );
	}

private:

	SceneObject *m_viewer; // The viewer scene object is used as a camera.

	char m_fpsText[16]; // Frame rate character string.
	Font *m_fpsFont; // Font for rendering the frame rate.

	char m_coordX[20];
	char m_coordY[20];
	char m_coordZ[20];
	Font *m_coordFont;
};

//-----------------------------------------------------------------------------
// Application specific state setup.
//-----------------------------------------------------------------------------
void StateSetup()
{
	g_engine->AddState( new TestState, true );
}

//-----------------------------------------------------------------------------
// Entry point for the application.
//-----------------------------------------------------------------------------
bool game_preload()
{
	g_engine->GetEngineSetup()->fullscreen = false;
	g_engine->GetEngineSetup()->screenwidth = 800;
	g_engine->GetEngineSetup()->screenheight = 600;

	SAFE_DELETE_ARRAY(g_engine->GetEngineSetup()->spawnerPath);
	g_engine->GetEngineSetup()->spawnerPath = new char[strlen( "./Assets/Spawners/" )+1];
	memcpy( g_engine->GetEngineSetup()->spawnerPath, "./Assets/Spawners/", ( strlen( "./Assets/Spawners/" )+1 ) * sizeof(char) );

	SAFE_DELETE_ARRAY(g_engine->GetEngineSetup()->scenePath);
	g_engine->GetEngineSetup()->scenePath = new char[strlen( "./Assets/Scenes/Level/" )+1];
	memcpy( g_engine->GetEngineSetup()->scenePath, "./Assets/Scenes/Level/", ( strlen( "./Assets/Scenes/Level/" )+1 ) * sizeof(char) );

	g_engine->GetEngineSetup()->scale = 0.01f;

	g_engine->GetEngineSetup()->name = "Scene Test";
	
	g_engine->GetEngineSetup()->StateSetup = StateSetup;

	return true;
}