#include "custom_types.h"
#include "os_bank.h"
#include "eq.h"

#ifdef __cplusplus
using namespace std;
#endif

static void SCr_SampleBank_CalculateRates (SCr_SampleBank_t bnk)
{
	bnk.smpRateOS = bnk.smpRateReal * (f64) bnk.overSampleFactor;
	bnk.upSampleGain = (f64) bnk.overSampleFactor;

	SCr_Eq_ChangeRate(bnk.filterUp, bnk.smpRateOS * 0.250);
	SCr_Eq_ChangeRate(bnk.filterDown, bnk.smpRateOS * 0.250);
}

void SCr_SampleBank_ClearBuffers (SCr_SampleBank_t bnk)
{
	SCr_Eq_ClearBuffers(bnk.filterUp);
	SCr_Eq_ClearBuffers(bnk.filterDown);

	int sampleCnt;
	for (sampleCnt = 0; sampleCnt < SCr_OverSampling_MaxFactor; sampleCnt++)
		bnk.sample[sampleCnt] = 0.0;
}
void SCr_SampleBank_Init (SCr_SampleBank_t bnk, const f64 sampleRate)
{
	int sampleCnt;
	for (sampleCnt = 0; sampleCnt < SCr_OverSampling_MaxFactor; sampleCnt++)
		bnk.sample[sampleCnt] = 0.0;

	bnk.smpRateReal = fmax(sampleRate, 44100.0);
	bnk.overSampleFactor = 1;

	SCr_Eq_Init(bnk.filterUp, 5.0, bnk.smpRateReal, bnk.smpRateReal, SCr_OverSampling_FilterPasses);
	SCr_Eq_Init(bnk.filterDown, 5.0, bnk.smpRateReal, bnk.smpRateReal, SCr_OverSampling_FilterPasses);

	SCr_SampleBank_CalculateRates(bnk);
}
void SCr_SampleBank_ChangeSampleRate (SCr_SampleBank_t bnk, const f64 newRate)
{
	SCr_SampleBank_ClearBuffers(bnk);

	bnk.smpRateReal = fmax(newRate, 44100.0);

	SCr_SampleBank_CalculateRates(bnk);
}
void SCr_SampleBank_ChangeOsFactor (SCr_SampleBank_t bnk, const int newOverSampleFactor)
{
	SCr_SampleBank_ClearBuffers(bnk);
	bnk.overSampleFactor = min(newOverSampleFactor, SCr_OverSampling_MaxFactor);

	SCr_SampleBank_CalculateRates(bnk);
}
void SCr_SampleBank_RunUpSample (SCr_SampleBank_t bnk)
{
	if (bnk.overSampleFactor < 2) return;

	int smpCount;
	for (smpCount = 1; smpCount < bnk.overSampleFactor; smpCount++)
		bnk.sample[smpCount] = 0.0;

	for (smpCount = 0; smpCount < bnk.overSampleFactor; smpCount++) {
		bnk.filterUp.sample = bnk.sample[smpCount];
		SCr_Eq_RunLowPass(bnk.filterUp);
		bnk.sample[smpCount] = bnk.filterUp.sample * bnk.upSampleGain;
	}
}
void SCr_SampleBank_RunDownSample (SCr_SampleBank_t bnk)
{
	if (bnk.overSampleFactor < 2) return;

	int smpCount;
	for (smpCount = 0; smpCount < bnk.overSampleFactor; smpCount++) {
		bnk.filterDown.sample = bnk.sample[smpCount];
		SCr_Eq_RunLowPass(bnk.filterUp);
		bnk.sample[smpCount] = bnk.filterDown.sample;
	}
}
