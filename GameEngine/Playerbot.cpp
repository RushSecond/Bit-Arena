// Playerbot.cpp
// Written by Blake Zoeckler

#include "stdafx.h"
#include "Playerbot.h"


Playerbot::Playerbot(BitmapClass* bitmap, D3DXVECTOR2 position, int depth, float scale, D3DXVECTOR2 velocity, bool enabled)
	: Cyberbot(bitmap, position, depth, scale, velocity, enabled)
{
	m_activeWeapon = 0;
	m_weapons[0] = 0;
	m_weapons[1] = 0;
}


Playerbot::~Playerbot(void)
{
}

void Playerbot::Shutdown()
{
	Cyberbot::Shutdown();
}

bool Playerbot::Frame(float dTime)
{
	return Cyberbot::Frame(dTime);
}

bool Playerbot::AddWeapon(Weapon* weapon)
{
	if (!m_weapons[0])
	{
		m_weapons[0] = weapon;
	}
	else if (!m_weapons[1])
	{
		m_weapons[1] = weapon;
	}
	else
	{
		m_weapons[m_activeWeapon] = weapon;
	}

	return true;
}

WeaponFireInfo& Playerbot::FireWeapon(D3DXVECTOR2 direction)
{
	m_ready = false;
	m_recharge = 0.0f;
	m_fireInterval = m_weapons[m_activeWeapon]->GetFireInterval();

	return m_weapons[m_activeWeapon]->Fire(m_position, direction);
}

void Playerbot::ChangeWeapon()
{
	m_activeWeapon = 1-m_activeWeapon;
}