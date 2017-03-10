#include "QUIX.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
//Engine *g_engine = NULL;


Engine::Engine()
{
	// Indicate that the engine is not yet loaded.
	m_loaded = false;

	m_window = NULL;

	// If no setup structure was passed in, then create a default one.
	// Otherwise, make a copy of the passed in structure.
	m_setup = new EngineSetup;
	//if( setup != NULL )
	//	memcpy( m_setup, setup, sizeof( EngineSetup ) );

	// Store a pointer to the engine in a global variable for easy access.
	g_engine = this;

	m_states = new LinkedList< State >;
	m_currentState = NULL;


	m_scriptManager = NULL;
	m_materialManager = NULL;
	m_meshManager = NULL;

	m_input = NULL;
	m_network = NULL;
	m_renderer = NULL;
	m_rendererValid = false;
	m_soundsystem = NULL;
	m_sceneManager = NULL; 
}

Engine::~Engine()
{
	// Ensure the engine is loaded.
	if( m_loaded == true )
	{
		// Destroy the states linked lists.
		if( m_currentState != NULL )
			m_currentState->Close();
		SAFE_DELETE( m_states );

		// Destroy everything.
		SAFE_DELETE( m_sceneManager );
		SAFE_DELETE( m_soundsystem );
		SAFE_DELETE( m_renderer );
		SAFE_DELETE( m_network );
		SAFE_DELETE( m_input );

		SAFE_DELETE( m_meshManager );
		SAFE_DELETE( m_materialManager );
		SAFE_DELETE( m_scriptManager );
	}

	// Destroy the engine setup structure.
	SAFE_DELETE( m_setup );
}

int Engine::InitScriptManager()
{
	m_scriptManager = new ResourceManager< Script >;

	return 1;
}

int Engine::Init()
{
	// Create the resource managers.
	if( m_scriptManager == NULL )
		m_scriptManager = new ResourceManager< Script >;

	m_materialManager = new ResourceManager< Material >( m_setup->CreateMaterialResource );
	m_meshManager = new ResourceManager< Mesh >;

	// Create the input object.
	m_input = new Input( m_window );

	m_network = new Network( m_setup->HandleNetworkMessage );

	m_renderer = new Renderer( m_window );

	if (!m_renderer->Init())
		return 0;

	m_soundsystem = new SoundSystem( m_setup->scale );

	m_sceneManager = new SceneManager( m_setup->scale, m_setup->spawnerPath );

	// Seed the random number generator with the current time.
	srand( timeGetTime() );

	// Allow the application to perform any state setup now.
	if( m_setup->StateSetup != NULL )
		m_setup->StateSetup();

	// The engine is fully loaded and ready to go.
	m_loaded = true;

	gameover = false;
	
	return 1;
}

void Engine::Shutdown()
{
	gameover = true;
}

