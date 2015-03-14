// BeamWeapon.h
// Written by Blake Zoeckler

#pragma once
#include "Weapon.h"

class BeamWeapon : public Weapon
{
public:
	BeamWeapon(float interval, Bullet* beamGraphic, int numShots, float shotSpread);
	~BeamWeapon(void);

	void Shutdown();

	WeaponFireInfo& Fire(D3DXVECTOR2 origin, D3DXVECTOR2 direction);

private:
	Bullet* m_beamGraphic;
	float m_beamWidth;
	float m_beamLength;

	int m_numShots;
	float m_shotSpread;
};

