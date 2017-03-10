#include "Main.h"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
Game *g_game = NULL;

//SceneObject *object = NULL;

//-----------------------------------------------------------------------------
// Game class constructor.
//-----------------------------------------------------------------------------
Game::Game() : State( STATE_GAME )
{
	// Store a pointer to the game state in a global variable for easy access.
	g_game = this;

	// Invalidate the score board font.
	m_scoreBoardFont = NULL;

	// Invalidate the player manager.
	m_bulletManager = NULL;
	m_playerManager = NULL;

	m_healthAmmoFont = NULL;

	m_updateFont = NULL;

	m_fpsFont = NULL;
}

//-----------------------------------------------------------------------------
// Allows the game state to preform any pre-processing construction.
//-----------------------------------------------------------------------------
void Game::Load()
{
	// Hide the mouse cursor.
	ShowCursor( false );

	// Create the score board font.
	m_scoreBoardFont = new Font( "Courier New", 14, FW_BOLD );
	m_scoreBoardNames[0] = 0;
	m_scoreBoardFrags[0] = 0;
	m_scoreBoardDeaths[0] = 0;

	// Create both the bullet manager and the player manager.
	m_bulletManager = new BulletManager;
	m_playerManager = new PlayerManager;

	// Load and play the in-game music.
	m_music = new Sound( "conspiracy8.mp3", g_engine->GetEngineSetup()->soundPath, SOUND_2D );
	m_music->Play( true );
	m_music->SetVolume(0.5f);

	// Create the font to display the frame rate.
	m_fpsFont = new Font();
	m_healthAmmoFont = new Font("Courier New", 44, FW_BOLD);

	m_crosshair = new Sprite();
	m_crosshair->LoadImage("Crosshair.dds", g_engine->GetEngineSetup()->spritePath);
	//m_crosshair->SetPosition( g_engine->GetRenderer()->GetDisplayMode()->Width / 2.0f - 16.0f, g_engine->GetRenderer()->GetDisplayMode()->Height / 2.0f - 16.0f );
	m_crosshair->SetPosition( g_engine->GetEngineSetup()->screenwidth / 2.0f - 16.0f, g_engine->GetEngineSetup()->screenheight / 2.0f - 16.0f );

	m_scope = new Sprite();
	m_scope->LoadImage("sniper_scope.png", g_engine->GetEngineSetup()->spritePath);
	m_scope->SetScale( ((float)g_engine->GetEngineSetup()->screenheight) / (float)m_scope->GetHeight());
	m_scope->SetPosition( g_engine->GetEngineSetup()->screenwidth/2.0f - (m_scope->GetWidth()/2.0f * m_scope->GetScale()), g_engine->GetEngineSetup()->screenheight/2.0f - (m_scope->GetHeight()/2.0f * m_scope->GetScale()) );

	m_healthHUD = new Sprite();
	m_healthHUD->LoadImage("HealthHUD.dds", g_engine->GetEngineSetup()->spritePath);
	m_healthHUD->SetPosition( 10, g_engine->GetEngineSetup()->screenheight - 10 - m_healthHUD->GetHeight() );

	m_ammoHUD = new Sprite();
	m_ammoHUD->LoadImage("AmmoHUD.dds", g_engine->GetEngineSetup()->spritePath);
	m_ammoHUD->SetPosition( g_engine->GetEngineSetup()->screenwidth - 10 - m_ammoHUD->GetWidth(), g_engine->GetEngineSetup()->screenheight - 10 - m_ammoHUD->GetHeight() );

	m_healthDamageHUD = new Sprite();
	m_healthDamageHUD->LoadImage("HealthDamageHUD.dds", g_engine->GetEngineSetup()->spritePath);
	m_healthDamageHUD->SetPosition( 10, g_engine->GetEngineSetup()->screenheight - 10 - m_healthDamageHUD->GetHeight() );

	m_ammoEmptyHUD = new Sprite();
	m_ammoEmptyHUD->LoadImage("AmmoEmptyHUD.dds", g_engine->GetEngineSetup()->spritePath);
	m_ammoEmptyHUD->SetPosition( g_engine->GetEngineSetup()->screenwidth - 10 - m_ammoEmptyHUD->GetWidth(), g_engine->GetEngineSetup()->screenheight - 10 - m_ammoEmptyHUD->GetHeight() );

	m_scoreBoard = new Sprite();
	m_scoreBoard->LoadImage("ScoreBoard.png", g_engine->GetEngineSetup()->spritePath);
	m_scoreBoard->SetPosition( g_engine->GetEngineSetup()->screenwidth/2 - m_scoreBoard->GetWidth()/2, g_engine->GetEngineSetup()->screenheight/2 - m_scoreBoard->GetHeight()/2 );

	m_updateFont = new Font( "Courier New", 14, FW_BOLD );
	m_updateText[0] = 0;
	m_updateTimer = 0.0f;
	/*
	m_asteroid = new Sprite();
	m_asteroid->LoadImage("asteroid.png", g_engine->GetEngineSetup()->spritePath);
	m_asteroid->SetColumns(8);
	m_asteroid->SetTotalFrames(64);
	m_asteroid->SetSize(60,60);
	m_asteroid->SetFrameTimer(30);
	m_asteroid->SetPosition(100,100);
	*/
}

