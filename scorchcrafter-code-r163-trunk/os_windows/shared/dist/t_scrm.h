#ifndef __t_scrmer_a__
#define __t_scrmer_a__

#include <cmath>
#include "custdata_types.h"
#include "lp_bworth.h"

class DstTScreamer
{
public:
	DstTScreamer(float inSRate = 44100.0f);
	~DstTScreamer();

	void setSmpRate(float nRate);
	void setGain(const float nGain);
	void clearBuffs();
	fhx applyDistortion(fhx inSample, fhx (gn));
	void setOverSamplingRate(int inp);
	void setQTone(float qSet);

private:
	const fhx zeroF;
	const fhx PI;
	const fhx hPI;
	const fhx PId;
	fhx runTube(fhx iSpl);

	fhx gGain;
	fhx drySamp;
	fhx dsSpl;
	fhx sRate;
	fhx rateFac;
	int oSampling;

	fhx filterPos;
	fhx toneSet;

	BTWorthLP_HD* ScFilter;
};

#endif
