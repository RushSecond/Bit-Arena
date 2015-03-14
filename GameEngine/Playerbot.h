// Playerbot.h
// Written by Blake Zoeckler

#pragma once

#include "Cyberbot.h"

class Playerbot : public Cyberbot
{
public:
	Playerbot(BitmapClass*, D3DXVECTOR2 position, int depth, float scale, D3DXVECTOR2 velocity, bool enabled);
	~Playerbot(void);

	void Shutdown();
	bool Frame(float dTime);

	bool AddWeapon(Weapon* weapon);
	WeaponFireInfo& FireWeapon(D3DXVECTOR2 direction);

	void ChangeWeapon();

private:

	Weapon* m_weapons[2];
	int m_activeWeapon;
};
