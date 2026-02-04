#ifndef __SC_Impulse_Internal_A__
#define __SC_Impulse_Internal_A__

#include "../custdata_types.h"
#include "../eq/lp_btw.h"
#include "../eq/csts_eq.h"
#include "imp_wavs_a.h"
#include "tstack_params.h"

#ifdef BUILDVST_C120
#include "../plugin_c120.h"
#endif
#ifdef BUILDVST_GLZ60
#include "../plugin_glz60.h"
#endif

#define SC_NUM_IMPULSE_A_IMPULSES_CAB 6
#define SC_NUM_IMPULSE_A_IMPULSES 6

class Impulse_Internal_A
{
public:
	Impulse_Internal_A(fsx nRate, const SC_ToneStack_Params inpParams);
	~Impulse_Internal_A();

	void setNewSampleRate(const fsx nRate);
	void l_SetInput(const flx inp) { l_spl = (flx) inp; };
	void s_SetInput(const fsx inp) { s_spl = (fsx) inp; };
	void h_SetInput(const fhx inp) { h_spl = (fhx) inp; };
	flx l_GetProcessedOutput() { return (flx) l_spl; };
	fsx s_GetProcessedOutput() { return (fsx) s_spl; };
	fhx h_GetProcessedOutput() { return (fhx) h_spl; };
	void setCabMic(const bool inp) { CabMic = inp; };
	void ClearBuffs();
	void SetPrecision(int nLevel);
	void RunConvolution() { if (CabMic) RunConvolutionCab(); };
	void SetCabMicNum(int inp) { curCab=inp; if(inp>SC_NUM_IMPULSE_A_IMPULSES_CAB) inp=SC_NUM_IMPULSE_A_IMPULSES_CAB-1; };

private:
	void CreateToneImpulses();
	void RunConvolutionCab();

	const fhx cShiftPcnt;
	const fhx cSkewPcnt;
	int currImpShift;

	SC_ToneStack_Params prms;

	RateFC *rfV;

	flx l_spl;
	fsx s_spl;
	fhx h_spl;

	fhx sRate;
	bool CabMic;
	int curCab;

	flx *smpLcab;
	fsx *smpScab;
	fhx *smpHcab;

	int spLengthCab;

	int fpPrec;

	flx **impulseLcab;
	fsx **impulseScab;
	fhx **impulseHcab;

	int impLength[SC_NUM_IMPULSE_A_IMPULSES];

	int smpPosC;

	flx volAdjC_l[SC_NUM_IMPULSE_A_IMPULSES];
	fsx volAdjC_s[SC_NUM_IMPULSE_A_IMPULSES];
	fhx volAdjC_h[SC_NUM_IMPULSE_A_IMPULSES];
};

#endif
