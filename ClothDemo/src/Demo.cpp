#include "Main.h"

Demo::Demo() : State( STATE_DEMO )
{
}

void Demo::Load()
{
	// Hide the mouse cursor.
	ShowCursor( false );

	// Create a scene object that will be used as a camera.
	m_viewer = new SceneObject;
	m_viewer->SetBoundingSphere( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 60.0f, D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) );
	m_viewer->SetBoundingBox( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ), 120.0f );
	m_viewer->SetTranslation( 0.0f, 200.0f, -500.0f );
	m_viewer->SetFriction( 8.0f );
	m_viewer->SetFlyMode(true);
	g_engine->GetSceneManager()->AddObject( m_viewer );

	g_engine->GetSceneManager()->LoadScene( "Dust.txt", g_engine->GetEngineSetup()->scenePath );

	m_cloth = new Cloth(-150.0f, 500.0f, 0.0f, 50, 30, 10);
	gustThreshold = 0;

	// Create the font to display the frame rate.
	m_fpsFont = new Font();
}

//-----------------------------------------------------------------------------
// Allows the game state to preform any post-processing destruction.
//-----------------------------------------------------------------------------
void Demo::Close()
{
	// Show the mouse cursor.
	ShowCursor( true );

	SAFE_DELETE( m_cloth );

	// Destroy the scene.
	g_engine->GetSceneManager()->DestroyScene();

	SAFE_DELETE( m_fpsFont );
}

//-----------------------------------------------------------------------------
// Returns the view setup details for the given frame.
//-----------------------------------------------------------------------------
void Demo::RequestViewer( ViewerSetup *viewer )
{
	viewer->viewer = m_viewer;
	viewer->viewClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
}

//-----------------------------------------------------------------------------
// Update the game state.
//-----------------------------------------------------------------------------
void Demo::Update( float elapsed )
{
	if( g_engine->GetInput()->GetKeyPress( DIK_R ) == true )
	{
		m_viewer->SetTranslation( 0.0f, 0.0f, 0.0f );
	}

	if( g_engine->GetInput()->GetKeyPress( DIK_ESCAPE ) == true )
	{
		g_engine->Shutdown();
	}

	// Allow the camera (scene object) to be moved around.
	if( g_engine->GetInput()->GetKeyPress( DIK_W, true ) == true )
		m_viewer->Drive( 4000.0f * elapsed, !m_viewer->GetFlyMode() );
	if( g_engine->GetInput()->GetKeyPress( DIK_S, true ) == true )
		m_viewer->Drive( -4000.0f * elapsed, !m_viewer->GetFlyMode() );
	if( g_engine->GetInput()->GetKeyPress( DIK_A, true ) == true )
		m_viewer->Strafe( -2000.0f * elapsed, !m_viewer->GetFlyMode() );
	if( g_engine->GetInput()->GetKeyPress( DIK_D, true ) == true )
		m_viewer->Strafe( 2000.0f * elapsed, !m_viewer->GetFlyMode() );


	// Adjust the rotation of the camera based on the mouse movement.
	m_viewer->AddRotation( (float)g_engine->GetInput()->GetDeltaY() * elapsed * 0.4f, (float)g_engine->GetInput()->GetDeltaX() * elapsed * 0.4f, 0.0f );
		
	m_cloth->AddForce(Vector3(0.0f,-98100.0f,0.0f)*TIME_STEPSIZE2); // add gravity each frame, pointing down
	m_cloth->AddWindForce_CreateNormals(Vector3(50000.0f,0.0f,4400.0f)*TIME_STEPSIZE2); // generate some wind each frame
	m_cloth->Update(); // calculate the particle positions of the next frame
	m_cloth->BallCollision(Vector3(m_viewer->GetBoundingSphere()->centre.x,m_viewer->GetBoundingSphere()->centre.y,m_viewer->GetBoundingSphere()->centre.z), 
							m_viewer->GetBoundingSphere()->radius); // resolve collision with the ball

	// Calculate the frame rate.
	static float frameTime = 0.0f;
	static int frameCount = 0;
	frameTime += elapsed;
	frameCount++;

	// Update the fps every half a second.
	if( frameTime > 0.5f )
	{
		sprintf_s( m_fpsText, 16, "%d", (int)(frameCount / frameTime) );

		frameTime = 0.0f;
		frameCount = 0;
	}	
}

//-----------------------------------------------------------------------------
// Render the game state.
//-----------------------------------------------------------------------------
void Demo::Render()
{
	// Ensure the scene is loaded.
	if( g_engine->GetSceneManager()->IsLoaded() == false )
		return;

	m_cloth->Render();

	// Render the frame rate.
	m_fpsFont->Render( m_fpsText, 0, 0, D3DCOLOR_ARGB(255,255,255,255) );
}
