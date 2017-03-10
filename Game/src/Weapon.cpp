#include "Main.h"

//-----------------------------------------------------------------------------
// The weapon class constructor.
//-----------------------------------------------------------------------------
Weapon::Weapon( Script *script, D3DXVECTOR3 viewWeaponOffset, char* playerModel )
{
	// Get the name of the weapon.
	m_name = new char[strlen( script->GetStringData( "name" ) ) + 1];
	strcpy( m_name, script->GetStringData( "name" ) );

	// Weapons are valid by default.
	m_valid = true;

	// Store the view weapon offset.
	m_viewWeaponOffset = viewWeaponOffset;

	// Clear the weapon movement smoothing variables.
	m_lastViewPoint = m_offsetViewPoint = D3DXVECTOR3(0.0f,0.0f,0.0f);

	// Do not use the view weapon by default.
	m_useViewWeapon = false;

	// Get all the details about the weapon's firing capabilities.
	m_fireTimer = m_rof = *script->GetFloatData( "rate_of_fire" );
	m_velocity = *script->GetFloatData( "muzzle_velocity" ) / g_engine->GetScale();
	m_damage = *script->GetFloatData( "max_damage" );
	m_range = *script->GetFloatData( "range" ) / g_engine->GetScale();
	m_recoilTimer = *script->GetFloatData( "recoil_time" );
	m_recoil = *script->GetFloatData( "recoil_factor" ) / g_engine->GetScale();
	m_ammo = m_ammoDefault = (unsigned int)*script->GetNumberData( "default_ammo" );

	// Create an object for the weapon on the player's body.
	m_bodyWeapon = new SceneObject( TYPE_SCENE_OBJECT, script->GetStringData( "mesh" ), script->GetStringData( "mesh_path" ) );

	// Create an object for the hands in the player's view.
	char* temp = new char[strlen(playerModel)+5];
	strcpy( temp, playerModel);
	char* temp_dir = new char[strlen(playerModel) + strlen(g_engine->GetEngineSetup()->characterPath) +2];
	strcpy( temp_dir, g_engine->GetEngineSetup()->characterPath );
	strcat( temp_dir, temp );
	strcat( temp_dir, "/" );
	strcat( temp, ".txt" );
	Script *playerScript = new Script( temp, temp_dir );
	m_viewWeapon = new AnimatedObject( playerScript->GetStringData( "mesh_hands" ), playerScript->GetStringData( "mesh_hands_path" ) );
	SAFE_DELETE( playerScript );
	SAFE_DELETE_ARRAY(temp);
	SAFE_DELETE_ARRAY(temp_dir);

	// Create the list of shot flashes for this weapon.
	m_flashes = new LinkedList< Material >;
	char flash[16] = { "flash0" };
	while( script->GetStringData( flash ) != NULL )
	{
		m_flashes->Add( new Material( script->GetStringData( flash ), script->GetStringData( "flash_path" ) ) );
		sprintf( flash, "flash%d", m_flashes->GetTotalElements() );
	}
	m_displayFlash = false;

	// Load the shot sound and create an audio path for it.
	m_shotSound = new Sound( script->GetStringData( "sound" ) );
	m_shotSound->SetMinAttDist( 7.0f / g_engine->GetScale() );
	m_shotAudioPath = new AudioPath3D;
}

//-----------------------------------------------------------------------------
// The weapon class destructor.
//-----------------------------------------------------------------------------
Weapon::~Weapon()
{
	// Destroy the string buffer containing the name of the weapon.
	SAFE_DELETE_ARRAY( m_name );

	// Destroy the weapon objects.
	SAFE_DELETE( m_bodyWeapon );
	SAFE_DELETE( m_viewWeapon );

	// Destroy the list of shot flashes.
	SAFE_DELETE( m_flashes );

	// Destroy the shot sound and its audio path.
	SAFE_DELETE( m_shotSound );
	SAFE_DELETE( m_shotAudioPath );
}

