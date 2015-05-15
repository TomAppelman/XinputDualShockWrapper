#include "Dualshock4.h"
#include <math.h>

__forceinline float AnalogNormal(int pos, int min_val, int max_val){
	return (2.f * (pos - min_val) / (max_val - min_val)) - 1.f;
}
__forceinline float AnalogNormalUnsigned(int pos, int min_val, int max_val){
	return (float(pos) - float(min_val)) / (float(max_val) - float(min_val));
}
__forceinline short SAnalogNormal(const short& pos, const short& min_val, const short& max_val){
	return (2 * (pos - min_val) / (max_val - min_val)) - 1;
}


__forceinline float Clamp(const float& value, const float& min_value, const float& max_value){
	float result = value;
	if (value > max_value)result = max_value;
	if (value < min_value)result = min_value;
	return result;
}

__forceinline short SClamp(const short& value, const short& min_value, const short& max_value){
	short result = value;
	if (value > max_value)result = max_value;
	if (value < min_value)result = min_value;
	return result;
}

const short Ramp(const short& value, const short& min_value, const short& max_value){
	short result = value;
	if (value > min_value && value < max_value){
		result = max_value;
	}
	else if (value < -min_value && value > -max_value){
		result = -max_value;
	}
	else if ((value > 0 && value < min_value) || (value < 0 && value > -min_value)){
		result = 0;
	}
	return result;
}
// custom data structor for getting button andf dpad info
__declspec(align(32))
struct ControllerData{
	uint32_t Direction : 4;
	uint32_t Buttons : 12;
};

__forceinline int ProcessDS4Raw(DualShock4State& state, unsigned char* rawdata){
	ControllerData* cd = (ControllerData*)&rawdata[5];
	//const short stickRange = 128;

	//state.StickLeft[0] = SClamp(SAnalogNormal(-stickRange + rawdata[1], -stickRange, stickRange), -128, 128) * 32767;
	//state.StickLeft[1] = SClamp(SAnalogNormal(-stickRange + rawdata[2], -stickRange, stickRange), -128, 128) * 32767;
	///state.StickRight[0] = SClamp(SAnalogNormal(-stickRange + rawdata[3], -stickRange, stickRange), -128, 128) * 32767;
	//state.StickRight[1] = SClamp(SAnalogNormal(-stickRange + rawdata[4], -stickRange, stickRange), -128, 128) * 32767;
	//const short stickRangeStep = (1 / 128) * 32767;
	state.StickLeft[0] = SClamp(Ramp(-128 + rawdata[1], 10, 30), -128, 128) * 255;
	state.StickLeft[1] = -SClamp(Ramp(-128 + rawdata[2], 10, 30), -128, 128) * 255;
	state.StickRight[0] = SClamp(Ramp(-128 + rawdata[3], 10, 30), -128, 128) * 255;
	state.StickRight[1] = -SClamp(Ramp(-128 + rawdata[4], 10, 30), -128, 128) * 255;

	state.TriggerLeft = rawdata[8]; //AnalogNormalUnsigned(rawdata[8], 0, 255);
	state.TriggerRight = rawdata[9]; //AnalogNormalUnsigned(rawdata[9], 0, 255);
	state.DPad = cd->Direction;
	state.Button = cd->Buttons;
	return true;
}
int ProcessDS4(DualShock4State& state, RAWHID* pRawDualShock4){
	RAWHID* hid = pRawDualShock4;
	return ProcessDS4Raw(state, (unsigned char*)&hid->bRawData[0]);
}

