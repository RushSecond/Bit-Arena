// FpsClass.h
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html

#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "winmm.lib")

//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <mmsystem.h>

class FpsClass
{
public:
	FpsClass(void);
	~FpsClass(void);

	void Initialize();
	void Frame();
	int GetFps();

private:
	int m_fps, m_count;
	unsigned long m_startTime;
};

