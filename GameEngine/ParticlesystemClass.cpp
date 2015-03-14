// ParticlesystemClass.cpp
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html

////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#define _USE_MATH_DEFINES

#include "particlesystemclass.h"

ParticleSystemClass::ParticleSystemClass()
{
	m_Texture = 0;
	m_particleList = 0;
	m_vertexBuffer = 0;
	m_instanceBuffer = 0;
}


ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass& other)
{
}


ParticleSystemClass::~ParticleSystemClass()
{
}

// The Initialize function first loads the texture that will be used for the particles.
// After the texture is loaded it then initializes the particle system. Once the particle system
// has been initialized it then creates the initial empty vertex and index buffers.
// The buffers are created empty at first as there are no particles emitted yet.
bool ParticleSystemClass::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;


	// Load the texture that is used for the particles.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	// Initialize the particle system.
	result = InitializeParticleSystem();
	if(!result)
	{
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

// The Shutdown function releases the buffers, particle system, and particle texture.
void ParticleSystemClass::Shutdown()
{
	// Release the buffers.
	ShutdownBuffers();

	// Release the particle system.
	ShutdownParticleSystem();

	// Release the texture used for the particles.
	ReleaseTexture();

	return;
}

// The Frame function is where we do the majority of the particle system work. Each frame
// we first check if we need to clear some of the particles that have reached the end of their render life.
// Secondly we emit new particles if it is time to do so. After we emit new particles we then update
// all the particles that are currently emitted, in this tutorial we update their height position to
// create a falling effect. After the particles have been updated we then need to update the vertex
// buffer with the updated location of each particle. The vertex buffer is dynamic so updating it is easy to do.
bool ParticleSystemClass::Frame(float frameTime)
{
	bool result;

	// Release old particles.
	KillParticles();

	// Emit new particles.
	// EmitParticles(frameTime);
	
	// Update the position of the particles.
	UpdateParticles(frameTime);

	return true;
}

// The Render function calls the RenderBuffers private function to render the particles.
void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Update the dynamic vertex buffer with the new position of each particle.
	UpdateBuffers(deviceContext);

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

void ParticleSystemClass::ForceEmit(D3DXVECTOR2 position)
{
	bool emitParticle, found;
	D3DXVECTOR4 color = D3DXVECTOR4();
	float velocity, radian, depth, life;
	int index, n, i, j;

	for (n = 0; n < m_particleEmitAmount; n++)
	{
		if (m_currentParticleCount < (m_maxParticles - 1))
		{
			m_currentParticleCount++;
	
			depth = (((float)rand())/RAND_MAX) * 10.0f;
			radian = ((float)rand()/RAND_MAX) * M_PI*2;
			velocity = m_particleVelocity + (((float)rand())/RAND_MAX) * m_particleVelocityVariation;
			life = m_particleLife + (((float)rand())/RAND_MAX) * m_particleLifeVariation;

			// color.x  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
			// color.y  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
			// color.z  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
			// color.w  = 1.0f;
			color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

			// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
			// We will sort using Z depth so we need to find where in the list the particle should be inserted.
			index = 0;
			found = false;
			while(!found)
			{
				if((m_particleList[index].active == false) || (m_particleList[index].position.z < depth))
				{
					found = true;
				}
				else
				{
					index++;
				}
			}

			// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
			i = m_currentParticleCount;
			j = i - 1;

			while(i != index)
			{
				m_particleList[i].position  = m_particleList[j].position;
				m_particleList[i].color     = m_particleList[j].color;
				m_particleList[i].velocity  = m_particleList[j].velocity;
				m_particleList[i].timeAlive = m_particleList[j].timeAlive;
				m_particleList[i].active    = m_particleList[j].active;
				i--;
				j--;
			}

			// Now insert it into the particle array in the correct depth order.
			m_particleList[index].position = D3DXVECTOR3(position.x - velocity*cos(radian)/10.0f, position.y - velocity*sin(radian)/10.0f, depth);
			m_particleList[index].color    = color;
			m_particleList[index].velocity  = D3DXVECTOR2(velocity*cos(radian), velocity*sin(radian));
			m_particleList[index].timeAlive = life;
			m_particleList[index].active    = true;
		}
	}

	return;
}

// GetTexture returns a pointer to the particle texture resource.
ID3D11ShaderResourceView* ParticleSystemClass::GetTexture()
{
	return m_Texture->GetTexture();
}

int ParticleSystemClass::GetVertexCount()
{
	return m_vertexCount;
}

int ParticleSystemClass::GetInstanceCount()
{
	return m_currentParticleCount;
}

// LoadTexture loads the star.dds file into a texture resource that can be used for rendering the particles.
bool ParticleSystemClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

// ReleaseTexture releases the texture resource that was used for rendering the particles.
void ParticleSystemClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

// The InitializeParticleSystem is where we initialize all the parameters and the particle system to be ready for frame processing.
bool ParticleSystemClass::InitializeParticleSystem()
{

// We start by initializing all the different elements that will be used for the particle properties.
// For this particle system we set the random deviation of where the particles will spawn in terms of location.
// We also set the speed they will fall at and the random deviation of speed for each particle.
// After that we set the size of the particles. And finally we set how many particles will be emitted
// every second as well as the total amount of particles allowed in the system at one time.

	// Set the speed and speed variation of particles.
	m_particleVelocity = 50.0f;
	m_particleVelocityVariation = 300.0f;

	// Set the lifetime of the particles
	m_particleLife = 0.6f;
	m_particleLifeVariation = 0.6f;

	// Set the physical size of the particles.
	m_particleSize = 4.0f;

	// Set the number of particles to emit.
	m_particleEmitAmount = 100;

	m_timeDelay = 2.0f;

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	InitialParticles();

	return true;
}

// ---------- EDIT
// InitialParticles is a new method that creates particles when the system is initialized.
// This is useful for explosion effects where the system starts with many particles on the screen
// instead of creating them over time.
void ParticleSystemClass::InitialParticles()
{
	
}

// EmitParticles is called each frame to emit new particles. It determines when to emit a particle
// based on the frame time and the particles per second variable. If there is a new particle
// to be emitted then the new particle is created and it properties are set. After that it is
// inserted into the particle array in Z depth order. The particle array needs to be sorted
// in correct depth order for rendering to work using an alpha blend. If it is not sorted you will get some visual artifacts.
void ParticleSystemClass::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	D3DXVECTOR4 color = D3DXVECTOR4();
	float velocity, radian, depth, life;
	int index, n, i, j;

	// Increment the frame time.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;
	
	// Check if it is time to emit new particles or not.
	if(m_accumulatedTime > (m_timeDelay))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if(emitParticle == true)
	{
		for (n = 0; n < m_particleEmitAmount; n++)
		{if (m_currentParticleCount < (m_maxParticles - 1))
		{
			m_currentParticleCount++;
	
			depth = (((float)rand())/RAND_MAX) * 10.0f;
			radian = ((float)rand()/RAND_MAX) * M_PI*2;
			velocity = m_particleVelocity + (((float)rand())/RAND_MAX) * m_particleVelocityVariation;
			life = m_particleLife + (((float)rand())/RAND_MAX) * m_particleLifeVariation;

			// color.x  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
			// color.y  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
			// color.z  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
			// color.w  = 1.0f;
			color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);

			// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
			// We will sort using Z depth so we need to find where in the list the particle should be inserted.
			index = 0;
			found = false;
			while(!found)
			{
				if((m_particleList[index].active == false) || (m_particleList[index].position.z < depth))
				{
					found = true;
				}
				else
				{
					index++;
				}
			}

			// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
			i = m_currentParticleCount;
			j = i - 1;

			while(i != index)
			{
				m_particleList[i].position  = m_particleList[j].position;
				m_particleList[i].color     = m_particleList[j].color;
				m_particleList[i].velocity  = m_particleList[j].velocity;
				m_particleList[i].timeAlive = m_particleList[j].timeAlive;
				m_particleList[i].active    = m_particleList[j].active;
				i--;
				j--;
			}

			// Now insert it into the particle array in the correct depth order.
			m_particleList[index].position = D3DXVECTOR3(-velocity*cos(radian)/10.0f, -velocity*sin(radian)/10.0f, depth);
			m_particleList[index].color    = color;
			m_particleList[index].velocity  = D3DXVECTOR2(velocity*cos(radian), velocity*sin(radian));
			m_particleList[index].timeAlive = life;
			m_particleList[index].active    = true;
		}}
	}

	return;
}

