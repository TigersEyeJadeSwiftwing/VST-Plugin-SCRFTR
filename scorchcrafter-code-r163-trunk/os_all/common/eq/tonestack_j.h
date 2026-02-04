#ifndef __SC_eq_tstack_J__
#define __SC_eq_tstack_J__

#include "../custdata_types.h"
#include "../eq/lp_btw.h"
#include "../eq/csts_eq.h"
#include "tstack_params.h"
#include "tstack_wav_j.h"

#ifdef BUILDVST_C120
#include "../plugin_c120.h"
#endif
#ifdef BUILDVST_GLZ60
#include "../plugin_glz60.h"
#endif

#define SC_NUM_TSTACK_J_DEFS_AMP 6
#define SC_NUM_TSTACK_J_DEFS_CAB 6
#define SC_NUM_TSTACK_J_DEFS 6

class ToneStack_J
{
public:
	ToneStack_J(const fsx nRate, const SC_ToneStack_Params inpParams);
	~ToneStack_J();

	void setNewSampleRate(const fhx nRate);
	void setBandValue(const int bNum, fsx value);
	void l_SetInput(const flx inp) { l_spl = (flx) inp; };
	void s_SetInput(const fsx inp) { s_spl = (fsx) inp; };
	void h_SetInput(const fhx inp) { h_spl = (fhx) inp; };
	flx l_GetProcessedOutput() { return (flx) l_spl; };
	fsx s_GetProcessedOutput() { return (fsx) s_spl; };
	fhx h_GetProcessedOutput() { return (fhx) h_spl; };
	void setEvenShaped(const bool isShaped);
	void ClearBuffs();
	void SetPrecision(int nLevel);
	void RunEQ_Controls();
	void RunEQ_Input();
	void RunEQ_Output();
	void l_SetDynamicQ(const flx inp);
	void s_SetDynamicQ(const fsx inp);
	void h_SetDynamicQ(const fhx inp);
	void EnableDynamicQ(bool inp) { dynQenable = inp; };
	void SetAmpShapeNum(int inp) { curAmp=inp; if(inp>SC_NUM_TSTACK_J_DEFS_AMP) inp=SC_NUM_TSTACK_J_DEFS_AMP-1; };
	void SetAmpBright(const bool inp) { bright = inp; };

	int curAmp;
	bool setShaped;
	bool bright;

private:
	SC_ToneStack_Params prms;

//	Cst_EQ *AmpHeadEQ[SC_NUM_TSTACK_J_DEFS_AMP][TStack_Max_Elements];
	BTW_LP *AmpHeadEQA[SC_NUM_TSTACK_J_DEFS_AMP][TStack_Max_Elements];
	BTW_LP *AmpHeadEQB[SC_NUM_TSTACK_J_DEFS_AMP][TStack_Max_Elements];
	Cst_EQ *AmpInputEQ[2][TStack_Max_Elements];

	Cst_EQ *DynamicQ;
	Cst_EQ *qBand[6];
	Cst_EQ *DynBass;
	Cst_EQ *StaticQBand[2];
	Cst_EQ *DynamicHard[2];

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

	fhx ToneStRate;
	// bool setShaped;
	bool dynQenable;
	// bool bright;

	// int curAmp;
	int fpPrec;

	flx l_bValue[6];
	fsx s_bValue[6];
	fhx h_bValue[6];
};

#endif