//-----------------------------------------------------------------------------
// Enters the engine into the main processing loop.
//-----------------------------------------------------------------------------
void Engine::Update()
{


//This bool is required after I found a bug causing objects such as the players to fall down during startup.
//This is because all the initialization takes time within a single frame and this HUGE "elapsed" gets sent the first time.
//This causes gravity to act right at startup giving the objects a negative velocity and sending them through the ground.
//FIND A SOLUTION FOR THIS AND WORK YOUR WAY AROUND.
static bool enableElapsed = true;
static float temp_elapsed = 0.0f;

// Used to retrieve details about the viewer from the application.
ViewerSetup viewer;


	// Ensure the engine is loaded.
	if( m_loaded == true )
	{

			if( m_active == true )
			{
				// Calculate the elapsed time.
				unsigned long currentTime = timeGetTime();
				static unsigned long lastTime = currentTime;
				float elapsed = ( currentTime - lastTime ) * 0.001f;
				lastTime = currentTime;

				// If game hung for a long time, prevent it from iterating too far
				if( elapsed > 0.25f )
				{
					elapsed = 0.25f;
				}

/*
if (m_renderer->CheckRenderDeviceLost())
			return;
*/

				// Update the input object, reading the keyboard and mouse.
				m_input->Update();

				m_network->Update();

				m_soundsystem->Update();

				// Request the viewer from the current state, if there is one.
				if( m_currentState != NULL )
					m_currentState->RequestViewer( &viewer );


				if( viewer.viewer != NULL )
				{
					
							temp_elapsed = elapsed;
							
							if( enableElapsed == true )
							{
								temp_elapsed = 0.0f;
								enableElapsed = false;
							}
					

					// Update the scene.
					m_sceneManager->Update( temp_elapsed, viewer.viewer->GetViewMatrix() );
					//m_sceneManager->Update( elapsed, viewer.viewer->GetViewMatrix() );

					// Set the view transformation.
					if( m_rendererValid == true)
						m_renderer->GetDevice()->SetTransform( D3DTS_VIEW, viewer.viewer->GetViewMatrix() );

					// Update the 3D sound listener.
					D3DXVECTOR3 translation = viewer.viewer->GetTranslation();
					D3DXVECTOR3 velocity = viewer.viewer->GetVelocity();
					D3DXVECTOR3 forward = viewer.viewer->GetForwardVector();
					D3DXVECTOR3 up = viewer.viewer->GetUpVector();
					Vector3 t = Vector3( translation.x, translation.y, translation.z );
					Vector3 v = Vector3( velocity.x, velocity.y, velocity.z );
					Vector3 f = Vector3( forward.x, forward.y, forward.z );
					Vector3 u = Vector3( up.x, up.y, up.z );
					m_soundsystem->UpdateListener( t, v, f, u );
					//m_soundsystem->Update();
				}
				
				else
				{
					enableElapsed = true;
				}
				

				// Update the current state (if there is one), taking state
				// changes into account.
				m_stateChanged = false;
				if( m_currentState != NULL )
					m_currentState->Update( elapsed );
				if( m_stateChanged == true )
					return;

				// Begin the scene.
				if( m_rendererValid == true)
				{
					if( m_renderer->BeginScene(viewer.viewClearFlags) )
					{
						// Render the scene, if there is a valid viewer.
						if( viewer.viewer != NULL )
							m_sceneManager->Render( viewer.viewer->GetTranslation() );
						
						// Render the current state, if there is one.
						if( m_currentState != NULL )
							m_currentState->Render();

						// Begin 2D render
						m_renderer->Begin2D();

						if( m_currentState != NULL )
							m_currentState->Render2D();

						// End 2D render
						m_renderer->End2D();
						
						// End the scene and present it.
						m_renderer->EndScene();

						/*
						// Keep track of the index of the current back buffer.
						if( ++m_currentBackBuffer == m_setup->totalBackBuffers + 1 )
							m_currentBackBuffer = 0;
						*/
					}
				}
			}
			
			else if( m_active == false )
			{
				enableElapsed = true;
			}
			
	}

}

//-----------------------------------------------------------------------------
// Adds a state to the engine.
//-----------------------------------------------------------------------------
void Engine::AddState( State *state, bool change )
{
	m_states->Add( state );

	if( change == false )
		return;

	if( m_currentState != NULL )
		m_currentState->Close();

	m_currentState = m_states->GetLast();
	m_currentState->Load();
}

//-----------------------------------------------------------------------------
// Removes a state from the engine
//-----------------------------------------------------------------------------
void Engine::RemoveState( State *state )
{
	m_states->Remove( &state );
}

//-----------------------------------------------------------------------------
// Changes processing to the state with the specified ID.
//-----------------------------------------------------------------------------
void Engine::ChangeState( unsigned long id )
{
	// Iterate through the list of states and find the new state to change to.
	m_states->Iterate( true );
	while( m_states->Iterate() != NULL )
	{
		if( m_states->GetCurrent()->GetID() == id )
		{
			// Close the old state.
			if( m_currentState != NULL )
				m_currentState->Close();

			// Set the new current state and load it.
			m_currentState = m_states->GetCurrent();
			m_currentState->Load();

			// Indicate that the state has changed.
			m_stateChanged = true;

			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Returns a pointer to the current state.
//-----------------------------------------------------------------------------
State *Engine::GetCurrentState()
{
	return m_currentState;
}

ResourceManager< Script > *Engine::GetScriptManager()
{
	return m_scriptManager;
}

ResourceManager< Material > *Engine::GetMaterialManager()
{
	return m_materialManager;
}

ResourceManager< Mesh > *Engine::GetMeshManager()
{
	return m_meshManager;
}

Input *Engine::GetInput()
{
	return m_input;
}

Network *Engine::GetNetwork()
{
	return m_network;
}

Renderer *Engine::GetRenderer()
{
	return m_renderer;
}

SoundSystem *Engine::GetSoundSystem()
{
	return m_soundsystem;
}

SceneManager *Engine::GetSceneManager()
{
	return m_sceneManager;
}