#pragma once
#include <wtypes.h>

class Mutex{
public:
	Mutex();
	~Mutex();
	bool Open(const wchar_t* name = L"");
	void Close();
	//int WaitForRelease();
private:
	HANDLE	m_hMutex;
};