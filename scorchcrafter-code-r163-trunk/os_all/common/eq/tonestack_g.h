#ifndef __SC_eq_tstack_g__
#define __SC_eq_tstack_g__

class RateFC;
class Cst_EQ;

#include <cmath>
#include "../eq/lp_btw.h"
#include "../eq/csts_eq.h"
#include "../custdata_types.h"
#include "tstack_wav_e.h"

#ifdef BUILDVST_C120
#include "../plugin_c120.h"
#endif
#ifdef BUILDVST_GLZ60
#include "../plugin_glz60.h"
#endif

#define SC_NUM_TSTACK_IMPULSES 6
#define SC_NUM_TSTACK_E_IMPULSES_CAB SC_NUM_TSTACK_IMPULSES

class ToneStack_G
{
public:
	ToneStack_G(fsx nRate = 44100.0, fsx CnvShift = 0.50, fsx dcSkew = 0.0);
	~ToneStack_G();

	void setNewSampleRate(const fsx nRate);
	void setBandValue(const int bNum, fsx value);
	void SetInputL(const flx inp) {l_spl = inp;};
	void SetInputS(const fsx inp) {s_spl = inp;};
	void SetInputH(const fhx inp) {h_spl = inp;};
	flx GetProcessedOutputL() {return l_spl;};
	fsx GetProcessedOutputS() {return s_spl;};
	fhx GetProcessedOutputH() {return h_spl;};
	void setEvenShaped(const bool isShaped);
	void setCabMic(const bool inp) {CabMic = inp;};
	void ClearBuffs();
	void SetPrecision(int nLevel);
	void RunEQ();
	void RunConv() { return; };
	void SetDynamicQ(const fsx inp);
	void EnableDynamicQ(bool inp) { dynQenable = inp; };
	void SetAmpShapeNum(int inp);
	void SetCabMicNum(int inp) { curCab=inp; if(inp>SC_NUM_TSTACK_E_IMPULSES_CAB) inp=SC_NUM_TSTACK_E_IMPULSES_CAB-1; };

private:
	void InitToneImpulses();
	void RunConvolutionCab();

	flx l_qBmin[6];
	flx l_qBmag[6];
	fsx s_qBmin[6];
	fsx s_qBmag[6];
	fhx h_qBmin[6];
	fhx h_qBmag[6];

	flx l_fixedVol;
	fsx s_fixedVol;
	fhx h_fixedVol;

	const fsx cShiftPcnt;
	const fsx cSkewPcnt;
	int currImpShift;

	RateFC *rfV;

	Cst_EQ *qBand[6];
	Cst_EQ *FixedBand;
	Cst_EQ *DynBass;

	flx l_master;
	fsx s_master;
	fhx h_master;

	fhx h_envPos;
	fsx s_envPos;
	flx l_envPos;

	flx l_spl;
	fsx s_spl;
	fhx h_spl;

	fhx sRate;
	bool setShaped;
	bool CabMic;
	bool dynQenable;

	int curAmp;
	int curCab;

	flx l_bValue[6];
	fsx s_bValue[6];
	fhx h_bValue[6];

	flx *smpLcab;
	fsx *smpScab;
	fhx *smpHcab;

	int spLengthCab;

	int fpPrec;

	flx **impulseLcab;
	fsx **impulseScab;
	fhx **impulseHcab;

	int impLength[SC_NUM_TSTACK_IMPULSES];

	int smpPosC;

	flx volAdjC_l[SC_NUM_TSTACK_IMPULSES];
	fsx volAdjC_s[SC_NUM_TSTACK_IMPULSES];
	fhx volAdjC_h[SC_NUM_TSTACK_IMPULSES];
};

#endif
