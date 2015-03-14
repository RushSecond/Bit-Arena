#include "stdafx.h"
#include "Weapon.h"

Weapon::Weapon(float interval)
{
	m_fireInterval = interval;
}

Weapon::~Weapon(void)
{
}

float Weapon::GetFireInterval()
{
	return m_fireInterval;
}