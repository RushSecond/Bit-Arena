// EntityManager.cpp
// Written by Blake Zoeckler

#include "stdafx.h"
#include "EntityManager.h"
#include "Playerbot.h"
#include "Bullet.h"
#include "ParticlesystemClass.h"
#include "ParticleShaderClass.h"
#include "BulletWeapon.h"
#include "BeamWeapon.h"

#include <iostream>

EntityManager::EntityManager(void)
{
	m_d3dDevice = 0;
	m_ParticleSystem = 0;
}

EntityManager::~EntityManager(void)
{
}

// 
bool EntityManager::Initialize(ID3D11Device* device, int width, int height)
{
	m_d3dDevice = device;
	screenWidth = width; screenHeight = height;

	// Create the particle system object.
	m_ParticleSystem = new ParticleSystemClass;
	if(!m_ParticleSystem)
	{
		return false;
	}

	// Initialize the particle system object.
	bool result = m_ParticleSystem->Initialize(m_d3dDevice, L"../Textures/particle.png");
	if(!result)
	{
		return false;
	}

	return true;
}

// Cleans up members
void EntityManager::Shutdown()
{
	for (int i = 0; i < NUM_ENTITYTYPES; i++)
	{
		if (!m_entityLists[i].empty())
		{
			m_entityLists[i].clear();
		}
	}

	if (!m_GUIList.empty())
	{
		m_GUIList.clear();
	}

	if (!m_Bitmaps.empty())
	{
		for (map<string, BitmapClass*>::iterator iter = m_Bitmaps.begin(); iter != m_Bitmaps.end(); iter++)
		{
			iter->second->Shutdown();
		}

		m_Bitmaps.clear();
	}

	if (!m_Weapons.empty())
	{
		for (map<string, Weapon*>::iterator iter = m_Weapons.begin(); iter != m_Weapons.end(); iter++)
		{
			iter->second->Shutdown();
		}

		m_Weapons.clear();
	}

	m_d3dDevice = 0;
}

bool EntityManager::Frame(float dTime)
{
	list<Entity*>::iterator iter;
	Entity* iterEntity;

	// Update entities
	for (int i = NUM_ENTITYTYPES-1; i >= ENTITYTYPE_OTHER; i--)
	{
		iter = m_entityLists[i].begin();
		while (iter != m_entityLists[i].end())
		{
			iterEntity = iter._Ptr->_Myval;
			if (iterEntity->IsEnabled())
			{
				iterEntity->Frame(dTime);
			
				// Check death, out of bounds, and collision. A true return means the entity
				// should be deleted and the iterator doesn't need to be incremented
				if (iterEntity->IsDead())
				{
					// Do a particle explosion on enemy death
					if (i == ENTITYTYPE_ENEMY)
						m_ParticleSystem->ForceEmit(iterEntity->GetPosition());

					iterEntity->Shutdown();
					delete iterEntity;
					iter = m_entityLists[i].erase(iter);
				}
				else if (OutOfBounds(iterEntity, i))
				{
					iterEntity->Shutdown();
					delete iterEntity;
					iter = m_entityLists[i].erase(iter);
				}
				else if (Collided(iterEntity, i))
				{
					iterEntity->Shutdown();
					delete iterEntity;
					iter = m_entityLists[i].erase(iter);
				}
				else iter++;
			}
			else iter++;
		}
	}

	// Update GUI
	for (list<Entity*>::iterator iter = m_GUIList.begin(); iter != m_GUIList.end(); iter++)
	{
		iter._Ptr->_Myval->Frame(dTime);
	}

	// Update particles
	m_ParticleSystem->Frame(dTime);

	return true;
}

// Adds a new bitmap into the library, using a string key
bool EntityManager::AddBitmap(WCHAR* textureFilename, string name)
{
	BitmapClass* newBitmap = new BitmapClass();
	bool result;

	result = newBitmap->Initialize(m_d3dDevice, screenWidth, screenHeight, textureFilename, 1, 1);
	m_Bitmaps.insert(std::pair<string,BitmapClass*>(name,newBitmap));

	return result;
}

// Adds a new animated bitmap into the library, using a string key
bool EntityManager::AddBitmapAnimated(WCHAR* textureFilename, int frames, int columns, string name)
{
	BitmapClass* newBitmap = new BitmapClass();
	bool result;

	result = newBitmap->Initialize(m_d3dDevice, screenWidth, screenHeight, textureFilename, frames, columns);
	m_Bitmaps.insert(std::pair<string,BitmapClass*>(name,newBitmap));

	return true;
}

