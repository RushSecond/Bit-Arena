// TimerClass.h
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html

#pragma once

#include <windows.h>

class TimerClass
{
public:
	TimerClass(void);
	~TimerClass(void);

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};

