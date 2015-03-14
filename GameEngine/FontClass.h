// FontClass.h
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html

#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include "textureclass.h"
using namespace std;

class FontClass
{
private:

// The FontType structure is used to hold the indexing data read in from the font index file.
// The left and right are the TU texture coordinates. The size is the width of the character in pixels.

	struct FontType
	{
		float left, right;
		int size;
	};

// The VertexType structure is for the actual vertex data used to build the square to render the text character on.
// The individual character will require two triangles to make a square. Those triangles will have position and texture data only.

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	FontClass(void);
	~FontClass(void);

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	TextureClass* m_Texture;
};

