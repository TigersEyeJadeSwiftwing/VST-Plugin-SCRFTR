#ifndef __eq_tstack_a__
#define __eq_tstack_a__

#include <cmath>
#include "lp_bworth.h"
#include "custdata_types.h"
#include "sample_bank.h"

class ToneStack_A
{
public:
	ToneStack_A(fsx nRate = 44100.0);
	~ToneStack_A();

	void setNewSampleRate(fsx nRate);
	void setOverSampling(int inp);
	void setBandValue(const int bNum, const fsx value);
	void SetInput(const fhx inp);
	void RunInternals();
	fhx GetProcessedOutput() {return fhx (SplEQBank->smp[0]);};
	void setEvenShaped(const bool isShaped);
	void ClearBuffs();
	void SetPrecision(int nLevel);

private:
	void RunEQ(const int spNum);

	fhx sRate;
	fhx rateFac;
	int ovSampleRate;
	const fhx zeroF;
	bool setShaped;
	fhx bValue[6];

	SampleBank *SplEQBank;
	int fpPrec;

	fhx EqBand_Freq[5][2];
	fhx EqBand_Pos[5][2];
	fhx subSpl[5];
};

#endif
