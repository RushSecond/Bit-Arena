#pragma once

#include <d3dx10math.h>

class CameraClass
{
public:
	CameraClass(void);
	~CameraClass(void);

	void SetPosition(float x, float y, float z);
	void SetPosition(float x, float y);
	void Translate(float x, float y);
	void SetRotation(float, float, float);

	void SetShift(float x, float y);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_shiftX, m_shiftY;
	float m_rotationX, m_rotationY, m_rotationZ;
	D3DXMATRIX m_viewMatrix;
};

