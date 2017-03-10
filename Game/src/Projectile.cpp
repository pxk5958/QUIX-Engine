#include "Main.h"

Projectile::Projectile( D3DXVECTOR3 translation, D3DXVECTOR3 direction, float velocity )
{
	m_translation = translation;
	m_direction = direction;
	m_velocity = velocity;
}

Projectile::~Projectile()
{

}

void Projectile::Update(float elapsed)
{
	
}