//-----------------------------------------------------------------------------
// Allows the game state to preform any post-processing destruction.
//-----------------------------------------------------------------------------
void Game::Close()
{
	// Show the mouse cursor.
	ShowCursor( true );

	// Terminate the session.
	g_engine->GetNetwork()->Terminate();

	// Destroy the scene.
	g_engine->GetSceneManager()->DestroyScene();

	// Destroy both the bullet manager and the player manager.
	SAFE_DELETE( m_bulletManager );
	SAFE_DELETE( m_playerManager );

	// Destroy the score board font.
	SAFE_DELETE( m_scoreBoardFont );

	// Destroy the in-game music.
	SAFE_DELETE( m_music );

	SAFE_DELETE( m_fpsFont );

	SAFE_DELETE( m_healthAmmoFont );

	SAFE_DELETE(m_scoreBoard);
	SAFE_DELETE( m_healthHUD );
	SAFE_DELETE( m_ammoHUD );

	SAFE_DELETE( m_healthDamageHUD );
	SAFE_DELETE( m_ammoEmptyHUD );

	SAFE_DELETE( m_updateFont );

	SAFE_DELETE( m_scope );
	SAFE_DELETE( m_crosshair );
}

//-----------------------------------------------------------------------------
// Returns the view setup details for the given frame.
//-----------------------------------------------------------------------------
void Game::RequestViewer( ViewerSetup *viewer )
{
	viewer->viewer = m_playerManager->GetViewingPlayer();
	//viewer->viewClearFlags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
	viewer->viewClearFlags = D3DCLEAR_ZBUFFER;
}

