#include "slot.h"
#include "custom_types.h"

#ifdef __cplusplus
using namespace std;
#endif

void SCr_Slot_Init (SCr_Slot_t slt, const f64 smpRate)
{
	slt.currentContents = SCr_slot_Empty;
	slt.sampleL = 0.0;
	slt.sampleR = 0.0;
	slt.runL = 1;
	slt.runR = 1;
	slt.rate = smpRate;

	SCr_Tube_A_Init(slt.Tube_A, 8.0, slt.rate, 1.0);
}
void SCr_Slot_ChangeContents (SCr_Slot_t slt, const SCr_Slots nSlotType)
{
	if (slt.currentContents == nSlotType) return;

	slt.currentContents = nSlotType;
	SCr_Slot_ClearBuffers(slt);
}
void SCr_Slot_Run (SCr_Slot_t slt)
{
	if (slt.currentContents == SCr_slot_Empty) return;
	if ( (!slt.runL) && (!slt.runR) ) return;

	const f64 smpM = SCmath_Average(slt.sampleL, slt.sampleR);
	const f64 smpL = slt.sampleL;
	const f64 smpR = slt.sampleR;
	f64 outL;
	f64 outR;
	f64 outM;
	if (slt.currentContents == SCr_slotComponents_Tube_A) {
		if (slt.runL && slt.runR) outM = smpM;
		else if (slt.runL) outM = smpL;
		else if (slt.runR) outM = smpR;

		outM = SCr_Tube_A_Run(slt.Tube_A, outM);
		outL = outR = outM;
	} else {
		outL = smpL;
		outR = smpR;
	}
	slt.sampleL = outL;
	slt.sampleR = outR;
}
void SCr_Slot_SetParameter (SCr_Slot_t slt, const int param, const f64 val)
{
	if (param >= SCr_Params_Per_Slot) return;
}
void SCr_Slot_ClearBuffers (SCr_Slot_t slt)
{
	slt.sampleL = 0.0;
	slt.sampleR = 0.0;

	if (slt.currentContents == SCr_slotComponents_Tube_A)
		SCr_Tube_A_ZeroBuffers(slt.Tube_A);
}
void SCr_Slot_ChangeSampleRate (SCr_Slot_t slt, const f64 nRate)
{
	slt.rate = nRate;
	if (slt.rate < 44100.0)
		slt.rate = 44100.0;

	SCr_Tube_A_SetRate(slt.Tube_A, slt.rate);

	SCr_Slot_ClearBuffers(slt);
}
