// Cyberbot.cpp
// Written by Blake Zoeckler

#include "stdafx.h"
#include "Cyberbot.h"
#include "BulletWeapon.h"
#include "BeamWeapon.h"
#include "Bullet.h"
#include "Weapon.h"

Cyberbot::Cyberbot(BitmapClass* bitmap, D3DXVECTOR2 position, int depth, float scale, D3DXVECTOR2 velocity, bool enabled)
	: Entity(bitmap, position, depth, scale, velocity, enabled)
{
	m_hp = 3;	
	m_weapon = 0;
}


Cyberbot::~Cyberbot(void)
{
}

void Cyberbot::Shutdown()
{
	Entity::Shutdown();
}


bool Cyberbot::Frame(float dTime)
{
	if (!Entity::Frame(dTime)) return true;

	D3DXVECTOR2 acceleration;
	D3DXVECTOR2 translation;
	
	if (D3DXVec2Length(&m_force) > 0)
	{
		// Increase speed based on force	
		D3DXVec2Scale(&acceleration, &m_force, 4.0f*dTime);
		D3DXVec2Add(&m_velocity, &acceleration, &m_velocity);

		// Clamp speed
		if (D3DXVec2Length(&m_velocity) > m_maxSpeed)
		{
			D3DXVec2Normalize(&m_velocity, &m_velocity);
			D3DXVec2Scale(&m_velocity, &m_velocity, m_maxSpeed);
		}
	}
	else
	{
		// Brake
		float brakeSpeed = m_maxSpeed * 4.0f * dTime;
		float newSpeed = D3DXVec2Length(&m_velocity) - brakeSpeed;
		if (newSpeed <= 0)
		{
			D3DXVec2Scale(&m_velocity, &m_velocity, 0.0f);
		}
		else
		{
			D3DXVec2Normalize(&m_velocity, &m_velocity);
			D3DXVec2Scale(&m_velocity, &m_velocity, newSpeed);
		}
	}

	// Translate from speed
	D3DXVec2Scale(&translation, &m_velocity, dTime);
	Translate(translation);

	// Update weapon recharge
	if (!m_ready)
	{
		m_recharge += dTime;
		if (m_recharge >= m_fireInterval)
			m_ready = true;
	}

	return true;
}

void Cyberbot::TakeDamage(int amount)
{
	m_hp -= amount;
}

bool Cyberbot::IsDead()
{
	return (m_hp <= 0);
}

bool Cyberbot::AddWeapon(Weapon* weapon)
{
	m_weapon = weapon;

	return true;
}

WeaponFireInfo& Cyberbot::FireWeapon(D3DXVECTOR2 direction)
{
	m_ready = false;
	m_recharge = 0.0f;
	m_fireInterval = m_weapon->GetFireInterval();

	return m_weapon->Fire(m_position, direction);
}

bool Cyberbot::CanFire()
{
	return m_ready;
}