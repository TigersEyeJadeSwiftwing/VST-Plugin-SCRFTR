#ifndef __scorch_cr_c_Eq__
#define __scorch_cr_c_Eq__

#include "custom_types.h"

typedef struct SCr_Eq_s {
	int numPasses;
	f64 freqHigh;
	f64 freqLow;
	f64 sample;
	f64 rate;
	f64 high_x;
	f64 high_y;
	f64 high_buf[SCr_EQ_MaxNumPasses];
	f64 low_x;
	f64 low_y;
	f64 low_buf[SCr_EQ_MaxNumPasses];
} SCr_Eq_t;

void SCr_Eq_ClearBuffers(SCr_Eq_t filt);
void SCr_Eq_Init(SCr_Eq_t filt, const f64 bandLow, const f64 bandHigh, const f64 sRate, const int nPasses);
void SCr_Eq_ChangeRate(SCr_Eq_t filt, const f64 sRate);
void SCr_Eq_RunLowPass(SCr_Eq_t filt);
void SCr_Eq_RunHighPass(SCr_Eq_t filt);
void SCr_Eq_RunBothPass(SCr_Eq_t filt);

#endif
