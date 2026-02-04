#include "plugin_jykkr.h"
#include <algorithm>

using namespace ScMath;

SC_JYKKR_Amp_Head::SC_JYKKR_Amp_Head(double sRt) :
	fadeFac(fsx (SCr_JYKR_AmpHead_Fade_Factor)),
	smpRate(fsx (sRt)),
	splInputMono(fsx (0.0)),
	splInputLeft(fsx (0.0)),
	splInputRight(fsx (0.0)),
	splOutputMono(fsx (0.0)),
	splOutputLeft(fsx (0.0)),
	splOutputRight(fsx (0.0)),
	l_deNormSuppress(flx (0.20f)),
	s_deNormSuppress(fsx (0.20)),
	h_deNormSuppress(fhx (0.20)),
	l_dNrmThresh(flx (powf(10.f, -28.f))),
	s_dNrmThresh(fsx (pow(10.0, -30.0))),
	h_dNrmThresh(fhx (powl(10.0, -32.0))),
	l_PI(flx (SC_PI)),
	s_PI(fsx (SC_PI)),
	h_PI(fhx (SC_PI)),
	// convLengthSet(0),
#ifdef SC_JYKR_ENABLE_STEREO
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

	fhx init_tFreqA[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_TubeFreqA;
	fhx init_tFreqB[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_TubeFreqB;
	int init_tFreqStages[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_TubeFreqStages;
	fhx init_hrd_Pre[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_PreAmp_Hardness;
	fhx init_hrd_Power[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_PowerAmp_Hardness;
	fhx init_slope_Pre[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_PreAmp_Slope;
	fhx init_slope_Power[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_PowerAmp_Slope;
	fhx init_tPr_FreqPre[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_PreAmp_Frequency;
	fhx init_tPr_FreqPower[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_PowerAmp_Frequency;
	fhx init_tPr_GritPre[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_PreAmp_Grit;
	fhx init_tPr_GritPower[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_POwerAmp_Grit;
	fhx init_tToneFilter_Low[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_Distortion_ToneFilter_Low;
	fhx init_tToneFilter_High[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_Distortion_ToneFilter_High;
	fhx init_ampMultPre[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_AmpMult_Pre;
	fhx init_ampMultPower[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_AmpMult_Power;
	fhx init_volumeAdj[SC_Number_AmpChannels_C120] = Dist_JYKR_TB_OutVolume_Adjust;

	SC_TubeDistortion_Params initCenter;
	SC_TubeDistortion_Params initLeft;
	SC_TubeDistortion_Params initRight;
	initCenter.qSpatialFactor = SCr_JYKR_AmpHead_QSpatialFactor_Mono;
	initLeft.qSpatialFactor = SCr_JYKR_AmpHead_QSpatialFactor_Left;
	initRight.qSpatialFactor = SCr_JYKR_AmpHead_QSpatialFactor_Right;

	fhx rnd_freq[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	fhx rnd_tone[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	fhx rnd_grit[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	fhx rnd_slpe[6] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
	for (int x = 0; x < 6; x++)
	{
		const int r_freq = int (rand() % 200) - 100;
		const int r_tone = int (rand() % 200) - 100;
		const int r_grit = int (rand() % 200) - 100;
		const int r_slpe = int (rand() % 200) - 100;

		const fhx rf_freq = fhx (fhx (r_freq) * fhx (0.010));
		const fhx rf_tone = fhx (fhx (r_tone) * fhx (0.010));
		const fhx rf_grit = fhx (fhx (r_grit) * fhx (0.010));
		const fhx rf_slpe = fhx (fhx (r_slpe) * fhx (0.010));

		rnd_freq[x] += fhx ((fhx) rf_freq * (fhx) 0.010 * (fhx) Dist_JYKR_TB_Freq_Rand_Percent);
		rnd_tone[x] += fhx ((fhx) rf_tone * (fhx) 0.010 * (fhx) Dist_JYKR_TB_Distortion_ToneFilter_Rand_Percent);
		rnd_grit[x] += fhx ((fhx) rf_grit * (fhx) 0.010 * (fhx) Dist_JYKR_TB_Grit_Rand_Percent);
		rnd_slpe[x] += fhx ((fhx) rf_slpe * (fhx) 0.010 * (fhx) Dist_JYKR_TB_Amp_Slope_Rand_Percent);
	}

	for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
	{
		initCenter.tFreqA = initLeft.tFreqA = initRight.tFreqA = init_tFreqA[x];
		initCenter.tFreqB = initLeft.tFreqB = initRight.tFreqB = init_tFreqB[x];
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

		if (stereoEnabled)
		{
			PluGin[x][0] = new Tube_Bank_I(&initLeft, fsx (smpRate));
			PluGin[x][1] = new Tube_Bank_I(&initRight, fsx (smpRate));

			PluGin[x][0]->InitTubes();
			PluGin[x][0]->SetFPprecision(1);
			PluGin[x][0]->setOverSampling(1);
			PluGin[x][0]->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));

			PluGin[x][1]->InitTubes();
			PluGin[x][1]->SetFPprecision(1);
			PluGin[x][1]->setOverSampling(1);
			PluGin[x][1]->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));
		} else
		{
			PluGin[x][0] = new Tube_Bank_I(&initCenter, fsx (smpRate));

			PluGin[x][0]->InitTubes();
			PluGin[x][0]->SetFPprecision(1);
			PluGin[x][0]->setOverSampling(1);
			PluGin[x][0]->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));
		}
	}

	SC_ToneStack_Params tStackPrm;
	tStackPrm.ImpSyncPowerFac = SC_JYKR_TSTACK_IMPULSE_SyncRate_Power_Factor;
	tStackPrm.ImpStartSmpRate = (fhx) SC_JYKR_TSTACK_IMPULSE_SAMPLE_RATE;
	tStackPrm.ImpTailPadFac = (fhx) SC_JYKR_TSTACK_IMPULSE_Tail_PadLength_Factor;
	tStackPrm.DynamicTonePassFreq = fsx (205.0);
	tStackPrm.DynamicToneFloorFreq = fsx (30.0);
	tStackPrm.DynamicToneTravelFreq = fsx (20.0);
	tStackPrm.DynamicToneRange = fhx (3.0);
	tStackPrm.DynamicToneCurve = fhx (32.0);
	tStackPrm.DynamicToneOffset = fhx (2.0);

	tStackPrm.qb_a_low = SCr_JYKR_TStack_EqBand_Start_Low;
	tStackPrm.qb_a_mid = SCr_JYKR_TStack_EqBand_Start_Mid;
	tStackPrm.qb_a_high = SCr_JYKR_TStack_EqBand_Start_High;
	tStackPrm.qb_a_cntA = SCr_JYKR_TStack_EqBand_Start_CntrA;
	tStackPrm.qb_a_cntB = SCr_JYKR_TStack_EqBand_Start_CntrB;
	tStackPrm.qb_a_pres = SCr_JYKR_TStack_EqBand_Start_Pres;

	tStackPrm.qb_b_low = SCr_JYKR_TStack_EqBand_End_Low;
	tStackPrm.qb_b_mid = SCr_JYKR_TStack_EqBand_End_Mid;
	tStackPrm.qb_b_high = SCr_JYKR_TStack_EqBand_End_High;
	tStackPrm.qb_b_cntA = SCr_JYKR_TStack_EqBand_End_CntrA;
	tStackPrm.qb_b_cntB = SCr_JYKR_TStack_EqBand_End_CntrB;
	tStackPrm.qb_b_pres = SCr_JYKR_TStack_EqBand_End_Pres;

	const int lngA = SC_JYKR_TSTACK_IMPULSE_Lengths_Amp;
	const int lngC = SC_JYKR_TSTACK_IMPULSE_Lengths_Cab;

	fhx rnd_cnv_phase[3] = {0.0, 0.0, 0.0};
	fhx rnd_cnv_skew[3] = {0.0, 0.0, 0.0};
	for (int x = 0; x < 3; x++)
	{
		const int r_phase = int (rand() % 100);
		const int r_skew = int (rand() % 200) - 100;

		rnd_cnv_phase[x] = fhx (fhx (r_phase) * fhx (0.00010) * fhx (SCr_JYKR_AmpHead_CnvPhase_Rand_Percent));
		rnd_cnv_skew[x] = fhx (fhx (r_skew) * fhx (0.00010) * fhx (SCr_JYKR_AmpHead_CnvSkew_Rand_Percent));

		tStackPrm.convShift = (fhx) rnd_cnv_phase[x];
		tStackPrm.DCskew = (fhx) rnd_cnv_skew[x];
		tStackPrm.ImpLenAmpI = (long) lngA;
		tStackPrm.ImpLenCabI = (long) lngC;
		tStackPrm.ImpLenAmpF = (fhx) lngA;
		tStackPrm.ImpLenCabF = (fhx) lngC;

		if (stereoEnabled)
		{
			if (x > 0)
				Qstack[x-1] = new ToneStack_H(smpRate, tStackPrm);
		} else
		{
			if (x == 0)
				Qstack[x] = new ToneStack_H(smpRate, tStackPrm);
		}
	}

	InpFilter[0] = new Cst_EQ();
	InpFilter[0]->SetSmpRate(smpRate);
	InpFilter[0]->SetBands(SCr_JYKR_AmpHead_InputQ_Low_Standard, SCr_JYKR_AmpHead_InputQ_Ceiling);
	InpFilter[1] = new Cst_EQ();
	InpFilter[1]->SetSmpRate(smpRate);
	InpFilter[1]->SetBands(SCr_JYKR_AmpHead_InputQ_Low_Standard, SCr_JYKR_AmpHead_InputQ_Ceiling);

	InpFilterB[0] = new Cst_EQ();
	InpFilterB[0]->SetSmpRate(smpRate);
	InpFilterB[0]->SetBands(SCr_JYKR_AmpHead_InputQ_Low_Bright, SCr_JYKR_AmpHead_InputQ_Ceiling);
	InpFilterB[1] = new Cst_EQ();
	InpFilterB[1]->SetSmpRate(smpRate);
	InpFilterB[1]->SetBands(SCr_JYKR_AmpHead_InputQ_Low_Bright, SCr_JYKR_AmpHead_InputQ_Ceiling);

	IntParam[ScPrmI_Active_And_Ready] = 1;
}
SC_JYKKR_Amp_Head::~SC_JYKKR_Amp_Head()
{
	for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
	{
		delete PluGin[x][0];
		if (stereoEnabled) delete PluGin[x][1];
	}

	delete Qstack[0];
	if (stereoEnabled) delete Qstack[1];

	delete InpFilter[0];
	delete InpFilterB[0];
	delete InpFilter[1];
	delete InpFilterB[1];
}
void SC_JYKKR_Amp_Head::SetNewSampleRate(const double inp)
{
	smpRate = fsx (inp);
	if(fsx (smpRate) < fsx (44100.0)) smpRate = fsx (44100.0);
//	if(fsx (smpRate) > fsx (6144000.0)) smpRate = fsx (6144000.0);	// Maximum Sample Rate to avoid absurd, invalid values

	for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
	{
		PluGin[x][0]->setNSampleRate(fsx (smpRate));
		if (stereoEnabled) PluGin[x][1]->setNSampleRate(fsx (smpRate));
	}

	Qstack[0]->setNewSampleRate(fsx (smpRate));
	if (stereoEnabled) Qstack[1]->setNewSampleRate(fsx (smpRate));

	fadeMax = long (fsx (fadeFac) * fsx (smpRate / 44100.0));
	fadeCount = fadeMax;

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);

	InpFilter[0]->SetSmpRate(smpRate);
	InpFilter[0]->SetBands(SCr_JYKR_AmpHead_InputQ_Low_Standard, SCr_JYKR_AmpHead_InputQ_Ceiling);
	InpFilterB[0]->SetSmpRate(smpRate);
	InpFilterB[0]->SetBands(SCr_JYKR_AmpHead_InputQ_Low_Bright, SCr_JYKR_AmpHead_InputQ_Ceiling);
	InpFilter[1]->SetSmpRate(smpRate);
	InpFilter[1]->SetBands(SCr_JYKR_AmpHead_InputQ_Low_Standard, SCr_JYKR_AmpHead_InputQ_Ceiling);
	InpFilterB[1]->SetSmpRate(smpRate);
	InpFilterB[1]->SetBands(SCr_JYKR_AmpHead_InputQ_Low_Bright, SCr_JYKR_AmpHead_InputQ_Ceiling);
}
void SC_JYKKR_Amp_Head::ClearAllBuffers()
{
	for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
	{
		PluGin[x][0]->clearBuffs();
		if (stereoEnabled) PluGin[x][1]->clearBuffs();
	}

	Qstack[0]->ClearBuffs();
	if (stereoEnabled) Qstack[1]->ClearBuffs();

	fadeCount = fadeMax;

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);
}
void SC_JYKKR_Amp_Head::SetFpParam(const int prm, float newValue)
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

		Qstack[0]->setBandValue(0, fsx (newValue));
		if (stereoEnabled) Qstack[1]->setBandValue(0, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Mid)
	{
		FpParam[prm] = flx (newValue);

		Qstack[0]->setBandValue(1, fsx (newValue));
		if (stereoEnabled) Qstack[1]->setBandValue(1, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_High)
	{
		FpParam[prm] = flx (newValue);

		Qstack[0]->setBandValue(2, fsx (newValue));
		if (stereoEnabled) Qstack[1]->setBandValue(2, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Contour)
	{
		FpParam[prm] = flx (newValue);

		Qstack[0]->setBandValue(3, fsx (newValue));
		if (stereoEnabled) Qstack[1]->setBandValue(3, fsx (newValue));
		Qstack[0]->setBandValue(4, fsx (newValue));
		if (stereoEnabled) Qstack[1]->setBandValue(4, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Presence)
	{
		FpParam[prm] = flx (newValue);

		Qstack[0]->setBandValue(5, fsx (newValue));
		if (stereoEnabled) Qstack[1]->setBandValue(5, fsx (newValue));
	}
}
void SC_JYKKR_Amp_Head::SetIntParam(const int prm, int newValue)
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
				PluGin[x][0]->SetFPprecision(0);
				if (stereoEnabled) PluGin[x][1]->SetFPprecision(0);
			}

			Qstack[0]->SetPrecision(0);
			if (stereoEnabled) Qstack[1]->SetPrecision(0);
		}
		if(newValue == SCv_Bit_Depth_64)
		{
			for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
			{
				PluGin[x][0]->SetFPprecision(1);
				if (stereoEnabled) PluGin[x][1]->SetFPprecision(1);
			}

			Qstack[0]->SetPrecision(1);
			if (stereoEnabled) Qstack[1]->SetPrecision(1);
		}
		if(newValue == SCv_Bit_Depth_128)
		{
			for (int x = 0; x < SC_Number_AmpChannels_C120; x++)
			{
				PluGin[x][0]->SetFPprecision(2);
				if (stereoEnabled) PluGin[x][1]->SetFPprecision(2);
			}

			Qstack[0]->SetPrecision(2);
			if (stereoEnabled) Qstack[1]->SetPrecision(2);
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
			PluGin[y][0]->setOverSampling(x);
			if (stereoEnabled) PluGin[y][1]->setOverSampling(x);
		}

		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_Stereo)
	{
		if(newValue < 0) return;
		if(newValue > 1) return;
		if (stereoEnabled) IntParam[prm] = newValue;
		else IntParam[prm] = 0;
	}
	if(prm == ScPrmI_Bright)
		IntParam[prm] = newValue;
	if(prm == ScPrmI_CabMic_Active)
	{
		if(newValue == 1)
		{
			Qstack[0]->setCabMic(true);
			if (stereoEnabled) Qstack[1]->setCabMic(true);
		} else
		{
			Qstack[0]->setCabMic(false);
			if (stereoEnabled) Qstack[1]->setCabMic(false);
		}
	}
	if(prm == ScPrmI_CabMic_Num)
	{
		Qstack[0]->SetCabMicNum(newValue);
		if (stereoEnabled) Qstack[1]->SetCabMicNum(newValue);
	}
	if(prm == ScPrmI_TStack_Shape)
	{
		if(newValue == 1)
		{
			Qstack[0]->setEvenShaped(true);
			if (stereoEnabled) Qstack[1]->setEvenShaped(true);
		} else
		{
			Qstack[0]->setEvenShaped(false);
			if (stereoEnabled) Qstack[1]->setEvenShaped(false);
		}
	}
	if(prm == ScPrmI_TStack_ShapeNum)
	{
		Qstack[0]->SetAmpShapeNum(newValue);
		if (stereoEnabled) Qstack[1]->SetAmpShapeNum(newValue);
	}
	if(prm == ScPrmI_Phase_Control)
		IntParam[prm] = newValue;
	if(prm == ScPrmI_TStack_Dynamic)
	{
		if(newValue == 1)
		{
			Qstack[0]->EnableDynamicQ(true);
			if (stereoEnabled) Qstack[1]->EnableDynamicQ(true);
		} else
		{
			Qstack[0]->EnableDynamicQ(false);
			if (stereoEnabled) Qstack[1]->EnableDynamicQ(false);
		}
	}
	if(prm == ScPrmI_TStack_Before_Power)
	{
		IntParam[prm] = std::min(1, std::max(0, newValue));
	}
}
void SC_JYKKR_Amp_Head::InputSample(const double inp)
{
	splInputMono = fsx (inp);
	splInputLeft = fsx (inp);
	splInputRight = fsx (inp);
}
void SC_JYKKR_Amp_Head::InputSampleStereo(const double inpL, const double inpR)
{
	splInputLeft = (fsx) inpL;
	splInputRight = (fsx) inpR;
	splInputMono = fsx (fsx (fsx (inpL) + fsx (inpR)) * fsx (0.50));
}
double SC_JYKKR_Amp_Head::GetMonoOut()
{
	return fsx (splOutputMono);
}
double SC_JYKKR_Amp_Head::GetStereoOutLeft()
{
	return fsx (splOutputLeft);
}
double SC_JYKKR_Amp_Head::GetStereoOutRight()
{
	return fsx (splOutputRight);
}
float SC_JYKKR_Amp_Head::GetFpParam(const int prm)
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
int SC_JYKKR_Amp_Head::GetIntParam(const int prm)
{
	if(prm < 0) return 0;

	if(prm == ScPrmI_Active_And_Ready) return IntParam[prm];
	if(prm == ScPrmI_Error_Status) return IntParam[prm];
	if(prm == ScPrmI_Drive_Channel) return IntParam[prm];
	if(prm == ScPrmI_FP_Precision_Bit_Depth) return IntParam[prm];
	if(prm == ScPrmI_OverSampling_Rate) return IntParam[prm];

	return 0;
}
long long SC_JYKKR_Amp_Head::GetLongParam(const int prm)
{
	return 0;
}
void SC_JYKKR_Amp_Head::RunPlugin()
{
//! ----------------------------------------------------------------------------------------------------------------
//! 32-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_32)
	{
		l_dNrm = flx (l_dNrmThresh * flx (0.1f) * (flx) sinf(l_PI * l_deNormSuppress));
		l_deNormSuppress += flx (0.250f);
		if (l_deNormSuppress > flx (4.0f)) l_deNormSuppress -= flx (2.0f);

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

		PluGin[ampChan][0]->l_SetAmpVt(l_smp[0]);
		if (stereoEnabled) PluGin[ampChan][1]->l_SetAmpVt(l_smp[1]);
		Qstack[0]->l_SetDynamicQ(l_smp[0]);
		if (stereoEnabled) Qstack[1]->l_SetDynamicQ(l_smp[1]);

		if(IntParam[ScPrmI_Stereo] == 0)
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[0]->SplInL(l_smp[0]);
				InpFilterB[0]->RunQL();
				l_smp[0] = (flx) InpFilterB[0]->GetOutputL();
			} else
			{
				InpFilter[0]->SplInL(l_smp[0]);
				InpFilter[0]->RunQL();
				l_smp[0] = (flx) InpFilter[0]->GetOutputL();
			}
			PluGin[ampChan][0]->l_InputSample(l_smp[0]);
			PluGin[ampChan][0]->runUpSampling();
			PluGin[ampChan][0]->ApplyDistortion();
			PluGin[ampChan][0]->runDownSampling();

			l_smp[0] = (flx) PluGin[ampChan][0]->GetOutPL();
			if((l_smp[0] < l_dNrmThresh) && (l_smp[0] > -l_dNrmThresh)) l_smp[0] += l_dNrm;

			Qstack[0]->l_SetInput(l_smp[0]);
			Qstack[0]->RunConv();
			Qstack[0]->RunEQ();
			l_smp[0] = (flx) Qstack[0]->GetProcessedOutputL();

			l_smp[0] *= flx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (l_smp[0]);
			splOutputLeft = fsx (l_smp[0]);
			splOutputRight = fsx (l_smp[0]);
		} else
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[0]->SplInL(l_smp[1]);
				InpFilterB[1]->SplInL(l_smp[2]);
				InpFilterB[0]->RunQL();
				InpFilterB[1]->RunQL();
				l_smp[1] = (flx) InpFilterB[0]->GetOutputL();
				l_smp[2] = (flx) InpFilterB[1]->GetOutputL();
			} else
			{
				InpFilter[0]->SplInL(l_smp[1]);
				InpFilter[1]->SplInL(l_smp[2]);
				InpFilter[0]->RunQL();
				InpFilter[1]->RunQL();
				l_smp[1] = (flx) InpFilter[0]->GetOutputL();
				l_smp[2] = (flx) InpFilter[1]->GetOutputL();
			}
			PluGin[ampChan][0]->InputSmp(l_smp[1]);
			PluGin[ampChan][0]->runUpSampling();
			PluGin[ampChan][0]->ApplyDistortion();
			PluGin[ampChan][0]->runDownSampling();

			PluGin[ampChan][1]->InputSmp(l_smp[2]);
			PluGin[ampChan][1]->runUpSampling();
			PluGin[ampChan][1]->ApplyDistortion();
			PluGin[ampChan][1]->runDownSampling();

			l_smp[1] = (flx) PluGin[ampChan][0]->GetOutPL();
			l_smp[2] = (flx) PluGin[ampChan][1]->GetOutPL();
			if((l_smp[1] < l_dNrmThresh) && (l_smp[1] > -l_dNrmThresh)) l_smp[1] += l_dNrm;
			if((l_smp[2] < l_dNrmThresh) && (l_smp[2] > -l_dNrmThresh)) l_smp[2] += l_dNrm;

			Qstack[0]->l_SetInput(l_smp[1]);
			Qstack[0]->RunConv();
			Qstack[0]->RunEQ();
			l_smp[1] = (flx) Qstack[0]->GetProcessedOutputL();

			Qstack[1]->l_SetInput(l_smp[2]);
			Qstack[1]->RunConv();
			Qstack[1]->RunEQ();
			l_smp[2] = (flx) Qstack[1]->GetProcessedOutputL();

			l_smp[1] *= flx (FpParam[ScPrmF_Master_Volume]);
			l_smp[2] *= flx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (l_smp[0]);
			splOutputLeft = fsx (l_smp[1]);
			splOutputRight = fsx (l_smp[2]);
		}

		if(fadeCount > 0)
		{
			fadeCount--;
			splOutputMono = fsx (0.0);
			splOutputLeft = fsx (0.0);
			splOutputRight = fsx (0.0);
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 64-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_64)
	{
		s_dNrm = fsx (s_dNrmThresh * fsx (0.10) * (fsx) sin(s_PI * s_deNormSuppress));
		s_deNormSuppress += fsx (0.250);
		if (s_deNormSuppress > fsx (4.0)) s_deNormSuppress -= fsx (2.0);

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

		PluGin[ampChan][0]->s_SetAmpVt(s_smp[0]);
		if (stereoEnabled) PluGin[ampChan][1]->s_SetAmpVt(s_smp[1]);
		Qstack[0]->s_SetDynamicQ(s_smp[0]);
		if (stereoEnabled) Qstack[1]->s_SetDynamicQ(s_smp[1]);

		if(IntParam[ScPrmI_Stereo] == 0)
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[0]->SplInS(s_smp[0]);
				InpFilterB[0]->RunQS();
				s_smp[0] = (fsx) InpFilterB[0]->GetOutputS();
			} else
			{
				InpFilter[0]->SplInS(s_smp[0]);
				InpFilter[0]->RunQS();
				s_smp[0] = (fsx) InpFilter[0]->GetOutputS();
			}
			PluGin[ampChan][0]->InputSmp(s_smp[0]);
			PluGin[ampChan][0]->runUpSampling();
			PluGin[ampChan][0]->ApplyDistortion();
			PluGin[ampChan][0]->runDownSampling();

			s_smp[0] = (fsx) PluGin[ampChan][0]->GetOutPS();

			if((s_smp[0] < s_dNrmThresh) && (s_smp[0] > -s_dNrmThresh)) s_smp[0] += s_dNrm;

			Qstack[0]->s_SetInput(s_smp[0]);
			Qstack[0]->RunConv();
			Qstack[0]->RunEQ();
			s_smp[0] = (fsx) Qstack[0]->GetProcessedOutputS();

			s_smp[0] *= fsx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (s_smp[0]);
			splOutputLeft = fsx (s_smp[0]);
			splOutputRight = fsx (s_smp[0]);
		} else
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[0]->SplInS(s_smp[1]);
				InpFilterB[1]->SplInS(s_smp[2]);
				InpFilterB[0]->RunQS();
				InpFilterB[1]->RunQS();
				s_smp[1] = (fsx) InpFilterB[0]->GetOutputS();
				s_smp[2] = (fsx) InpFilterB[1]->GetOutputS();
			} else
			{
				InpFilter[0]->SplInS(s_smp[1]);
				InpFilter[1]->SplInS(s_smp[2]);
				InpFilter[0]->RunQS();
				InpFilter[1]->RunQS();
				s_smp[1] = (fsx) InpFilter[0]->GetOutputS();
				s_smp[2] = (fsx) InpFilter[1]->GetOutputS();
			}
			PluGin[ampChan][0]->InputSmp(s_smp[1]);
			PluGin[ampChan][0]->runUpSampling();
			PluGin[ampChan][0]->ApplyDistortion();
			PluGin[ampChan][0]->runDownSampling();

			PluGin[ampChan][1]->InputSmp(s_smp[2]);
			PluGin[ampChan][1]->runUpSampling();
			PluGin[ampChan][1]->ApplyDistortion();
			PluGin[ampChan][1]->runDownSampling();

			s_smp[1] = (fsx) PluGin[ampChan][0]->GetOutPS();
			s_smp[2] = (fsx) PluGin[ampChan][1]->GetOutPS();
			if((s_smp[1] < s_dNrmThresh) && (s_smp[1] > -s_dNrmThresh)) s_smp[1] += s_dNrm;
			if((s_smp[2] < s_dNrmThresh) && (s_smp[2] > -s_dNrmThresh)) s_smp[2] += s_dNrm;

			Qstack[0]->s_SetInput(s_smp[1]);
			Qstack[0]->RunConv();
			Qstack[0]->RunEQ();
			s_smp[1] = (fsx) Qstack[0]->GetProcessedOutputS();

			Qstack[1]->s_SetInput(s_smp[2]);
			Qstack[1]->RunConv();
			Qstack[1]->RunEQ();
			s_smp[2] = (fsx) Qstack[1]->GetProcessedOutputS();

			s_smp[1] *= fsx (FpParam[ScPrmF_Master_Volume]);
			s_smp[2] *= fsx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (s_smp[0]);
			splOutputLeft = fsx (s_smp[1]);
			splOutputRight = fsx (s_smp[2]);
		}

		if(fadeCount > 0)
		{
			fadeCount--;
			splOutputMono = fsx (0.0);
			splOutputLeft = fsx (0.0);
			splOutputRight = fsx (0.0);
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 128-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_128)
	{
		h_dNrm = fhx (h_dNrmThresh * fhx (0.10) * (fhx) sinl(h_PI * h_deNormSuppress));
		h_deNormSuppress += fhx (0.250);
		if (h_deNormSuppress > fhx (4.0)) h_deNormSuppress -= fhx (2.0);

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

		PluGin[ampChan][0]->h_SetAmpVt(h_smp[0]);
		if (stereoEnabled) PluGin[ampChan][1]->h_SetAmpVt(h_smp[1]);
		Qstack[0]->h_SetDynamicQ(h_smp[0]);
		if (stereoEnabled) Qstack[1]->h_SetDynamicQ(h_smp[1]);

		if(IntParam[ScPrmI_Stereo] == 0)
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[0]->SplInH(h_smp[0]);
				InpFilterB[0]->RunQH();
				h_smp[0] = (fhx) InpFilterB[0]->GetOutputH();
			} else
			{
				InpFilter[0]->SplInH(h_smp[0]);
				InpFilter[0]->RunQH();
				h_smp[0] = (fhx) InpFilter[0]->GetOutputH();
			}
			PluGin[ampChan][0]->h_InputSample(h_smp[0]);
			PluGin[ampChan][0]->runUpSampling();
			PluGin[ampChan][0]->ApplyDistortion();
			PluGin[ampChan][0]->runDownSampling();

			h_smp[0] = (fhx) PluGin[ampChan][0]->h_GetOutPut();
			if((h_smp[0] < h_dNrmThresh) && (h_smp[0] > -h_dNrmThresh)) h_smp[0] += h_dNrm;

			Qstack[0]->h_SetInput(h_smp[0]);
			Qstack[0]->RunConv();
			Qstack[0]->RunEQ();
			h_smp[0] = (fhx) Qstack[0]->GetProcessedOutputH();

			h_smp[0] *= fhx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (h_smp[0]);
			splOutputLeft = fsx (h_smp[0]);
			splOutputRight = fsx (h_smp[0]);
		} else
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[0]->SplInH(h_smp[1]);
				InpFilterB[1]->SplInH(h_smp[2]);
				InpFilterB[0]->RunQH();
				InpFilterB[1]->RunQH();
				h_smp[1] = (fhx) InpFilterB[0]->GetOutputH();
				h_smp[2] = (fhx) InpFilterB[1]->GetOutputH();
			} else
			{
				InpFilter[0]->SplInH(h_smp[1]);
				InpFilter[1]->SplInH(h_smp[2]);
				InpFilter[0]->RunQH();
				InpFilter[1]->RunQH();
				h_smp[1] = (fhx) InpFilter[0]->GetOutputH();
				h_smp[2] = (fhx) InpFilter[1]->GetOutputH();
			}
			PluGin[ampChan][0]->h_InputSample(h_smp[1]);
			PluGin[ampChan][0]->runUpSampling();
			PluGin[ampChan][0]->ApplyDistortion();
			PluGin[ampChan][0]->runDownSampling();

			PluGin[ampChan][1]->h_InputSample(h_smp[2]);
			PluGin[ampChan][1]->runUpSampling();
			PluGin[ampChan][1]->ApplyDistortion();
			PluGin[ampChan][1]->runDownSampling();

			h_smp[1] = (fhx) PluGin[ampChan][0]->h_GetOutPut();
			h_smp[2] = (fhx) PluGin[ampChan][1]->h_GetOutPut();
			if((h_smp[1] < h_dNrmThresh) && (h_smp[1] > -h_dNrmThresh)) h_smp[1] += h_dNrm;
			if((h_smp[2] < h_dNrmThresh) && (h_smp[2] > -h_dNrmThresh)) h_smp[2] += h_dNrm;

			Qstack[0]->h_SetInput(h_smp[1]);
			Qstack[0]->RunConv();
			Qstack[0]->RunEQ();
			h_smp[1] = (fhx) Qstack[0]->GetProcessedOutputH();

			Qstack[1]->h_SetInput(h_smp[2]);
			Qstack[1]->RunConv();
			Qstack[1]->RunEQ();
			h_smp[2] = (fhx) Qstack[1]->GetProcessedOutputH();

			h_smp[1] *= fhx (FpParam[ScPrmF_Master_Volume]);
			h_smp[2] *= fhx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (h_smp[0]);
			splOutputLeft = fsx (h_smp[1]);
			splOutputRight = fsx (h_smp[2]);
		}

		if(fadeCount > 0)
		{
			fadeCount--;
			splOutputMono = fsx (0.0);
			splOutputLeft = fsx (0.0);
			splOutputRight = fsx (0.0);
		}
	}
}
