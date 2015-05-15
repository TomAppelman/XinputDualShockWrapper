#pragma once
#include "Dualshock4.h"
#include "Mutex.h"

class InputInterface{
public:
	InputInterface();
	~InputInterface();
	int Init(HWND hWnd);
	void Shutdown();

	int		HandleRawInput(HRAWINPUT* pRawData);
	void	DeviceAdded(HANDLE hDevice);
	void	DeviceRemoved(HANDLE hDevice);

	// DualShock 4 
	int		GetDualShock4State(const int index, DualShock4State& state);
	int		PollDualShock4State(const int Index, DualShock4State& state);
	bool	IsDualShock4Connected(const int index);
	int		SetDualShock4Rumble(const int index, float LeftMotor, float RightMotor);
	int		SetDualShock4LightBarColor(const int index, float Red, float Green, float Blue);


private:
	Mutex				m_DualShockMutex;
	DualShock4Info		m_DualShock4Info[4];
};