// The UpdateParticles function is where we update the properties of the particles each frame.
// In this tutorial we are updating the height position of the particle based on its speed which
// creates the particle water fall effect. This function can easily be extended to do numerous
// other effects and movement for the particles.
void ParticleSystemClass::UpdateParticles(float frameTime)
{
	int i;


	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for(i=0; i<m_currentParticleCount; i++)
	{
		m_particleList[i].position.x = m_particleList[i].position.x - (m_particleList[i].velocity.x * frameTime);
		m_particleList[i].position.y = m_particleList[i].position.y - (m_particleList[i].velocity.y * frameTime);
		m_particleList[i].timeAlive -= frameTime;

		// m_particleList[i].color.w = 1.0f - m_particleList[i].timeAlive / m_particleLife;

		m_particleList[i].color.x = m_particleList[i].color.y = m_particleList[i].color.z = min((m_particleList[i].timeAlive / m_particleLife), 1.0f);
	}

	return;
}

//The KillParticles function is used to remove particles from the system that have
// exceeded their rendering life time. This function is called each frame to check
// if any particles should be removed. In this tutorial the function checks if they
// have exceeded m_particleLife, and if so they are removed and the array is shifted back into depth order again.
void ParticleSystemClass::KillParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for(i=0; i<m_maxParticles; i++)
	{
		if((m_particleList[i].active == true) && (m_particleList[i].timeAlive < 0.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for(j=i; j<m_maxParticles-1; j++)
			{
				m_particleList[j].position  = m_particleList[j+1].position;
				m_particleList[j].color     = m_particleList[j+1].color;
				m_particleList[j].velocity  = m_particleList[j+1].velocity;
				m_particleList[j].timeAlive = m_particleList[j+1].timeAlive;
				m_particleList[j].active    = m_particleList[j+1].active;
			}
		}
	}

	return;
}

