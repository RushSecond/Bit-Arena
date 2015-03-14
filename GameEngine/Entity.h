// Entity.h
// Written by Blake Zoeckler

#pragma once

#include "BitmapClass.h"

class Entity
{
public:
	Entity(BitmapClass*, D3DXVECTOR2 position, int depth, float scale, D3DXVECTOR2 velocity, bool enabled);
	~Entity(void);

	void Shutdown();
	Entity* Copy(bool enabled);

	virtual bool Frame(float dTime);
	bool Render(ID3D11DeviceContext*, TextureShaderClass*, D3DMATRIX, D3DMATRIX, D3DMATRIX);

	bool IsEnabled();
	void SetEnable(bool flag);

	void Translate(D3DXVECTOR2);
	void SetPosition(D3DXVECTOR2);
	void GetPosition(float& x, float& y);
	D3DXVECTOR2 GetPosition();

	void SetRotation(float r);
	void Rotate(float r);

	void SetDepth(int depth);
	int GetDepth();

	void SetForce(float x, float y);
	void SetMaxSpeed(float speed);

	D3DXVECTOR2 GetDimensions();

	float GetCollisionRadius();
	bool Collided(Entity* other);
	virtual bool IsDead();

protected:
	BitmapClass* m_bitmap;

	int m_depth;
	float m_scale;
	float m_rotation;

	D3DXVECTOR2 m_position;
	float m_maxSpeed;
	float m_collisionRadius;
	D3DXVECTOR2 m_velocity;
	D3DXVECTOR2 m_force;

	bool m_enabled;
	bool m_animated;
	int m_currentFrame, m_frameCount;
};

