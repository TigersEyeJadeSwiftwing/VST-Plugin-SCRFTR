#ifndef __dist_tube_a__
#define __dist_tube_a__

#include <cmath>
#include "custdata_types.h"
#include "lp_bworth.h"

class DstTubeA
{
public:
	DstTubeA(float inSRate);
	~DstTubeA();

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
};

#endif
