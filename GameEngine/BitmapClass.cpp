#include "stdafx.h"

#include "BitmapClass.h"

BitmapClass::BitmapClass(void)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}


BitmapClass::~BitmapClass(void)
{
}

// -----------------------------------------------------------------
// New initialization function that can find the size of the texture automatically
// and leave scaling alone for later rendering
// -----------------------------------------------------------------
bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int numFrames, int numColumns)
{
	bool result;

	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Find the size of the texture
	D3DX11_IMAGE_INFO *pSrcInfo = new D3DX11_IMAGE_INFO;
	HRESULT hr;
	D3DX11GetImageInfoFromFileW(textureFilename, NULL, pSrcInfo, &hr);

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = pSrcInfo->Width;
	m_bitmapHeight = pSrcInfo->Height;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;
	m_previousFrame = -1;

	// Set up animation variables
	m_frames = numFrames;
	m_columns = numColumns;
	int rows = m_frames / m_columns;

	m_cellWidth = m_bitmapWidth / m_columns;
	m_cellHeight = m_bitmapHeight / rows;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

//------------------------------------------------------------------------------------
// New bitmap render function that also takes scale and depth
//------------------------------------------------------------------------------------
bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, float positionX, float positionY, float positionZ, float scale, float rotation,
						 int frame, TextureShaderClass* shader, D3DMATRIX worldMatrix, D3DMATRIX viewMatrix, D3DMATRIX orthoMatrix)
{
	bool result;

	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	if (m_frames > 1)
		result = UpdateBuffersAnimated(deviceContext, positionX, positionY, positionZ, scale, rotation, frame);
	else
		result = UpdateBuffers(deviceContext, positionX, positionY, positionZ, scale, rotation);

	if(!result)
	{
		return false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	result = shader->Render(deviceContext, GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, GetTexture());
	if(!result)
	{
		return false;
	}

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

// Returns the width of the texture.
int BitmapClass::GetWidth()
{
	if (m_frames > 1) return m_cellWidth;
	else return m_bitmapWidth;
}

// Returns the height of the texture. This is used to calculate the collision radius
int BitmapClass::GetHeight()
{
	if (m_frames > 1) return m_cellHeight;
	else return m_cellHeight;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_Texture->GetTexture();
}

int BitmapClass::GetFrameCount()
{
	return m_frames;
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// We set the vertices to six since we are making a square out of two triangles,
	// so six points are needed. The indices will be the same.

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Load the index array with data.
	for(i=0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// We are now creating a dynamic vertex buffer so we can modify the data inside the vertex buffer each frame if we need to.
	// To make it dynamic we set Usage to D3D11_USAGE_DYNAMIC and CPUAccessFlags to D3D11_CPU_ACCESS_WRITE in the description.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// We don't need to make the index buffer dynamic since the six indices will always point
	// to the same six vertices even though the coordinates of the vertex may change.

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

//------------------------------------------------------------------------------------
// New update buffer with scale, depth, and rotation parameters
//------------------------------------------------------------------------------------
bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, float positionX, float positionY,
								float positionZ, float scale, float rotation)
{
	float left, right, top, bottom, depth;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// If the parameters we are rendering this bitmap to has not changed then don't update the vertex buffer
	if(((int)positionX == m_previousPosX) && ((int)positionY == m_previousPosY) && (rotation == m_previousRotation) &&
		(scale == m_previousScale))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = (int)positionX; m_previousPosY = (int)positionY;
	m_previousRotation = rotation; m_previousScale = scale;

	// Calculate rendering size
	float renderWidth = (float)m_bitmapWidth * scale;
	float renderHeight = (float)m_bitmapHeight * scale;

	// Calculate depth
	depth = positionZ;

	// Calculate the local coordinates of the sides of the bitmap.
	left = -renderWidth / 2;
	right = left + renderWidth;
	top = renderHeight / 2;
	bottom = top - renderHeight;
	
	// Make a square
	D3DXVECTOR4 points[4];
	points[0] = D3DXVECTOR4(left, top, 0, 1); // Top left
	points[1] = D3DXVECTOR4(right, top, 0, 1); // Top right
	points[2] = D3DXVECTOR4(left, bottom, 0, 1); // Bottom left
	points[3] = D3DXVECTOR4(right, bottom, 0, 1); // Bottom right

	// Rotate and translate to screen coordinates
	D3DXMATRIX rotator, translator, transformer;
	D3DXMatrixRotationZ(&rotator, rotation);
	D3DXMatrixTranslation(&translator, positionX, positionY, 0);
	D3DXMatrixMultiply(&transformer, &rotator, &translator);
	for (int i = 0; i < 4; i++)
		D3DXVec4Transform(&points[i], &points[i], &transformer);

	// Now that the coordinates are calculated create a temporary vertex array and fill it with the new six vertex points.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = D3DXVECTOR3(points[0].x, points[0].y, depth);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(points[3].x, points[3].y, depth);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(points[2].x, points[2].y, depth);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = D3DXVECTOR3(points[0].x, points[0].y, depth);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(points[1].x, points[1].y, depth);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(points[3].x, points[3].y, depth);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	// Now copy the contents of the vertex array into the vertex buffer using the Map and memcpy functions.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}

//------------------------------------------------------------------------------------
// Update buffers with scale, depth, rotation, and frame parameters for animated sprites
//------------------------------------------------------------------------------------
bool BitmapClass::UpdateBuffersAnimated(ID3D11DeviceContext* deviceContext, float positionX, float positionY,
								float positionZ, float scale, float rotation, int frame)
{
	float left, right, top, bottom, depth;
	float leftTex, rightTex, topTex, bottomTex;
	int frameX, frameY;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	// Clamp the frame parameter
	frame %= m_frames;

	// If the parameters we are rendering this bitmap to has not changed then don't update the vertex buffer
	if(((int)positionX == m_previousPosX) && ((int)positionY == m_previousPosY) && (rotation == m_previousRotation) &&
		(scale == m_previousScale) && (frame == m_previousFrame))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = (int)positionX; m_previousPosY = (int)positionY;
	m_previousRotation = rotation; m_previousScale = scale; m_previousFrame = frame;

	// Find the correct cell of the texture to render
	frameX = frame % m_columns;
	frameY = frame / m_columns;

	// Calculate rendering size
	float renderWidth = (float)m_cellWidth * scale;
	float renderHeight = (float)m_cellHeight * scale;

	// Calculate depth
	depth = positionZ;

	// Calculate the local coordinates of the sides of the bitmap.
	left = -renderWidth / 2;
	right = left + renderWidth;
	top = renderHeight / 2;
	bottom = top - renderHeight;

	// Calculate the texture coordinates
	leftTex = (float)(m_cellWidth*frameX++) / (float)m_bitmapWidth;
	rightTex = (float)(m_cellWidth*frameX) / (float)m_bitmapWidth;
	topTex = (float)(m_cellHeight*frameY++) / (float)m_bitmapHeight;
	bottomTex = (float)(m_cellHeight*frameY) / (float)m_bitmapHeight;
	
	// Make a square
	D3DXVECTOR4 points[4];
	points[0] = D3DXVECTOR4(left, top, 0, 1); // Top left
	points[1] = D3DXVECTOR4(right, top, 0, 1); // Top right
	points[2] = D3DXVECTOR4(left, bottom, 0, 1); // Bottom left
	points[3] = D3DXVECTOR4(right, bottom, 0, 1); // Bottom right

	// Rotate and translate to screen coordinates
	D3DXMATRIX rotator, translator, transformer;
	D3DXMatrixRotationZ(&rotator, rotation);
	D3DXMatrixTranslation(&translator, positionX, positionY, 0);
	D3DXMatrixMultiply(&transformer, &rotator, &translator);
	for (int i = 0; i < 4; i++)
		D3DXVec4Transform(&points[i], &points[i], &transformer);

	// Now that the coordinates are calculated create a temporary vertex array and fill it with the new six vertex points.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = D3DXVECTOR3(points[0].x, points[0].y, depth);  // Top left.
	vertices[0].texture = D3DXVECTOR2(leftTex, topTex);

	vertices[1].position = D3DXVECTOR3(points[3].x, points[3].y, depth);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(rightTex, bottomTex);

	vertices[2].position = D3DXVECTOR3(points[2].x, points[2].y, depth);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(leftTex, bottomTex);

	// Second triangle.
	vertices[3].position = D3DXVECTOR3(points[0].x, points[0].y, depth);  // Top left.
	vertices[3].texture = D3DXVECTOR2(leftTex, topTex);

	vertices[4].position = D3DXVECTOR3(points[1].x, points[1].y, depth);  // Top right.
	vertices[4].texture = D3DXVECTOR2(rightTex, topTex);

	vertices[5].position = D3DXVECTOR3(points[3].x, points[3].y, depth);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(rightTex, bottomTex);

	// Now copy the contents of the vertex array into the vertex buffer using the Map and memcpy functions.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
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

void BitmapClass::ReleaseTexture()
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