//-----------------------------------------------------------------------------
// Updates the weapon.
//-----------------------------------------------------------------------------
void Weapon::Update( float elapsed, bool fire, SceneObject *parent, D3DXVECTOR3 viewPoint )
{
	static float move = 0.0f;
	
	// Update the fire timer.
	m_fireTimer += elapsed;

	// Update weapon, using either the 3rd person or the 1st person object.
	if( m_useViewWeapon == false )
		m_bodyWeapon->Update( elapsed, false );
	else
	{
		// Calculate the offset in model space.
		D3DXVECTOR3 offset;

			D3DXMATRIX rotationXMatrix;
			D3DXMatrixRotationX( &rotationXMatrix, ( (PlayerObject*)parent )->GetViewTilt() );
			D3DXMATRIX combinedRotation;
			D3DXMatrixMultiply( &combinedRotation, &rotationXMatrix, parent->GetRotationMatrix() );
			D3DXVec3TransformCoord( &offset, &m_viewWeaponOffset, &combinedRotation );
		//D3DXVec3TransformCoord( &offset, &m_viewWeaponOffset, parent->GetRotationMatrix() );

		// If this is the first time through, then set the view point y value.
		if( D3DXVec3Length(&m_lastViewPoint) == 0.0f )
			m_lastViewPoint = viewPoint;

		// When the player's mesh animates, the head moves, which causes the
		// view point to move. Take this movement into account so that the
		// weapon does not fly all over the place on the screen.
		m_offsetViewPoint.y += ( viewPoint.y - m_lastViewPoint.y ) * 0.35f;  // 0.95f
		m_offsetViewPoint.x += ( viewPoint.x - m_lastViewPoint.x ) * 0.25f;  // 0.85f
		m_offsetViewPoint.z += ( viewPoint.z - m_lastViewPoint.z ) * 0.25f;  // 0.85f
		offset += m_offsetViewPoint;
		m_lastViewPoint = viewPoint;


		// Check if the weapon is recoiling (local player only).
		if( m_fireTimer > (0.5f * m_recoilTimer) && m_fireTimer <= m_recoilTimer)
		{
			// Bring back weapon to front.
			move += m_recoil * elapsed * exp(1 - 0.5f * m_recoil * elapsed);
		}
		else if( m_fireTimer > 0.0f && m_fireTimer <= (0.5f * m_recoilTimer) )
		{
			// Kick back weapon.
			move -= m_recoil * elapsed * exp(1 - 0.5f * m_recoil * elapsed);
		}
		else
		{
			move = 0.0f;
		}

		// Recoil
		offset += parent->GetForwardVector() * move /*- up * move * 0.5f*/;
		

		// Position the weapon correctly, then update it.
		//m_viewWeapon->SetTranslation( parent->GetTranslation() + offset );
		m_viewWeapon->SetTranslation( parent->GetTranslation() + viewPoint + offset );
		m_viewWeapon->SetRotation( parent->GetRotation() );
		m_viewWeapon->AddRotation( ( (PlayerObject*)parent )->GetViewTilt() + 0.005f * move, 0.0f, 0.0f );


/*
//
			//get model space attach point (rp_weapon_attach) of player
			//transform and find attach point, by combining rp_weapon_attach's final transformation matrix and player's world transformation matrix and use it on
			m_bodyWeapon->SetTranslation( m_viewWeapon->GetMesh()->GetReferencePoint( "rp_weapon_attach" )->GetTranslation()
											+ m_viewWeapon->GetTranslation());
			m_bodyWeapon->SetRotation( m_viewWeapon->GetRotation() );
//			
*/


		m_viewWeapon->Update( elapsed, false );

/*
//
			m_bodyWeapon->Update( elapsed, false );
//
*/

		// Randomly recoil player's camera
		//( (PlayerObject*)parent )->AddRotation(0.0f, -(0.00015f * move) + (float)rand()/((float)RAND_MAX/(0.0003f * move)), 0.0f);
		//( (PlayerObject*)parent )->SetViewTilt( ( (PlayerObject*)parent )->GetViewTilt() + 0.00008f * move );
	}
	
	// Ensure the weapon is firing and is loaded.
	if( fire == false || IsLoaded() == false )
		return;

	// Check if it is time to fire another shot.
	if( m_fireTimer > m_rof )
	{
		// Indicate that a flash needs to be displayed this frame.
		m_displayFlash = true;

		// Check if the view weapon is being used (i.e. 1st person view).
		if( m_useViewWeapon == true )
		{
			// Calculate the muzzel point of the weapon in world space
			// using the 1st person view weapon object.
			m_muzzelPoint = m_bodyWeapon->GetMesh()->GetReferencePoint( "rp_sight" )->GetTranslation();

			Frame *attach = m_viewWeapon->GetMesh()->GetReferencePoint( "rp_weapon_attach" );
			D3DXMATRIX transform;
			D3DXMatrixMultiply( &transform, &attach->finalTransformationMatrix, m_viewWeapon->GetWorldMatrix() );

			D3DXVec3TransformCoord( &m_muzzelPoint, &m_muzzelPoint, &transform );

			



			D3DXVECTOR3 temp = m_bodyWeapon->GetMesh()->GetReferencePoint( "rp_sight" )->GetTranslation();
			D3DXVec3TransformCoord( &temp, &temp, &m_viewWeapon->GetMesh()->GetReferencePoint( "rp_weapon_attach" )->finalTransformationMatrix );
			temp +=  m_viewWeaponOffset;
			

			m_shotAudioPath->SetPosition( Vector3(temp.x, temp.y, temp.z) );
			m_shotAudioPath->SetVelocity( Vector3(parent->GetVelocity().x, parent->GetVelocity().y, parent->GetVelocity().z) );

			// Play the shot sound.
			m_shotAudioPath->Play( m_shotSound->GetSound(), false, SOUND_RELATIVE );

		}
		else
		{
			// Calculate the muzzel point of the weapon in world space
			// using the 3rd person view weapon object.
			m_muzzelPoint = m_bodyWeapon->GetMesh()->GetReferencePoint( "rp_sight" )->GetTranslation();

			Frame *attach = ( (PlayerObject*)parent )->GetMesh()->GetReferencePoint( "rp_weapon_attach" );
			D3DXMATRIX transform;
			D3DXMatrixMultiply( &transform, &attach->finalTransformationMatrix, parent->GetWorldMatrix() );

			D3DXVec3TransformCoord( &m_muzzelPoint, &m_muzzelPoint, &transform );

			// Update the shot sound's audio path.
			m_shotAudioPath->SetPosition( Vector3( m_muzzelPoint.x, m_muzzelPoint.y, m_muzzelPoint.z ) );
			m_shotAudioPath->SetVelocity( Vector3( parent->GetVelocity().x, parent->GetVelocity().y, parent->GetVelocity().z ) );

			// Play the shot sound.
			m_shotAudioPath->Play( m_shotSound->GetSound(), false, SOUND_ABSOLUTE );
		}

		// Add the new bullet to the bullet manager.
		g_game->GetBulletManager()->AddBullet( parent, ( (PlayerObject*)parent )->GetEyePoint(), ( (PlayerObject*)parent )->GetForwardVector(), m_velocity, m_range, m_damage );
		
		// Push the player back due to the force. Ignore y-axis value
		//( (PlayerObject*)parent )->ForceAlongDir(0.2f * m_recoil, D3DXVECTOR3( -parent->GetForwardVector().x, 0.0f, -parent->GetForwardVector().z) );
		
		// Decrement Ammo
		m_ammo--;

		// Restart the timer.
		m_fireTimer = 0.0f;
	}
}

