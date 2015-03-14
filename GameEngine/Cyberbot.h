// Cyberbot.h
// Written by Blake Zoeckler

#pragma once

#include "Entity.h"
#include "Weapon.h"

class Cyberbot : public Entity
{
public:
	Cyberbot(BitmapClass*, D3DXVECTOR2 position, int depth, float scale, D3DXVECTOR2 velocity, bool enabled);
	~Cyberbot(void);

	virtual void Shutdown();
	virtual bool Frame(float dTime);

	void TakeDamage(int amount);
	bool IsDead();

	virtual bool AddWeapon(Weapon* weapon);
	virtual WeaponFireInfo& FireWeapon(D3DXVECTOR2 direction);
	bool CanFire();

protected:
	int m_hp;
	bool m_ready;
	float m_fireInterval;
	float m_recharge;

private:
	Weapon* m_weapon;
};

// enemies additionally need
// - fire automatically
// - target the player
// - movement ai


