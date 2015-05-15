#pragma once
#include <windows.h>

struct RawInputDeviceType{

};
struct RawInputDevice{
	RawInputDevice(int device_type, HWND hWnd);
	RAWINPUTDEVICE Rid;
};

class RawInput{
public:
	int RegisterDevices(RawInputDevice* pDevices, int DevicesCount);
	int UnRegisterDevices(RawInputDevice* pDevices, int DevicesCount);

	GetRawInputData
private:
};