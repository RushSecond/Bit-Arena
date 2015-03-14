// BitmapClass.h
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html

#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include "textureclass.h"
#include "TextureShaderClass.h"

#define ZeroVector D3DXVECTOR2(0,0)

class BitmapClass
{
protected:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	// For instancing
	struct InstanceType
	{
		D3DXVECTOR3 position;
	};

public:
	BitmapClass(void);
	~BitmapClass(void);

	bool Initialize(ID3D11Device*, int screenWidth, int screenHeight, WCHAR* filename, int numFrames, int numColumns);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, float x, float y, float z, float scale, float rotation, int frame,
		TextureShaderClass*, D3DMATRIX world, D3DMATRIX view, D3DMATRIX ortho);

	// For instancing
	int GetVertexCount();
	int GetInstanceCount();

	int GetIndexCount();
	int GetWidth();
	int GetHeight();
	ID3D11ShaderResourceView* GetTexture();
	virtual int GetFrameCount();

protected:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

	bool UpdateBuffers(ID3D11DeviceContext*, float x, float y, float z, float scale, float rotation);
	bool UpdateBuffersAnimated(ID3D11DeviceContext*, float x, float y, float z, float scale, float rotation, int frame);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

protected:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;

	int m_previousPosX, m_previousPosY;
	float m_previousRotation, m_previousScale;

	int m_frames, m_columns, m_previousFrame;
	int m_cellWidth, m_cellHeight;
};

