#include "WrapperMain.h"
#include "Thread.h"
#include <Windows.h>
#include <stdio.h>
#include <wchar.h>

//
InputInterface g_inputInterface;
Thread inputThread;

//
HWND hInputWnd = NULL;


//
int CreateTestWindow();
DWORD WINAPI InputThreadCallback(LPVOID pParameter);
LRESULT CALLBACK TestWndMsgCallback(HWND handle, UINT MessageID, WPARAM wParam, LPARAM lParam);


extern "C" VOID WINAPI reset(){
}
VOID InitInstance(HINSTANCE instance){

	inputThread.Open(InputThreadCallback,0);
}
VOID ExitInstance(){
	g_inputInterface.Shutdown();
	inputThread.WaitAndClose();
}

extern "C" BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved){
	UNREFERENCED_PARAMETER(lpReserved);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		InitInstance(hModule);
		break;
	case DLL_PROCESS_DETACH:
		ExitInstance();
		break;
	}

	return TRUE;
}
int appGetLastError(){
	DWORD err = GetLastError();
	wchar_t* Error = 0;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err, 0, (LPTSTR)&Error, 0, NULL) == 0)
	{
		return 0;
	}

	// Display message.
	MessageBox(0, Error, L"Error", MB_ICONEXCLAMATION | MB_OK);

	// Free the buffer.
	if (Error){
		LocalFree(Error);
		Error = 0;
	}
	return 1;
}
int	CreateTestWindow(){
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.cbWndExtra = NULL;
	wc.hInstance = GetModuleHandle(0);
	wc.lpszClassName = L"INPUTWND";
	wc.lpfnWndProc = TestWndMsgCallback;

	ATOM result = RegisterClassEx(&wc);
	if (!result){
		appGetLastError();
		return false;
	}

	hInputWnd = CreateWindowEx(0, L"INPUTWND", L"", 0, 0, 0, 0, 0, NULL, 0, GetModuleHandle(0), 0);
	if (!hInputWnd){
		appGetLastError();
		return false;
	}
	UpdateWindow(hInputWnd);
	ShowWindow(hInputWnd, SW_HIDE);
	return true;
}
LRESULT CALLBACK TestWndMsgCallback(HWND hWnd, UINT MessageID, WPARAM wParam, LPARAM lParam){
	
	switch (MessageID){
	default:break;
	case WM_DESTROY:
		return 1;
	case WM_INPUT:
		g_inputInterface.HandleRawInput((HRAWINPUT*)&lParam);
		break;
	case WM_INPUT_DEVICE_CHANGE:
		if (wParam == GIDC_ARRIVAL){
			g_inputInterface.DeviceAdded((HANDLE)lParam);
		}
		if (wParam == GIDC_REMOVAL){
			g_inputInterface.DeviceRemoved((HANDLE)lParam);
		}
		break;
	}
	return DefWindowProc(hWnd, MessageID, wParam, lParam);
}
DWORD WINAPI InputThreadCallback(LPVOID pParameter){
	MSG msg;
	if (!CreateTestWindow()){
		return false;
	}
	if (!g_inputInterface.Init(hInputWnd)){
		return false;
	}
	while (GetMessage(&msg, NULL, 0, 0) > 0){
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}