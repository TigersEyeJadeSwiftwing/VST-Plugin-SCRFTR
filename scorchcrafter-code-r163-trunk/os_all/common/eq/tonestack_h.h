#ifndef __SC_eq_tstack_h__
#define __SC_eq_tstack_h__

class RateFC;
class Cst_EQ;

// #include <cmath>
#include "../custdata_types.h"
#include "../eq/lp_btw.h"
#include "../eq/csts_eq.h"
#include "tstack_wav_e.h"
#include "tstack_params.h"

#ifdef BUILDVST_C120
#include "../plugin_c120.h"
#endif
#ifdef BUILDVST_GLZ60
#include "../plugin_glz60.h"
#endif

#define SC_NUM_TSTACK_H_IMPULSES_AMP 6
#define SC_NUM_TSTACK_H_IMPULSES_CAB 6
#define SC_NUM_TSTACK_H_IMPULSES 6

class ToneStack_H
{
public:
	ToneStack_H(fsx nRate, const SC_ToneStack_Params inpParams);
	~ToneStack_H();

	void setNewSampleRate(const fsx nRate);
	void setBandValue(const int bNum, fsx value);
	void SetInput(const fsx inp);
	void l_SetInput(const flx inp) { l_spl = (flx) inp; };
	void s_SetInput(const fsx inp) { s_spl = (fsx) inp; };
	void h_SetInput(const fhx inp) { h_spl = (fhx) inp; };
	fsx GetProcessedOutput();
	flx GetProcessedOutputL() { return (flx) l_spl; };
	fsx GetProcessedOutputS() { return (fsx) s_spl; };
	fhx GetProcessedOutputH() { return (fhx) h_spl; };
	flx l_GetProcessedOutput() { return (flx) l_spl; };
	fsx s_GetProcessedOutput() { return (fsx) s_spl; };
	fhx h_GetProcessedOutput() { return (fhx) h_spl; };
	void setEvenShaped(const bool isShaped);
	void setCabMic(const bool inp) { CabMic = inp; };
	void ClearBuffs();
	void SetPrecision(int nLevel);
	void RunEQ();
	void RunConv();
	void RunConvAmp() { if (setShaped) RunConvolutionAmp(); };
	void RunConvCabMic() { if (CabMic) RunConvolutionCab(); };
	void SetDynamicQ(const fsx inp);
	void l_SetDynamicQ(const flx inp);
	void s_SetDynamicQ(const fsx inp);
	void h_SetDynamicQ(const fhx inp);
	void EnableDynamicQ(bool inp) { dynQenable = inp; };
	void SetAmpShapeNum(int inp) { curAmp=inp; if(inp>SC_NUM_TSTACK_H_IMPULSES_AMP) inp=SC_NUM_TSTACK_H_IMPULSES_AMP-1; };
	void SetCabMicNum(int inp) { curCab=inp; if(inp>SC_NUM_TSTACK_H_IMPULSES_CAB) inp=SC_NUM_TSTACK_H_IMPULSES_CAB-1; };

private:
	void CreateToneImpulses();
	void RunConvolutionAmp();
	void RunConvolutionCab();

	const fhx cShiftPcnt;
	const fhx cSkewPcnt;
	int currImpShift;

	SC_ToneStack_Params prms;

	RateFC *rfV;

	Cst_EQ *DynamicQ;
	Cst_EQ *qBand[6];
	Cst_EQ *DynBass;
	Cst_EQ *TrebleRec;

	fhx h_DynEnvPos;
	fhx h_DynEnvRange;
	fhx h_DynEnvOffset;
	fhx h_DynEnvCurve;
	fsx s_DynEnvPos;
	fsx s_DynEnvRange;
	fsx s_DynEnvOffset;
	fsx s_DynEnvCurve;
	flx l_DynEnvPos;
	flx l_DynEnvRange;
	flx l_DynEnvOffset;
	flx l_DynEnvCurve;

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

	int impLength[SC_NUM_TSTACK_H_IMPULSES];

	int smpPosA;
	int smpPosC;

	flx volAdjA_l[SC_NUM_TSTACK_H_IMPULSES];
	fsx volAdjA_s[SC_NUM_TSTACK_H_IMPULSES];
	fhx volAdjA_h[SC_NUM_TSTACK_H_IMPULSES];

	flx volAdjC_l[SC_NUM_TSTACK_H_IMPULSES];
	fsx volAdjC_s[SC_NUM_TSTACK_H_IMPULSES];
	fhx volAdjC_h[SC_NUM_TSTACK_H_IMPULSES];
};

#endif
