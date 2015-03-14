// Entity.cpp
// Written by Blake Zoeckler

#include "stdafx.h"
#include "Entity.h"

Entity::Entity(BitmapClass* bitmap, D3DXVECTOR2 position, int depth, float scale, D3DXVECTOR2 velocity, bool enabled)
{
	m_bitmap = bitmap;
	m_position = position, m_scale = scale; m_depth = depth; m_rotation = 0.0f;

	m_velocity = velocity;
	m_force.x = 0.0f; m_force.y = 0.0f;
	m_maxSpeed = 300.0f;
	m_collisionRadius = bitmap->GetHeight() * scale * 0.5f;

	m_currentFrame = -1;
	m_frameCount = bitmap->GetFrameCount();
	m_animated = (m_frameCount > 1);
	
	m_enabled = enabled;
}


Entity::~Entity(void)
{
}

void Entity::Shutdown()
{
	m_bitmap = 0;
}

// Copies this entity and returns the pointer
Entity* Entity::Copy(bool enabled)
{
	Entity* copy = new Entity(m_bitmap, m_position, m_depth, m_scale, m_velocity, enabled);

	return copy;
}

bool Entity::Frame(float dTime)
{
	if (!m_enabled) return true;

	if (m_animated)
	{
		m_currentFrame++;
		m_currentFrame %= m_frameCount;
	}
	
	return true;
}

bool Entity::Render(ID3D11DeviceContext* deviceContext, TextureShaderClass* shader,
						 D3DMATRIX worldMatrix, D3DMATRIX viewMatrix, D3DMATRIX orthoMatrix)
{
	bool result;
	if (!m_enabled) return true;

	result = m_bitmap->Render(deviceContext, m_position.x, m_position.y, (float)m_depth, m_scale, m_rotation, m_currentFrame,
			shader, worldMatrix, viewMatrix, orthoMatrix);

	return result;
}

bool Entity::IsEnabled()
{
	return m_enabled;
}

void Entity::SetEnable(bool flag)
{
	m_enabled = flag;
}

void Entity::Translate(D3DXVECTOR2 vec)
{
	D3DXVec2Add(&m_position, &m_position, &vec);
}

void Entity::SetPosition(D3DXVECTOR2 position)
{
	m_position = position;
}

void Entity::GetPosition(float& x, float& y)
{
	x = m_position.x;
	y = m_position.y;
}

D3DXVECTOR2 Entity::GetPosition()
{
	return m_position;
}

void Entity::SetRotation(float r)
{
	m_rotation = r;
}

void Entity::Rotate(float r)
{
	m_rotation += r;
}

void Entity::SetDepth(int depth)
{
	m_depth = depth;
}

int Entity::GetDepth()
{
	return m_depth;
}

void Entity::SetForce(float x, float y)
{
	m_force.x = x;
	m_force.y = y;
}

void Entity::SetMaxSpeed(float speed)
{
	m_maxSpeed = speed;
}

D3DXVECTOR2 Entity::GetDimensions()
{
	return D3DXVECTOR2(m_bitmap->GetWidth(), m_bitmap->GetHeight());
}

float Entity::GetCollisionRadius()
{
	return m_collisionRadius;
}

bool Entity::Collided(Entity* other)
{
	D3DXVECTOR2 distance;
	D3DXVec2Subtract(&distance, &m_position, &other->GetPosition());

	if (m_collisionRadius + other->m_collisionRadius > D3DXVec2Length(&distance))
		return true;

	return false;
}

bool Entity::IsDead()
{
	return false;
}