#ifndef __SC_eq_tstack_e__
#define __SC_eq_tstack_e__

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

#define SC_NUM_TSTACK_E_IMPULSES_AMP 6
#define SC_NUM_TSTACK_E_IMPULSES_CAB 6
#define SC_NUM_TSTACK_IMPULSES 6

class ToneStack_E
{
public:
	ToneStack_E(fsx nRate = 44100.0, fsx CnvShift = 0.50, fsx dcSkew = 0.0);
	~ToneStack_E();

	void setNewSampleRate(const fsx nRate);
	void setBandValue(const int bNum, fsx value);
	void SetInput(const fsx inp);
	void l_SetInput(const flx inp) { l_spl = inp; };
	void s_SetInput(const fsx inp) { s_spl = inp; };
	void h_SetInput(const fhx inp) { h_spl = inp; };
	fsx GetProcessedOutput();
	flx GetProcessedOutputL() { return l_spl; };
	fsx GetProcessedOutputS() { return s_spl; };
	fhx GetProcessedOutputH() { return h_spl; };
	flx l_GetProcessedOutput() { return l_spl; };
	fsx s_GetProcessedOutput() { return s_spl; };
	fhx h_GetProcessedOutput() { return h_spl; };
	void setEvenShaped(const bool isShaped);
	void setCabMic(const bool inp) {CabMic = inp;};
	void ClearBuffs();
	void SetPrecision(int nLevel);
	void RunEQ();
	void RunConv();
	void SetDynamicQ(const fsx inp);
	void EnableDynamicQ(bool inp) { dynQenable = inp; };
	void SetAmpShapeNum(int inp) { curAmp=inp; if(inp>SC_NUM_TSTACK_E_IMPULSES_AMP) inp=SC_NUM_TSTACK_E_IMPULSES_AMP-1; };
	void SetCabMicNum(int inp) { curCab=inp; if(inp>SC_NUM_TSTACK_E_IMPULSES_CAB) inp=SC_NUM_TSTACK_E_IMPULSES_CAB-1; };

private:
	void InitToneImpulses();
	void CreateToneImpulses();
	void RunConvolutionAmp();
	void RunConvolutionCab();

	static const fhx qBandRange_Low[2];
	static const fhx qBandRange_Mid[2];
	static const fhx qBandRange_High[2];
	static const fhx qBandRange_ContourL[2];
	static const fhx qBandRange_ContourH[2];
	static const fhx qBandRange_Presence[2];

	const fsx cShiftPcnt;
	const fsx cSkewPcnt;
	int currImpShift;

	RateFC *rfV;

	Cst_EQ *qBand[6];
	Cst_EQ *DynBass;
	Cst_EQ *TrebleRec;

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

	flx *smpLamp;
	fsx *smpSamp;
	fhx *smpHamp;
	flx *smpLcab;
	fsx *smpScab;
	fhx *smpHcab;

	int spLengthAmp;
	int spLengthCab;

	int fpPrec;

	flx **impulseLamp;
	fsx **impulseSamp;
	fhx **impulseHamp;
	flx **impulseLcab;
	fsx **impulseScab;
	fhx **impulseHcab;

	int impLength[SC_NUM_TSTACK_IMPULSES];

	int smpPosA;
	int smpPosC;

	flx volAdjA_l[SC_NUM_TSTACK_IMPULSES];
	fsx volAdjA_s[SC_NUM_TSTACK_IMPULSES];
	fhx volAdjA_h[SC_NUM_TSTACK_IMPULSES];

	flx volAdjC_l[SC_NUM_TSTACK_IMPULSES];
	fsx volAdjC_s[SC_NUM_TSTACK_IMPULSES];
	fhx volAdjC_h[SC_NUM_TSTACK_IMPULSES];
};

#endif
