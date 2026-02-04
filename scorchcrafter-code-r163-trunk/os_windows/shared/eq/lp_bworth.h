#ifndef __shared_eq_bworthLP__
#define __shared_eq_bworthLP__

#include <cmath>
#include "custdata_types.h"

class BTWorthLP
{
public:
	BTWorthLP(fsx inpFreq, fsx inpSRate);
	~BTWorthLP() {};
//	BTWorthLP& operator=(const BTWorthLP& rh);

	void setSampleRate(const fsx inpSRate);
	void reset();
	void zeroBuffers();
	void changeFreq(fsx nFreq);
	flx runFilter(flx inp);
	fsx runFilter(fsx inp);
	fhx runFilter(fhx inp);

private:
	const fhx PI;
	const fhx hPI;
	const fhx PId;
	const fhx zeroF;

	fhx sRate;
	fhx cutOff;

	fhx sT[4];
	fhx sCoef[4];
	fhx sHist[4];
	fhx wp;
	fhx sGain;
};

class BTWorthLP_SD
{
public:
	BTWorthLP_SD(fsx inpFreq, float inpSRate);
	~BTWorthLP_SD() {};
//	BTWorthLP& operator=(const BTWorthLP& rh);

	void setSampleRate(const float inpSRate);
	void reset();
	void zeroBuffers();
	void changeFreq(fsx nFreq);
	fsx runFilter(fsx inp);

private:
	const fsx PI;
	const fsx hPI;
	const fsx PId;
	const fsx zeroF;

	fsx sRate;
	fsx cutOff;

	fsx sT[4];
	fsx sCoef[4];
	fsx sHist[4];
	fsx wp;
	fsx sGain;
};

class BTWorthLP_HD
{
public:
	BTWorthLP_HD(fhx inpFreq, float inpSRate);
	~BTWorthLP_HD() {};
//	BTWorthLP& operator=(const BTWorthLP& rh);

	void setSampleRate(const float inpSRate);
	void reset();
	void zeroBuffers();
	void changeFreq(fhx nFreq);
	fhx runFilter(fhx inp);

private:
	const fhx PI;
	const fhx hPI;
	const fhx PId;
	const fhx zeroF;

	fhx sRate;
	fhx cutOff;

	fhx sT[4];
	fhx sCoef[4];
	fhx sHist[4];
	fhx wp;
	fhx sGain;
};

#endif
