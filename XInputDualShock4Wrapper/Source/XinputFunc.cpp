#include "XinputFunc.h"
#include "WrapperMain.h"
#include <string.h>
#include <wchar.h>


extern "C" DWORD WINAPI XInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState){
	ZeroMemory(pState, sizeof(XINPUT_STATE));
	DualShock4State state = { 0 };
	if (!g_inputInterface.IsDualShock4Connected(dwUserIndex)){
		OutputDebugString(L"controller not detected\n");
		return ERROR_DEVICE_NOT_CONNECTED;// ERROR_DEVICE_NOT_CONNECTED;
	}
	
	if (g_inputInterface.GetDualShock4State(dwUserIndex, state)){
		state.test = 0;
		pState->dwPacketNumber = state.packetNum;
		pState->Gamepad.bLeftTrigger = state.TriggerLeft;
		pState->Gamepad.bRightTrigger = state.TriggerRight;
	
		pState->Gamepad.sThumbLX = state.StickLeft[0];
		pState->Gamepad.sThumbLY = state.StickLeft[1];
		pState->Gamepad.sThumbRX = state.StickRight[0];
		pState->Gamepad.sThumbRY = state.StickRight[1];

	
		pState->Gamepad.wButtons = 0;
		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kCross) ? XINPUT_GAMEPAD_A : 0;
		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kCircle) ? XINPUT_GAMEPAD_B : 0;
		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kSquare) ? XINPUT_GAMEPAD_X : 0;
		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kTriangle ) ? XINPUT_GAMEPAD_Y : 0;
	
		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kL1) ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0;
		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kL3) ? XINPUT_GAMEPAD_LEFT_THUMB : 0;

		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kR1) ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0;
		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kR3) ? XINPUT_GAMEPAD_RIGHT_THUMB : 0;

		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kOptions ) ? XINPUT_GAMEPAD_START : 0;
		pState->Gamepad.wButtons |= (state.Button & DualShock4Button::kShare) ? XINPUT_GAMEPAD_BACK : 0;

		if (state.DPad == DualShock4DPad::kUp || state.DPad == DualShock4DPad::kUpLeft || state.DPad == DualShock4DPad::kUpRight){
			pState->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
		}
		if (state.DPad == DualShock4DPad::kDown || state.DPad == DualShock4DPad::kDownLeft || state.DPad == DualShock4DPad::kDownRight){
			pState->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
		}
		if (state.DPad == DualShock4DPad::kRight || state.DPad == DualShock4DPad::kDownRight || state.DPad == DualShock4DPad::kUpRight){
			pState->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
		}
		if (state.DPad == DualShock4DPad::kLeft || state.DPad == DualShock4DPad::kDownLeft || state.DPad == DualShock4DPad::kUpLeft){
			pState->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
		}
	}

	return ERROR_SUCCESS;
}
extern "C" DWORD WINAPI XInputSetState(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration){
	if (!g_inputInterface.IsDualShock4Connected(dwUserIndex)){
		return ERROR_DEVICE_NOT_CONNECTED;
	}
	float left = (float)pVibration->wLeftMotorSpeed * 255;
	float right = (float)pVibration->wRightMotorSpeed * 255;
	g_inputInterface.SetDualShock4Rumble(dwUserIndex, left,right);
	return ERROR_SUCCESS;
}
extern "C" DWORD WINAPI XInputGetCapabilities(DWORD dwUserIndex, DWORD dwFlags, XINPUT_CAPABILITIES* pCapabilities){

	return ERROR_SUCCESS;
}
extern "C" VOID WINAPI XInputEnable(BOOL enable){
	enable = true;
}
extern "C" DWORD WINAPI XInputGetDSoundAudioDeviceGuids(DWORD dwUserIndex, GUID* pDSoundRenderGuid, GUID* pDSoundCaptureGuid){
	return ERROR_SUCCESS;
}
extern "C" DWORD WINAPI XInputGetBatteryInformation(DWORD  dwUserIndex, BYTE devType, XINPUT_BATTERY_INFORMATION* pBatteryInformation){
	return ERROR_SUCCESS;
}
extern "C" DWORD WINAPI XInputGetKeystroke(DWORD dwUserIndex, DWORD dwReserved, XINPUT_KEYSTROKE* pKeystroke){
	return 0;
}
extern "C" DWORD WINAPI XInputGetStateEx(DWORD dwUserIndex, XINPUT_STATE *pState){
	return XInputGetState(dwUserIndex, pState);
}
extern "C" DWORD WINAPI XInputWaitForGuideButton(DWORD dwUserIndex, DWORD dwFlag, LPVOID pVoid){
	return ERROR_SUCCESS;
}
extern "C" DWORD WINAPI XInputCancelGuideButtonWait(DWORD dwUserIndex){
	return ERROR_SUCCESS;
}
extern "C" DWORD WINAPI XInputPowerOffController(DWORD dwUserIndex){
	return ERROR_SUCCESS;
}
extern "C" DWORD WINAPI XInputGetAudioDeviceIds(DWORD dwUserIndex, LPWSTR pRenderDeviceId, UINT* pRenderCount, LPWSTR pCaptureDeviceId, UINT* pCaptureCount){
	return ERROR_SUCCESS;
}
extern "C" DWORD WINAPI XInputGetBaseBusInformation(DWORD dwUserIndex, struct XINPUT_BUSINFO* pBusinfo){
	return ERROR_SUCCESS;
}
extern "C" DWORD WINAPI XInputGetCapabilitiesEx(DWORD unk1, DWORD dwUserIndex, DWORD dwFlags, struct XINPUT_CAPABILITIESEX* pCapabilitiesEx){

	return ERROR_SUCCESS;
}