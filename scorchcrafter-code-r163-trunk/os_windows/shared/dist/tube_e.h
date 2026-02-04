#ifndef __dist_tube_e__
#define __dist_tube_e__

#include <cmath>
#include "custdata_types.h"
#include "lp_bworth.h"

class DstTubeE
{
public:
	DstTubeE(fsx inSRate);
	~DstTubeE();

	void setSmpRate(fsx nRate);
	void setGain(const fsx nGain);
	void clearBuffs();
	fhx applyDistortion(fhx inSample, const fhx gn);
	void setOverSamplingRate(int inp);

private:
	const fhx zeroF;
	const fhx PI;
	const fhx hPI;
	const fhx PId;
	const fhx lwCutFreq;
	fhx runTube(fhx iSpl);
	fhx DirtySine(fhx inp);

	BTWorthLP *dsFilter;
	// BTWorthLP_HD *dsFilterL;

	fhx gGain;
	fhx dryPos;
	fhx drySamp;
	fhx lastPoint;
	fhx srPoint;
	fhx tubePullFac;
	fhx dsSpl;
	fhx dEffect;
	fhx dgDist;
	fhx distP[2];
	fhx sRate;
	fhx rateFac;
	int oSampling;

	fhx filterPos;
	fhx leanT;
	fhx leanB;
	fhx lean;
	fhx leanFac;

	fhx iFiltFreq;
	fhx iFiltPos;

	bool topSide;

// new
	fhx leanFrq;
	fhx tbAtt;
};

#endif
