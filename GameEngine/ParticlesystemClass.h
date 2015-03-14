// ParticlesystemClass.h
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html

#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <cmath>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ParticleSystemClass
////////////////////////////////////////////////////////////////////////////////
class ParticleSystemClass
{
protected:

	struct ParticleType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
		D3DXVECTOR2 velocity;
		float timeAlive;
		bool active;
	};

// The VertexType for rendering particles just requires position and texture coordinates

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	ParticleSystemClass();
	ParticleSystemClass(const ParticleSystemClass&);
	~ParticleSystemClass();

// The class functions are the regular initialize, shutdown, frame, and render. However note that
// the Frame function is where we do all the work of updating, sorting, and rebuilding
// the of vertex buffer each frame so the particles can be rendered correctly.

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	bool Frame(float);
	void Render(ID3D11DeviceContext*);
	void ForceEmit(D3DXVECTOR2);

	ID3D11ShaderResourceView* GetTexture();
	int GetVertexCount();
	int GetInstanceCount();

protected:
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	virtual bool InitializeParticleSystem();
	virtual void InitialParticles();
	virtual void EmitParticles(float);
	virtual void UpdateParticles(float);
	virtual void KillParticles();

	void ShutdownParticleSystem();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

	bool UpdateBuffers(ID3D11DeviceContext*);

	void RenderBuffers(ID3D11DeviceContext*);

// The following private class variables are the ones used for the particle properties.
// They define how the particle system will work and changing each of them has a unique effect
// on how the particle system will react. If you plan to add more functionality to the particle system
// you would add it here by using additional variables for modifying the particles.

private:
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleLife, m_particleLifeVariation;

protected:
	float m_particleSize, m_timeDelay;
	int m_maxParticles, m_particleEmitAmount;

// We require a count and an accumulated time variable for timing the emission of particles.

	int m_currentParticleCount;
	float m_accumulatedTime;

// We use a single texture for all the particles in this tutorial.

	TextureClass* m_Texture;

// The particle system is an array of particles made up from the ParticleType structure.

	ParticleType* m_particleList;

// The particle system is rendered using a single vertex and instance buffer. Note that the instance buffer will be dynamic.

	int m_vertexCount;
	ID3D11Buffer *m_vertexBuffer, *m_instanceBuffer;
};

