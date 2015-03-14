// CpuClass.h
// From http://web.archive.org/web/20140213145557/http://rastertek.com/tutdx11.html

#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "pdh.lib")

//////////////
// INCLUDES //
//////////////
#include <pdh.h>

class CpuClass
{
public:
	CpuClass(void);
	~CpuClass(void);

	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuPercentage();

private:
	bool m_canReadCpu;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime;
	long m_cpuUsage;
};

