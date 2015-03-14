// EntityManager.h
// Written by Blake Zoeckler

#pragma once

// entity type definitions
// higher numbers are rendered on the top layer
#define ENTITYTYPE_GUI			12

#define ENTITYTYPE_PLAYER		11
#define ENTITYTYPE_ENEMY		10
#define ENTITYTYPE_ENEMYBULLET	9
#define ENTITYTYPE_PLAYERBULLET	8
#define ENTITYTYPE_PLAYERBEAM	7
#define ENTITYTYPE_EVIRONMENT	6
#define ENTITYTYPE_OTHER		0

#define NUM_ENTITYTYPES			12

// Arena size global
#define ARENA_SIZE				500.0f

#include "Entity.h"
#include <list>
#include <map>

class ParticleSystemClass;
class ParticleShaderClass;
class Weapon;
class Cyberbot;
class Playerbot;

class EntityManager
{
	// comparer for the map
	struct comparer
	{
		public:
		bool operator()(string x, string y)
		{
			 return x.compare(y)<0;
		}
	};

public:
	EntityManager(void);
	~EntityManager(void);

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight);
	void Shutdown();
	bool Frame(float dTime);

	bool AddBitmap(WCHAR* fileName, string name);	
	bool AddBitmapAnimated(WCHAR* fileName, int frames, int columns, string name);
	bool AddEntity(string bitMapName, int entityType, float xPosition, float yPosition, float scale, D3DXVECTOR2 velocity,
		float lifeTime, bool enabled, Entity** outEntity);
	bool AddEntity(Entity* newEntity);

	bool AddWeapon(string weaponName, string bitMapName, int entityType, float bitMapScale, int weaponType, float interval,
		float projectileSpeed, float projectileLifeTime, int numShots, float shotSpread);
	bool AddWeaponToPlayer(string weaponName);
	bool AddWeaponToBot(Cyberbot* bot, string weaponName);

	Playerbot* GetPlayer();

	bool RenderEntities(ID3D11DeviceContext* deviceContext, TextureShaderClass* shader, D3DMATRIX world, D3DMATRIX view, D3DMATRIX orthographic);
	bool RenderGUI(ID3D11DeviceContext* deviceContext, TextureShaderClass* shader, D3DMATRIX world, D3DMATRIX GUIview, D3DMATRIX orthographic);
	bool RenderParticles(ID3D11DeviceContext* deviceContext, ParticleShaderClass* shader, D3DMATRIX world, D3DMATRIX view, D3DMATRIX orthographic);

	void RayDamage(D3DXVECTOR2 origin, D3DXVECTOR2 direction, int collideType, int damage);

private:

	bool OutOfBounds(Entity* entity, int entityType);
	bool Collided(Entity* entity, int entityType);

private:
	list<Entity*> m_GUIList;
	list<Entity*> m_entityLists[NUM_ENTITYTYPES];

	// Strings are mapped to bitmaps
	map<string, BitmapClass*, comparer> m_Bitmaps;

	// Strings are also mapped to weapons
	map<string, Weapon*, comparer> m_Weapons;

	ID3D11Device* m_d3dDevice;
	ParticleSystemClass* m_ParticleSystem;

	int screenWidth, screenHeight;
};

