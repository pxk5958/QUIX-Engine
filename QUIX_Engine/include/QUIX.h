#ifndef QUIX_H
#define QUIX_H

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#define _WINSOCKAPI_    // stops windows.h in wxwidgets from including winsock.h since enet has to include winsock2.h

#ifdef _DEBUG
#include "vld.h"
#endif

//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>

//-----------------------------------------------------------------------------
// System Includes
//-----------------------------------------------------------------------------
//#include <stdio.h>
//#include <tchar.h>
//#include <windowsx.h>
#include <math.h>
#include <minmax.h>
#include <vector>
#include <stdexcept>

//-----------------------------------------------------------------------------
// Platform Specific Includes
//-----------------------------------------------------------------------------
#include <tchar.h>

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define SAFE_DELETE( p )       { if( p ) { delete ( p );     ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p );   ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if( p ) { ( p )->Release(); ( p ) = NULL; } }

#define PI 3.14159265358979323

//-----------------------------------------------------------------------------
// Engine Includes
//-----------------------------------------------------------------------------
#include "Vector3.h"
#include "LinkedList.h"
#include "ResourceManagement.h"
#include "Geometry.h"
#include "Input.h"
#include "Scripting.h"
#include "Renderer.h"
#include "Font.h"
#include "SoundSystem.h"
#include "BoundingVolume.h"
#include "Material.h"
#include "Mesh.h"
#include "Sprite.h"
#include "SceneObject.h"
#include "AnimatedObject.h"
#include "SpawnerObject.h"
#include "ViewFrustum.h"
#include "RenderCache.h"
#include "SceneManager.h"
#include "CollisionDetection.h"
#include "State.h"
#include "Network.h"

extern bool game_preload();

//-----------------------------------------------------------------------------
// Engine Setup Structure
//-----------------------------------------------------------------------------
struct EngineSetup
{
	//HINSTANCE instance; // Application instance handle.
	//GUID guid; // Application GUID.
	char *name; // Name of the application.
	float scale; // Unit scale in meters/unit.
	float fov; // Field of view.
	//unsigned char totalBackBuffers; // Number of back buffers to use.
	void (*HandleNetworkMessage)( ReceivedMessage *msg ); // Network message handler.
	void (*StateSetup)(); // State setup function.
	void (*CreateMaterialResource)( Material **resource, char *name, char *path ); // Material resource creation function.
	char *spawnerPath; // Path for locating the spawner object scripts.
	char *soundPath;
	char *scenePath;
	char *characterPath;
	char *spritePath;

	bool fullscreen;
	int screenwidth;
	int screenheight;

	//-------------------------------------------------------------------------
	// The engine setup structure constructor.
	//-------------------------------------------------------------------------
	EngineSetup()
	{
		//GUID defaultGUID = { 0x24215591, 0x24c0, 0x4316, { 0xb5, 0xb2, 0x67, 0x98, 0x2c, 0xb3, 0x82, 0x54 } };

		//instance = NULL;
		//guid = defaultGUID;
		name = "QUIX Application";
		scale = 1.0f;
		fov = 90.0f;
		//totalBackBuffers = 1;
		HandleNetworkMessage = NULL;
		StateSetup = NULL;
		CreateMaterialResource = NULL;

		scenePath = new char[ 3 ];
		memcpy( scenePath, "./", 3 * sizeof(char) );

		spawnerPath = new char[ 3 ];
		memcpy( spawnerPath, "./", 3 * sizeof(char) );

		soundPath = new char[ 3 ];
		memcpy( soundPath, "./", 3 * sizeof(char) );

		characterPath = new char[ 3 ];
		memcpy( characterPath, "./", 3 * sizeof(char) );

		spritePath = new char[ 3 ];
		memcpy( spritePath, "./", 3 * sizeof(char) );

		fullscreen = false;
		screenwidth = 640;
		screenheight = 480;
	}

	~EngineSetup()
	{
		SAFE_DELETE_ARRAY(spawnerPath);
		SAFE_DELETE_ARRAY(scenePath);
		SAFE_DELETE_ARRAY(characterPath);
		SAFE_DELETE_ARRAY(soundPath);
		SAFE_DELETE_ARRAY(spritePath);
	}
};

//-----------------------------------------------------------------------------
// Engine Class
//-----------------------------------------------------------------------------
class Engine
{
public:
	Engine();
	virtual ~Engine();

	void Update();

	int Init();
	//ScriptManager may be optionally initialized well before other subsystems as it does not depend on Engine Setup structure or the window for initialization.
	//This is provided because the end-user might require to read scripts well before the window is created or the game is initialized.
	//TO DO : Add support for separate initizlization of other inidividual subsystems if possible.
	int InitScriptManager();

	void Shutdown();
	
	void AddState( State *state, bool change = true );
	void RemoveState( State *state );
	void ChangeState( unsigned long id );
	State *GetCurrentState();

	Input *GetInput();
	Network *GetNetwork();
	Renderer *GetRenderer();
	SoundSystem *GetSoundSystem();
	SceneManager *GetSceneManager();

	ResourceManager< Script > *GetScriptManager();	
	ResourceManager< Material > *GetMaterialManager();
	ResourceManager< Mesh > *GetMeshManager();
	
HWND m_window; // Main window handle.

//Main window frame for dialog boxes,etc. to disable fullscreen mode.
//WinFrame* mainWindowFrame;

	//HWND GetWindow();
	void SetActiveFlag( bool active ) {	m_active = active; }
	bool GetActiveFlag() { return m_active; }
	char*  GetAppTitle() { return m_setup->name; }
	void SetAppTitle(char*  value) { m_setup->name = value; }
	int GetScreenWidth() { return m_setup->screenwidth; }
	void SetScreenWidth(int value) { m_setup->screenwidth = value; }
	int GetScreenHeight() { return m_setup->screenheight; }
	void SetScreenHeight(int value) { m_setup->screenheight = value; }
	bool GetFullScreen() { return m_setup->fullscreen; }
	void SetFullScreen(bool value) { m_setup->fullscreen = value; }

	void SetScale( float scale ) {	m_setup->scale = scale; }
	float GetScale() { return m_setup->scale; }

	void SetFov( float fov ) {	m_setup->fov = fov; }
	float GetFov() { return m_setup->fov; }

	EngineSetup *GetEngineSetup() { return m_setup; }

	bool GetRendererValid() { return m_rendererValid; }
	void SetRendererValid( bool value ) { m_rendererValid = value; }

	bool IsGameOver() { return gameover; }

private:
	
	bool gameover;
	bool m_loaded; // Indicates if the engine is loading.
	//HWND m_window; // Main window handle.
	bool m_active; // Indicates if the application is active or not.
	
	EngineSetup *m_setup; // Copy of the engine setup structure.

	//EngineSetup *m_setup; // Copy of the engine setup structure.
	LinkedList< State > *m_states; // Linked list of states.
	State *m_currentState; // Pointer to the current state.
	bool m_stateChanged; // Indicates if the state changed in the current frame.

	ResourceManager< Script > *m_scriptManager; // Script manager.
	ResourceManager< Material > *m_materialManager; // Material manager.
	ResourceManager< Mesh > *m_meshManager; // Mesh manager.
	
	Input *m_input; // Input object.
	Network *m_network;
	Renderer *m_renderer;
	bool m_rendererValid;
	SoundSystem *m_soundsystem;
	SceneManager *m_sceneManager; // Scene manager.
};

//-----------------------------------------------------------------------------
// Externals
//-----------------------------------------------------------------------------
extern Engine *g_engine;

#endif