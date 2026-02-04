#ifndef __scorch_cr_c_SampleBank__
#define __scorch_cr_c_SampleBank__

#include "custom_types.h"
#include "eq.h"

typedef struct SCr_SampleBank_s {
	f64 sample[SCr_OverSampling_MaxFactor];
	int overSampleFactor;
	f64 smpRateReal;
	f64 smpRateOS;
	f64 upSampleGain;

	SCr_Eq_t filterUp;
	SCr_Eq_t filterDown;
} SCr_SampleBank_t;

void SCr_SampleBank_ClearBuffers(SCr_SampleBank_t bnk);
void SCr_SampleBank_Init(SCr_SampleBank_t bnk, const f64 sampleRate);
void SCr_SampleBank_ChangeSampleRate(SCr_SampleBank_t bnk, const f64 newRate);
void SCr_SampleBank_ChangeOsFactor(SCr_SampleBank_t bnk, const int newOverSampleFactor);
void SCr_SampleBank_RunUpSample(SCr_SampleBank_t bnk);
void SCr_SampleBank_RunDownSample(SCr_SampleBank_t bnk);

#endif
