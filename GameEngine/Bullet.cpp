#include "stdafx.h"
#include "Bullet.h"


Bullet::Bullet(BitmapClass* bitmap, D3DXVECTOR2 position, int depth, float scale, D3DXVECTOR2 velocity, bool enabled, float lifeTime)
	: Entity(bitmap, position, depth, scale, velocity, enabled)
{
	m_lifeTime = lifeTime;
}

Bullet::~Bullet(void)
{
}

Bullet* Bullet::Copy(bool enabled)
{
	Bullet* copy = new Bullet(m_bitmap, m_position, m_depth, m_scale, m_velocity, enabled, m_lifeTime);
	copy->SetMaxSpeed(m_maxSpeed);

	return copy;
}

bool Bullet::Frame(float dTime)
{
	if (!Entity::Frame(dTime)) return true;

	// Translate from speed
	D3DXVECTOR2 translation;
	D3DXVec2Scale(&translation, &m_velocity, dTime);
	Translate(translation);

	// Track time spent alive
	m_lifeTime -= dTime;

	return true;
}

void Bullet::SetDirection(D3DXVECTOR2 direction)
{
	D3DXVec2Normalize(&direction, &direction);
	D3DXVec2Scale(&m_velocity, &direction, m_maxSpeed);
}

bool Bullet::IsDead()
{
	return (m_lifeTime < 0.0f);
}