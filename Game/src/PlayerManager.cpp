#include "Main.h"

//-----------------------------------------------------------------------------
// Player manager class constructor.
//-----------------------------------------------------------------------------
PlayerManager::PlayerManager()
{
	// Create the list of player objects.
	m_players = new LinkedList< PlayerObject >;
	m_viewingPlayer = NULL;

	// Clear all the local player's movement details.
	m_localMovement = false;
	m_localDrive = 0.0f;
	m_localStrafe = 0.0f;
	//m_localJump = false;
	m_localFire = false;

	// Indicate that the local player needs to be spawned.
	m_spawnLocalPlayer = true;
	m_requestedSpawnPoint = false;

	zoomedIn = false;
	zoomLevel = 0;
}

//-----------------------------------------------------------------------------
// Player manager class destructor.
//-----------------------------------------------------------------------------
PlayerManager::~PlayerManager()
{
	// Destroy the list of player objects.
	m_players->ClearPointers();
	SAFE_DELETE( m_players );
}

//-----------------------------------------------------------------------------
// Updates all of the players.
//-----------------------------------------------------------------------------
void PlayerManager::Update( float elapsed )
{
	// Ensure the scene is loaded.
	if( g_engine->GetSceneManager()->IsLoaded() == false )
		return;

	// Get a pointer to the local player.
	PlayerObject *localPlayer = m_players->GetFirst();
	if( localPlayer == NULL )
		return;

	// Check if the local player is dead.
	if( localPlayer->GetHealth() <= 0.0f && m_spawnLocalPlayer == false )
	{
		// Spawn the local player on a left mouse button press.
		if( g_engine->GetInput()->GetButtonPress( 0 ) == true )
			m_spawnLocalPlayer = true;

		return;
	}

	// If the local player needs to be spawned and a spawn point hasn't already
	// been requested, then request a spawn point from the host.
	if( m_spawnLocalPlayer == true && m_requestedSpawnPoint == false )
	{
		// Send a request spawn point message to the host.
		NetworkMessage rspm;
		rspm.msgid = MSGID_SPAWN_POINT_REQUEST;
		rspm.id = g_engine->GetNetwork()->GetID();
		g_engine->GetNetwork()->Send( &rspm, sizeof( NetworkMessage ) );

		// Indicate that a spawn point has been requested.
		m_requestedSpawnPoint = true;

		return;
	}
	else if( m_spawnLocalPlayer == true )
	{
		// The local player needs to be spawned, but a spawn point has already
		// been requested. So just return and wait for the request response.
		return;
	}

	// Calculate a delayed elapsed time, used for smoothing out view movement.
	static float delayedElapsed = 0.0f;
	delayedElapsed = delayedElapsed * 0.99f + elapsed * 0.01f;

	// Skip any further input if the local player is dying.
	if( localPlayer->GetDying() == true )
		return;

	// It is impossible to send network messages for every mouse movement,
	// therefore the player's looking direction will be updated directly.
	localPlayer->MouseLook( delayedElapsed, (float)g_engine->GetInput()->GetDeltaY() * 2, (float)g_engine->GetInput()->GetDeltaX() * 2 );

	// Send a periodic player look update to sync the other players.
	static unsigned long lookUpdate = timeGetTime();
	if( lookUpdate + 100 < timeGetTime() && localPlayer->GetEnabled() == true )
	{
		PlayerLookUpdateMsg plum;
		plum.msgid = MSGID_PLAYER_LOOK_UPDATE;
		plum.id = localPlayer->GetID();
		plum.viewTilt = localPlayer->GetViewTilt();
		plum.rotationY = localPlayer->GetRotation().y;
		g_engine->GetNetwork()->Send( &plum, sizeof( PlayerLookUpdateMsg ), false );

		lookUpdate = timeGetTime();
	}

	// Used for storing the local player's desired movement.
	float desiredDrive = 0.0f;
	float desiredStrafe = 0.0f;
	bool desiredFire = false;
	//bool desiredJump = false;

	// Check if the local player is trying to drive fowards or backwards.
	if( g_engine->GetInput()->GetKeyPress( DIK_W, true ) )
		desiredDrive = 1.0;
	else if( g_engine->GetInput()->GetKeyPress( DIK_S, true ) )
		desiredDrive = -1.0;

	if( localPlayer->GetFlyMode() == false )
	{
		static float jumpInterval = 0.0f;
		jumpInterval += elapsed;
		if( g_engine->GetInput()->GetKeyPress( DIK_SPACE ) == true && localPlayer->IsTouchingGround() == true )
			if( jumpInterval > 0.30f )
			{
				//desiredJump = true;
				
				NetworkMessage pjum;
				pjum.msgid = MSGID_PLAYER_JUMP_UPDATE;
				pjum.id = localPlayer->GetID();
				g_engine->GetNetwork()->Send( &pjum, sizeof( NetworkMessage ), false );

				localPlayer->SetJump( true );

				jumpInterval = 0.0f;
			}
	}

	// Check if the local player is trying to strafe left or right.
	if( g_engine->GetInput()->GetKeyPress( DIK_D, true ) )
		desiredStrafe = 1.0;
	else if( g_engine->GetInput()->GetKeyPress( DIK_A, true ) )
		desiredStrafe = -1.0;

	// Check if the local player is trying to fire their weapon.
	desiredFire = g_engine->GetInput()->GetButtonPress( 0, true );

	// Check if we need to drive the local player.
	if( m_localDrive != desiredDrive )
	{
		m_localDrive = desiredDrive;
		m_localMovement = true;
	}

	// Check if we need to strafe the local player.
	if( m_localStrafe != desiredStrafe )
	{
		m_localStrafe = desiredStrafe;
		m_localMovement = true;
	}

	/*
	// Check if we need to jump the local player.
	if( m_localJump != desiredJump )
	{
		m_localJump = desiredJump;
		m_localMovement = true;
	}
	*/

	// Check if we need to fire the local player's weapon.
	if(desiredFire == true && localPlayer->GetCurrentWeapon()->IsLoaded() == false)
	{
		// If player is trying to fire , but the ammo is empty, automatically try to change to another weapon
		char startIndex = localPlayer->GetCurrentWeaponIndex();
		char index = startIndex + 1;
		if( index > 9 )
		{
			index = 0;
		}
		do
		{
			if(localPlayer->GetWeapon(index) != NULL
				&& localPlayer->GetWeapon(index)->GetValid() == true
				&& localPlayer->GetWeapon(index)->IsLoaded() == true)
			{
				localPlayer->ChangeWeapon(0, index);
				break;
			}
			else
			{
				index++;
				if( index > 9 )
				{
					index = 0;
				}
			}
		}
		while(index != startIndex);
	}
	else if( m_localFire != desiredFire )
	{
		m_localFire = desiredFire;
		m_localMovement = true;
	}

	// If the local player's movement changed or the move update timer expires,
	// update the local player across the network. Since the network message is
	// sent using DPNSEND_NOLOOPBACK, the local player will need to update too.
	static unsigned long moveUpdate = timeGetTime();
	if( ( m_localMovement == true || moveUpdate + 200 < timeGetTime() ) && localPlayer->GetEnabled() == true )
	{
		PlayerMoveUpdateMsg pmum;
		pmum.msgid = MSGID_PLAYER_MOVE_UPDATE;
		pmum.id = localPlayer->GetID();
		pmum.translation = localPlayer->GetTranslation();
		pmum.drive = m_localDrive;
		pmum.strafe = m_localStrafe;
		//pmum.jump = m_localJump;
		pmum.fire = m_localFire;
		g_engine->GetNetwork()->Send( &pmum, sizeof( PlayerMoveUpdateMsg ), false );
		m_localMovement = false;

		localPlayer->SetDrive( m_localDrive );
		localPlayer->SetStrafe( m_localStrafe );
		//localPlayer->SetJump( m_localJump );
		localPlayer->SetFire( m_localFire );

		moveUpdate = timeGetTime();
	}

	// Check if the local player wants to change weapons with the scroll wheel.
	char changeWeapon = 0;
	if( g_engine->GetInput()->GetDeltaWheel() > 0 )
		changeWeapon = 1;
	else if( g_engine->GetInput()->GetDeltaWheel() < 0 )
		changeWeapon = -1;

	// Change the local player's weapon if the scroll wheel moved.
	if( changeWeapon != 0 )
	{
		if(zoomedIn == true)
		{
			D3DDISPLAYMODE *display;
			display = g_engine->GetRenderer()->GetDisplayMode();
			D3DXMATRIX projection;
			D3DXMatrixPerspectiveFovLH( &projection, D3DXToRadian( g_engine->GetFov() ), (float)display->Width / (float)display->Height, 0.01f / g_engine->GetScale(), g_engine->GetSceneManager()->GetMesh()->GetBoundingSphere()->radius * 2.0f );
			g_engine->GetRenderer()->GetDevice()->SetTransform( D3DTS_PROJECTION, &projection );

			zoomLevel = 0;
			zoomedIn = false;
		}

		localPlayer->ChangeWeapon( changeWeapon );
	}

	// Check if the local player is trying to change to a specific weapon.
	if( g_engine->GetInput()->GetKeyPress( DIK_1 ) == true )
		localPlayer->ChangeWeapon( 0, 0 );
	if( g_engine->GetInput()->GetKeyPress( DIK_2 ) == true )
		localPlayer->ChangeWeapon( 0, 1 );
	if( g_engine->GetInput()->GetKeyPress( DIK_3 ) == true )
		localPlayer->ChangeWeapon( 0, 2 );
	if( g_engine->GetInput()->GetKeyPress( DIK_4 ) == true )
		localPlayer->ChangeWeapon( 0, 3 );
	if( g_engine->GetInput()->GetKeyPress( DIK_5 ) == true )
		localPlayer->ChangeWeapon( 0, 4 );
	if( g_engine->GetInput()->GetKeyPress( DIK_6 ) == true )
		localPlayer->ChangeWeapon( 0, 5 );
	if( g_engine->GetInput()->GetKeyPress( DIK_7 ) == true )
		localPlayer->ChangeWeapon( 0, 6 );
	if( g_engine->GetInput()->GetKeyPress( DIK_8 ) == true )
		localPlayer->ChangeWeapon( 0, 7 );
	if( g_engine->GetInput()->GetKeyPress( DIK_9 ) == true )
		localPlayer->ChangeWeapon( 0, 8 );
	if( g_engine->GetInput()->GetKeyPress( DIK_0 ) == true )
		localPlayer->ChangeWeapon( 0, 9 );

	if( g_engine->GetInput()->GetButtonPress( 1, false ) == true )
	{
		SecondaryFire();
	}
}

