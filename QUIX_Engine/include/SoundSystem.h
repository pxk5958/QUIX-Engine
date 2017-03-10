#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H

#include "fmod.hpp"

#define SOUND_2D FMOD_2D
#define SOUND_3D FMOD_3D

#define SOUND_ABSOLUTE FMOD_3D_WORLDRELATIVE
#define SOUND_RELATIVE FMOD_3D_HEADRELATIVE

class SoundSystem
{
public:
	SoundSystem( float scale = 1.0f );
	virtual ~SoundSystem();

	void UpdateListener( Vector3 position, Vector3 velocity, Vector3 forward, Vector3 up );
	void Update();

	//void GarbageCollection();

	void SetVolume( float volume );

	FMOD::System *GetSystem() { return m_system; }

	//float GetScale() { return m_scale; }
	
private:
	float m_scale; // Unit scale in meters/unit.
	FMOD::System *m_system;	
};

// A sound created with SOUND_2D should never be attached to an AudioPath3D later
class Sound
{
public:
	Sound( char* name, char* path = "./", FMOD_MODE mode = SOUND_3D );
	virtual ~Sound();

	void SetMinAttDist( float minAttDistInUnits );

	void Play( bool loop = false );
	void Pause();
	void Resume();
	void Stop();
	void SetVolume( float volume );

	FMOD::Sound *GetSound();
	
private:
	FMOD_MODE m_mode;
	FMOD::Sound *m_sound;
	FMOD::Channel *m_channel;
};


class AudioPath3D
{
public:
	AudioPath3D();
	virtual ~AudioPath3D();

	void SetPosition( Vector3 position );
	void SetVelocity( Vector3 velocity );

	void Play( FMOD::Sound *sound, bool loop = false, FMOD_MODE mode = SOUND_ABSOLUTE );
	void Pause();
	void Resume();
	void Stop();
	void SetVolume( float volume );

private:
	FMOD::Channel *m_channel;
	FMOD_VECTOR m_pos;
	FMOD_VECTOR m_vel;
};

#endif