#ifndef __dist_tube_c__
#define __dist_tube_c__

#include <cmath>
#include "custdata_types.h"
#include "lp_bworth.h"

class DstTubeC
{
public:
	DstTubeC(float inSRate);
	~DstTubeC();

	void setSmpRate(float nRate);
	void setGain(const float nGain);
	void clearBuffs();
	fhx applyDistortion(fhx inSample, fhx (gn));
	void setOverSamplingRate(int inp);

private:
	const fhx zeroF;
	const fhx PI;
	const fhx hPI;
	const fhx PId;
	fhx runTube(fhx iSpl);

	fhx sBuff[128];
	fhx tBuff[104];
	int sBuffIndex;
	int tBuffIndex;
	fhx cnvFac;

	BTWorthLP_HD *dsFilter;

	fhx gGain;
	fhx dryPos;
	fhx drySamp;
	fhx lastPoint;
	fhx tubePullFac;
	fhx dsSpl;
	fhx dEffect;
	fhx dgDist;
	fhx distP;
	fhx sRate;
	fhx rateFac;
	int oSampling;

	fhx filterPos;
	fhx leanT;
	fhx leanB;
	fhx lean;
	fhx leanFac;

	bool topSide;
};

#endif

