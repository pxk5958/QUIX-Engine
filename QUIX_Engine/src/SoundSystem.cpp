#include "QUIX.h"


SoundSystem::SoundSystem( float scale )
{
	m_system = NULL;
	
	FMOD_RESULT result;
	
	result = FMOD::System_Create(&m_system);		// Create the main system object.
	if (result != FMOD_OK)
	{
		MessageBox(0, _T("FMOD error!"), _T("System_Create"),0);
	}

	result = m_system->init(100, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.
	if (result != FMOD_OK)
	{
		MessageBox(0, _T("FMOD error!"), _T("System_init"),0);
	}

	m_scale = scale;

	m_system->set3DSettings( 1.0f, (float)(1 / m_scale), 1.5f );
}

SoundSystem::~SoundSystem()
{
	m_system->release();
}

void SoundSystem::UpdateListener( Vector3 position, Vector3 velocity, Vector3 forward, Vector3 up )
{
	//position *= m_scale;
	//velocity *= m_scale;
	
	FMOD_VECTOR pos = { position.getX(), position.getY(), position.getZ() };
	FMOD_VECTOR vel = { velocity.getX(), velocity.getY(), velocity.getZ() };
	FMOD_VECTOR f = { forward.getX(), forward.getY(), forward.getZ() };
	FMOD_VECTOR u = { up.getX(), up.getY(), up.getZ() };
	
	m_system->set3DListenerAttributes(0, &pos, &vel, &f, &u);
}

void SoundSystem::Update()
{
	m_system->update();
}

/*
void SoundSystem::GarbageCollection()
{
	m_loader->CollectGarbage();
}
*/

void SoundSystem::SetVolume( float volume )
{
	FMOD::ChannelGroup *group;
	m_system->getMasterChannelGroup(&group);
	group->setVolume( volume );
}


Sound::Sound( char* name, char* path, FMOD_MODE mode )
{
	m_sound = NULL;
	m_channel = NULL;
	
	FMOD_RESULT result;

	char* temp_filename = new char[strlen( name ) + strlen( path ) + 1];
	sprintf( temp_filename, ("%s%s"), path, name );

	m_mode = mode;

	if( mode == SOUND_2D )
		result = g_engine->GetSoundSystem()->GetSystem()->createSound(temp_filename, (FMOD_MODE)(FMOD_2D), 0, &m_sound);
	else
		result = g_engine->GetSoundSystem()->GetSystem()->createSound(temp_filename, (FMOD_MODE)(FMOD_3D /*| FMOD_3D_LINEARROLLOFF*/), 0, &m_sound);

	if (result != FMOD_OK)
	{
		MessageBox(0, _T("FMOD error!"), _T("Sound_create"),0);
	}

	SAFE_DELETE_ARRAY(temp_filename);
}


Sound::~Sound()
{
	m_channel->stop();

	FMOD_RESULT result = m_sound->release();
	if (result != FMOD_OK)
	{
		MessageBox(0, _T("FMOD error!"), _T("Sound_destroy"),0);
	}

	m_sound = NULL;
	m_channel = NULL;
}

void Sound::SetMinAttDist( float minAttDistInUnits )
{
	if( m_mode == SOUND_2D )
		return;

	m_sound->set3DMinMaxDistance( minAttDistInUnits, (10000.0f / g_engine->GetScale()) );
}

void Sound::Play( bool loop )
{
	FMOD_RESULT result;
	
	result = g_engine->GetSoundSystem()->GetSystem()->playSound(FMOD_CHANNEL_FREE, m_sound, true, &m_channel);

	if (result != FMOD_OK)
	{
		MessageBox(0, _T("FMOD error!"), _T("Sound_play"),0);
	}

	if( m_mode == SOUND_3D )
		m_channel->setMode(SOUND_RELATIVE);

	if( loop == true )
		m_channel->setMode(FMOD_LOOP_NORMAL);
	
	m_channel->setPaused(false);	
}

void Sound::Pause()
{
	m_channel->setPaused(true);
}

void Sound::Resume()
{
	m_channel->setPaused(false);
}

void Sound::Stop()
{
	m_channel->stop();
}

void Sound::SetVolume( float volume )
{
	m_channel->setVolume( volume );
}


FMOD::Sound *Sound::GetSound()
{
	return m_sound;
}


AudioPath3D::AudioPath3D()
{
	m_channel = NULL;
	FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	m_pos = pos;
	m_vel = vel;
}

AudioPath3D::~AudioPath3D()
{
	m_channel = NULL;
}

void AudioPath3D::SetPosition( Vector3 position )
{
	//position *= g_engine->GetScale();
	FMOD_VECTOR pos = { position.getX(), position.getY(), position.getZ() };
	m_pos = pos;
}

void AudioPath3D::SetVelocity( Vector3 velocity )
{
	//velocity *= g_engine->GetScale();
	FMOD_VECTOR vel = { velocity.getX(), velocity.getY(), velocity.getZ() };
	m_vel = vel;
}

void AudioPath3D::Play( FMOD::Sound *sound, bool loop, FMOD_MODE mode )
{
	FMOD_RESULT result;
	
	result = g_engine->GetSoundSystem()->GetSystem()->playSound(FMOD_CHANNEL_FREE, sound, true, &m_channel);

	if (result != FMOD_OK)
	{
		MessageBox(0, _T("FMOD error!"), _T("Sound_play"),0);
	}

	m_channel->setMode(mode);

	if( loop == true )
		m_channel->setMode(FMOD_LOOP_NORMAL);

	m_channel->set3DAttributes(&m_pos, &m_vel);
	
	m_channel->setPaused(false);
}

void AudioPath3D::Pause()
{
	m_channel->setPaused(true);
}

void AudioPath3D::Resume()
{
	m_channel->setPaused(false);
}

void AudioPath3D::Stop()
{
	m_channel->stop();;
}

void AudioPath3D::SetVolume( float volume )
{
	m_channel->setVolume( volume );
}