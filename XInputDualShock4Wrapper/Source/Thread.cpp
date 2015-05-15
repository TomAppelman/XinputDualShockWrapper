#include "Thread.h"

Thread::Thread(){
	m_Handle = NULL;
}
Thread::~Thread(){

}
int Thread::Open(LPTHREAD_START_ROUTINE ThreadCallback, LPVOID pParams){
	if (!(m_Handle = CreateThread(0, 0, ThreadCallback, 0, 0, 0))){
		return false;
	}
	return true;
}
void Thread::Wait(){
	WaitForSingleObject(m_Handle, INFINITE);
}
void Thread::Close(){
	CloseHandle(m_Handle);
}
void Thread::WaitAndClose(){
	WaitForSingleObject(m_Handle, INFINITE);
	CloseHandle(m_Handle);
}