// Adds a new entity to the game
bool EntityManager::AddEntity(string bitmapname, int entityType, float xPos, float yPos, float scale, D3DXVECTOR2 velocity,
							  float lifeTime, bool enabled, Entity **outEntity)
{
	Entity* newEntity;

	// Get the bitmap
	BitmapClass* bitmap = m_Bitmaps[bitmapname];

	// Initialize based on entity type
	switch (entityType)
	{
	case ENTITYTYPE_PLAYER:
		newEntity = new Playerbot(bitmap, D3DXVECTOR2(xPos, yPos), entityType, scale, velocity, enabled);
		break;
	case ENTITYTYPE_ENEMY:
		newEntity = new Cyberbot(bitmap, D3DXVECTOR2(xPos, yPos), entityType, scale, velocity, enabled);
		break;
	case ENTITYTYPE_PLAYERBULLET:
	case ENTITYTYPE_PLAYERBEAM:
	case ENTITYTYPE_ENEMYBULLET:
		newEntity = new Bullet(bitmap, D3DXVECTOR2(xPos, yPos), entityType, scale, velocity, enabled, lifeTime);	
		break;
	default:
		newEntity = new Entity(bitmap, D3DXVECTOR2(xPos, yPos), entityType, scale, velocity, enabled);
		break;
	}

	// Add it to the proper entity list
	if (entityType == ENTITYTYPE_GUI)
		m_GUIList.push_front(newEntity);
	else
		m_entityLists[entityType].push_front(newEntity);
	
	// If the outEntity isn't null, save it as the new entity
	if (outEntity)
		*outEntity = newEntity;

	return true;
}

// Adds an existing entity to the manager
bool EntityManager::AddEntity(Entity* newEntity)
{
	int entityType = newEntity->GetDepth();

	// Add it to the proper entity list
	if (entityType == ENTITYTYPE_GUI)
		m_GUIList.push_front(newEntity);
	else
		m_entityLists[entityType].push_front(newEntity);

	return true;
}

bool EntityManager::AddWeapon(string weaponName, string bitMapName, int entityType, float bitMapScale, int weaponType, float interval,
							  float projectileSpeed, float projectileLifeTime, int numShots, float shotSpread)
{
	Bullet* weaponBullet = 0;
	AddEntity(bitMapName, entityType, 0, 0, bitMapScale, ZeroVector, projectileLifeTime, false, (Entity**)&weaponBullet);

	// Initialize new weapon based on weapon type
	Weapon* newWeapon = 0;
	switch (weaponType)
	{
	case WEAPONTYPE_BULLET:
		newWeapon = new BulletWeapon(interval, weaponBullet, projectileSpeed, numShots, shotSpread);
		break;
	case WEAPONTYPE_BEAM:
		newWeapon = new BeamWeapon(interval, weaponBullet, numShots, shotSpread);
		break;
	default:
		return false;
	}

	m_Weapons.insert(std::pair<string,Weapon*>(weaponName,newWeapon));

	return true;
}

bool EntityManager::AddWeaponToPlayer(string weaponName)
{
	// Get the weapon
	Weapon* weapon = m_Weapons[weaponName];

	GetPlayer()->AddWeapon(weapon);

	return true;
}

bool EntityManager::AddWeaponToBot(Cyberbot* bot, string weaponName)
{
	// Get the weapon
	Weapon* weapon = m_Weapons[weaponName];

	bot->AddWeapon(weapon);

	return true;
}

// returns a pointer to the player
Playerbot* EntityManager::GetPlayer()
{
	return (Playerbot*)m_entityLists[ENTITYTYPE_PLAYER].front();
}

// Renders all the entities on the screen using the given shader
bool EntityManager::RenderEntities(ID3D11DeviceContext* deviceContext, TextureShaderClass* shader,
								   D3DMATRIX worldMatrix, D3DMATRIX viewMatrix, D3DMATRIX orthoMatrix)
{
	bool result;

	// Render entities
	for (int i = 0; i < NUM_ENTITYTYPES; i++)
	{
		for (list<Entity*>::iterator iter = m_entityLists[i].begin(); iter != m_entityLists[i].end(); iter++)
		{
			result = iter._Ptr->_Myval->Render(deviceContext, shader, worldMatrix, viewMatrix, orthoMatrix);
			if (!result)
				return false;
		}
	}

	return true;
}

