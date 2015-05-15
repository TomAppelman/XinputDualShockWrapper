#pragma once

#pragma once
#define WIN32_LEAN_AND_MEAN
#include <wtypes.h>
#include <stdint.h>

// device information
#define DUALSHOCK4_DEVICE_PRODUCT_ID 1476
#define DUALSHOCK4_DEVICE_VENDOR_ID 1356 

__declspec(align(32))
struct DualShock4ControllerData{
	uint32_t Direction : 4;
	uint32_t Buttons : 12;
};

struct DualShock4DPad{ // only one possible 
	enum{
		kUp = 0,
		kUpRight,
		kRight,
		kDownRight,
		kDown,
		kDownLeft,
		kLeft,
		kUpLeft,
		kNone,
		kUnknonwn,
	};
};
struct DualShock4Button{
	enum{
		kSquare = 1 << 0,
		kCross = 1 << 1,
		kCircle = 1 << 2,
		kTriangle = 1 << 3,
		kL1 = 1 << 4,
		kR1 = 1 << 5,
		kL2 = 1 << 6,
		kR2 = 1 << 7,
		kShare = 1 << 8,
		kOptions = 1 << 9,
		kL3 = 1 << 10,
		kR3 = 1 << 11,
		kPS = 1 << 12,
		kUnknown,
	};
};

struct DualShock4State{
	uint8_t		DPad;
	uint16_t	Button;
	BYTE		TriggerLeft;
	BYTE		TriggerRight;
	short		StickLeft[2];
	short		StickRight[2];
	DWORD				packetNum;
	int					test;
};
struct DualShock4Info{
	bool				Connected;
	HANDLE				hDevice;
	HANDLE				hConnection;
	DualShock4State		state;


};

extern int ProcessDS4Raw(DualShock4State& state, unsigned char* pRawData);
extern int ProcessDS4(DualShock4State& state, RAWHID* pRawDualShock4);
