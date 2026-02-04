#ifndef __shared_sample_bank_Custom_b__
#define __shared_sample_bank_Custom_b__

class RateFC;

#include "custdata_types.h"
#include "fir_eq.h"

using namespace ScMath;

class SampleBank_Cst_B
{
public:
	SampleBank_Cst_B(fsx iSrate = 44100.0, const int firSize = 20);
	~SampleBank_Cst_B();
	void setSampleRate(fsx nSRate);
	void zeroAllSamples();
	void setOverSampling(const int oSLevel);
	void runUpSampling();
	void runDownSampling();
	void SetFPprecision(const int nLevel);
	void InputSmp(const fsx inp);
	fsx GetOutP();
	flx GetOutPL();
	fsx GetOutPS();
	fhx GetOutPH();

	void l_InputSample(const flx inp) { smpl[0] = inp; };
	void s_InputSample(const fsx inp) { smps[0] = inp; };
	void h_InputSample(const fhx inp) { smph[0] = inp; };

	flx l_GetOutPut() { return (flx) smpl[0]; };
	fsx s_GetOutPut() { return (fsx) smps[0]; };
	fhx h_GetOutPut() { return (fhx) smph[0]; };

	flx smpl[16];
	fsx smps[16];
	fhx smph[16];

	RateFC *rfV;
	int osRate;
	int precisionLevel;

	flx sRateL;
	fsx sRateS;
	fhx sRateH;

	flx cRateL;
	fsx cRateS;
	fhx cRateH;

private:
	FIR_LP_EQ *UpFilter[5];
	FIR_LP_EQ *DnFilter[5];
};

#endif