//-----------------------------------------------------------------------------
// Update the game state.
//-----------------------------------------------------------------------------
void Game::Update( float elapsed )
{

	m_updateTimer += elapsed;

	/*
	if( g_engine->GetInput()->GetKeyPress( DIK_LALT, true ) == true && g_engine->GetInput()->GetKeyPress( DIK_RETURN ) == true )
	{
		if( g_engine->GetFullScreen() == true )
		{
			//SetWindowLongPtr(g_engine->m_window, GWL_EXSTYLE, WS_EX_LEFT);
			SetWindowLongPtr(g_engine->m_window, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE);
			ChangeDisplaySettings(NULL, CDS_RESET);
			SetWindowPos(g_engine->m_window, HWND_NOTOPMOST, 
							( GetSystemMetrics(SM_CXSCREEN) - g_engine->GetEngineSetup()->screenwidth ) / 2, 
							( GetSystemMetrics(SM_CYSCREEN) - g_engine->GetEngineSetup()->screenheight ) / 2, 
							g_engine->GetEngineSetup()->screenwidth, 
							g_engine->GetEngineSetup()->screenheight, 
							SWP_DRAWFRAME | SWP_SHOWWINDOW);
			ShowWindow(g_engine->m_window, SW_SHOW);

			g_engine->SetFullScreen( false );
		}
		else
		{
			DEVMODE fullscreenSettings;

			EnumDisplaySettings(NULL, 0, &fullscreenSettings);
			fullscreenSettings.dmPelsWidth        = g_engine->GetEngineSetup()->screenwidth;
			fullscreenSettings.dmPelsHeight       = g_engine->GetEngineSetup()->screenheight;
			fullscreenSettings.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT;

			//SetWindowLongPtr(g_engine->m_window, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
			SetWindowLongPtr(g_engine->m_window, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetWindowPos(g_engine->m_window, HWND_TOPMOST,
							( GetSystemMetrics(SM_CXSCREEN) - g_engine->GetEngineSetup()->screenwidth ) / 2, 
							( GetSystemMetrics(SM_CYSCREEN) - g_engine->GetEngineSetup()->screenheight ) / 2, 
							g_engine->GetEngineSetup()->screenwidth, 
							g_engine->GetEngineSetup()->screenheight, 
							SWP_SHOWWINDOW);
			ChangeDisplaySettings(&fullscreenSettings, CDS_FULLSCREEN);
			ShowWindow(g_engine->m_window, SW_SHOWMAXIMIZED);

			g_engine->SetFullScreen( true );
		}
		g_engine->GetRenderer()->DeInit();
		g_engine->GetRenderer()->Init();
		g_engine->GetSceneManager()->RestoreScene();
	}
	*/

	/*
	// Allow the player to respawn by pressing the R key.
	if( g_engine->GetInput()->GetKeyPress( DIK_R ) == true )
	{
		m_playerManager->GetLocalPlayer()->SetEnabled( false );
		m_playerManager->SpawnLocalPlayer();
	}
	*/

	// Update the bullet manager before the player manager. This will prevent
	// bullets that are fired this frame from being processed this frame.
	m_bulletManager->Update( elapsed );

	// Update the player manager.
	m_playerManager->Update( elapsed );

	// Check if the user is holding down the tab key.
	if( g_engine->GetInput()->GetKeyPress( DIK_TAB, true ) == true )
	{
		// Build the score board text.
		//sprintf( m_scoreBoardNames, "PLAYER\n" );
		//sprintf( m_scoreBoardFrags, "FRAGS\n" );
		//sprintf( m_scoreBoardDeaths, "DEATHS\n" );
		m_scoreBoardNames[0] = 0;
		m_scoreBoardFrags[0] = 0;
		m_scoreBoardDeaths[0] = 0;

		// Add each player's details to the score board.
		PlayerObject *player = m_playerManager->GetNextPlayer( true );
		while( player != NULL )
		{
			strcat( m_scoreBoardNames, player->GetName() );
			strcat( m_scoreBoardNames, "\n" );

			sprintf( m_scoreBoardFrags, "%s%d", m_scoreBoardFrags, player->GetFrags() );
			strcat( m_scoreBoardFrags, "\n" );

			sprintf( m_scoreBoardDeaths, "%s%d", m_scoreBoardDeaths, player->GetDeaths() );
			strcat( m_scoreBoardDeaths, "\n" );

			player = m_playerManager->GetNextPlayer();
		}
	}

	// Calculate the frame rate.
	static float frameTime = 1.0f;
	static int frameCount = 0;
	frameTime += elapsed;
	frameCount++;

	// Update the fps every second.
	if( frameTime > 1.0f )
	{
		sprintf_s( m_fpsText, 16, "%d", frameCount );
		strcat(m_fpsText, " FPS");

		frameTime = 0.0f;
		frameCount = 0;
	}

	if( m_playerManager->GetLocalPlayer() )
	{
		sprintf_s( m_healthText, 4, "%d", (int)m_playerManager->GetLocalPlayer()->GetHealth() );
		sprintf_s( m_ammoText, 4, "%d", (int)m_playerManager->GetLocalPlayer()->GetCurrentWeapon()->GetAmmo() );
	}

	if(m_updateTimer > 3.0f)
	{
		m_updateText[0] = 0;
		m_updateTimer = 0.0f;
	}

	// Check if the user wants to exit back to the menu.
	if( g_engine->GetInput()->GetKeyPress( DIK_ESCAPE ) )
		g_engine->ChangeState( STATE_MENU );
}

