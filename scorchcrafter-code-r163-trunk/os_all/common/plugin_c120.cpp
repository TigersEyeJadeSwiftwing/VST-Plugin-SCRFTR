#include "plugin_c120.h"
#include <algorithm>

using namespace ScMath;

SC_C120_Amp_Head::SC_C120_Amp_Head(double sRt) :
	fadeFac(fsx (SCr_C120_AmpHead_Fade_Factor)),
	smpRate(fsx (sRt)),
	splInputMono(fsx (0.0)),
	splInputLeft(fsx (0.0)),
	splInputRight(fsx (0.0)),
	splOutputMono(fsx (0.0)),
	splOutputLeft(fsx (0.0)),
	splOutputRight(fsx (0.0)),
	l_deNormSuppress(flx (0.10f)),
	s_deNormSuppress(fsx (0.10)),
	h_deNormSuppress(fhx (0.10)),
	l_dNrmThresh(flx (powf(l_C_10, -28.f))),
	s_dNrmThresh(fsx (pow(s_C_10, -28.0))),
	h_dNrmThresh(fhx (powl(h_C_10, (fhx) -30.0))),
#ifdef SC_C120_ENABLE_STEREO
	stereoEnabled(true)
#else
	stereoEnabled(false)
#endif
{
	FpParam[ScPrmF_Master_Volume] = flx (0.50f);
	FpParam[ScPrmF_Drive_Gain] = flx (0.50f);
	FpParam[ScPrmF_EQ_Low] = flx (0.50f);
	FpParam[ScPrmF_EQ_Mid] = flx (0.50f);
	FpParam[ScPrmF_EQ_High] = flx (0.50f);
	FpParam[ScPrmF_EQ_Contour] = flx (0.50f);
	FpParam[ScPrmF_EQ_Presence] = flx (0.50f);

	IntParam[ScPrmI_Active_And_Ready] = 0;
	IntParam[ScPrmI_Error_Status] = 0;
	IntParam[ScPrmI_Drive_Channel] = ampChan = 2;
	IntParam[ScPrmI_FP_Precision_Bit_Depth] = SCv_Bit_Depth_64;
	IntParam[ScPrmI_OverSampling_Rate] = SCv_OverSampling_2x;
	IntParam[ScPrmI_Stereo] = 0;
	IntParam[ScPrmI_Phase_Control] = 0;

	fadeMax = long (fsx (fadeFac) * fsx (smpRate / 44100.0));
	fadeCount = fadeMax;

	fhx init_tFreqA[SC_Number_AmpChannels_C120] = Dist_C120_TB_TubeFreqA;
	fhx init_tFreqB[SC_Number_AmpChannels_C120] = Dist_C120_TB_TubeFreqB;
	fhx init_tFreqC[SC_Number_AmpChannels_C120] = Dist_C120_TB_TubeFreqC;
	fhx init_tFreqD[SC_Number_AmpChannels_C120] = Dist_C120_TB_TubeFreqD;
	int init_tFreqPasses[SC_Number_AmpChannels_C120] = Dist_C120_TB_TubeFreqGritPasses;
	int init_tFreqStages[SC_Number_AmpChannels_C120] = Dist_C120_TB_TubeFreqStages;
	fhx init_hrd_Pre[SC_Number_AmpChannels_C120] = Dist_C120_TB_PreAmp_Hardness;
	fhx init_hrd_Power[SC_Number_AmpChannels_C120] = Dist_C120_TB_PowerAmp_Hardness;
	fhx init_slope_Pre[SC_Number_AmpChannels_C120] = Dist_C120_TB_PreAmp_Slope;
	fhx init_slope_Power[SC_Number_AmpChannels_C120] = Dist_C120_TB_PowerAmp_Slope;
	fhx init_tPr_FreqPre[SC_Number_AmpChannels_C120] = Dist_C120_TB_PreAmp_Frequency;
	fhx init_tPr_FreqPower[SC_Number_AmpChannels_C120] = Dist_C120_TB_PowerAmp_Frequency;
	fhx init_tPr_GritPre[SC_Number_AmpChannels_C120] = Dist_C120_TB_PreAmp_Grit;
	fhx init_tPr_GritPower[SC_Number_AmpChannels_C120] = Dist_C120_TB_POwerAmp_Grit;
	fhx init_tToneFilter_Low[SC_Number_AmpChannels_C120] = Dist_C120_TB_Distortion_ToneFilter_Low;
	fhx init_tToneFilter_High[SC_Number_AmpChannels_C120] = Dist_C120_TB_Distortion_ToneFilter_High;
	fhx init_ampMultPre[SC_Number_AmpChannels_C120] = Dist_C120_TB_AmpMult_Pre;
	fhx init_ampMultPower[SC_Number_AmpChannels_C120] = Dist_C120_TB_AmpMult_Power;
	fhx init_volumeAdj[SC_Number_AmpChannels_C120] = Dist_C120_TB_OutVolume_Adjust;
	fhx init_RectAdj[SC_Number_AmpChannels_C120] = Dist_C120_TB_RectVolume_Adjust;
	int init_Rectifier_Passes[SC_Number_AmpChannels_C120] = Dist_C120_TB_RectifierPasses;
	int init_layers_pre[SC_Number_AmpChannels_C120] = Dist_C120_TB_NumberOfStages_Pre;
	int init_layers_power[SC_Number_AmpChannels_C120] = Dist_C120_TB_NumberOfStages_Power;

	SC_TubeDistortion_Params initCenter;
	SC_TubeDistortion_Params initLeft;
	SC_TubeDistortion_Params initRight;
	initCenter.qSpatialFactor = SCr_C120_AmpHead_QSpatialFactor_Mono;
	initLeft.qSpatialFactor = SCr_C120_AmpHead_QSpatialFactor_Left;
	initRight.qSpatialFactor = SCr_C120_AmpHead_QSpatialFactor_Right;

	fhx rnd_freq[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	fhx rnd_tone[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	fhx rnd_grit[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	fhx rnd_slpe[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	fhx rnd_tube[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	for (int x = 0; x < 6; x++)
	{
		const int r_freq = int (rand() % 2000) - 1000;
		const int r_tone = int (rand() % 2000) - 1000;
		const int r_grit = int (rand() % 2000) - 1000;
		const int r_slpe = int (rand() % 2000) - 1000;
		const int r_tube = int (rand() % 2000) - 1000;

		const fhx rf_freq = fhx (fhx (r_freq) * fhx (0.0010));
		const fhx rf_tone = fhx (fhx (r_tone) * fhx (0.0010));
		const fhx rf_grit = fhx (fhx (r_grit) * fhx (0.0010));
		const fhx rf_slpe = fhx (fhx (r_slpe) * fhx (0.0010));
		const fhx rf_tube = fhx (fhx (r_tube) * fhx (0.0010));

		rnd_freq[x] += fhx ((fhx) rf_freq * (fhx) 0.010 * (fhx) Dist_C120_TB_Freq_Rand_Percent);
		rnd_tone[x] += fhx ((fhx) rf_tone * (fhx) 0.010 * (fhx) Dist_C120_TB_Distortion_ToneFilter_Rand_Percent);
		rnd_grit[x] += fhx ((fhx) rf_grit * (fhx) 0.010 * (fhx) Dist_C120_TB_Grit_Rand_Percent);
		rnd_slpe[x] += fhx ((fhx) rf_slpe * (fhx) 0.010 * (fhx) Dist_C120_TB_Amp_Slope_Rand_Percent);
		rnd_tube[x] += fhx ((fhx) rf_tube * (fhx) 0.010 * (fhx) Dist_C120_TB_TubeFreq_Rand_Percent);
	}

	SC_ToneStack_Params tStackPrm;
//	tStackPrm.ImpSyncPowerFac = SC_C120_TSTACK_IMPULSE_SyncRate_Power_Factor;
//	tStackPrm.ImpStartSmpRate = (fhx) SC_C120_TSTACK_IMPULSE_SAMPLE_RATE;
//	tStackPrm.ImpTailPadFac = (fhx) SC_C120_TSTACK_IMPULSE_Tail_PadLength_Factor;

	tStackPrm.DynamicTonePassFreq = fsx (SCr_C120_TStack_DynBass_Freq_A);
	tStackPrm.DynamicToneFloorFreq = fsx (SCr_C120_TStack_DynBass_Freq_B);
	tStackPrm.DynamicToneTravelFreq = fsx (SCr_C120_TStack_DynBass_Freq_Travel);
	tStackPrm.DynamicToneRange = fhx (SCr_C120_TStack_DynBass_Range);
	tStackPrm.DynamicToneCurve = fhx (SCr_C120_TStack_DynBass_Curve);
	tStackPrm.DynamicToneOffset = fhx (SCr_C120_TStack_DynBass_Offset);

	tStackPrm.qb_a_low = SCr_C120_TStack_EqBand_Start_Low;
	tStackPrm.qb_a_mid = SCr_C120_TStack_EqBand_Start_Mid;
	tStackPrm.qb_a_high = SCr_C120_TStack_EqBand_Start_High;
	tStackPrm.qb_a_cntA = SCr_C120_TStack_EqBand_Start_CntrA;
	tStackPrm.qb_a_cntB = SCr_C120_TStack_EqBand_Start_CntrB;
	tStackPrm.qb_a_pres = SCr_C120_TStack_EqBand_Start_Pres;

	tStackPrm.qb_b_low = SCr_C120_TStack_EqBand_End_Low;
	tStackPrm.qb_b_mid = SCr_C120_TStack_EqBand_End_Mid;
	tStackPrm.qb_b_high = SCr_C120_TStack_EqBand_End_High;
	tStackPrm.qb_b_cntA = SCr_C120_TStack_EqBand_End_CntrA;
	tStackPrm.qb_b_cntB = SCr_C120_TStack_EqBand_End_CntrB;
	tStackPrm.qb_b_pres = SCr_C120_TStack_EqBand_End_Pres;
/*
	const int lngA = SC_C120_TSTACK_IMPULSE_Lengths_Amp;
	tStackPrm.ImpLenAmpI = (long) lngA;
	tStackPrm.ImpLenAmpF = (fhx) lngA;

	const int lngC = SC_C120_TSTACK_IMPULSE_Lengths_Cab;
	tStackPrm.ImpLenCabI = (long) lngC;
	tStackPrm.ImpLenCabF = (fhx) lngC;
*/
	for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
	{
		initCenter.tFreqA = initLeft.tFreqA = initRight.tFreqA = init_tFreqA[x];
		initCenter.tFreqB = initLeft.tFreqB = initRight.tFreqB = init_tFreqB[x];
		initCenter.tFreqC = initLeft.tFreqC = initRight.tFreqC = init_tFreqC[x];
		initCenter.tFreqD = initLeft.tFreqD = initRight.tFreqD = init_tFreqD[x];
		initCenter.tFreqPasses = initLeft.tFreqPasses = initRight.tFreqPasses = init_tFreqPasses[x];
		initCenter.tFreqStages = initLeft.tFreqStages = initRight.tFreqStages = init_tFreqStages[x];
		initCenter.hrd_Pre = initLeft.hrd_Pre = initRight.hrd_Pre = init_hrd_Pre[x];
		initCenter.slope_Pre = initLeft.slope_Pre = initRight.slope_Pre = init_slope_Pre[x];
		initCenter.tPr_FreqPre = initLeft.tPr_FreqPre = initRight.tPr_FreqPre = init_tPr_FreqPre[x];
		initCenter.tPr_GritPre = initLeft.tPr_GritPre = initRight.tPr_GritPre = init_tPr_GritPre[x];
		initCenter.ampMultPre = initLeft.ampMultPre = initRight.ampMultPre = init_ampMultPre[x];
		initCenter.hrd_Power = initLeft.hrd_Power = initRight.hrd_Power = init_hrd_Power[x];
		initCenter.slope_Power = initLeft.slope_Power = initRight.slope_Power = init_slope_Power[x];
		initCenter.tPr_FreqPower = initLeft.tPr_FreqPower = initRight.tPr_FreqPower = init_tPr_FreqPower[x];
		initCenter.tPr_GritPower = initLeft.tPr_GritPower = initRight.tPr_GritPower = init_tPr_GritPower[x];
		initCenter.ampMultPower = initLeft.ampMultPower = initRight.ampMultPower = init_ampMultPower[x];
		initCenter.tToneFilter_Low = initLeft.tToneFilter_Low = initRight.tToneFilter_Low = init_tToneFilter_Low[x];
		initCenter.tToneFilter_High = initLeft.tToneFilter_High = initRight.tToneFilter_High = init_tToneFilter_High[x];
		initCenter.volumeAdj = initLeft.volumeAdj = initRight.volumeAdj = init_volumeAdj[x];
		initCenter.VolumeRectifierAdj = initLeft.VolumeRectifierAdj = initRight.VolumeRectifierAdj = init_RectAdj[x];
		initCenter.tRectifierPasses = initLeft.tRectifierPasses = initRight.tRectifierPasses = init_Rectifier_Passes[x];
		initCenter.tNumStages_Pre = initLeft.tNumStages_Pre = initRight.tNumStages_Pre = init_layers_pre[x];
		initCenter.tNumStages_Power = initLeft.tNumStages_Power = initRight.tNumStages_Power = init_layers_power[x];

		initCenter.tPr_FreqPre *= fhx (rnd_freq[0]);
		initLeft.tPr_FreqPre *= fhx (rnd_freq[1]);
		initRight.tPr_FreqPre *= fhx (rnd_freq[2]);
		initCenter.tPr_FreqPower *= fhx (rnd_freq[3]);
		initLeft.tPr_FreqPower *= fhx (rnd_freq[4]);
		initRight.tPr_FreqPower *= fhx (rnd_freq[5]);

		initCenter.tToneFilter_Low *= fhx (rnd_tone[0]);
		initCenter.tToneFilter_High *= fhx (rnd_tone[1]);
		initLeft.tToneFilter_Low *= fhx (rnd_tone[2]);
		initLeft.tToneFilter_High *= fhx (rnd_tone[3]);
		initRight.tToneFilter_Low *= fhx (rnd_tone[4]);
		initRight.tToneFilter_High *= fhx (rnd_tone[5]);

		initCenter.tPr_GritPre *= fhx (rnd_grit[0]);
		initLeft.tPr_GritPre *= fhx (rnd_grit[1]);
		initRight.tPr_GritPre *= fhx (rnd_grit[2]);
		initCenter.tPr_GritPower *= fhx (rnd_grit[3]);
		initLeft.tPr_GritPower *= fhx (rnd_grit[4]);
		initRight.tPr_GritPower *= fhx (rnd_grit[5]);

		initCenter.slope_Pre *= fhx (rnd_slpe[0]);
		initLeft.slope_Pre *= fhx (rnd_slpe[1]);
		initRight.slope_Pre *= fhx (rnd_slpe[2]);
		initCenter.slope_Power *= fhx (rnd_slpe[3]);
		initLeft.slope_Power *= fhx (rnd_slpe[4]);
		initRight.slope_Power *= fhx (rnd_slpe[5]);

		initCenter.tFreqC *= fhx (rnd_tube[0]);
		initCenter.tFreqD *= fhx (rnd_tube[1]);
		initLeft.tFreqC *= fhx (rnd_tube[2]);
		initLeft.tFreqD *= fhx (rnd_tube[3]);
		initRight.tFreqC *= fhx (rnd_tube[4]);
		initRight.tFreqD *= fhx (rnd_tube[5]);

		if (stereoEnabled)
		{
			PluGin[x][0] = new Tube_Bank_J(fsx (smpRate), initLeft, tStackPrm);
			PluGin[x][1] = new Tube_Bank_J(fsx (smpRate), initRight, tStackPrm);

			PluGin[x][0]->InitTubes();
			PluGin[x][0]->SetNewFPprecision(1);
			PluGin[x][0]->setOverSamplingLevel(1);
			PluGin[x][0]->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));

			PluGin[x][1]->InitTubes();
			PluGin[x][1]->SetNewFPprecision(1);
			PluGin[x][1]->setOverSamplingLevel(1);
			PluGin[x][1]->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));
		} else
		{
			PluGin[x][0] = new Tube_Bank_J(fsx (smpRate), initCenter, tStackPrm);
			PluGin[x][0]->InitTubes();
			PluGin[x][0]->SetNewFPprecision(1);
			PluGin[x][0]->setOverSamplingLevel(1);
			PluGin[x][0]->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));
		}
	}

	for (int x = 0; x < 6; x++)
	{
		CabCurves[0][x] = new FIR_Multi_EQ(smpRate, CabSettings[x]);
		if (stereoEnabled) CabCurves[1][x] = new FIR_Multi_EQ(smpRate, CabSettings[x]);
	}

	int x_rSkew_a = rand() % 20000;
	int x_rSkew_b = rand() % 20000;
	x_rSkew_a -= 10000;
	x_rSkew_b -= 10000;
	fhx x_skwA = fhx (x_rSkew_a);
	fhx x_skwB = fhx (x_rSkew_b);
	x_skwA /= fhx (10000.0);
	x_skwB /= fhx (10000.0);
	x_skwA *= fhx (SCr_C120_AmpHead_RandFactor);
	x_skwB *= fhx (SCr_C120_AmpHead_RandFactor);

	h_rSkew[0] = fhx (x_skwA);
	s_rSkew[0] = fsx (x_skwA);
	l_rSkew[0] = flx (x_skwA);
	h_rSkew[1] = fhx (x_skwB);
	s_rSkew[1] = fsx (x_skwB);
	l_rSkew[1] = flx (x_skwB);

	IntParam[ScPrmI_Active_And_Ready] = 1;
}
SC_C120_Amp_Head::~SC_C120_Amp_Head()
{
	for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
	{
		delete PluGin[x][0];
		if (stereoEnabled) delete PluGin[x][1];
	}
	for (int x = 0; x < 6; x++)
	{
		delete CabCurves[0][x];
		if (stereoEnabled) delete CabCurves[1][x];
	}
}
void SC_C120_Amp_Head::SetNewSampleRate(const double inp)
{
	smpRate = fsx (inp);
	if(fsx (smpRate) < fsx (44100.0)) smpRate = fsx (44100.0);
//	if(fsx (smpRate) > fsx (6144000.0)) smpRate = fsx (6144000.0);	// Maximum Sample Rate to avoid absurd, invalid values

	for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
	{
		PluGin[x][0]->setNSampleRate(fsx (smpRate));
		if (stereoEnabled) PluGin[x][1]->setNSampleRate(fsx (smpRate));
	}

	fadeMax = long (fsx (fadeFac) * fsx (smpRate / 44100.0));
	fadeCount = fadeMax;

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);

	for (int x = 0; x < 6; x++)
	{
		CabCurves[0][x]->ChangeSmpRate(smpRate);
		if (stereoEnabled) CabCurves[1][x]->ChangeSmpRate(smpRate);
	}
}
void SC_C120_Amp_Head::ClearAllBuffers()
{
	for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
	{
		PluGin[x][0]->clearBuffs();
		if (stereoEnabled) PluGin[x][1]->clearBuffs();
	}

	fadeCount = fadeMax;

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);
}
void SC_C120_Amp_Head::SetFpParam(const int prm, float newValue)
{
	if(prm < 0) return;
	if(prm > ScPrmF_MAX_COUNT) return;
	if(flx (newValue) < flx (0.0f)) return;
	if(flx (newValue) > flx (1.0f)) return;

	if(prm == ScPrmF_Master_Volume) FpParam[prm] = flx (newValue);
	if(prm == ScPrmF_Drive_Gain)
	{
		FpParam[prm] = flx (newValue);
		for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
		{
			PluGin[x][0]->SetGain(fsx (newValue));
			if (stereoEnabled) PluGin[x][1]->SetGain(fsx (newValue));
		}
	}
	if(prm == ScPrmF_EQ_Low)
	{
		FpParam[prm] = flx (newValue);
		for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
		{
			PluGin[x][0]->setBandValue(0, fsx (newValue));
			if (stereoEnabled) PluGin[x][1]->setBandValue(0, fsx (newValue));
		}
	}
	if(prm == ScPrmF_EQ_Mid)
	{
		FpParam[prm] = flx (newValue);
		for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
		{
			PluGin[x][0]->setBandValue(1, fsx (newValue));
			if (stereoEnabled) PluGin[x][1]->setBandValue(1, fsx (newValue));
		}
	}
	if(prm == ScPrmF_EQ_High)
	{
		FpParam[prm] = flx (newValue);
		for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
		{
			PluGin[x][0]->setBandValue(2, fsx (newValue));
			if (stereoEnabled) PluGin[x][1]->setBandValue(2, fsx (newValue));
		}
	}
	if(prm == ScPrmF_EQ_Contour)
	{
		FpParam[prm] = flx (newValue);
		for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
		{
			PluGin[x][0]->setBandValue(3, fsx (newValue));
			PluGin[x][0]->setBandValue(4, fsx (newValue));
			if (stereoEnabled) PluGin[x][1]->setBandValue(3, fsx (newValue));
			if (stereoEnabled) PluGin[x][1]->setBandValue(4, fsx (newValue));
		}
	}
	if(prm == ScPrmF_EQ_Presence)
	{
		FpParam[prm] = flx (newValue);
		for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
		{
			PluGin[x][0]->setBandValue(5, fsx (newValue));
			if (stereoEnabled) PluGin[x][1]->setBandValue(5, fsx (newValue));
		}
	}
}
void SC_C120_Amp_Head::SetIntParam(const int prm, int newValue)
{
	if(prm < 0) return;
	if(prm > ScPrmI_MAX_COUNT) return;
	if(newValue < 0) return;

	if(prm == ScPrmI_Error_Status)
	{
		IntParam[ScPrmI_Error_Status] = 0;
		ClearAllBuffers();
	}
	if(prm == ScPrmI_Active_And_Ready)
	{
		if(newValue > 1) return;
		IntParam[prm] = newValue;
		ClearAllBuffers();
	}
	if(prm == ScPrmI_Drive_Channel)
	{
		if(newValue >= SC_Number_AmpChannels_C120) return;

		IntParam[prm] = ampChan = newValue;
		ClearAllBuffers();
	}
	if(prm == ScPrmI_FP_Precision_Bit_Depth)
	{
		if(newValue >= SCv_Bit_Depth_MAX_COUNT) return;

		IntParam[prm] = newValue;
		if(newValue == SCv_Bit_Depth_32)
		{
			for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
			{
				PluGin[x][0]->SetNewFPprecision(0);
				if (stereoEnabled) PluGin[x][1]->SetNewFPprecision(0);
			}
		}
		if(newValue == SCv_Bit_Depth_64)
		{
			for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
			{
				PluGin[x][0]->SetNewFPprecision(1);
				if (stereoEnabled) PluGin[x][1]->SetNewFPprecision(1);
			}
		}
		if(newValue == SCv_Bit_Depth_128)
		{
			for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
			{
				PluGin[x][0]->SetNewFPprecision(2);
				if (stereoEnabled) PluGin[x][1]->SetNewFPprecision(2);
			}
		}
		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_OverSampling_Rate)
	{
		if(newValue >= SCv_OverSampling_MAX_COUNT) return;
		IntParam[prm] = newValue;
		int x = 1;
		if(newValue == SCv_OverSampling_1x) x = 1;
		if(newValue == SCv_OverSampling_2x) x = 2;
		if(newValue == SCv_OverSampling_4x) x = 3;
		if(newValue == SCv_OverSampling_8x) x = 4;
		if(newValue == SCv_OverSampling_16x) x = 5;

		for (int y = 0; y < SC_Number_AmpChannels_C120; y++)
		{
			PluGin[y][0]->setOverSamplingLevel(x);
			if (stereoEnabled) PluGin[y][1]->setOverSamplingLevel(x);
		}

		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_Stereo)
	{
		if(newValue < 0) return;
		if(newValue > 1) return;
		if (stereoEnabled)
		{
			IntParam[prm] = newValue;
			fadeCount = fadeMax;
		}
		else IntParam[prm] = 0;
	}
	if(prm == ScPrmI_Bright)
	{
		IntParam[prm] = newValue;
		fadeCount = fadeMax;

		bool xx = false;
		if (newValue == 1) xx = true;

		for (int y = 0; y < SC_Number_AmpChannels_C120; y++)
		{
			PluGin[y][0]->SetAmpBright(xx);
			if (stereoEnabled) PluGin[y][1]->SetAmpBright(xx);
		}
	}
	if(prm == ScPrmI_CabMic_Active)
	{
		IntParam[prm] = newValue;
		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_CabMic_Num)
	{
		IntParam[prm] = newValue;
		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_TStack_Shape)
	{
		if(newValue == 1)
		{
			for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
			{
				PluGin[x][0]->setEvenShaped(true);
				if (stereoEnabled) PluGin[x][1]->setEvenShaped(true);
			}
		} else
		{
			for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
			{
				PluGin[x][0]->setEvenShaped(false);
				if (stereoEnabled) PluGin[x][1]->setEvenShaped(false);
			}
		}
		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_TStack_ShapeNum)
	{
		for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
		{
			PluGin[x][0]->SetAmpShapeNum(newValue);
			if (stereoEnabled) PluGin[x][1]->SetAmpShapeNum(newValue);
		}
		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_Phase_Control)
		IntParam[prm] = newValue;
	if(prm == ScPrmI_TStack_Dynamic)
	{
		if(newValue == 1)
		{
			for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
			{
				PluGin[x][0]->EnableDynamicQ(true);
				if (stereoEnabled) PluGin[x][1]->EnableDynamicQ(true);
			}
		} else
		{
			for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
			{
				PluGin[x][0]->EnableDynamicQ(false);
				if (stereoEnabled) PluGin[x][1]->EnableDynamicQ(false);
			}
		}
	}
}
void SC_C120_Amp_Head::InputSample(const double inp)
{
	splInputMono = fsx (inp);
	splInputLeft = fsx (inp);
	splInputRight = fsx (inp);
}
void SC_C120_Amp_Head::InputSampleStereo(const double inpL, const double inpR)
{
	splInputLeft = (fsx) inpL;
	splInputRight = (fsx) inpR;
	splInputMono = fsx (fsx (fsx (inpL) + fsx (inpR)) * fsx (0.50));
}
double SC_C120_Amp_Head::GetMonoOut()
{
	return fsx (splOutputMono);
}
double SC_C120_Amp_Head::GetStereoOutLeft()
{
	return fsx (splOutputLeft);
}
double SC_C120_Amp_Head::GetStereoOutRight()
{
	return fsx (splOutputRight);
}
float SC_C120_Amp_Head::GetFpParam(const int prm)
{
	if(prm < 0) return float (0.0f);

	if(prm == ScPrmF_Master_Volume) return float (FpParam[prm]);
	if(prm == ScPrmF_Drive_Gain) return float (FpParam[prm]);
	if(prm == ScPrmF_EQ_Low) return float (FpParam[prm]);
	if(prm == ScPrmF_EQ_Mid) return float (FpParam[prm]);
	if(prm == ScPrmF_EQ_High) return float (FpParam[prm]);
	if(prm == ScPrmF_EQ_Contour) return float (FpParam[prm]);

	return float (0.0f);
}
int SC_C120_Amp_Head::GetIntParam(const int prm)
{
	if(prm < 0) return 0;

	if(prm == ScPrmI_Active_And_Ready) return IntParam[prm];
	if(prm == ScPrmI_Error_Status) return IntParam[prm];
	if(prm == ScPrmI_Drive_Channel) return IntParam[prm];
	if(prm == ScPrmI_FP_Precision_Bit_Depth) return IntParam[prm];
	if(prm == ScPrmI_OverSampling_Rate) return IntParam[prm];

	return 0;
}
long long SC_C120_Amp_Head::GetLongParam(const int prm)
{
	return 0;
}
void SC_C120_Amp_Head::RunPlugin()
{
//! ----------------------------------------------------------------------------------------------------------------
//! 32-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_32)
	{
		l_dNrm = flx ( l_dNrmThresh * l_C_tenthQrt * (flx) l_UglySine(l_deNormSuppress) );
		l_deNormSuppress += flx (l_C_half);
		if (l_deNormSuppress >= l_C_2) l_deNormSuppress -= flx (l_C_4);

		if((IntParam[ScPrmI_Active_And_Ready] == 0) || (IntParam[ScPrmI_OverSampling_Rate] == SCv_OverSampling_OFF))
		{
			fadeCount = fadeMax;
			splOutputMono = fsx (l_dNrm);
			splOutputLeft = fsx (l_dNrm);
			splOutputRight = fsx (l_dNrm);
			return;
		}

		if(IntParam[ScPrmI_Phase_Control] == 0) l_smp[0] = flx (splInputMono);
		else l_smp[0] = flx (-splInputMono);

		l_smp[0] += flx (l_dNrm);
		l_smp[1] = flx (splInputLeft + l_dNrm);
		l_smp[2] = flx (splInputRight + l_dNrm);

		if(IntParam[ScPrmI_Stereo] == 0)
		{
			if (l_abs(l_smp[0] < l_dNrmThresh)) l_smp[0] += l_dNrm;

			PluGin[ampChan][0]->l_InputSample(l_smp[0] + (l_Diode_S(l_smp[0], l_C_1) * l_rSkew[0]));
			PluGin[ampChan][0]->ApplyDistortion();

			l_smp[0] = (flx) PluGin[ampChan][0]->l_GetOutPut();

			if (IntParam[ScPrmI_CabMic_Active])
			{
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->l_Input(l_smp[0]);
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->l_RunQ();
				l_smp[0] = (flx) CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->l_GetOutput();
			}

			l_smp[0] *= flx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (l_smp[0]);
			splOutputLeft = fsx (l_smp[0]);
			splOutputRight = fsx (l_smp[0]);
		} else
		{
			if (l_abs(l_smp[1] < l_dNrmThresh)) l_smp[1] += l_dNrm;
			if (l_abs(l_smp[2] < l_dNrmThresh)) l_smp[2] += l_dNrm;

			PluGin[ampChan][0]->l_InputSample(l_smp[1] + (l_Diode_S(l_smp[1], l_C_1) * l_rSkew[0]));
			PluGin[ampChan][0]->ApplyDistortion();

			PluGin[ampChan][1]->l_InputSample(-l_smp[2] + (l_Diode_S(-l_smp[2], l_C_1) * l_rSkew[1]));
			PluGin[ampChan][1]->ApplyDistortion();

			l_smp[1] = (flx) PluGin[ampChan][0]->l_GetOutPut();
			l_smp[2] = (flx) PluGin[ampChan][1]->l_GetOutPut();

			if (IntParam[ScPrmI_CabMic_Active])
			{
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->l_Input(l_smp[1]);
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->l_RunQ();
				l_smp[1] = (flx) CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->l_GetOutput();

				CabCurves[1][IntParam[ScPrmI_CabMic_Num]]->l_Input(l_smp[2]);
				CabCurves[1][IntParam[ScPrmI_CabMic_Num]]->l_RunQ();
				l_smp[2] = (flx) CabCurves[1][IntParam[ScPrmI_CabMic_Num]]->l_GetOutput();
			}
			l_smp[1] *= flx (FpParam[ScPrmF_Master_Volume]);
			l_smp[2] *= flx (-FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (l_smp[0]);
			splOutputLeft = fsx (l_smp[1]);
			splOutputRight = fsx (l_smp[2]);
		}

		if(fadeCount > 0)
		{
			fadeCount--;
			const flx muteLevel = flx (flx (fadeCount) / flx (fadeMax));
			splOutputMono *= fsx (muteLevel);
			splOutputLeft *= fsx (muteLevel);
			splOutputRight *= fsx (muteLevel);
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 64-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_64)
	{
		s_dNrm = fsx ( s_dNrmThresh * s_C_tenthQrt * (fsx) s_UglySine(s_deNormSuppress) );
		s_deNormSuppress += fsx (s_C_half);
		if (s_deNormSuppress >= s_C_2) s_deNormSuppress -= fsx (s_C_4);

		if((IntParam[ScPrmI_Active_And_Ready] == 0) || (IntParam[ScPrmI_OverSampling_Rate] == SCv_OverSampling_OFF))
		{
			fadeCount = fadeMax;
			splOutputMono = fsx (s_dNrm);
			splOutputLeft = fsx (s_dNrm);
			splOutputRight = fsx (s_dNrm);
			return;
		}

		if(IntParam[ScPrmI_Phase_Control] == 0) s_smp[0] = fsx (splInputMono);
		else s_smp[0] = fsx (-splInputMono);

		s_smp[0] += fsx (s_dNrm);
		s_smp[1] = fsx (splInputLeft + s_dNrm);
		s_smp[2] = fsx (splInputRight + s_dNrm);

		if(IntParam[ScPrmI_Stereo] == 0)
		{
			if (s_abs(s_smp[0] < s_dNrmThresh)) s_smp[0] += s_dNrm;

			PluGin[ampChan][0]->s_InputSample(s_smp[0] + (s_Diode_S(s_smp[0], s_C_1) * s_rSkew[0]));
			PluGin[ampChan][0]->ApplyDistortion();

			s_smp[0] = (fsx) PluGin[ampChan][0]->s_GetOutPut();

			if (IntParam[ScPrmI_CabMic_Active])
			{
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->s_Input(s_smp[0]);
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->s_RunQ();
				s_smp[0] = (fsx) CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->s_GetOutput();
			}
			s_smp[0] *= fsx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (s_smp[0]);
			splOutputLeft = fsx (s_smp[0]);
			splOutputRight = fsx (s_smp[0]);
		} else
		{
			if (s_abs(s_smp[1] < s_dNrmThresh)) s_smp[1] += s_dNrm;
			if (s_abs(s_smp[2] < s_dNrmThresh)) s_smp[2] += s_dNrm;

			PluGin[ampChan][0]->s_InputSample(s_smp[1] + (s_Diode_S(s_smp[1], s_C_1) * s_rSkew[0]));
			PluGin[ampChan][0]->ApplyDistortion();

			PluGin[ampChan][1]->s_InputSample(-s_smp[2] + (s_Diode_S(-s_smp[2], s_C_1) * s_rSkew[1]));
			PluGin[ampChan][1]->ApplyDistortion();

			s_smp[1] = (fsx) PluGin[ampChan][0]->s_GetOutPut();
			s_smp[2] = (fsx) PluGin[ampChan][1]->s_GetOutPut();

			if (IntParam[ScPrmI_CabMic_Active])
			{
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->s_Input(s_smp[1]);
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->s_RunQ();
				s_smp[1] = (fsx) CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->s_GetOutput();

				CabCurves[1][IntParam[ScPrmI_CabMic_Num]]->s_Input(s_smp[2]);
				CabCurves[1][IntParam[ScPrmI_CabMic_Num]]->s_RunQ();
				s_smp[2] = (fsx) CabCurves[1][IntParam[ScPrmI_CabMic_Num]]->s_GetOutput();
			}
			s_smp[1] *= fsx (FpParam[ScPrmF_Master_Volume]);
			s_smp[2] *= fsx (-FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (s_smp[0]);
			splOutputLeft = fsx (s_smp[1]);
			splOutputRight = fsx (s_smp[2]);
		}

		if(fadeCount > 0)
		{
			fadeCount--;
			const fsx muteLevel = fsx (fsx (fadeCount) / fsx (fadeMax));
			splOutputMono *= fsx (muteLevel);
			splOutputLeft *= fsx (muteLevel);
			splOutputRight *= fsx (muteLevel);
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 128-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_128)
	{
		h_dNrm = fhx ( h_dNrmThresh * h_C_tenthQrt * (fhx) h_UglySine(h_deNormSuppress) );
		h_deNormSuppress += fhx (h_C_half);
		if (h_deNormSuppress >= h_C_2) h_deNormSuppress -= fhx (h_C_4);

		if((IntParam[ScPrmI_Active_And_Ready] == 0) || (IntParam[ScPrmI_OverSampling_Rate] == SCv_OverSampling_OFF))
		{
			fadeCount = fadeMax;
			splOutputMono = fsx (h_dNrm);
			splOutputLeft = fsx (h_dNrm);
			splOutputRight = fsx (h_dNrm);
			return;
		}

		if(IntParam[ScPrmI_Phase_Control] == 0) h_smp[0] = fhx (splInputMono);
		else h_smp[0] = fhx (-splInputMono);

		h_smp[0] += fhx (h_dNrm);
		h_smp[1] = fhx (splInputLeft + h_dNrm);
		h_smp[2] = fhx (splInputRight + h_dNrm);

		if(IntParam[ScPrmI_Stereo] == 0)
		{
			if (h_abs(h_smp[0] < h_dNrmThresh)) h_smp[0] += h_dNrm;

			PluGin[ampChan][0]->h_InputSample(h_smp[0] + (h_Diode_S(h_smp[0], h_C_1) * h_rSkew[0]));
			PluGin[ampChan][0]->ApplyDistortion();

			h_smp[0] = (fhx) PluGin[ampChan][0]->h_GetOutPut();

			if (IntParam[ScPrmI_CabMic_Active])
			{
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->h_Input(h_smp[0]);
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->h_RunQ();
				h_smp[0] = (fhx) CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->h_GetOutput();
			}
			h_smp[0] *= fhx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (h_smp[0]);
			splOutputLeft = fsx (h_smp[0]);
			splOutputRight = fsx (h_smp[0]);
		} else
		{
			if (h_abs(h_smp[1] < h_dNrmThresh)) h_smp[1] += h_dNrm;
			if (h_abs(h_smp[2] < h_dNrmThresh)) h_smp[2] += h_dNrm;

			PluGin[ampChan][0]->h_InputSample(h_smp[1] + (h_Diode_S(h_smp[1], h_C_1) * h_rSkew[0]));
			PluGin[ampChan][0]->ApplyDistortion();

			PluGin[ampChan][1]->h_InputSample(-h_smp[2] + (h_Diode_S(-h_smp[2], h_C_1) * h_rSkew[1]));
			PluGin[ampChan][1]->ApplyDistortion();

			h_smp[1] = (fhx) PluGin[ampChan][0]->h_GetOutPut();
			h_smp[2] = (fhx) PluGin[ampChan][1]->h_GetOutPut();

			if (IntParam[ScPrmI_CabMic_Active])
			{
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->h_Input(h_smp[1]);
				CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->h_RunQ();
				h_smp[1] = (fhx) CabCurves[0][IntParam[ScPrmI_CabMic_Num]]->h_GetOutput();

				CabCurves[1][IntParam[ScPrmI_CabMic_Num]]->h_Input(h_smp[2]);
				CabCurves[1][IntParam[ScPrmI_CabMic_Num]]->h_RunQ();
				h_smp[2] = (fhx) CabCurves[1][IntParam[ScPrmI_CabMic_Num]]->h_GetOutput();
			}
			h_smp[1] *= fhx (FpParam[ScPrmF_Master_Volume]);
			h_smp[2] *= fhx (-FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (h_smp[0]);
			splOutputLeft = fsx (h_smp[1]);
			splOutputRight = fsx (h_smp[2]);
		}

		if(fadeCount > 0)
		{
			fadeCount--;
			const fhx muteLevel = fhx (fhx (fadeCount) / fhx (fadeMax));
			splOutputMono *= fsx (muteLevel);
			splOutputLeft *= fsx (muteLevel);
			splOutputRight *= fsx (muteLevel);
		}
	}
}
