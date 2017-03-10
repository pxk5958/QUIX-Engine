#ifndef PROJECTILE_H
#define PROJECTILE_H

//-----------------------------------------------------------------------------
// Bullet Class
//-----------------------------------------------------------------------------
class Projectile
{
public:
	Projectile( D3DXVECTOR3 translation, D3DXVECTOR3 direction, float velocity );
	virtual ~Projectile();

	void Update( float elapsed );

	//bool IsExpired();

private:
	//float m_totalDistance; // Total distance covered by the projectile.
	//bool m_expired; // Indicates if the projectile has expired yet.

	D3DXVECTOR3 m_translation; // Current translation of the projectile.
	D3DXVECTOR3 m_direction; // Direction the projectile is travelling.
	float m_velocity; // Velocity of the projectile.
	//float m_range; // How far the projectile can travel.
	//float m_damage; // How much damage the projectile does.
};

#endif