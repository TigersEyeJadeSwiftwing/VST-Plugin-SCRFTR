#ifndef __module_jykwrakker__
#define __module_jykwrakker__

#include <cmath>
#include "custdata_types.h"
#include "jyk_config.h"
#include "lp_bworth.h"
#include "sample_bank.h"
#include "tube_x_a.h"
// #include "tube_x_b.h"

enum intrnl_param_jykwrakker
{
	jykParam_Gain,
	jykParam_MAX_COUNT
};

class MdJykwrakker
{
public:
	MdJykwrakker(fsx insRate);
	~MdJykwrakker();

	void inputSample(const fhx inp);
	fhx getOutSample();

	void changeSampleRate(const fsx nRate);
	void ClearBuffers();

	void setInternalParam(const int prm, float value);
	void setPrecision(const int nLevel);
	void setOverSampleRate(const int vLevel);

	void setBypass(const bool bypassEngage);

	void RunInternals(const int dChannel);

private:
	const fhx zeroF;

	SampleBank *Smp;

	BTWorthLP *inFiltL;
	BTWorthLP *inFiltH;
	BTWorthLP *inFiltCap;

	fhx splB;

	int FPUlevel;
	int overSampling;
	fsx smplRate;
	bool moduleIsOnline;

	float iParam[jykParam_MAX_COUNT];

	DstTubeXa *DrStage[4][MAX_DIST_STAGES];
};

#endif