// Renders all the GUI objects on the screen using the given shader
bool EntityManager::RenderGUI(ID3D11DeviceContext* deviceContext, TextureShaderClass* shader,
								   D3DMATRIX worldMatrix, D3DMATRIX GUIMatrix, D3DMATRIX orthoMatrix)
{
	bool result;

	// Render GUI
	for (list<Entity*>::iterator iter = m_GUIList.begin(); iter != m_GUIList.end(); iter++)
	{
		result = iter._Ptr->_Myval->Render(deviceContext, shader, worldMatrix, GUIMatrix, orthoMatrix);
		if (!result)
			return false;
	}

	return true;
}

bool EntityManager::RenderParticles(ID3D11DeviceContext* deviceContext, ParticleShaderClass* shader,
								   D3DMATRIX worldMatrix, D3DMATRIX viewMatrix, D3DMATRIX orthoMatrix)
{
	bool result;

	// Put the particle system vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_ParticleSystem->Render(deviceContext);

	// Render the model using the texture shader.
	result = shader->Render(deviceContext, m_ParticleSystem->GetVertexCount(), m_ParticleSystem->GetInstanceCount(), worldMatrix, viewMatrix, orthoMatrix, 
					  m_ParticleSystem->GetTexture());
	if (!result) return false;

	return true;
}

// Checks a ray for collision with collideType entities and damage them
void EntityManager::RayDamage(D3DXVECTOR2 origin, D3DXVECTOR2 direction, int collideType, int damage)
{
	list<Entity*> results;
	list<Entity*>::iterator iter;
	Cyberbot* iterEntity;
	D3DXVECTOR2 difference;
	float qa, qb, qc;

	iter = m_entityLists[collideType].begin();
	while (iter != m_entityLists[collideType].end())
	{
		//Use shenanigans to determine if ray hit this object

		iterEntity = (Cyberbot*)iter._Ptr->_Myval;
		D3DXVec2Subtract(&difference, &(iterEntity->GetPosition()), &origin);
		qb = D3DXVec2Dot(&difference, &direction);

		if (qb > 0.0f)
		{
			qa = D3DXVec2LengthSq(&direction);
			qc = D3DXVec2LengthSq(&difference) - pow(iterEntity->GetCollisionRadius(),2);
			if (pow(qb,2) > qa * qc)
			{
				iterEntity->TakeDamage(damage);
			}
		}
		
		iter++;
	}
}

// Acts on entities that hit the playable bounds. Returns true if entity is to be
// deleted as a result
bool EntityManager::OutOfBounds(Entity* entity, int entityType)
{
	// Only check enemies, players, and their bullets
	if (entityType < ENTITYTYPE_ENEMY || entityType > ENTITYTYPE_PLAYERBULLET)
		return false;

	D3DXVECTOR2 position;
	entity->GetPosition(position.x, position.y);
	float collideRadius = entity->GetCollisionRadius();

	if (D3DXVec2Length(&position) + collideRadius > ARENA_SIZE)
	{
		// Entity hit the edge of the arena, so do something based on its type
		switch (entityType)
		{
		case ENTITYTYPE_PLAYERBEAM:
			return false;
		case ENTITYTYPE_PLAYERBULLET:
		case ENTITYTYPE_ENEMYBULLET:
			return true;
		default:
			D3DXVec2Normalize(&position, &position);
			D3DXVec2Scale(&position, &position, ARENA_SIZE-(collideRadius));
			entity->SetPosition(position);
			break;
		}
	}

	return false;
}

// Checks entities for collision. Returns true if entity is to be deleted
bool EntityManager::Collided(Entity* entity, int entityType)
{
	// Only check bullets for now
	if (entityType != ENTITYTYPE_ENEMYBULLET && entityType != ENTITYTYPE_PLAYERBULLET)
		return false;

	list<Entity*>::iterator iter;
	Cyberbot* iterEntity;
	
	// Player bullets check for enemies
	if (entityType == ENTITYTYPE_PLAYERBULLET)
	{
		iter = m_entityLists[ENTITYTYPE_ENEMY].begin();
		while (iter != m_entityLists[ENTITYTYPE_ENEMY].end())
		{
			iterEntity = (Cyberbot*)iter._Ptr->_Myval;
			
			if (entity->Collided(iterEntity))
			{
				iterEntity->TakeDamage(1);
				return true;
			}
			else iter++;
		}
	}

	// Enemy bullets check for players
	if (entityType == ENTITYTYPE_ENEMYBULLET)
	{
		iter = m_entityLists[ENTITYTYPE_PLAYER].begin();
		while (iter != m_entityLists[ENTITYTYPE_PLAYER].end())
		{
			iterEntity = (Cyberbot*)iter._Ptr->_Myval;
			
			if (entity->Collided(iterEntity))
			{
				iterEntity->TakeDamage(1);
				return true;
			}
			else iter++;
		}
	}

	return false;
}