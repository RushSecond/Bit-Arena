#include "stdafx.h"
#include "BulletWeapon.h"
#include "Bullet.h"

BulletWeapon::BulletWeapon(float interval, Bullet* shot, float shotSpeed, int numShots, float shotSpread) : Weapon(interval)
{
	m_bullet = shot;
	m_bullet->SetMaxSpeed(shotSpeed);
	m_numShots = numShots;
	m_shotSpread = shotSpread;
}


BulletWeapon::~BulletWeapon(void)
{
}

void BulletWeapon::Shutdown()
{
	if (m_bullet)
	{
		m_bullet->Shutdown();
		delete m_bullet;
		m_bullet = 0;
	}
}

WeaponFireInfo& BulletWeapon::Fire(D3DXVECTOR2 origin, D3DXVECTOR2 direction)
{
	WeaponFireInfo output;
	Bullet* copy;

	float spreadFactor;
	D3DXVECTOR2 newDirection;
	D3DXMATRIX rotation;

	output.entitiesOut = new Bullet*[m_numShots];
	output.entitiesNum = m_numShots;
	output.raysOut = 0;
	output.raysNum = 0;

	for (int i = 0; i < m_numShots; i++)
	{
		copy = m_bullet->Copy(true);
		copy->SetPosition(origin);

		if (m_shotSpread != 0.0f)
		{
			// Determine new direction
			spreadFactor = (float)i - (float)(m_numShots - 1) / 2.0f;
			D3DXMatrixRotationZ(&rotation, spreadFactor * m_shotSpread);
			D3DXVec2TransformCoord(&newDirection, &direction, &rotation);
		}
		else
		{
			// Use direction from parameter
			newDirection = direction;
		}
		
		copy->SetDirection(newDirection);
		*(output.entitiesOut+i) = copy;
	}

	return output;
}