//-----------------------------------------------------------------------------
// Render the game state.
//-----------------------------------------------------------------------------
void Game::Render()
{
	// Ensure the scene is loaded.
	//if( g_engine->GetSceneManager()->IsLoaded() == false )
	//	return;	
}

void Game::Render2D()
{
	// Ensure the scene is loaded.
	if( g_engine->GetSceneManager()->IsLoaded() == false )
		return;

	static float previousHealth = 0.0f;

	m_updateFont->Render( m_updateText, 20, 50, D3DCOLOR_RGBA(255,245,104,255) );

	// If the user is holding down the tab key, then render the score board.
	if( g_engine->GetInput()->GetKeyPress( DIK_TAB, true ) == true )
	{
		m_scoreBoard->Draw(0.1f);
		m_scoreBoardFont->Render( "PLAYER", m_scoreBoard->GetPosition().getX() + 130, m_scoreBoard->GetPosition().getY() + 25, D3DCOLOR_RGBA(255,245,104,255) );
		m_scoreBoardFont->Render( "FRAGS", m_scoreBoard->GetPosition().getX() + 390, m_scoreBoard->GetPosition().getY() + 25, D3DCOLOR_RGBA(255,245,104,255) );
		m_scoreBoardFont->Render( "DEATHS", m_scoreBoard->GetPosition().getX() + 530, m_scoreBoard->GetPosition().getY() + 25, D3DCOLOR_RGBA(255,245,104,255) );
		m_scoreBoardFont->Render( m_scoreBoardNames, m_scoreBoard->GetPosition().getX() + 20, m_scoreBoard->GetPosition().getY() + 80, D3DCOLOR_RGBA(255,245,104,255) );
		m_scoreBoardFont->Render( m_scoreBoardFrags, m_scoreBoard->GetPosition().getX() + 410, m_scoreBoard->GetPosition().getY() + 80, D3DCOLOR_RGBA(255,245,104,255) );
		m_scoreBoardFont->Render( m_scoreBoardDeaths, m_scoreBoard->GetPosition().getX() + 555, m_scoreBoard->GetPosition().getY() + 80, D3DCOLOR_RGBA(255,245,104,255) );
	}

	if(m_playerManager->IsZoomedIn() == false)
	{
		m_crosshair->Draw(0.2f);
	}
	else
	{
		m_scope->Draw(0.2f);
	}

	/*
	if( m_playerManager->GetLocalPlayer() == NULL )
	{
		// Player not ready / added to players list yet
		return;
	}
	*/

	if(m_playerManager->GetLocalPlayer()->GetHealth() < previousHealth || m_playerManager->GetLocalPlayer()->GetHealth() == 0.0f)
	{
		m_healthDamageHUD->Draw(0.1f);
	}
	else
	{
		m_healthHUD->Draw(0.1f);
	}
	previousHealth = m_playerManager->GetLocalPlayer()->GetHealth();
	
	if(m_playerManager->GetLocalPlayer()->GetCurrentWeapon()->GetAmmo() == 0)
	{
		m_ammoEmptyHUD->Draw(0.1f);
	}
	else
	{
		m_ammoHUD->Draw(0.1f);
	}

	m_healthAmmoFont->Render( m_healthText,
							m_healthHUD->GetWidth()*0.5f -  7,
							g_engine->GetEngineSetup()->screenheight - 10 - m_healthHUD->GetHeight() +  12,
							D3DCOLOR_RGBA(255,245,104,255));
	m_healthAmmoFont->Render( m_ammoText, 
							g_engine->GetEngineSetup()->screenwidth - 10 - m_ammoHUD->GetWidth() +  10, 
							g_engine->GetEngineSetup()->screenheight - 10 - m_ammoHUD->GetHeight() +  12,
							D3DCOLOR_RGBA(255,245,104,255));

	// Render the frame rate.
	m_fpsFont->Render( m_fpsText, 0, 0 );
}

