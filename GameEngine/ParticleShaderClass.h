#pragma once

#include "TextureShaderClass.h"

class ParticleShaderClass : public TextureShaderClass
{

public:
	ParticleShaderClass(void);
	~ParticleShaderClass(void);

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int vertexCount, int indexCount, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);
	void RenderShader(ID3D11DeviceContext*, int, int);

};
