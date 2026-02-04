#ifndef __dist_tube_param_struct__
#define __dist_tube_param_struct__

#include "../custdata_types.h"

using namespace ScMath;

struct SC_TubeDistortion_Params
{
	fhx qSpatialFactor;
	fhx tFreqA;
	fhx tFreqB;
	fhx tFreqC;
	fhx tFreqD;
	int tFreqStages;
	fhx hrd_Pre;
	fhx hrd_Power;
	fhx slope_Pre;
	fhx slope_Power;
	fhx tPr_FreqPre;
	fhx tPr_FreqPower;
	fhx tPr_GritPre;
	fhx tPr_GritPower;
	fhx tToneFilter_Low;
	fhx tToneFilter_High;
	fhx ampMultPre;
	fhx ampMultPower;
	fhx volumeAdj;
	fhx VolumeRectifierAdj;
	int tNumStages_Pre;
	int tNumStages_Power;
	int tFreqPasses;
	int tRectifierPasses;
};

#endif
