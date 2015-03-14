#pragma once

#include "Entity.h"

class Bullet : public Entity
{
public:
	Bullet(BitmapClass* bitmap, D3DXVECTOR2 position, int depth, float scale,
		D3DXVECTOR2 velocity, bool enabled, float lifeTime);
	~Bullet(void);

	Bullet* Copy(bool enabled);
	bool Frame(float dTime);

	void SetDirection(D3DXVECTOR2 direction);
	bool IsDead();

private:
	float m_lifeTime;
};

