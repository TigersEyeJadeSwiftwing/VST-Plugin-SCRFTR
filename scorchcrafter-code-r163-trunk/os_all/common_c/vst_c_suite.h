#ifndef __scorch_cr_c_Suite__
#define __scorch_cr_c_Suite__

#include "custom_types.h"
#include "eq.h"
#include "os_bank.h"

#include "slot.h"

typedef struct SCr_Suite_s {
	f64 smp[2];
	SCr_SampleBank_t Samples[2];
	f64 masterVol;
	f64 inpBoost;
	int overSmpLevelCurrent;
	int overSmpLevelOld;

	SCr_Slot_t slot[SCr_Number_Slots];
} SCr_Suite_t;

void SCr_Suite_Init(SCr_Suite_t *plg, const f64 sRate);
void SCr_Suite_ChangeSampleRate(SCr_Suite_t *plg, const f64 newRate);
void SCr_Suite_InputStereo(SCr_Suite_t *plg, const f64 inL, const f64 inR);
void SCr_Suite_Run(SCr_Suite_t *plg);
void SCr_Suite_ClearBuffers(SCr_Suite_t *plg);
void SCr_Suite_Set_MasterVol(SCr_Suite_t *plg, const f64 inp);
void SCr_Suite_Set_InputBoost(SCr_Suite_t *plg, const f64 inp);
void SCr_Suite_Set_OverSampling(SCr_Suite_t *plg, const int inp);

#endif
