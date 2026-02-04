#ifndef __scorchc_slot__
#define __scorchc_slot__

#include "custom_types.h"
#include "eq.h"

#include "components.h"
#include "heads.h"
#include "fx.h"

typedef struct SCr_Slot_s {
	SCr_Slots currentContents;
	f64 sampleL;
	f64 sampleR;
	int runL;
	int runR;
	f64 rate;

	SCr_Tube_Alpha_t Tube_A;
} SCr_Slot_t;

void SCr_Slot_Init(SCr_Slot_t slt, const f64 smpRate);
void SCr_Slot_ChangeContents(SCr_Slot_t slt, const SCr_Slots nSlotType);
void SCr_Slot_Run(SCr_Slot_t slt);
void SCr_Slot_SetParameter(SCr_Slot_t slt, const int param, const f64 val);
void SCr_Slot_ClearBuffers(SCr_Slot_t slt);
void SCr_Slot_ChangeSampleRate(SCr_Slot_t slt, const f64 nRate);

#endif
