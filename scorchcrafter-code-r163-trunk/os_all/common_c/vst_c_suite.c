#include "vst_c_suite.h"
#include "custom_types.h"
#include "eq.h"
#include "os_bank.h"
#include "slot.h"

#ifdef __cplusplus
using namespace std;
#endif

void SCr_Suite_Init (SCr_Suite_t *plg, const f64 sRate)
{
	SCr_SampleBank_Init(plg->Samples[0], sRate);
	SCr_SampleBank_Init(plg->Samples[1], sRate);

	plg->smp[0] = 0.0;
	plg->smp[1] = 0.0;

	plg->masterVol = 1.0;
	plg->inpBoost = 0.0;
	plg->overSmpLevelCurrent = 1;
	plg->overSmpLevelOld = 1;
	SCr_SampleBank_ChangeOsFactor(plg->Samples[0], plg->overSmpLevelCurrent);
	SCr_SampleBank_ChangeOsFactor(plg->Samples[1], plg->overSmpLevelCurrent);

	int cnt;
	for (cnt = 0; cnt < SCr_Number_Slots; cnt++)
		SCr_Slot_Init(plg->slot[cnt], sRate);
}
void SCr_Suite_ChangeSampleRate (SCr_Suite_t *plg, const f64 newRate)
{
	SCr_SampleBank_ChangeSampleRate(plg->Samples[0], newRate);
	SCr_SampleBank_ChangeSampleRate(plg->Samples[1], newRate);

	int cnt;
	for (cnt = 0; cnt < SCr_Number_Slots; cnt++)
		SCr_Slot_ChangeSampleRate(plg->slot[cnt], newRate*plg->overSmpLevelCurrent);

	plg->smp[0] = 0.0;
	plg->smp[1] = 0.0;
}
void SCr_Suite_InputStereo (SCr_Suite_t *plg, const f64 inL, const f64 inR)
{
	plg->smp[0] = inL * ((plg->inpBoost * 3.0) + 1.0);
	plg->smp[1] = inR * ((plg->inpBoost * 3.0) + 1.0);
}
void SCr_Suite_Run (SCr_Suite_t *plg)
{
	if(plg->overSmpLevelCurrent == 0) return;

	plg->Samples[0].sample[0] = plg->smp[0];
	plg->Samples[1].sample[0] = plg->smp[1];

	SCr_SampleBank_RunUpSample(plg->Samples[0]);
	SCr_SampleBank_RunUpSample(plg->Samples[1]);

	int cnt;
	int slotCount;
	f64 chanL;
	f64 chanR;
	for (cnt = 0; cnt < plg->overSmpLevelCurrent; cnt++) {
		chanL = plg->Samples[0].sample[cnt];
		chanR = plg->Samples[1].sample[cnt];

		for (slotCount = 0; slotCount < SCr_Number_Slots; slotCount++) {
			plg->slot[slotCount].sampleL = chanL;
			plg->slot[slotCount].sampleR = chanR;

			SCr_Slot_Run(plg->slot[slotCount]);

			chanL = plg->slot[slotCount].sampleL;
			chanR = plg->slot[slotCount].sampleR;
		}

		plg->Samples[0].sample[cnt] = chanL;
		plg->Samples[1].sample[cnt] = chanR;
	}

	SCr_SampleBank_RunDownSample(plg->Samples[0]);
	SCr_SampleBank_RunDownSample(plg->Samples[1]);

	plg->smp[0] = plg->Samples[0].sample[0] * plg->masterVol;
	plg->smp[1] = plg->Samples[1].sample[0] * plg->masterVol;
}
void SCr_Suite_ClearBuffers (SCr_Suite_t *plg)
{
	SCr_SampleBank_ClearBuffers(plg->Samples[0]);
	SCr_SampleBank_ClearBuffers(plg->Samples[1]);

	int cnt;
	for (cnt = 0; cnt < SCr_Number_Slots; cnt++)
		SCr_Slot_ClearBuffers(plg->slot[cnt]);

	plg->smp[0] = 0.0;
	plg->smp[1] = 0.0;
}
void SCr_Suite_Set_MasterVol (SCr_Suite_t *plg, const f64 inp)
{
	plg->masterVol = inp;
}
void SCr_Suite_Set_InputBoost (SCr_Suite_t *plg, const f64 inp)
{
	plg->inpBoost = inp;
}
void SCr_Suite_Set_OverSampling (SCr_Suite_t *plg, const int inp)
{
	plg->overSmpLevelCurrent = inp;

	if (plg->overSmpLevelCurrent != plg->overSmpLevelOld) {
		plg->overSmpLevelOld = plg->overSmpLevelCurrent;

		SCr_SampleBank_ChangeOsFactor(plg->Samples[0], plg->overSmpLevelCurrent);
		SCr_SampleBank_ChangeOsFactor(plg->Samples[1], plg->overSmpLevelCurrent);

		int cnt;
		for (cnt = 0; cnt < SCr_Number_Slots; cnt++)
			SCr_Slot_ChangeSampleRate(plg->slot[cnt], inp*plg->overSmpLevelCurrent);

		/* SCr_Suite_ClearBuffers(plg); */
		plg->smp[0] = 0.0;
		plg->smp[1] = 0.0;
	}
}
