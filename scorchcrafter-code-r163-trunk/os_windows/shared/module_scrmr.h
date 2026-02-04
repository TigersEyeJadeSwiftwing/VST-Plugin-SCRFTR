#ifndef __module_screamer__
#define __module_screamer__

#include <cmath>
#include "custdata_types.h"
#include "lp_bworth.h"
#include "sample_bank.h"
#include "t_scrm.h"

enum intrnl_param_screamer
{
	tscream_Gain,
	tscream_Tone,
	tscream_MAX_COUNT
};

class MdTScreamer
{
public:
	MdTScreamer(float insRate = 44100.0f);
	~MdTScreamer();

	void inputSample(const flx inp);
	void inputSample(const fsx inp);
	void inputSample(const fhx inp);
	flx getOutSampleLD();
	fsx getOutSampleSD();
	fhx getOutSampleHD();

	void changeSampleRate(const float nRate);
	void ClearBuffers();

	void setInternalParam(const int prm, float value);
	void setPrecision(const bool setHigh);
	void setOverSampleRate(const int vLevel);

	void setBypass(const bool bypassEngage);

	void RunInternals(float gain, int numTubes);

private:
	const fhx zeroFH;
	const fsx zeroFS;

	SampleBank_SD *SmpS;
	SampleBank_HD *SmpH;

	DstTScreamer* DrStage;

	bool fPUprecisionHigh;
	int overSampling;
	float smplRate;
	bool moduleIsOnline;

	float iParam[tscream_MAX_COUNT];
};

#endif
