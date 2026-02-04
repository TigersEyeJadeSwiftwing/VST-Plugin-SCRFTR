#ifndef __eq_tstack_b__
#define __eq_tstack_b__

#include <cmath>
#include "lp_bworth.h"
#include "../custdata_types.h"
// #include "sample_bank_fp.h"

#define SC_TSTACK_C_IMPULSE_MAX_I 500
#define SC_TSTACK_C_IMPULSE_MAX_F 500.0
#define SC_NUM_TSTACK_C_IMPULSES 2

class ToneStack_C
{
public:
	ToneStack_C(fsx nRate = 44100.0);
	~ToneStack_C();

	void setNewSampleRate(const fsx nRate);
	void setBandValue(const int bNum, fsx value);
	void SetInput(const fsx inp);
	fsx GetProcessedOutput();
	void setEvenShaped(const bool isShaped);
	void setCabMic(const bool inp) {CabMic = inp;};
	void ClearBuffs();
	void SetPrecision(int nLevel);
	void RunEQ();
	void RunConv();
	void SetDynamicQ(const fsx inp);

private:
	void InitToneImpulses();

	RateFC *rfV;

	fhx h_envPos;
	fsx s_envPos;
	flx l_envPos;

	flx l_spl;
	fsx s_spl;
	fhx h_spl;

	fhx sRate;
	bool setShaped;
	bool CabMic;

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

	int impLength[SC_NUM_TSTACK_C_IMPULSES];

	int smpPos;

	flx l_EqBand_Freq[6][2];
	flx l_EqBand_Pos[6][2];
	flx l_subSpl[6];
	flx volAdj_l[SC_NUM_TSTACK_C_IMPULSES];

	fsx s_EqBand_Freq[6][2];
	fsx s_EqBand_Pos[6][2];
	fsx s_subSpl[6];
	fsx volAdj_s[SC_NUM_TSTACK_C_IMPULSES];

	fhx h_EqBand_Freq[6][2];
	fhx h_EqBand_Pos[6][2];
	fhx h_subSpl[6];
	fhx volAdj_h[SC_NUM_TSTACK_C_IMPULSES];
};

#endif
