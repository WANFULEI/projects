#ifndef time_elapsed_h__
#define time_elapsed_h__

#ifdef _WIN32
#include "windows.h"

class time_elapsed
{
public:
	time_elapsed(){ QueryPerformanceFrequency(&m_start_freq); start(); }
	void start(){ QueryPerformanceCounter(&m_start_time); }
	int stop(){
		LARGE_INTEGER end_time;
		QueryPerformanceCounter(&end_time);
		return(((end_time.QuadPart - m_start_time.QuadPart) * 1e6) 
			/ m_start_freq.QuadPart);
	}

private:
	LARGE_INTEGER m_start_freq;
	LARGE_INTEGER m_start_time;
};
#else
#endif

#endif // time_elapsed_h__