// The ShutdownParticleSystem function releases the particle array during shutdown.
void ParticleSystemClass::ShutdownParticleSystem()
{
	// Release the particle list.
	if(m_particleList)
	{
		delete [] m_particleList;
		m_particleList = 0;
	}

	return;
}

// InitializeBuffers prepares the vertex and index buffer that will be used for rendering the particles.
// As the particles will be updated every frame the vertex buffer will need to be created as a dynamic buffer.
// At the beginning there are no particles emitted so the vertex buffer will be created empty.
bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	HRESULT result;

	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the maximum number of particles allowed in the particle system.
	m_maxParticles = 5000;

	// Create the vertex array for the particles that will be rendered.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Load the vertex array with data.

	// Bottom left.
	vertices[0].position = D3DXVECTOR3(-m_particleSize, -m_particleSize, 0.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Top left.
	vertices[1].position = D3DXVECTOR3(-m_particleSize, m_particleSize, 0.0f);
	vertices[1].texture = D3DXVECTOR2(0.0f, 0.0f);

	// Bottom right.
	vertices[2].position = D3DXVECTOR3(m_particleSize, -m_particleSize, 0.0f);
	vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);

	// Bottom right.
	vertices[3].position = D3DXVECTOR3(m_particleSize, -m_particleSize, 0.0f);
	vertices[3].texture = D3DXVECTOR2(1.0f, 1.0f);

	// Top left.
	vertices[4].position = D3DXVECTOR3(-m_particleSize, m_particleSize, 0.0f);
	vertices[4].texture = D3DXVECTOR2(0.0f, 0.0f);

	// Top right.
	vertices[5].position = D3DXVECTOR3(m_particleSize, m_particleSize, 0.0f);
	vertices[5].texture = D3DXVECTOR2(1.0f, 0.0f);

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex array now that the vertex buffer has been created and loaded.
	delete [] vertices;
	vertices = 0;

	// Create the particle list.
	m_particleList = new ParticleType[m_maxParticles];
	if(!m_particleList)
	{
		return false;
	}

	// Initialize the particle list.
	for(int i=0; i<m_maxParticles; i++)
	{
		m_particleList[i].position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_particleList[i].color = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		m_particleList[i].velocity = D3DXVECTOR2(0.0f, 0.0f);
		m_particleList[i].active = false;
	}

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(ParticleType) * m_maxParticles;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = m_particleList;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

// The ShutdownBuffers function releases the vertex and index buffer during shutdown.
void ParticleSystemClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

// The UpdateBuffers function is called each frame and rebuilds the entire dynamic vertex buffer
// with the updated position of all the particles in the particle system.
bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ParticleType* instancesPtr;
	
	// Lock the instance buffer.
	result = deviceContext->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	instancesPtr = (ParticleType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(instancesPtr, (void*)m_particleList, (sizeof(ParticleType) * m_maxParticles));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_instanceBuffer, 0);

	return true;
}

// RenderBuffers is used to draw the particle buffers. It places the geometry on the pipeline so that the shader can render it.
void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer strides.
	strides[0] = sizeof(VertexType); 
	strides[1] = sizeof(ParticleType); 

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;	
	bufferPointers[1] = m_instanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

