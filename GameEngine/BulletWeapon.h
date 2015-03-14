#pragma once
#include "Weapon.h"

class Bullet;

class BulletWeapon : public Weapon
{
public:
	BulletWeapon(float interval, Bullet* shot, float shotSpeed, int numShots, float shotSpread);
	~BulletWeapon(void);

	void Shutdown();

	WeaponFireInfo& Fire(D3DXVECTOR2 origin, D3DXVECTOR2 direction);

private:
	Bullet* m_bullet;
	float m_bulletSpeed;

	int m_numShots;
	float m_shotSpread;
};

