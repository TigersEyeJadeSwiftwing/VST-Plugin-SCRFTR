#ifndef __dist_tube_bank_f__
#define __dist_tube_bank_f__

// class SampleBank_Cst;
// class Cst_EQ;

#include "custdata_types.h"
#include "lp_bworth.h"
#include "sample_bank_custom.h"
#include "csts_eq.h"
// #include "tube_stage.h"

#ifdef BUILDVST_C120
#include "../plugin_c120.h"
#endif
#ifdef BUILDVST_GLZ60
#include "../plugin_glz60.h"
#endif

#ifdef BUILDPLUG_C120
#include "../plugin_c120.h"
#endif
#ifdef BUILDPLUG_GLZ60
#include "../plugin_glz60.h"
#endif
#ifdef BUILDPLUG_JYK
#include "../plugin_jykkr.h"
#endif

class Tube_Bank_F : public SampleBank_Cst
{
public:
	Tube_Bank_F(fsx spRate, fhx mdfr);
	~Tube_Bank_F();
	void InitTubes();
	void zeroAllSamples();
	void ApplyDistortion();
	void setSampleRate(fsx nSRate);
	void SetGain(const fsx nGain);
	void clearBuffs();
	void SetChannel(const int nChan);
	void setOverSampling(const int oSLevel);
	void SetAmpVt(const fsx inp);
	void RunPreAmp();
	void RunPowerAmp();

private:
	const flx l_mpHard;
	const fsx s_mpHard;
	const fhx h_mpHard;
	const flx l_mprHard;
	const fsx s_mprHard;
	const fhx h_mprHard;

	Cst_EQ *FilterA;
	Cst_EQ *FilterB;
	Cst_EQ *FilterC;

	const fhx stMod;
	const fhx h_distFreqHi;
	const fhx h_distFreqLo;
	const fsx s_distFreqHi;
	const fsx s_distFreqLo;
	const flx l_distFreqHi;
	const flx l_distFreqLo;

	const fhx h_tRatio;
	const fsx s_tRatio;
	const flx l_tRatio;

	const flx l_Pre_Grit;
	const fsx s_Pre_Grit;
	const fhx h_Pre_Grit;

	const flx l_Power_Grit;
	const fsx s_Power_Grit;
	const fhx h_Power_Grit;

	const flx l_Slope_Pre;
	const fsx s_Slope_Pre;
	const fhx h_Slope_Pre;
	const flx l_Slope_Power;
	const fsx s_Slope_Power;
	const fhx h_Slope_Power;

	int currChannel;

	bool waveSide;

	fhx h_Gain;
	fhx h_smp;
	const fhx h_pi;
	fhx h_dfltA;
	fhx h_dfltB;
	fhx h_qFiltT[Dist_TB_TubeFreqStages];
	fhx h_eff;

	fsx s_Gain;
	fsx s_smp;
	const fsx s_pi;
	fsx s_dfltA;
	fsx s_dfltB;
	fsx s_qFiltT[Dist_TB_TubeFreqStages];
	fsx s_eff;

	flx l_Gain;
	flx l_smp;
	const flx l_pi;
	flx l_dfltA;
	flx l_dfltB;
	flx l_qFiltT[Dist_TB_TubeFreqStages];
	flx l_eff;

	fhx h_md;
	fsx s_md;
	flx l_md;

	fhx h_sv;
	fsx s_sv;
	flx l_sv;

	fhx mdV;

	flx l_PwrFeedback;
	fsx s_PwrFeedback;
	fhx h_PwrFeedback;

	Cst_EQ *PowerFiltA;
	Cst_EQ *PowerFiltB;
	Cst_EQ *PreFiltA;
	Cst_EQ *PreFiltB;

	flx l_PreAmpMult[4];
	fsx s_PreAmpMult[4];
	fhx h_PreAmpMult[4];
	flx l_PwrAmpMult[4];
	fsx s_PwrAmpMult[4];
	fhx h_PwrAmpMult[4];

	flx l_spdFilter;
	fsx s_spdFilter;
	fhx h_spdFilter;
};

#endif
