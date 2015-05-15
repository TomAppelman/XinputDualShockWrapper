#include "InputInterface.h"
#include <Windows.h>
#include <stdio.h>

InputInterface::InputInterface(){

}
InputInterface::~InputInterface(){

}

//
int InputInterface::Init(HWND hWnd){
	RAWINPUTDEVICE Rid[1];

	// this enabled joystick detection
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x05;
	Rid[0].dwFlags = RIDEV_DEVNOTIFY;
	Rid[0].hwndTarget = hWnd;


	//  register input
	if (!RegisterRawInputDevices(Rid, 1, sizeof(RAWINPUTDEVICE))){
		// failed to register 
		return 0;
	}
	m_DualShockMutex.Open();
	ZeroMemory(&m_DualShock4Info[0], sizeof(DualShock4Info) * 4);
	m_DualShockMutex.Close();
	return true;
}
void InputInterface::Shutdown(){
	
	m_DualShockMutex.Open();
	// close connections
	for (unsigned int i = 0; i < 4; i++){
		if (m_DualShock4Info[i].Connected){
			SetDualShock4LightBarColor(i, 0, 0, 0);
			SetDualShock4Rumble(i, 0, 0);
			CloseHandle(m_DualShock4Info[i].hConnection);
			m_DualShock4Info[i].hConnection = NULL;
		}
	}
	m_DualShockMutex.Close();
	
	// this does init but removes all devices
	RAWINPUTDEVICE Rid[1];
	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x05;
	Rid[0].dwFlags = RIDEV_REMOVE;
	Rid[0].hwndTarget = 0;

	RegisterRawInputDevices(Rid, 1, sizeof(RAWINPUTDEVICE));
}
int	InputInterface::HandleRawInput(HRAWINPUT* pRawData){
	if (pRawData == NULL){
		return false;
	}
	
	UINT dwSize;
	GetRawInputData(*pRawData, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));


	BYTE lpb[4096] = { 0 };// = new BYTE[dwSize];
	//if (lpb == NULL)
	//{
		//return 0;
	//}
	int readSize = GetRawInputData((HRAWINPUT)*pRawData, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
	if (readSize != dwSize){
		OutputDebugString(L"ERROR:  GetRawInputData didn't return correct size!\n" ) ; 
		return 0;
	}
	
	RAWINPUT* pRaw = (RAWINPUT*)lpb;
	if (pRaw->header.dwType == RIM_TYPEHID){
		RID_DEVICE_INFO device_info = { 0 };
		unsigned int SizeInBytes = sizeof(RID_DEVICE_INFO);
		m_DualShockMutex.Open();
		for (unsigned int i = 0; i < 4; i++){
			if (m_DualShock4Info[i].hDevice == pRaw->header.hDevice && m_DualShock4Info[i].Connected){
				const int index = i;
				DualShock4State state = { 0 };
				ProcessDS4(state, &pRaw->data.hid);
				

				break;
			}
		}
		m_DualShockMutex.Close();
		
	}
	return true;
}

