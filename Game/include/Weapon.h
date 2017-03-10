#ifndef WEAPON_H
#define WEAPON_H

//-----------------------------------------------------------------------------
// Weapon Class
//-----------------------------------------------------------------------------
class Weapon
{
public:
	Weapon( Script *script, D3DXVECTOR3 viewWeaponOffset, char* playerModel );
	virtual ~Weapon();

	void Update( float elapsed, bool fire, SceneObject *parent, D3DXVECTOR3 viewPoint );
	void Render( SceneObject *parent );

	void RaiseLowerWeapon( float elapsed, SceneObject *parent, float move );

	void UseViewWeapon( bool use );

	char *GetName();

	void SetValid( bool valid );
	bool GetValid();

	unsigned int GetDefaultAmmo();
	void ResetAmmo();
	unsigned int GetAmmo();
	void SetAmmo(unsigned int ammo);
	void FillAmmo(unsigned int ammo);
	bool IsLoaded();

private:
	char *m_name; // Name of the weapon.
	bool m_valid; // Indicates if the weapon is valid.
	D3DXVECTOR3 m_viewWeaponOffset; // Offset to display the weapon at in the 1st person view.

	D3DXVECTOR3 m_lastViewPoint; // Stores the latest view point value passed to the Update() function.
	D3DXVECTOR3 m_offsetViewPoint; // Maintains the offset for smoothing out 1st person weapon movement during animation.

	float m_rof; // Weapon's rate of fire.
	float m_velocity; // Velcotiy that the weapon fires its bullets at.
	float m_damage; // Damage caused by this weapon's bullets.
	float m_range; // Range the weapon can fire a bullet.
	float m_recoilTimer; // Recoil timer
	float m_recoil; // Recoil constant of weapon.
	unsigned int m_ammoDefault;
	unsigned int m_ammo;
	

	float m_fireTimer; // Timer used for firing the weapon.
	D3DXVECTOR3 m_muzzelPoint; // Point at the end of the weapon's barrel in world space.

	bool m_useViewWeapon; // Indicates if the weapon should use the 1st person view mesh.
	SceneObject *m_bodyWeapon; // Object representing the 3rd person weapon.
	SceneObject *m_viewWeapon; // Object representing the 1st person weapon.

	bool m_displayFlash; // Indicates if a flash sould be displayed in the current frame.
	LinkedList< Material > *m_flashes; // Linked list of flashes used by this weapon.

	Sound *m_shotSound; // Shot sound made by this weapon.
	AudioPath3D *m_shotAudioPath; // Audio path used for playing the shot sound.
};

#endif