// BeamWeapon.cpp
// Written by Blake Zoeckler

#include "stdafx.h"
#define _USE_MATH_DEFINES

#include "BeamWeapon.h"
#include "Bullet.h"
#include <cmath>

BeamWeapon::BeamWeapon(float interval, Bullet* beamGraphic, int numShots, float shotSpread) : Weapon(interval)
{
	m_beamGraphic = beamGraphic;
	m_beamGraphic->SetMaxSpeed(0.0f);
	m_beamWidth = m_beamGraphic->GetCollisionRadius();
	m_beamLength = m_beamGraphic->GetDimensions().x;
	m_numShots = numShots;
	m_shotSpread = shotSpread;
}

BeamWeapon::~BeamWeapon(void)
{
}

void BeamWeapon::Shutdown()
{
	if (m_beamGraphic)
	{
		m_beamGraphic->Shutdown();
		delete m_beamGraphic;
		m_beamGraphic = 0;
	}
}

WeaponFireInfo& BeamWeapon::Fire(D3DXVECTOR2 origin, D3DXVECTOR2 direction)
{
	WeaponFireInfo output;
	Bullet* copy;

	float spreadFactor;
	D3DXVECTOR2 newDirection;
	D3DXVECTOR2 translation;
	D3DXMATRIX rotation;

	D3DXVec2Normalize(&direction, &direction);

	output.entitiesOut = new Bullet*[m_numShots];
	output.entitiesNum = m_numShots;
	output.raysOut = new D3DXVECTOR2[m_numShots];
	output.raysNum = m_numShots;

	for (int i = 0; i < m_numShots; i++)
	{
		copy = m_beamGraphic->Copy(true);
		copy->SetPosition(origin);

		if (m_shotSpread != 0.0f)
		{
			// Determine new direction
			spreadFactor = (float)i - (float)(m_numShots - 1) / 2.0f;
			D3DXMatrixRotationZ(&rotation, spreadFactor * m_shotSpread);
			D3DXVec2TransformCoord(&newDirection, &direction, &rotation);
		}
		else
		{	
			// Use direction from parameter
			newDirection = direction;			
		}

		output.raysOut[i] = newDirection;

		D3DXVec2Scale(&translation, &newDirection, m_beamLength + 20.0f);
		copy->Translate(translation);

		float rotate = acos(newDirection.x);
		if (newDirection.y < 0.0f) rotate *= -1.0f;
		copy->SetRotation(rotate);
			
		*(output.entitiesOut+i) = copy;
	}

	return output;
}