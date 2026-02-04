#ifndef __SC_toneStack_param_struct__
#define __SC_toneStack_param_struct__

#include "../custdata_types.h"

using namespace ScMath;

struct SC_ToneStack_Params
{
	long ImpLenAmpI;
	long ImpLenCabI;
	fhx ImpLenAmpF;
	fhx ImpLenCabF;
	fhx ImpStartSmpRate;
	int ImpSyncPowerFac;
	fhx ImpTailPadFac;
	fhx convShift;
	fhx DCskew;

	fhx qb_a_low;
	fhx qb_a_mid;
	fhx qb_a_high;
	fhx qb_a_cntA;
	fhx qb_a_cntB;
	fhx qb_a_pres;

	fhx qb_b_low;
	fhx qb_b_mid;
	fhx qb_b_high;
	fhx qb_b_cntA;
	fhx qb_b_cntB;
	fhx qb_b_pres;

	fhx DynamicToneRange;
	fhx DynamicToneOffset;
	fhx DynamicToneCurve;
	fsx DynamicToneTravelFreq;
	fsx DynamicTonePassFreq;
	fsx DynamicToneFloorFreq;
};

#endif