//
void InputInterface::DeviceAdded(HANDLE hDevice){
	RID_DEVICE_INFO device_info = { 0 };
	uint32_t DeviceVendorID = 0;
	uint32_t DeviceProductID = 0;
	unsigned int SizeInBytes = sizeof(RID_DEVICE_INFO);

	GetRawInputDeviceInfo(hDevice, RIDI_DEVICEINFO, &device_info, &SizeInBytes);
	DeviceVendorID = device_info.hid.dwVendorId;
	DeviceProductID = device_info.hid.dwProductId;

	if (DeviceVendorID == DUALSHOCK4_DEVICE_VENDOR_ID &&
		DeviceProductID == DUALSHOCK4_DEVICE_PRODUCT_ID)
	{

		int controller_index = -1;
		for (unsigned int i = 0; i < 4; i++){
			m_DualShockMutex.Open();
			if (!m_DualShock4Info[i].Connected){
				wchar_t DeviceName[256] = L"";
				unsigned int DeviceNameLenght = 256;

				GetRawInputDeviceInfo(hDevice, RIDI_DEVICENAME, &DeviceName, &DeviceNameLenght);
				HANDLE HIDHandle = CreateFile(DeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
				m_DualShock4Info[i].hDevice = hDevice;
				m_DualShock4Info[i].Connected = true;
				m_DualShock4Info[i].hConnection = HIDHandle;
				if (i == 0){
					SetDualShock4LightBarColor(i, 0, 0, 1);
				}
				if (i == 1){
					SetDualShock4LightBarColor(i, 0, 1, 0);
				}
				if (i == 2){
					SetDualShock4LightBarColor(i, 1, 0, 0);
				}
				if (i == 3){
					SetDualShock4LightBarColor(i, 1, 1, 0);
				}
				ZeroMemory(&m_DualShock4Info[i].state, sizeof(DualShock4State));
				//m_DualShock4Info[i].state.packetNum = 0;
				//PollDualShock4State(i, m_DualShock4Info[i].state);
				controller_index = i;
				break;
			}
		}
		m_DualShockMutex.Close();
	}
}
void InputInterface::DeviceRemoved(HANDLE hDevice){
	int controller_index = -1;
	m_DualShockMutex.Open();
	for (unsigned int i = 0; i < 4; i++){
		if (m_DualShock4Info[i].hDevice == hDevice){
			CloseHandle(m_DualShock4Info[i].hConnection);
			m_DualShock4Info[i].hDevice = 0;
			m_DualShock4Info[i].Connected = false;
			m_DualShock4Info[i].hConnection = NULL;
			controller_index = i;
		}
	}
	m_DualShockMutex.Close();
}

// dualshock 4
int	InputInterface::GetDualShock4State(const int index, DualShock4State& state){
	m_DualShockMutex.Open();
	bool connected = m_DualShock4Info[index].Connected;
	if (connected){
		

		if (m_DualShock4Info[index].state.packetNum == 0 ||
			state.Button != m_DualShock4Info[index].state.Button ||
			state.DPad != m_DualShock4Info[index].state.DPad ||
			state.StickLeft[0] != m_DualShock4Info[index].state.StickLeft[0] ||
			state.StickLeft[1] != m_DualShock4Info[index].state.StickLeft[1] ||
			state.StickRight[0] != m_DualShock4Info[index].state.StickRight[0] ||
			state.StickRight[1] != m_DualShock4Info[index].state.StickRight[1] ||
			state.TriggerLeft != m_DualShock4Info[index].state.TriggerLeft ||
			state.TriggerRight != m_DualShock4Info[index].state.TriggerRight)
		{
			m_DualShock4Info[index].state.packetNum++;
			state.packetNum = m_DualShock4Info[index].state.packetNum;
			m_DualShock4Info[index].state.Button = state.Button;
			m_DualShock4Info[index].state.DPad = state.DPad;
			m_DualShock4Info[index].state.StickLeft[0] = state.StickLeft[0];
			m_DualShock4Info[index].state.StickLeft[1] = state.StickLeft[1];
			m_DualShock4Info[index].state.StickRight[0] = state.StickRight[0];
			m_DualShock4Info[index].state.StickRight[1] = state.StickRight[1];
			m_DualShock4Info[index].state.TriggerLeft = state.TriggerLeft;
			m_DualShock4Info[index].state.TriggerRight = state.TriggerRight;

		}
	}
	m_DualShockMutex.Close();
	return connected;
}

int	InputInterface::PollDualShock4State(const int index, DualShock4State& state){
	m_DualShockMutex.Open();
	if (!m_DualShock4Info[index].Connected){
		m_DualShockMutex.Close();
		return false;
	}
	int result = false;
	unsigned char reportData[78] = { 0 };
	DWORD bytes_written = 0;
	if (!ReadFile(m_DualShock4Info[index].hConnection, &reportData, sizeof(reportData), &bytes_written, NULL)){
		state = m_DualShock4Info[index].state;
		m_DualShockMutex.Close();
		return false;
	}
	//ZeroMemory(&state, sizeof(DualShock4State));
	ProcessDS4Raw(state, reportData);
	
	
	if (m_DualShock4Info[index].state.packetNum == 0 ||
		state.Button != m_DualShock4Info[index].state.Button ||
		state.DPad != m_DualShock4Info[index].state.DPad ||
		state.StickLeft[0] != m_DualShock4Info[index].state.StickLeft[0] ||
		state.StickLeft[1] != m_DualShock4Info[index].state.StickLeft[1] ||
		state.StickRight[0] != m_DualShock4Info[index].state.StickRight[0] ||
		state.StickRight[1] != m_DualShock4Info[index].state.StickRight[1] ||
		state.TriggerLeft != m_DualShock4Info[index].state.TriggerLeft ||
		state.TriggerRight != m_DualShock4Info[index].state.TriggerRight)
	{
		m_DualShock4Info[index].state.packetNum++;
		state.packetNum = m_DualShock4Info[index].state.packetNum;
		m_DualShock4Info[index].state.Button = state.Button;
		m_DualShock4Info[index].state.DPad = state.DPad;
		m_DualShock4Info[index].state.StickLeft[0] = state.StickLeft[0];
		m_DualShock4Info[index].state.StickLeft[1] = state.StickLeft[1];
		m_DualShock4Info[index].state.StickRight[0] = state.StickRight[0];
		m_DualShock4Info[index].state.StickRight[1] = state.StickRight[1];
		m_DualShock4Info[index].state.TriggerLeft = state.TriggerLeft;
		m_DualShock4Info[index].state.TriggerRight = state.TriggerRight;
	}


	state.packetNum = m_DualShock4Info[index].state.packetNum;
	m_DualShockMutex.Close();
	return true;
}
bool InputInterface::IsDualShock4Connected(const int index){
	if (index > 4){
		return false;
	}
	m_DualShockMutex.Open();
	bool result = m_DualShock4Info[index].Connected;
	m_DualShockMutex.Close();
	return result;
}
int	InputInterface::SetDualShock4Rumble(const int index, float LeftMotor, float RightMotor){
	m_DualShockMutex.Open();
	if (!m_DualShock4Info[index].Connected){
		m_DualShockMutex.Close();
		return false;
	}
	DWORD byte_written = 0;
	uint8_t reportData[32] = { 0 };
	reportData[0] = 0x05;
	reportData[1] = 0xF1;
	reportData[4] = (char)RightMotor * 255;         // 0-255
	reportData[5] = (char)LeftMotor * 255;       // 0-255
	//printf("motor volume : %i, %i \n", reportData[4], reportData[5]);
	if (!WriteFile(m_DualShock4Info[index].hConnection, &reportData, sizeof(reportData), &byte_written, NULL)){
		//printf("Rumble failed\n");
		m_DualShockMutex.Close();
		return false;
	}
	m_DualShockMutex.Close();
	return true;
}
int InputInterface::SetDualShock4LightBarColor(const int index, float Red, float Green, float Blue){
	m_DualShockMutex.Open();
	if (!m_DualShock4Info[index].Connected){
		m_DualShockMutex.Close();
		return false;
	}

	DWORD byte_written = 0;
	uint8_t reportData[32] = { 0 };
	reportData[0] = 0x05;
	reportData[1] = 0xff & ~0xf1;
	reportData[6] = (char)Red * 255;
	reportData[7] = (char)Green * 255;
	reportData[8] = (char)Blue * 255;

	if (!WriteFile(m_DualShock4Info[index].hConnection, &reportData, sizeof(reportData), &byte_written, NULL)){
		m_DualShockMutex.Close();
		return false;
	}
	m_DualShockMutex.Close();
	return false;
}
