#include "Mutex.h"

Mutex::Mutex(){
	m_hMutex = NULL;
}
Mutex::~Mutex(){

}
bool Mutex::Open(const wchar_t* name){
	WaitForSingleObject(m_hMutex,INFINITE);
	m_hMutex = CreateMutex(0, 0, name);
	return (m_hMutex != NULL);
}
void Mutex::Close(){
	ReleaseMutex(m_hMutex);
}