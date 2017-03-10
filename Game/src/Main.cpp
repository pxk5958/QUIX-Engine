#include "Main.h"

void HandleNetworkMessage( ReceivedMessage *msg )
{
	// Only allow network messages to be processed while in the game state.
	if( g_engine->GetCurrentState()->GetID() != STATE_GAME )
		return;

	// Pass control over to the game state's network message handler.
	( (Game*)g_engine->GetCurrentState() )->HandleNetworkMessage( msg );
}

void StateSetup()
{
	g_engine->AddState( new Menu, true );
	g_engine->AddState( new Game, false );
}

void CreateMaterialResource( Material **resource, char *name, char *path = "./" )
{
	if( strcmp( name, "defaultMaterial" ) == 0 )
		*resource = new GameMaterial();
	else
		*resource = new GameMaterial( name, path );
}

bool game_preload()
{
	g_engine->InitScriptManager();


	// Create the engine setup structure.
	Script *settings = new Script( "EngineSettings.txt" );
	/*
	if( settings->GetNumberData( _T("screen_width") ) == NULL )
	{
		g_engine->GetEngineSetup()->fullscreen = false;
		g_engine->GetEngineSetup()->screenwidth = 800;
		g_engine->GetEngineSetup()->screenheight = 600;
		g_engine->GetEngineSetup()->spawnerPath = _T("./Assets/Objects/");
		g_engine->GetEngineSetup()->soundPath = _T("./Assets/Sounds/");
		g_engine->GetEngineSetup()->scenePath = _T("./Assets/Scenes/");
		g_engine->GetEngineSetup()->characterPath = _T("./Assets/Characters/");
		g_engine->GetEngineSetup()->scale = 0.01f;
		g_engine->GetEngineSetup()->fov = 90.0f;

		settings->AddVariable( _T("fullscreen_mode"), VARIABLE_BOOL, &g_engine->GetEngineSetup()->fullscreen );
		settings->AddVariable( _T("screen_width"), VARIABLE_NUMBER, &g_engine->GetEngineSetup()->screenwidth );
		settings->AddVariable( _T("screen_height"), VARIABLE_NUMBER, &g_engine->GetEngineSetup()->screenheight );
		settings->AddVariable( _T("scene_path"), VARIABLE_STRING, g_engine->GetEngineSetup()->scenePath );
		settings->AddVariable( _T("character_path"), VARIABLE_STRING, g_engine->GetEngineSetup()->characterPath );
		settings->AddVariable( _T("spawner_path"), VARIABLE_STRING, g_engine->GetEngineSetup()->spawnerPath );
		settings->AddVariable( _T("sound_path"), VARIABLE_STRING, g_engine->GetEngineSetup()->soundPath );
		settings->AddVariable( _T("scale"), VARIABLE_FLOAT, &g_engine->GetEngineSetup()->scale );
		settings->AddVariable( _T("fov"), VARIABLE_FLOAT, &g_engine->GetEngineSetup()->fov );

		settings->SaveScript();
	}
	*/
	

		g_engine->GetEngineSetup()->fullscreen = *settings->GetBoolData( "fullscreen_mode" );
		g_engine->GetEngineSetup()->screenwidth = *settings->GetNumberData( "screen_width" );
		g_engine->GetEngineSetup()->screenheight = *settings->GetNumberData( "screen_height" );

		SAFE_DELETE_ARRAY(g_engine->GetEngineSetup()->spawnerPath);
		g_engine->GetEngineSetup()->spawnerPath = new char[strlen( settings->GetStringData( "spawner_path" ) )+1];
		memcpy( g_engine->GetEngineSetup()->spawnerPath, settings->GetStringData( "spawner_path" ), ( strlen( settings->GetStringData( "spawner_path" ) )+1 ) * sizeof(char) );

		SAFE_DELETE_ARRAY(g_engine->GetEngineSetup()->soundPath);
		g_engine->GetEngineSetup()->soundPath = new char[strlen( settings->GetStringData( "sound_path" ) )+1];
		memcpy( g_engine->GetEngineSetup()->soundPath, settings->GetStringData( "sound_path" ), ( strlen( settings->GetStringData( "sound_path" ) )+1 ) * sizeof(char) );

		SAFE_DELETE_ARRAY(g_engine->GetEngineSetup()->scenePath);
		g_engine->GetEngineSetup()->scenePath = new char[strlen( settings->GetStringData( "scene_path" ) )+1];
		memcpy( g_engine->GetEngineSetup()->scenePath, settings->GetStringData( "scene_path" ), ( strlen( settings->GetStringData( "scene_path" ) )+1 ) * sizeof(char) );

		SAFE_DELETE_ARRAY(g_engine->GetEngineSetup()->characterPath);
		g_engine->GetEngineSetup()->characterPath = new char[strlen( settings->GetStringData( "character_path" ) )+1];
		memcpy( g_engine->GetEngineSetup()->characterPath, settings->GetStringData( "character_path" ), ( strlen( settings->GetStringData( "character_path" ) )+1 ) * sizeof(char) );

		SAFE_DELETE_ARRAY(g_engine->GetEngineSetup()->spritePath);
		g_engine->GetEngineSetup()->spritePath = new char[strlen( settings->GetStringData( "sprite_path" ) )+1];
		memcpy( g_engine->GetEngineSetup()->spritePath, settings->GetStringData( "sprite_path" ), ( strlen( settings->GetStringData( "sprite_path" ) )+1 ) * sizeof(char) );

		g_engine->GetEngineSetup()->scale = *settings->GetFloatData( "scale" );
		g_engine->GetEngineSetup()->fov = *settings->GetFloatData( "fov" );

	SAFE_DELETE( settings );


	g_engine->GetEngineSetup()->name = "FPS Game";
	
	g_engine->GetEngineSetup()->StateSetup = StateSetup;
	g_engine->GetEngineSetup()->CreateMaterialResource = CreateMaterialResource;
	g_engine->GetEngineSetup()->HandleNetworkMessage = HandleNetworkMessage;

	return true;
}