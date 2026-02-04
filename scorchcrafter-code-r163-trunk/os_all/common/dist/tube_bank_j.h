#ifndef __dist_tube_bank_j__
#define __dist_tube_bank_j__

#include "custdata_types.h"

#include "lp_btw.h"
#include "csts_eq.h"
#include "sample_bank_custom_b.h"
#include "tube_params.h"
#include "tstack_params.h"
#include "fir_eq.h"
#include "tstack_wav_j.h"

#ifndef Dist_TB_TubeFreqStages_Max
#define Dist_TB_TubeFreqStages_Max 6
#endif

#define Dist_TB_J_MAX_LAYERS_PRE	12
#define Dist_TB_J_MAX_LAYERS_POWER	12

#define Dist_TB_J_MAX_DRV_Freq_Passes	4

class Tube_Bank_J : public SampleBank_Cst_B , public ToneStack_J
{
public:
	Tube_Bank_J(const fsx spRate, const SC_TubeDistortion_Params initParams, const SC_ToneStack_Params toneStackParams);
	virtual ~Tube_Bank_J();
	void InitTubes();
	void zeroAllSamples();
	void ApplyDistortion();
	void setNSampleRate(fsx nSRate);
	void SetGain(const fsx nGain);
	void clearBuffs();
	void setOverSamplingLevel(const int oSLevel);
	void SetNewFPprecision(const int nLevel) { SampleBank_Cst_B::SetFPprecision(nLevel); ToneStack_J::SetPrecision(nLevel); };
	void RunPreAmp();
	void RunPowerAmp();
	void Run_Input();
	void Run_Output();

private:
	void l_RunTubes_Pre(const int layer);
	void s_RunTubes_Pre(const int layer);
	void h_RunTubes_Pre(const int layer);
	void l_RunTubes_Power(const int layer);
	void s_RunTubes_Power(const int layer);
	void h_RunTubes_Power(const int layer);

	const SC_TubeDistortion_Params params;
	const fhx tbLow;
	const fhx tbHigh;

	const flx l_pre_Hard;
	const fsx s_pre_Hard;
	const fhx h_pre_Hard;
	const flx l_pwr_Hard;
	const fsx s_pwr_Hard;
	const fhx h_pwr_Hard;

	int freqStages;
	Cst_EQ *MidFilter[Dist_TB_TubeFreqStages_Max];

	const fhx h_distFreqHi;
	const fhx h_distFreqLo;
	const fsx s_distFreqHi;
	const fsx s_distFreqLo;
	const flx l_distFreqHi;
	const flx l_distFreqLo;

	const flx l_pre_Grit;
	const fsx s_pre_Grit;
	const fhx h_pre_Grit;

	const flx l_pwr_Grit;
	const fsx s_pwr_Grit;
	const fhx h_pwr_Grit;

	const flx l_pre_Slope;
	const fsx s_pre_Slope;
	const fhx h_pre_Slope;
	const flx l_pwr_Slope;
	const fsx s_pwr_Slope;
	const fhx h_pwr_Slope;

	const flx l_outVol;
	const fsx s_outVol;
	const fhx h_outVol;

	fhx h_Gain;
	fhx h_smp;
	fsx s_Gain;
	fsx s_smp;
	flx l_Gain;
	flx l_smp;

	FIR_Multi_EQ *QCurves[3][6];
	FIR_Multi_EQ *QInput[2];
	FIR_LP_EQ *outFilter;

	flx l_pre_travelTop[Dist_TB_J_MAX_LAYERS_PRE];
	flx l_pre_travelBtm[Dist_TB_J_MAX_LAYERS_PRE];
	flx l_pwr_travelTop[Dist_TB_J_MAX_LAYERS_POWER];
	flx l_pwr_travelBtm[Dist_TB_J_MAX_LAYERS_POWER];
	fsx s_pre_travelTop[Dist_TB_J_MAX_LAYERS_PRE];
	fsx s_pre_travelBtm[Dist_TB_J_MAX_LAYERS_PRE];
	fsx s_pwr_travelTop[Dist_TB_J_MAX_LAYERS_POWER];
	fsx s_pwr_travelBtm[Dist_TB_J_MAX_LAYERS_POWER];
	fhx h_pre_travelTop[Dist_TB_J_MAX_LAYERS_PRE];
	fhx h_pre_travelBtm[Dist_TB_J_MAX_LAYERS_PRE];
	fhx h_pwr_travelTop[Dist_TB_J_MAX_LAYERS_POWER];
	fhx h_pwr_travelBtm[Dist_TB_J_MAX_LAYERS_POWER];

	flx l_pre_TravelBuffer[Dist_TB_J_MAX_LAYERS_PRE];
	flx l_pwr_TravelBuffer[Dist_TB_J_MAX_LAYERS_POWER];
	fsx s_pre_TravelBuffer[Dist_TB_J_MAX_LAYERS_PRE];
	fsx s_pwr_TravelBuffer[Dist_TB_J_MAX_LAYERS_POWER];
	fhx h_pre_TravelBuffer[Dist_TB_J_MAX_LAYERS_PRE];
	fhx h_pwr_TravelBuffer[Dist_TB_J_MAX_LAYERS_POWER];

	Cst_EQ *f_pre_BiasBuffer[Dist_TB_J_MAX_LAYERS_PRE];
	Cst_EQ *f_pwr_BiasBuffer[Dist_TB_J_MAX_LAYERS_POWER];

	flx l_pre_Amp_Mult;
	fsx s_pre_Amp_Mult;
	fhx h_pre_Amp_Mult;
	flx l_pwr_Amp_Mult;
	fsx s_pwr_Amp_Mult;
	fhx h_pwr_Amp_Mult;
};

#endif
