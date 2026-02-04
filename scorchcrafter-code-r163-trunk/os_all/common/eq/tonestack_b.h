#ifndef __eq_tstack_b__
#define __eq_tstack_b__

#include <cmath>
#include "lp_bworth.h"
#include "../custdata_types.h"
// #include "sample_bank.h"

class ToneStack_A
{
public:
	ToneStack_A(fsx nRate = 44100.0);
	~ToneStack_A();

	void setNewSampleRate(const fsx nRate);
	void setBandValue(const int bNum, fsx value);
	void SetInput(const fsx inp);
	fsx GetProcessedOutput();
	void setEvenShaped(const bool isShaped);
	void ClearBuffs();
	void SetPrecision(int nLevel);
	void RunEQ();

private:
	void InitToneImpulses();

	flx l_spl;
	fsx s_spl;
	fhx h_spl;

	fhx sRate;
	bool setShaped;

	flx l_bValue[6];
	fsx s_bValue[6];
	fhx h_bValue[6];

	flx *smpL;
	fsx *smpS;
	fhx *smpH;

	int spLength;

	int fpPrec;

	flx **impulseL;
	fsx **impulseS;
	fhx **impulseH;

	int impLength[6];

	int smpPos;
};

#endif