//-----------------------------------------------------------------------------
// Renders the weapon.
//-----------------------------------------------------------------------------
void Weapon::Render( SceneObject *parent )
{
	// Render the weapon using the correct weapon object.
	if( m_useViewWeapon == true )
	{
		m_viewWeapon->Render();

		Frame *attach = m_viewWeapon->GetMesh()->GetReferencePoint( "rp_weapon_attach" );
		D3DXMATRIX transform;
		D3DXMatrixMultiply( &transform, &attach->finalTransformationMatrix, m_viewWeapon->GetWorldMatrix() );

		m_bodyWeapon->Render( &transform );
	}
	else
	{
		// Get the translation (world space) to render the 3rd person weapon.
		Frame *attach = ( (PlayerObject*)parent )->GetMesh()->GetReferencePoint( "rp_weapon_attach" );
		D3DXMATRIX transform;
		D3DXMatrixMultiply( &transform, &attach->finalTransformationMatrix, parent->GetWorldMatrix() );

		m_bodyWeapon->Render( &transform );
	}

	// Check if a flash needs to be displayed.
	if( m_displayFlash == true )
	{
		// Get a random flash material.
		Material *material = m_flashes->GetRandom();

		// Use the sprite interface to render the flash at the muzzel point.
		D3DXMATRIX view, world;
		D3DXMatrixIdentity( &world );
		g_engine->GetRenderer()->GetDevice()->GetTransform( D3DTS_VIEW, &view );
		g_engine->GetRenderer()->GetSpriteHandler()->SetWorldViewLH( &world, &view );
		g_engine->GetRenderer()->GetSpriteHandler()->Begin( D3DXSPRITE_BILLBOARD | D3DXSPRITE_ALPHABLEND );
		g_engine->GetRenderer()->GetSpriteHandler()->Draw( material->GetTexture(), NULL, &D3DXVECTOR3( material->GetWidth() / 2.0f, material->GetHeight() / 2.0f, 0.0f ), &m_muzzelPoint, 0xFFFFFFFF );
		g_engine->GetRenderer()->GetSpriteHandler()->End();

		// Indicate that the flash has been rendered this frame.
		m_displayFlash = false;
	}
}

