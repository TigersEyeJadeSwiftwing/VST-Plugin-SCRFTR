#ifndef __SCr_cmp_Tube_a__
#define __SCr_cmp_Tube_a__

#include "../custom_types.h"

#ifndef SCr_Tube_A_DynFreq_L
#define SCr_Tube_A_DynFreq_L	20.0
#endif

#ifndef SCr_Tube_A_DynFreq_H
#define SCr_Tube_A_DynFreq_H	7200.0
#endif

typedef struct SCr_Tube_Alpha_s {
	f64 gain;
	f64 power;
	f64 hardness;
	f64 rate;
	f64 dynHigh;
	f64 dynLow;
	f64 travelA;
	f64 travelB;
	f64 travelBuffer;
	f64 px;
	f64 py;
} SCr_Tube_Alpha_t;

void SCr_Tube_A_Init(SCr_Tube_Alpha_t tb, const f64 powerLevel, const f64 sRate, const f64 hard);
void SCr_Tube_A_SetGain(SCr_Tube_Alpha_t tb, const f64 inp);
void SCr_Tube_A_ZeroBuffers(SCr_Tube_Alpha_t tb);
void SCr_Tube_A_SetRate(SCr_Tube_Alpha_t tb, const f64 nRate);
f64 SCr_Tube_A_Run(SCr_Tube_Alpha_t tb, const f64 inp);

#endif
