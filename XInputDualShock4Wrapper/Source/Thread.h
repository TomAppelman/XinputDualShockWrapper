#pragma once
#include <wtypes.h>

class Thread{
public:
	Thread();
	~Thread();
	int Open(LPTHREAD_START_ROUTINE ThreadCallback, LPVOID pParams);
	void Close();
	void Wait();
	void WaitAndClose();
	//inline HANDLE GetHandle(){ return m_Handle; };
private:
	HANDLE	m_Handle;
};