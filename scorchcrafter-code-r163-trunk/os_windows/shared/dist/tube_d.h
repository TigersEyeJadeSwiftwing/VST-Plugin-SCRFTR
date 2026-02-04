#ifndef __dist_tube_d__
#define __dist_tube_d__

#include <cmath>
#include "custdata_types.h"
#include "lp_bworth.h"

class DstTubeD
{
public:
	DstTubeD(fsx inSRate);
	~DstTubeD();

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

	fhx *dBuff[2];
	const fhx dBuffFac;
	fhx dBuffRate;
	s9x dBuffSize;
	s9x dBuffPos[2];
	s9x dBuffDelayed[2];
	s9x dBuffOffSet;

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
};

#endif