//-----------------------------------------------------------------------------
// Raises/Lowers the weapon.
//-----------------------------------------------------------------------------
void Weapon::RaiseLowerWeapon( float elapsed, SceneObject *parent, float move )
{
	// Get the weapon's offset in model space.
	D3DXVECTOR3 offset;
			D3DXMATRIX rotationXMatrix;
			D3DXMatrixRotationX( &rotationXMatrix, ( (PlayerObject*)parent )->GetViewTilt() );
			D3DXMATRIX combinedRotation;
			D3DXMatrixMultiply( &combinedRotation, &rotationXMatrix, parent->GetRotationMatrix() );
			D3DXVec3TransformCoord( &offset, &m_viewWeaponOffset, &combinedRotation );
	//D3DXVec3TransformCoord( &offset, &m_viewWeaponOffset, parent->GetRotationMatrix() );

	// Get the parent's up vector.
	D3DXVECTOR3 up;
	D3DXVec3Cross( &up, &parent->GetForwardVector(), &parent->GetRightVector() );

	// Move the weapon (either up or down).
	offset += up * move;
	offset += ( -(parent->GetRightVector()) ) * move;

	// Apply the offset to the weapon's translation and update the object.
	m_viewWeapon->SetTranslation( parent->GetTranslation() + m_lastViewPoint + offset );
	m_viewWeapon->SetRotation( parent->GetRotation() );
	m_viewWeapon->AddRotation( ( (PlayerObject*)parent )->GetViewTilt(), 0.0f, 0.0f );
	m_viewWeapon->Update( elapsed, false );
}

//-----------------------------------------------------------------------------
// Set the weapon to use either the 1st person or the 3rd person weapon object.
//-----------------------------------------------------------------------------
void Weapon::UseViewWeapon( bool use )
{
	m_useViewWeapon = use;
}

//-----------------------------------------------------------------------------
// Get the name of the weapon.
//-----------------------------------------------------------------------------
char *Weapon::GetName()
{
	return m_name;
}

//-----------------------------------------------------------------------------
// Set the weapon's valid flag.
//-----------------------------------------------------------------------------
void Weapon::SetValid( bool valid )
{
	m_valid = valid;
}

//-----------------------------------------------------------------------------
// Get the weapon's valid flag.
//-----------------------------------------------------------------------------
bool Weapon::GetValid()
{
	return m_valid;
}



unsigned int Weapon::GetDefaultAmmo()
{
	return m_ammoDefault;
}

void Weapon::ResetAmmo()
{
	m_ammo = m_ammoDefault;
}

unsigned int Weapon::GetAmmo()
{
	return m_ammo;
}

void Weapon::FillAmmo(unsigned int ammo)
{
	m_ammo += ammo;

	if(m_ammo > 999)
		m_ammo = 999;
}

void Weapon::SetAmmo(unsigned int ammo)
{
	m_ammo = ammo;

	if(m_ammo > 999)
		m_ammo = 999;
}

bool Weapon::IsLoaded()
{
	if(m_ammo > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}