//-----------------------------------------------------------------------------
// Spawns the local player using the given spawn point.
//-----------------------------------------------------------------------------
void PlayerManager::SpawnLocalPlayer( long spawnPoint )
{
	// Ensure the spawn point request was successful.
	if( spawnPoint == -1 )
	{
		// End the spawn point request so a new one can be attempted.
		m_requestedSpawnPoint = false;

		// Indicate that the local player needs to be spawned.
		m_spawnLocalPlayer = true;

		return;
	}

	// Send a message to spawn the local player at the given spawn point.
	SpawnPlayerMsg spm;
	spm.msgid = MSGID_SPAWN_PLAYER;
	spm.id = g_engine->GetNetwork()->GetID();
	spm.translation = g_engine->GetSceneManager()->GetSpawnPointByID( spawnPoint )->GetTranslation();

	g_engine->GetNetwork()->Send( &spm, sizeof( SpawnPlayerMsg ) );
}

//-----------------------------------------------------------------------------
// Spawns the given player at the given translation.
//-----------------------------------------------------------------------------
void PlayerManager::SpawnPlayer(unsigned int id, D3DXVECTOR3 translation )
{
	// Find the player to spawn.
	m_players->Iterate( true );
	while( m_players->Iterate() )
		if( m_players->GetCurrent()->GetID() == id )
			break;
	if( m_players->GetCurrent() == NULL )
		return;

	// Enable the player object.
	m_players->GetCurrent()->SetEnabled( true );
	m_players->GetCurrent()->SetVisible( true );

	// Restore the player's health.
	m_players->GetCurrent()->SetHealth( 100.0f );
	m_players->GetCurrent()->SetDying( false );

	// Play the idle animation twice to ensure that both animation tracks 
	// contain the idle animation data. This will prevent players from 
	// transitioning from the death animation when respawning. 
	m_players->GetCurrent()->PlayAnimation( 0, 0.0f ); 
	m_players->GetCurrent()->PlayAnimation( 0, 0.0f );

	// Set the player's translation and rotation.
	m_players->GetCurrent()->SetTranslation( translation );
	m_players->GetCurrent()->MouseLook( 0.0f, 0.0f, 0.0f, true );

	// Check if the local player was spawned.
	if( m_players->GetCurrent()->GetID() == g_engine->GetNetwork()->GetID() )
	{
		// Set local player as the viewing player.
		m_players->GetCurrent()->SetIsViewing( true );
		m_viewingPlayer = m_players->GetCurrent();

		// The request for a local player spawn point was successful.
		m_requestedSpawnPoint = false;

		// Indicate that the local player has been spawned.
		m_spawnLocalPlayer = false;
	}
	else
		m_players->GetCurrent()->SetIsViewing( false );
}

