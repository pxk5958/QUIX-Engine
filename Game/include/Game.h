#ifndef GAME_H
#define GAME_H

//-----------------------------------------------------------------------------
// State ID Define
//-----------------------------------------------------------------------------
#define STATE_GAME 1

//-----------------------------------------------------------------------------
// Enumerated Spawn Object Types
//-----------------------------------------------------------------------------
enum{ WEAPON_SPAWN_OBJECT, AMMO_SPAWN_OBJECT, HEALTH_SPAWN_OBJECT };

//-----------------------------------------------------------------------------
// Custom Network Message Defines
//-----------------------------------------------------------------------------
#define MSGID_PLAYER_HEALTH          0x12006
#define MSGID_PLAYER_MOVE_UPDATE     0x12007
#define MSGID_PLAYER_LOOK_UPDATE     0x12008
#define MSGID_PLAYER_JUMP_UPDATE	 0x12009
#define MSGID_PLAYER_SCORE           0x12010
#define MSGID_PLAYER_WEAPON_CHANGE   0x12011
#define MSGID_PLAYER_WEAPON_CHANGING 0x12012
#define MSGID_SPAWN_POINT_REQUEST    0x12013
#define MSGID_SPAWN_POINT            0x12014
#define MSGID_SPAWN_PLAYER           0x12015

//-----------------------------------------------------------------------------
// Player Health Message Structure
//-----------------------------------------------------------------------------
struct PlayerHealthMsg : public NetworkMessage
{
	float health; // Absolute health of the player.
	unsigned int attackerid;
};

//-----------------------------------------------------------------------------
// Player Move Message Structure
//-----------------------------------------------------------------------------
struct PlayerMoveUpdateMsg : public NetworkMessage
{
	D3DXVECTOR3 translation; // Player's translation.
	float drive; // Player's drive direction.
	float strafe; // Player's strafe direction.
	//bool jump;
	bool fire; // Indicate's if the player is firing their weapon.
};

//-----------------------------------------------------------------------------
// Player Look Update Message Structure
//-----------------------------------------------------------------------------
struct PlayerLookUpdateMsg : public NetworkMessage
{
	float viewTilt; // Player's view tilt (i.e. rotation around the x axis).
	float rotationY; // Player's rotation around the y axis.
};

//-----------------------------------------------------------------------------
// Player Score Message Structure
//-----------------------------------------------------------------------------
struct PlayerScoreMsg : public NetworkMessage
{
	unsigned long frags; // Player's frag count.
	unsigned long deaths; // Player's death tally.
	unsigned int attackerid; // If the player has been fragged, who attacked him?
	unsigned int fragid; // If the player fragged someone, whom did he kill?
	bool waskilled; // Was the player killed or did the player kill someone else?
};

//-----------------------------------------------------------------------------
// Player Weapon Change Message Structure
//-----------------------------------------------------------------------------
struct PlayerWeaponChangeMsg : public NetworkMessage
{
	char weapon; // Weapon that the player has changed to.
};

//-----------------------------------------------------------------------------
// Spawn Point Message Structure
//-----------------------------------------------------------------------------
struct SpawnPointMsg : public NetworkMessage
{
	long spawnPoint; // ID of the spawn point to use.
};

//-----------------------------------------------------------------------------
// Spawn Point Message Structure
//-----------------------------------------------------------------------------
struct SpawnPlayerMsg : public NetworkMessage
{
	D3DXVECTOR3 translation; // Translation to spawn the player at.
};

//-----------------------------------------------------------------------------
// Game Class
//-----------------------------------------------------------------------------
class Game : public State
{
public:
	Game();

	virtual void Load();
	virtual void Close();

	virtual void RequestViewer( ViewerSetup *viewer );
	virtual void Update( float elapsed );
	virtual void Render();
	virtual void Render2D();

	BulletManager *GetBulletManager();

	void HandleNetworkMessage( ReceivedMessage *msg );

private:

	char m_scoreBoardNames[MAX_PATH]; // Text for displaying the names of all the connected players.
	char m_scoreBoardFrags[MAX_PATH]; // Text for displaying each player's frag count.
	char m_scoreBoardDeaths[MAX_PATH]; // Text for displaying each player's death tally.
	Font *m_scoreBoardFont; // Font used to render the score board.

	BulletManager *m_bulletManager; // Bullet manager.
	PlayerManager *m_playerManager; // Player manager.

	Sound *m_music; // The in-game music sound.

	char m_fpsText[16]; // Frame rate character string.
	Font *m_fpsFont; // Font for rendering the frame rate.
	char m_healthText[4];
	char m_ammoText[4];
	Font *m_healthAmmoFont; // Font for rendering Health and Ammo

	char m_updateText[75];
	float m_updateTimer;
	Font *m_updateFont; // Font for rendering updates

	Sprite* m_crosshair;
	Sprite* m_scope;
	Sprite* m_healthHUD;
	Sprite* m_ammoHUD;
	Sprite* m_healthDamageHUD;
	Sprite* m_ammoEmptyHUD;
	Sprite* m_scoreBoard;
};

//-----------------------------------------------------------------------------
// Externals
//-----------------------------------------------------------------------------
extern Game *g_game;

#endif