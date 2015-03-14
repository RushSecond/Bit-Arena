#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
class Bullet;

// weapon type definitions

#define WEAPONTYPE_BULLET		1
#define WEAPONTYPE_BEAM			0
#define NUM_WEAPONTYPES			2

struct WeaponFireInfo
{
	Bullet** entitiesOut;
	int entitiesNum;
	D3DXVECTOR2* raysOut;
	int raysNum;
};

class Weapon
{
public:
	Weapon(float interval);
	~Weapon(void);
	virtual void Shutdown() = 0;

	virtual WeaponFireInfo& Fire(D3DXVECTOR2 origin, D3DXVECTOR2 direction) = 0;

	float GetFireInterval();

protected:
	float m_fireInterval;
};