//-----------------------------------------------------------------------------
// Adds a new player.
//-----------------------------------------------------------------------------
PlayerObject *PlayerManager::AddPlayer( PlayerInfo *player )
{
	// Create the script for the player's character.
	char* temp = new char[strlen(player->model)+5];
	strcpy( temp, player->model);
	

	char* temp_dir = new char[strlen(player->model) + strlen(g_engine->GetEngineSetup()->characterPath) +2];
	strcpy( temp_dir, g_engine->GetEngineSetup()->characterPath );
	strcat( temp_dir, temp );
	strcat( temp_dir, "/" );
	
	strcat( temp, ".txt" );

	Script *script = new Script( temp, temp_dir );

	SAFE_DELETE_ARRAY(temp);
	SAFE_DELETE_ARRAY(temp_dir);

	// Create the player object.
	PlayerObject *object = m_players->Add( new PlayerObject( player, script ) );

//testing purposes - remove later
//	object->SetFlyMode(true);
//

	// Destroy the character script.
	SAFE_DELETE( script );

	return object;
}

//-----------------------------------------------------------------------------
// Removes the specified player.
//-----------------------------------------------------------------------------
void PlayerManager::RemovePlayer( unsigned int id )
{
	// Find the player to remove.
	m_players->Iterate( true );
	while( m_players->Iterate() )
		if( m_players->GetCurrent()->GetID() == id )
			break;
	if( m_players->GetCurrent() == NULL )
		return;

	// Remove the player object.
	PlayerObject *player = m_players->GetCurrent();
	m_players->Remove( &player );
}