//-----------------------------------------------------------------------------
// Returns a pointer to the bullet manager.
//-----------------------------------------------------------------------------
BulletManager *Game::GetBulletManager()
{
	return m_bulletManager;
}

//-----------------------------------------------------------------------------
// Handles the game specific network messages. Called by the network object.
//-----------------------------------------------------------------------------
void Game::HandleNetworkMessage( ReceivedMessage *msg )
{
	// Process the received messaged based on its type.
	switch( msg->msgid )
	{
		case MSGID_CREATE_PLAYER:
		{
			// Add the new player to the player manager and the scene.
			PlayerObject *object = m_playerManager->AddPlayer( g_engine->GetNetwork()->GetPlayer( msg->id ) );
			g_engine->GetSceneManager()->AddObject( object );

			// Check if the new player is the host player.
			if( object->GetID() == 0 )
			{
				// Load the scene from the host player's selection.
				char* temp = new char[strlen(( (PlayerInfo*)g_engine->GetNetwork()->GetPlayer( msg->id ) )->map) +5];
				strcpy( temp, ( (PlayerInfo*)g_engine->GetNetwork()->GetPlayer( msg->id ) )->map );
				
				char* temp_dir = new char[strlen(temp) + strlen(g_engine->GetEngineSetup()->scenePath) +2];
				strcpy( temp_dir, g_engine->GetEngineSetup()->scenePath );
				strcat( temp_dir, temp );
				for( unsigned int i = 0; i < strlen(temp_dir)+1; i++ )
				{
					if( temp_dir[i] == ' ' )
						temp_dir[i] = '_';
				}
				strcat( temp_dir, "/" );

				strcat( temp, ".txt" );
				g_engine->GetSceneManager()->LoadScene( temp, temp_dir );

				SAFE_DELETE_ARRAY(temp);
				SAFE_DELETE_ARRAY(temp_dir);

				// Allow the network to receive game specific messages.
				g_engine->GetNetwork()->SetReceiveAllowed( true );
			}

			break;
		}

		case MSGID_DESTROY_PLAYER:
		{
			// Remove the player from the player manager and the scene.
			SceneObject *object = m_playerManager->GetPlayer( msg->id );
			g_engine->GetSceneManager()->RemoveObject( &object );
			m_playerManager->RemovePlayer( msg->id );

			break;
		}

		case MSGID_PLAYER_HEALTH:
		{
			//if( g_engine->GetNetwork()->GetID() == msg->id )
			//	break;

			// Get a pointer to the game specific network message.
			PlayerHealthMsg *phm = (PlayerHealthMsg*)msg;

			// Set the player's health.
			float health = phm->health;
			if( health < 0.0f )
			{
				health = 0.0f;
			}
			m_playerManager->GetPlayer( phm->id )->SetHealth( health );

					
					// Check if the player has been killed.
					if( phm->health > 0.0f )
						break;

					if( m_playerManager->IsZoomedIn() == true )
					{
						m_playerManager->ZoomOut();
					}
					m_playerManager->GetPlayer( phm->id )->Kill();

					// Host should update score
					if( g_engine->GetNetwork()->GetID() != 0 )
						break;
					

					/*
					if( phm->health > 0.0f || g_engine->GetNetwork()->GetID() != 0 )
						break;
					*/

			// Check if the player fragged himself and act accordingly
			if( phm->id != phm->attackerid )
			{
				// Send a player score update message for the fragged player.
				PlayerScoreMsg psm1;
				psm1.msgid = MSGID_PLAYER_SCORE;
				psm1.id = phm->id;
				psm1.frags = m_playerManager->GetPlayer( phm->id )->GetFrags();
				psm1.deaths = m_playerManager->GetPlayer( phm->id )->GetDeaths() + 1;
				psm1.attackerid = phm->attackerid; // Who killed the player?
				psm1.waskilled = true;
				g_engine->GetNetwork()->Send( &psm1, sizeof( PlayerScoreMsg ) );

				// Send a player score update message for the bullet owner.
				PlayerScoreMsg psm2;
				psm2.msgid = MSGID_PLAYER_SCORE;
				psm2.id = phm->attackerid;
				psm2.frags = m_playerManager->GetPlayer( phm->attackerid )->GetFrags() + 1;
				psm2.deaths = m_playerManager->GetPlayer( phm->attackerid )->GetDeaths();
				psm2.fragid = phm->id; // Whom did the player kill?
				psm2.waskilled = false;
				g_engine->GetNetwork()->Send( &psm2, sizeof( PlayerScoreMsg ) );
			}
			else
			{
				PlayerScoreMsg psm3;
				psm3.msgid = MSGID_PLAYER_SCORE;
				psm3.id = phm->id;
				psm3.frags = m_playerManager->GetPlayer( phm->id )->GetFrags() - 1;
				psm3.deaths = m_playerManager->GetPlayer( phm->id )->GetDeaths() + 1;
				psm3.attackerid = phm->id; // The player killed himself/herself
				psm3.waskilled = true;
				g_engine->GetNetwork()->Send( &psm3, sizeof( PlayerScoreMsg ) );
			}

			break;
		}

		case MSGID_PLAYER_MOVE_UPDATE:
		{
			if( g_engine->GetNetwork()->GetID() == msg->id )
				break;

			// Get a pointer to the game specific network message.
			PlayerMoveUpdateMsg *pmum = (PlayerMoveUpdateMsg*)msg;

			// Get a pointer to the player in question.
			PlayerObject *player = m_playerManager->GetPlayer( pmum->id );

			// If the player is disabled then chances are the local player has
			// just joined the game, and is receiving messages about a player
			// that it does not realise has already spawned. So just enable
			// that player and start updating it.
			if( player->GetEnabled() == false )
			{
				player->SetEnabled( true );
				player->SetVisible( true );
			}

			// Update all the player's details.
			player->SetTranslation( pmum->translation.x, pmum->translation.y, pmum->translation.z );
			player->SetDrive( pmum->drive );
			player->SetStrafe( pmum->strafe );
			//player->SetJump( pmum->jump );
			player->SetFire( pmum->fire );

			break;
		}

		case MSGID_PLAYER_LOOK_UPDATE:
		{
			if( g_engine->GetNetwork()->GetID() == msg->id )
				break;

			// Get a pointer to the game specific network message.
			PlayerLookUpdateMsg *plum = (PlayerLookUpdateMsg*)msg;

			// Update the player's rotation and view tilt.
			m_playerManager->GetPlayer( plum->id )->SetRotation( 0.0f, plum->rotationY, 0.0f );
			m_playerManager->GetPlayer( plum->id )->SetViewTilt( plum->viewTilt );

			break;
		}

		case MSGID_PLAYER_JUMP_UPDATE:
		{
			if( g_engine->GetNetwork()->GetID() == msg->id )
				break;
			
			// Get a pointer to the game specific network message.
			NetworkMessage *pjum = (NetworkMessage*)msg;

			// Update the player's rotation and view tilt.
			m_playerManager->GetPlayer( pjum->id )->SetJump( true );

			break;
		}

		case MSGID_PLAYER_SCORE:
		{
			//if( g_engine->GetNetwork()->GetID() == msg->id )
			//	break;

			// Get a pointer to the game specific network message.
			PlayerScoreMsg *psm = (PlayerScoreMsg*)msg;

			if( psm->id == g_engine->GetNetwork()->GetID() )
			{
				// Score update is about you

				if( psm->waskilled == true )
				{
					if( psm->attackerid == psm->id )
					{
						// You killed yourself
						sprintf(m_updateText, "%s", "You killed yourself!");
					}
					else
					{
						// Someone else killed you
						sprintf(m_updateText, "%s%s", "You were killed by ", m_playerManager->GetPlayer( psm->attackerid )->GetName());
					}
				}
				else
				{
					// You killed someone
					sprintf(m_updateText, "%s%s", "You killed ", m_playerManager->GetPlayer( psm->fragid )->GetName());
				}
			}
			else
			{
				// Score update is about someone else

				if( psm->waskilled == true )
				{
					if( psm->attackerid == psm->id )
					{
						// The player killed himself/herself
						sprintf(m_updateText, "%s%s", m_playerManager->GetPlayer( psm->id )->GetName(), " committed suicide!");
					}
					else
					{
						// Someone else killed the player
						sprintf(m_updateText, "%s%s%s", m_playerManager->GetPlayer( psm->attackerid )->GetName(), " killed ", m_playerManager->GetPlayer( psm->id )->GetName());
					}
				}
				else
				{
					// The player killed someone
					sprintf(m_updateText, "%s%s%s", m_playerManager->GetPlayer( psm->id )->GetName(), " killed ", m_playerManager->GetPlayer( psm->fragid )->GetName());
				}
			}
			m_updateTimer = 0.0f;

			// Update the player's score.
			m_playerManager->GetPlayer( psm->id )->SetFrags( psm->frags );
			m_playerManager->GetPlayer( psm->id )->SetDeaths( psm->deaths );

			break;
		}

		case MSGID_PLAYER_WEAPON_CHANGE:
		{
			if( g_engine->GetNetwork()->GetID() == msg->id )
				break;

			// Get a pointer to the game specific network message.
			PlayerWeaponChangeMsg *pwcm = (PlayerWeaponChangeMsg*)msg;

			// Change the player's weapon.
			m_playerManager->GetPlayer( pwcm->id )->WeaponChanged( pwcm->weapon );

			break;
		}

		case MSGID_PLAYER_WEAPON_CHANGING:
		{
			if( g_engine->GetNetwork()->GetID() == msg->id )
				break;

			// Get a pointer to the game specific network message.
			PlayerWeaponChangeMsg *pwcm = (PlayerWeaponChangeMsg*)msg;

			// Indicate that this player is changing weapons.
			m_playerManager->GetPlayer( pwcm->id )->WeaponChanging();

			break;
		}

		case MSGID_SPAWN_POINT_REQUEST:
		{
			if( g_engine->GetNetwork()->GetID() != 0 )
				break;

			// Send a spawn point message back to the requesting player.
			SpawnPointMsg spm;
			spm.msgid = MSGID_SPAWN_POINT;
			spm.id = msg->id;
			spm.spawnPoint = g_engine->GetSceneManager()->GetSpawnPointID( g_engine->GetSceneManager()->GetRandomPlayerSpawnPoint() );
			g_engine->GetNetwork()->Send( &spm, sizeof( SpawnPointMsg ) );

			break;
		}

		case MSGID_SPAWN_POINT:
		{
			// Get a pointer to the game specific network message.
			SpawnPointMsg *spm = (SpawnPointMsg*)msg;

			// Ensure this message is for the local player.
			if( spm->id != g_engine->GetNetwork()->GetID() )
				break;

			// Spawn the local player using this spawn point.
			m_playerManager->SpawnLocalPlayer( spm->spawnPoint );

			break;
		}

		case MSGID_SPAWN_PLAYER:
		{
			// Get a pointer to the game specific network message.
			SpawnPlayerMsg *spm = (SpawnPlayerMsg*)msg;

			// Spawn the player.
			m_playerManager->SpawnPlayer( spm->id, spm->translation );

			break;
		}

		case MSGID_TERMINATE_SESSION:
		{
			// Switch to the menu state.
			g_engine->ChangeState( STATE_MENU );

			break;
		}
	}
}