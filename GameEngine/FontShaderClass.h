// FontShaderClass.h
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html

#pragma once

#include "TextureShaderClass.h"

class FontShaderClass : public TextureShaderClass
{
private:
	struct PixelBufferType
	{
		D3DXVECTOR4 pixelColor;
	};

public:
	FontShaderClass(void);
	~FontShaderClass(void);

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);

private:

	ID3D11Buffer* m_pixelBuffer;
};