//-----------------------------------------------------------------------------
// Returns the local player.
//-----------------------------------------------------------------------------
PlayerObject *PlayerManager::GetLocalPlayer()
{
	return m_players->GetFirst();
}

//-----------------------------------------------------------------------------
// Returns the specified player.
//-----------------------------------------------------------------------------
PlayerObject *PlayerManager::GetPlayer( unsigned int id )
{
	m_players->Iterate( true );
	while( m_players->Iterate() )
		if( m_players->GetCurrent()->GetID() == id )
			return m_players->GetCurrent();

	return NULL;
}

//-----------------------------------------------------------------------------
// Returns the next iterated player from the player list.
//-----------------------------------------------------------------------------
PlayerObject *PlayerManager::GetNextPlayer( bool restart )
{
	m_players->Iterate( restart );
	if( restart == true )
		m_players->Iterate();

	return m_players->GetCurrent();
}

//-----------------------------------------------------------------------------
// Returns the currently viewing player.
//-----------------------------------------------------------------------------
PlayerObject *PlayerManager::GetViewingPlayer()
{
	return m_viewingPlayer;
}

void PlayerManager::SecondaryFire()
{
	if(GetLocalPlayer()->GetCurrentWeaponIndex() == 2)
	{
		if(zoomedIn == false)
		{
			D3DDISPLAYMODE *display;
			display = g_engine->GetRenderer()->GetDisplayMode();
			D3DXMATRIX projection;
			D3DXMatrixPerspectiveFovLH( &projection, D3DXToRadian( g_engine->GetFov() ) / 5.0f , (float)display->Width / (float)display->Height, 0.01f / g_engine->GetScale(), g_engine->GetSceneManager()->GetMesh()->GetBoundingSphere()->radius * 2.0f );
			g_engine->GetRenderer()->GetDevice()->SetTransform( D3DTS_PROJECTION, &projection );

			zoomLevel = 1;
			zoomedIn = true;
		}
		else if(zoomedIn == true)
		{
			if(zoomLevel == 1)
			{
				D3DDISPLAYMODE *display;
				display = g_engine->GetRenderer()->GetDisplayMode();
				D3DXMATRIX projection;
				D3DXMatrixPerspectiveFovLH( &projection, D3DXToRadian( g_engine->GetFov() / 10.0f ), (float)display->Width / (float)display->Height, 0.01f / g_engine->GetScale(), g_engine->GetSceneManager()->GetMesh()->GetBoundingSphere()->radius * 2.0f );
				g_engine->GetRenderer()->GetDevice()->SetTransform( D3DTS_PROJECTION, &projection );

				zoomLevel = 2;
			}
			else if(zoomLevel == 2)
			{
				D3DDISPLAYMODE *display;
				display = g_engine->GetRenderer()->GetDisplayMode();
				D3DXMATRIX projection;
				D3DXMatrixPerspectiveFovLH( &projection, D3DXToRadian( g_engine->GetFov() / 15.0f ), (float)display->Width / (float)display->Height, 0.01f / g_engine->GetScale(), g_engine->GetSceneManager()->GetMesh()->GetBoundingSphere()->radius * 2.0f );
				g_engine->GetRenderer()->GetDevice()->SetTransform( D3DTS_PROJECTION, &projection );

				zoomLevel = 3;
			}
			else if(zoomLevel == 3)
			{
				D3DDISPLAYMODE *display;
				display = g_engine->GetRenderer()->GetDisplayMode();
				D3DXMATRIX projection;
				D3DXMatrixPerspectiveFovLH( &projection, D3DXToRadian( g_engine->GetFov() ), (float)display->Width / (float)display->Height, 0.01f / g_engine->GetScale(), g_engine->GetSceneManager()->GetMesh()->GetBoundingSphere()->radius * 2.0f );
				g_engine->GetRenderer()->GetDevice()->SetTransform( D3DTS_PROJECTION, &projection );

				zoomLevel = 0;
				zoomedIn = false;
			}
		}
	}
}

void PlayerManager::ZoomOut()
{
	D3DDISPLAYMODE *display;
	display = g_engine->GetRenderer()->GetDisplayMode();
	D3DXMATRIX projection;
	D3DXMatrixPerspectiveFovLH( &projection, D3DXToRadian( g_engine->GetFov() ), (float)display->Width / (float)display->Height, 0.01f / g_engine->GetScale(), g_engine->GetSceneManager()->GetMesh()->GetBoundingSphere()->radius * 2.0f );
	g_engine->GetRenderer()->GetDevice()->SetTransform( D3DTS_PROJECTION, &projection );

	zoomLevel = 0;
	zoomedIn = false;
}