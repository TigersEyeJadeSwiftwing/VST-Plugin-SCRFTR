#ifndef __dist_tube_b__
#define __dist_tube_b__

#include <cmath>
#include "custdata_types.h"
#include "lp_bworth.h"

class DstTubeB
{
public:
	DstTubeB(fsx inSRate);
	~DstTubeB();

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
	BTWorthLP *dsFilterD;

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

	fhx dsWave[3];

	fhx lim;
	fhx tgt_drve;
	fhx drvo;
	fhx drve;
	fhx kr;
	fhx kabs;
	fhx trim;
	fhx kb;
	fhx ka;
	fhx m00,m01,m02,m03;
	fhx dcFa, dcFb, dcFfa, dcFfb, fir0;
	fhx lim0;

	bool topSide;
};

#endif
