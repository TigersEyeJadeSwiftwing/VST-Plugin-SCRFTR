#ifndef __dist_tube_bank_i__
#define __dist_tube_bank_i__

#include "custdata_types.h"
#include "lp_btw.h"
#include "sample_bank_custom.h"
#include "csts_eq.h"
#include "tube_params.h"

#ifndef Dist_TB_TubeFreqStages_Max
#define Dist_TB_TubeFreqStages_Max 8
#endif

#define Dist_TB_I_LAYERS_PRE	3
#define Dist_TB_I_LAYERS_POWER	3

class Tube_Bank_I : public SampleBank_Cst
{
public:
	Tube_Bank_I(const SC_TubeDistortion_Params *initParams, fsx spRate);
	virtual ~Tube_Bank_I();
	void InitTubes();
	void zeroAllSamples();
	void ApplyDistortion();
	void setNSampleRate(fsx nSRate);
	void SetGain(const fsx nGain);
	void clearBuffs();
	void setOverSampling(const int oSLevel);
	void SetFPprecision(const int nLevel) { SampleBank_Cst::SetFPprecision(nLevel); };
	void SetAmpVt(const fsx inp);
	void l_SetAmpVt(const flx inp);
	void s_SetAmpVt(const fsx inp);
	void h_SetAmpVt(const fhx inp);
	void RunPreAmp();
	void RunPowerAmp();

private:
	const SC_TubeDistortion_Params params;
	const flx l_mpHard;
	const fsx s_mpHard;
	const fhx h_mpHard;
	const flx l_mprHard;
	const fsx s_mprHard;
	const fhx h_mprHard;

	Cst_EQ *FilterA;
	Cst_EQ *FilterB;
	Cst_EQ *FilterC;

	BTW_LP *outFilter;

	Cst_EQ *FilterVt;

	const fhx stMod;
	const fhx h_distFreqHi;
	const fhx h_distFreqLo;
	const fsx s_distFreqHi;
	const fsx s_distFreqLo;
	const flx l_distFreqHi;
	const flx l_distFreqLo;

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

	const flx l_outVol;
	const fsx s_outVol;
	const fhx h_outVol;

	const int freqStages;

	fhx h_Gain;
	fhx h_smp;
	const fhx h_pi;
	fhx h_dfltA;
	fhx h_dfltB;
	fhx h_qFiltT[Dist_TB_TubeFreqStages_Max];
	fhx h_eff;

	fsx s_Gain;
	fsx s_smp;
	const fsx s_pi;
	fsx s_dfltA;
	fsx s_dfltB;
	fsx s_qFiltT[Dist_TB_TubeFreqStages_Max];
	fsx s_eff;

	flx l_Gain;
	flx l_smp;
	const flx l_pi;
	flx l_dfltA;
	flx l_dfltB;
	flx l_qFiltT[Dist_TB_TubeFreqStages_Max];
	flx l_eff;

	fhx h_md;
	fsx s_md;
	flx l_md;

	fhx h_sv;
	fsx s_sv;
	flx l_sv;

	flx l_ampVt;
	fsx s_ampVt;
	fhx h_ampVt;
	flx l_ampVo;
	fsx s_ampVo;
	fhx h_ampVo;

	fhx mdV;

	Cst_EQ *PowerFiltA[Dist_TB_I_LAYERS_POWER];
	Cst_EQ *PowerFiltB[Dist_TB_I_LAYERS_POWER];
	Cst_EQ *PreFiltA[Dist_TB_I_LAYERS_PRE];
	Cst_EQ *PreFiltB[Dist_TB_I_LAYERS_PRE];

	flx l_PreAmpMult;
	fsx s_PreAmpMult;
	fhx h_PreAmpMult;
	flx l_PwrAmpMult;
	fsx s_PwrAmpMult;
	fhx h_PwrAmpMult;

	flx l_spdFilter;
	fsx s_spdFilter;
	fhx h_spdFilter;
};

#endif
