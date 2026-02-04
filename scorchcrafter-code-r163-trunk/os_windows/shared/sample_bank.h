#ifndef __shared_sample_bank__
#define __shared_sample_bank__

#include <cmath>
#include "custdata_types.h"
#include "lp_bworth.h"

class SampleBank
{
public:
	SampleBank(fsx iSrate = 44100.0);
	~SampleBank();
	void setSampleRate(fsx nSRate);
	void zeroAllSamples();
	void setOverSampling(int oSLevel);
	void runUpSampling();
	void runDownSampling();
	void SetFPprecision(const int nLevel);

	fhx smp[16];

private:
	BTWorthLP *UpFilter[5];
	BTWorthLP *DnFilter[5];

	const fhx zeroF;
	fhx sRate;
	int osRate;
	int precisionLevel;
};

class SampleBank_SD
{
public:
	SampleBank_SD(float iSrate);
	~SampleBank_SD();
	void setSampleRate(float nSRate);
	void zeroAllSamples();
	void setOverSampling(int oSLevel);
	void runUpSampling();
	void runDownSampling();

	fsx smp[16];

private:
	BTWorthLP_SD *UpFilter[5];
	BTWorthLP_SD *DnFilter[5];

	const fsx zeroF;
	fsx sRate;
	int osRate;
};

class SampleBank_HD
{
public:
	SampleBank_HD(float iSrate);
	~SampleBank_HD();
	void setSampleRate(float nSRate);
	void zeroAllSamples();
	void setOverSampling(int oSLevel);
	void runUpSampling();
	void runDownSampling();

	fhx smp[16];

private:
	BTWorthLP_HD *UpFilter[5];
	BTWorthLP_HD *DnFilter[5];

	const fhx zeroF;
	fhx sRate;
	int osRate;
};

#endif
