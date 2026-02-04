#include "tube_bank_j.h"
#include "custdata_types.h"

using namespace ScMath;

Tube_Bank_J::Tube_Bank_J(const fsx spRate, const SC_TubeDistortion_Params initParams, const SC_ToneStack_Params toneStackParams) :
SampleBank_Cst_B(spRate, 12),
ToneStack_J(spRate, toneStackParams),
	params(initParams),
	tbLow(initParams.tFreqC),
	tbHigh(initParams.tFreqD),
	h_distFreqHi(fhx (initParams.tFreqB)),
	h_distFreqLo(fhx (initParams.tFreqA)),
	s_distFreqHi(fsx (initParams.tFreqB)),
	s_distFreqLo(fsx (initParams.tFreqA)),
	l_distFreqHi(flx (initParams.tFreqB)),
	l_distFreqLo(flx (initParams.tFreqA)),
	l_pre_Hard(flx (initParams.hrd_Pre)),
	s_pre_Hard(fsx (initParams.hrd_Pre)),
	h_pre_Hard(fhx (initParams.hrd_Pre)),
	l_pwr_Hard(flx (initParams.hrd_Power)),
	s_pwr_Hard(fsx (initParams.hrd_Power)),
	h_pwr_Hard(fhx (initParams.hrd_Power)),
	l_pre_Grit(flx (initParams.tPr_GritPre)),
	s_pre_Grit(fsx (initParams.tPr_GritPre)),
	h_pre_Grit(fhx (initParams.tPr_GritPre)),
	l_pwr_Grit(flx (initParams.tPr_GritPower)),
	s_pwr_Grit(fsx (initParams.tPr_GritPower)),
	h_pwr_Grit(fhx (initParams.tPr_GritPower)),
	h_pre_Slope(fhx (initParams.slope_Pre)),
	h_pwr_Slope(fhx (initParams.slope_Power)),
	s_pre_Slope(fsx (initParams.slope_Pre)),
	s_pwr_Slope(fsx (initParams.slope_Power)),
	l_pre_Slope(flx (initParams.slope_Pre)),
	l_pwr_Slope(flx (initParams.slope_Power)),
	l_outVol(flx (initParams.volumeAdj)),
	s_outVol(fsx (initParams.volumeAdj)),
	h_outVol(fhx (initParams.volumeAdj)),
	freqStages(int (initParams.tFreqStages))
{
	l_pre_Amp_Mult = flx (s_pre_Amp_Mult = fsx (h_pre_Amp_Mult = fhx (params.ampMultPre)));
	l_pwr_Amp_Mult = flx (s_pwr_Amp_Mult = fsx (h_pwr_Amp_Mult = fhx (params.ampMultPower)));

	for (int x = 0; x < params.tNumStages_Pre; x++)
	{
		f_pre_BiasBuffer[x] = new Cst_EQ(true);
		f_pre_BiasBuffer[x]->SetBands(5.0, 50.0);
		f_pre_BiasBuffer[x]->SetSmpRate(cRateS);
	}
	for (int x = 0; x < params.tNumStages_Power; x++)
	{
		f_pwr_BiasBuffer[x] = new Cst_EQ(true);
		f_pwr_BiasBuffer[x]->SetBands(5.0, 50.0);
		f_pwr_BiasBuffer[x]->SetSmpRate(cRateS);
	}

	int fLayers = freqStages;
	if (freqStages > Dist_TB_TubeFreqStages_Max)
		freqStages = Dist_TB_TubeFreqStages_Max;

	for (int x = 0; x < freqStages; x++)
	{
		MidFilter[x] = new Cst_EQ(true);
		MidFilter[x]->SetBands(5.0, s_distFreqLo);
		MidFilter[x]->SetSmpRate(cRateS);
	}

	outFilter = new FIR_LP_EQ(sRateS, s_distFreqHi, 12);

	for (int x = 0; x < 3; x++)
	{
		QCurves[x][0] = new FIR_Multi_EQ(sRateS, FIRtones_A[x]);
		QCurves[x][1] = new FIR_Multi_EQ(sRateS, FIRtones_B[x]);
		QCurves[x][2] = new FIR_Multi_EQ(sRateS, FIRtones_C[x]);
		QCurves[x][3] = new FIR_Multi_EQ(sRateS, FIRtones_D[x]);
		QCurves[x][4] = new FIR_Multi_EQ(sRateS, FIRtones_E[x]);
		QCurves[x][5] = new FIR_Multi_EQ(sRateS, FIRtones_F[x]);
	}

	for (int x = 0; x < 2; x++)
		QInput[x] = new FIR_Multi_EQ(sRateS, Raw_Input_EQ[x]);

	SetGain(s_C_tenth);
	clearBuffs();
}
Tube_Bank_J::~Tube_Bank_J()
{
	for (int x = 0; x < params.tNumStages_Pre; x++)
		delete f_pre_BiasBuffer[x];

	for (int x = 0; x < params.tNumStages_Power; x++)
		delete f_pwr_BiasBuffer[x];

	for (int x = 0; x < freqStages; x++)
		delete MidFilter[x];

	delete outFilter;

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 6; y++)
			delete QCurves[x][y];
	}

	for (int x = 0; x < 2; x++)
		delete QInput[x];
}
void Tube_Bank_J::InitTubes()
{
	clearBuffs();
}
void Tube_Bank_J::setNSampleRate(fsx nSRate)
{
	SampleBank_Cst_B::setSampleRate(nSRate);
	ToneStack_J::setNewSampleRate(sRateH);

	clearBuffs();

	outFilter->ChangeRates(sRateS, s_distFreqHi);

	for (int x = 0; x < params.tNumStages_Pre; x++)
		f_pre_BiasBuffer[x]->SetSmpRate(cRateS);

	for (int x = 0; x < params.tNumStages_Power; x++)
		f_pwr_BiasBuffer[x]->SetSmpRate(cRateS);

	for (int x = 0; x < freqStages; x++)
		MidFilter[x]->SetSmpRate(cRateS);

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 6; y++)
			QCurves[x][y]->ChangeSmpRate(nSRate);
	}

	for (int x = 0; x < 2; x++)
		QInput[x]->ChangeSmpRate(nSRate);
}
void Tube_Bank_J::SetGain(const fsx nGain)
{
	h_Gain = (fhx) h_min(h_C_1, h_max(nGain, h_C_hundredth));
	s_Gain = fsx (h_Gain);
	l_Gain = flx (s_Gain);
}
void Tube_Bank_J::clearBuffs()
{
	for (int x = 0; x < Dist_TB_J_MAX_LAYERS_PRE; x++)
	{
		l_pre_travelTop[x] = l_C_0;
		l_pre_travelBtm[x] = l_C_0;
		s_pre_travelTop[x] = s_C_0;
		s_pre_travelBtm[x] = s_C_0;
		h_pre_travelTop[x] = h_C_0;
		h_pre_travelBtm[x] = h_C_0;
		l_pre_TravelBuffer[x] = l_C_0;
		s_pre_TravelBuffer[x] = s_C_0;
		h_pre_TravelBuffer[x] = h_C_0;
	}
	for (int x = 0; x < Dist_TB_J_MAX_LAYERS_POWER; x++)
	{
		l_pwr_travelTop[x] = l_C_0;
		l_pwr_travelBtm[x] = l_C_0;
		s_pwr_travelTop[x] = s_C_0;
		s_pwr_travelBtm[x] = s_C_0;
		h_pwr_travelTop[x] = h_C_0;
		h_pwr_travelBtm[x] = h_C_0;
		l_pwr_TravelBuffer[x] = l_C_0;
		s_pwr_TravelBuffer[x] = s_C_0;
		h_pwr_TravelBuffer[x] = h_C_0;
	}

	ToneStack_J::ClearBuffs();
}
void Tube_Bank_J::zeroAllSamples()
{
	SampleBank_Cst_B::zeroAllSamples();
}
void Tube_Bank_J::setOverSamplingLevel(const int oSLevel)
{
	SampleBank_Cst_B::setOverSampling(oSLevel);

	clearBuffs();

	for (int x = 0; x < params.tNumStages_Pre; x++)
		f_pre_BiasBuffer[x]->SetSmpRate(cRateS);

	for (int x = 0; x < params.tNumStages_Power; x++)
		f_pwr_BiasBuffer[x]->SetSmpRate(cRateS);

	for (int x = 0; x < freqStages; x++)
		MidFilter[x]->SetSmpRate(cRateS);
}
void Tube_Bank_J::RunPreAmp()
{
	if(osRate <= 0) return;
	if(osRate >= 6) return;

//!		Actual Pre-Amp
	if (params.tNumStages_Pre == 0) return;

	int stg = params.tNumStages_Pre;
	if (stg > Dist_TB_J_MAX_LAYERS_PRE)
		stg = Dist_TB_J_MAX_LAYERS_PRE;

	if(precisionLevel == 0)
	{
		for (int x = 0; x < stg; x++)
			l_RunTubes_Pre(x);

		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			for (int y = 0; y < freqStages; y++)
			{
				MidFilter[y]->l_InputSmpl(smpl[x]);
				MidFilter[y]->l_Run(true, false);
				smpl[x] = (flx) MidFilter[y]->l_GetOutput();
			}
		}
	} else
	if(precisionLevel == 1)
	{
		for (int x = 0; x < stg; x++)
			s_RunTubes_Pre(x);

		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			for (int y = 0; y < freqStages; y++)
			{
				MidFilter[y]->s_InputSmpl(smps[x]);
				MidFilter[y]->s_Run(true, false);
				smps[x] = (fsx) MidFilter[y]->s_GetOutput();
			}
		}
	} else
	if(precisionLevel == 2)
	{
		for (int x = 0; x < stg; x++)
			h_RunTubes_Pre(x);

		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			for (int y = 0; y < freqStages; y++)
			{
				MidFilter[y]->h_InputSmpl(smph[x]);
				MidFilter[y]->h_Run(true, false);
				smph[x] = (fhx) MidFilter[y]->h_GetOutput();
			}
		}
	}
}
void Tube_Bank_J::RunPowerAmp()
{
	if(osRate <= 0) return;
	if(osRate >= 6) return;

	if (params.tNumStages_Power == 0) return;

	int stg = params.tNumStages_Power;
	if (stg > Dist_TB_J_MAX_LAYERS_POWER)
		stg = Dist_TB_J_MAX_LAYERS_POWER;

	if(precisionLevel == 0)
	{
		for (int x = 0; x < stg; x++)
			l_RunTubes_Power(x);
	} else
	if(precisionLevel == 1)
	{
		for (int x = 0; x < stg; x++)
			s_RunTubes_Power(x);
	} else
	if(precisionLevel == 2)
	{
		for (int x = 0; x < stg; x++)
			h_RunTubes_Power(x);
	}

//!		Output Filtering
	if(precisionLevel == 0)
	{
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			for(int y = 0; y < params.tRectifierPasses; y++)
				smpl[x] = (flx) l_Rectify_Simple(smpl[x], (flx) params.VolumeRectifierAdj);
		}
	} else
	if(precisionLevel == 1)
	{
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			for(int y = 0; y < params.tRectifierPasses; y++)
				smps[x] = (fsx) s_Rectify_Simple(smps[x], (fsx) params.VolumeRectifierAdj);
		}
	} else
	if(precisionLevel == 2)
	{
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			for(int y = 0; y < params.tRectifierPasses; y++)
				smph[x] = (fhx) h_Rectify_Simple(smph[x], (fhx) params.VolumeRectifierAdj);
		}
	}
}
void Tube_Bank_J::ApplyDistortion()
{
	if (ToneStack_J::setShaped)
	{
		if(precisionLevel == 0)
		{
			ToneStack_J::l_SetDynamicQ(smpl[0]);
			QCurves[bright][curAmp]->l_Input(smpl[0]);
			QCurves[bright][curAmp]->l_RunQ();
			smpl[0] = QCurves[bright][curAmp]->l_GetOutput();
		} else
		if(precisionLevel == 1)
		{
			ToneStack_J::s_SetDynamicQ(smps[0]);
			QCurves[bright][curAmp]->s_Input(smps[0]);
			QCurves[bright][curAmp]->s_RunQ();
			smps[0] = QCurves[bright][curAmp]->s_GetOutput();
		} else
		if(precisionLevel == 2)
		{
			ToneStack_J::h_SetDynamicQ(smph[0]);
			QCurves[bright][curAmp]->h_Input(smph[0]);
			QCurves[bright][curAmp]->h_RunQ();
			smph[0] = QCurves[bright][curAmp]->h_GetOutput();
		}
	} else
	{
		if(precisionLevel == 0)
		{
			ToneStack_J::l_SetDynamicQ(smpl[0]);
			QInput[bright]->l_Input(smpl[0]);
			QInput[bright]->l_RunQ();
			smpl[0] = QInput[bright]->l_GetOutput();
		} else
		if(precisionLevel == 1)
		{
			ToneStack_J::s_SetDynamicQ(smps[0]);
			QInput[bright]->s_Input(smps[0]);
			QInput[bright]->s_RunQ();
			smps[0] = QInput[bright]->s_GetOutput();
		} else
		if(precisionLevel == 2)
		{
			ToneStack_J::h_SetDynamicQ(smph[0]);
			QInput[bright]->h_Input(smph[0]);
			QInput[bright]->h_RunQ();
			smph[0] = QInput[bright]->h_GetOutput();
		}
	}

	runUpSampling();

	RunPreAmp();
	RunPowerAmp();

	runDownSampling();

	if(precisionLevel == 0)
	{
		outFilter->l_Input(smpl[0]);
		outFilter->l_RunQ();
		smpl[0] = (flx) outFilter->l_GetOutput();

		if (ToneStack_J::setShaped)
		{
			QCurves[2][curAmp]->l_Input(smpl[0]);
			QCurves[2][curAmp]->l_RunQ();
			smpl[0] = (flx) QCurves[2][curAmp]->l_GetOutput();
		}

		ToneStack_J::l_SetInput(smpl[0]);
		ToneStack_J::RunEQ_Controls();
		smpl[0] = (flx) ToneStack_J::l_GetProcessedOutput();
		smpl[0] *= (flx) params.volumeAdj;
	} else
	if(precisionLevel == 1)
	{
		outFilter->s_Input(smps[0]);
		outFilter->s_RunQ();
		smps[0] = (fsx) outFilter->s_GetOutput();

		if (ToneStack_J::setShaped)
		{
			QCurves[2][curAmp]->s_Input(smps[0]);
			QCurves[2][curAmp]->s_RunQ();
			smps[0] = (fsx) QCurves[2][curAmp]->s_GetOutput();
		}

		ToneStack_J::s_SetInput(smps[0]);
		ToneStack_J::RunEQ_Controls();
		smps[0] = (fsx) ToneStack_J::s_GetProcessedOutput();
		smps[0] *= (fsx) params.volumeAdj;
	} else
	if(precisionLevel == 2)
	{
		outFilter->h_Input(smph[0]);
		outFilter->h_RunQ();
		smph[0] = (fhx) outFilter->h_GetOutput();

		if (ToneStack_J::setShaped)
		{
			QCurves[2][curAmp]->h_Input(smph[0]);
			QCurves[2][curAmp]->h_RunQ();
			smph[0] = (fhx) QCurves[2][curAmp]->h_GetOutput();
		}

		ToneStack_J::h_SetInput(smph[0]);
		ToneStack_J::RunEQ_Controls();
		smph[0] = (fhx) ToneStack_J::h_GetProcessedOutput();
		smph[0] *= (fhx) params.volumeAdj;
	}
}
void Tube_Bank_J::h_RunTubes_Pre(const int layer)
{
	for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
	{
		h_smp = fhx (smph[spNum]);

		f_pre_BiasBuffer[layer]->h_InputSmpl(h_abs(h_smp));
		f_pre_BiasBuffer[layer]->h_Run(true, false);
		if (h_smp >= h_C_0)
			h_smp -= (fhx) h_abs(h_DiodeScaled_S(h_abs(f_pre_BiasBuffer[layer]->h_GetOutput()), h_C_3, h_C_10 / (h_C_1 + h_pre_travelTop[layer])) * h_C_tenth);
		else
			h_smp -= (fhx) h_abs(h_DiodeScaled_S(h_abs(f_pre_BiasBuffer[layer]->h_GetOutput()), h_C_3, h_C_10 / (h_C_1 + h_pre_travelBtm[layer])) * h_C_tenth);

		h_smp *= fhx (h_pre_Amp_Mult * h_Gain);
		const fhx t_gain = fhx ( (h_Gain * h_pre_Hard) + h_C_1 );
		const fhx x_diff = fhx ( t_gain * h_pre_Grit * h_abs(h_smp - h_pre_TravelBuffer[layer]) );
		h_pre_TravelBuffer[layer] = (fhx) h_smp;
		h_pre_travelTop[layer] += (fhx) x_diff;
		h_pre_travelBtm[layer] += (fhx) x_diff;
		if (h_smp >= h_C_0)
		{
			h_pre_travelBtm[layer] = h_C_0;
			h_smp = (fhx) h_Triode_Simple(h_smp, t_gain, h_pre_travelTop[layer], h_pre_Slope);
		} else
		{
			h_pre_travelTop[layer] = h_C_0;
			h_smp = (fhx) h_Triode_Simple(h_smp, t_gain, h_pre_travelBtm[layer], h_pre_Slope);
		}

		smph[spNum] = fhx (h_smp);
	}
}
void Tube_Bank_J::h_RunTubes_Power(const int layer)
{
	for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
	{
		h_smp = fhx (smph[spNum]);

		f_pwr_BiasBuffer[layer]->h_InputSmpl(h_abs(h_smp));
		f_pwr_BiasBuffer[layer]->h_Run(true, false);
		if (h_smp >= h_C_0)
			h_smp -= (fhx) h_abs(h_DiodeScaled_S(h_abs(f_pwr_BiasBuffer[layer]->h_GetOutput()), h_C_3, h_C_10 / (h_C_1 + h_pwr_travelTop[layer])) * h_C_tenth);
		else
			h_smp -= (fhx) h_abs(h_DiodeScaled_S(h_abs(f_pwr_BiasBuffer[layer]->h_GetOutput()), h_C_3, h_C_10 / (h_C_1 + h_pwr_travelBtm[layer])) * h_C_tenth);

		h_smp *= fhx (h_pwr_Amp_Mult * h_Gain);
		const fhx t_gain = fhx ( (h_Gain * h_pwr_Hard) + h_C_1 );
		const fhx x_diff = fhx ( t_gain * h_pwr_Grit * h_abs(h_smp - h_pwr_TravelBuffer[layer]) );
		h_pwr_TravelBuffer[layer] = (fhx) h_smp;
		h_pwr_travelTop[layer] += (fhx) x_diff;
		h_pwr_travelBtm[layer] += (fhx) x_diff;
		if (h_smp >= h_C_0)
		{
			h_pwr_travelBtm[layer] = h_C_0;
			h_smp = (fhx) h_Triode_Simple(h_smp, t_gain, h_pwr_travelTop[layer], h_pwr_Slope);
		} else
		{
			h_pwr_travelTop[layer] = h_C_0;
			h_smp = (fhx) h_Triode_Simple(h_smp, t_gain, h_pwr_travelBtm[layer], h_pwr_Slope);
		}

		smph[spNum] = fhx (h_smp);
	}
}
void Tube_Bank_J::s_RunTubes_Pre(const int layer)
{
	for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
	{
		s_smp = fsx (smps[spNum]);

		f_pre_BiasBuffer[layer]->s_InputSmpl(s_abs(s_smp));
		f_pre_BiasBuffer[layer]->s_Run(true, false);
		if (s_smp >= s_C_0)
			s_smp -= (fsx) s_abs(s_DiodeScaled_S(s_abs(f_pre_BiasBuffer[layer]->s_GetOutput()), s_C_3, s_C_10 / (s_C_1 + s_pre_travelTop[layer])) * s_C_tenth);
		else
			s_smp -= (fsx) s_abs(s_DiodeScaled_S(s_abs(f_pre_BiasBuffer[layer]->s_GetOutput()), s_C_3, s_C_10 / (s_C_1 + s_pre_travelBtm[layer])) * s_C_tenth);

		s_smp *= fsx (s_pre_Amp_Mult * s_Gain);
		const fsx t_gain = fsx ( (s_Gain * s_pre_Hard) + s_C_1 );
		const fsx x_diff = fsx ( t_gain * s_pre_Grit * s_abs(s_smp - s_pre_TravelBuffer[layer]) );
		s_pre_TravelBuffer[layer] = (fsx) s_smp;
		s_pre_travelTop[layer] += (fsx) x_diff;
		s_pre_travelBtm[layer] += (fsx) x_diff;
		if (s_smp >= s_C_0)
		{
			s_pre_travelBtm[layer] = s_C_0;
			s_smp = (fsx) s_Triode_Simple(s_smp, t_gain, s_pre_travelTop[layer], s_pre_Slope);
		} else
		{
			s_pre_travelTop[layer] = s_C_0;
			s_smp = (fsx) s_Triode_Simple(s_smp, t_gain, s_pre_travelBtm[layer], s_pre_Slope);
		}

		smps[spNum] = fsx (s_smp);
	}
}
void Tube_Bank_J::s_RunTubes_Power(const int layer)
{
	for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
	{
		s_smp = fsx (smps[spNum]);

		f_pwr_BiasBuffer[layer]->s_InputSmpl(s_abs(s_smp));
		f_pwr_BiasBuffer[layer]->s_Run(true, false);
		if (s_smp >= s_C_0)
			s_smp -= (fsx) s_abs(s_DiodeScaled_S(s_abs(f_pwr_BiasBuffer[layer]->s_GetOutput()), s_C_3, s_C_10 / (s_C_1 + s_pwr_travelTop[layer])) * s_C_tenth);
		else
			s_smp -= (fsx) s_abs(s_DiodeScaled_S(s_abs(f_pwr_BiasBuffer[layer]->s_GetOutput()), s_C_3, s_C_10 / (s_C_1 + s_pwr_travelBtm[layer])) * s_C_tenth);

		s_smp *= fsx (s_pwr_Amp_Mult * s_Gain);
		const fsx t_gain = fsx ( (s_Gain * s_pwr_Hard) + s_C_1 );
		const fsx x_diff = fsx ( t_gain * s_pwr_Grit * s_abs(s_smp - s_pwr_TravelBuffer[layer]) );
		s_pwr_TravelBuffer[layer] = (fsx) s_smp;
		s_pwr_travelTop[layer] += (fsx) x_diff;
		s_pwr_travelBtm[layer] += (fsx) x_diff;
		if (s_smp >= s_C_0)
		{
			s_pwr_travelBtm[layer] = s_C_0;
			s_smp = (fsx) s_Triode_Simple(s_smp, t_gain, s_pwr_travelTop[layer], s_pwr_Slope);
		} else
		{
			s_pwr_travelTop[layer] = s_C_0;
			s_smp = (fsx) s_Triode_Simple(s_smp, t_gain, s_pwr_travelBtm[layer], s_pwr_Slope);
		}

		smps[spNum] = fsx (s_smp);
	}
}
void Tube_Bank_J::l_RunTubes_Pre(const int layer)
{
	for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
	{
		l_smp = flx (smpl[spNum]);

		f_pre_BiasBuffer[layer]->l_InputSmpl(l_abs(l_smp));
		f_pre_BiasBuffer[layer]->l_Run(true, false);
		if (l_smp >= l_C_0)
			l_smp -= (flx) l_abs(l_DiodeScaled_S(l_abs(f_pre_BiasBuffer[layer]->l_GetOutput()), l_C_3, l_C_10 / (l_C_1 + l_pre_travelTop[layer])) * l_C_tenth);
		else
			l_smp -= (flx) l_abs(l_DiodeScaled_S(l_abs(f_pre_BiasBuffer[layer]->l_GetOutput()), l_C_3, l_C_10 / (l_C_1 + l_pre_travelBtm[layer])) * l_C_tenth);

		l_smp *= flx (l_pre_Amp_Mult * l_Gain);
		const flx t_gain = flx ( (l_Gain * l_pre_Hard) + l_C_1 );
		const flx x_diff = flx ( t_gain * l_pre_Grit * l_abs(l_smp - l_pre_TravelBuffer[layer]) );
		l_pre_TravelBuffer[layer] = (flx) l_smp;
		l_pre_travelTop[layer] += (flx) x_diff;
		l_pre_travelBtm[layer] += (flx) x_diff;
		if (l_smp >= l_C_0)
		{
			l_pre_travelBtm[layer] = l_C_0;
			l_smp = (flx) l_Triode_Simple(l_smp, t_gain, l_pre_travelTop[layer], l_pre_Slope);
		} else
		{
			l_pre_travelTop[layer] = l_C_0;
			l_smp = (flx) l_Triode_Simple(l_smp, t_gain, l_pre_travelBtm[layer], l_pre_Slope);
		}

		smpl[spNum] = flx (l_smp);
	}
}
void Tube_Bank_J::l_RunTubes_Power(const int layer)
{
	for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
	{
		l_smp = flx (smpl[spNum]);

		f_pwr_BiasBuffer[layer]->l_InputSmpl(l_abs(l_smp));
		f_pwr_BiasBuffer[layer]->l_Run(true, false);
		if (l_smp >= l_C_0)
			l_smp -= (flx) l_abs(l_DiodeScaled_S(l_abs(f_pwr_BiasBuffer[layer]->l_GetOutput()), l_C_3, l_C_10 / (l_C_1 + l_pwr_travelTop[layer])) * l_C_tenth);
		else
			l_smp -= (flx) l_abs(l_DiodeScaled_S(l_abs(f_pwr_BiasBuffer[layer]->l_GetOutput()), l_C_3, l_C_10 / (l_C_1 + l_pwr_travelBtm[layer])) * l_C_tenth);

		l_smp *= flx (l_pwr_Amp_Mult * l_Gain);
		const flx t_gain = flx ( (l_Gain * l_pwr_Hard) + l_C_1 );
		const flx x_diff = flx ( t_gain * l_pwr_Grit * l_abs(l_smp - l_pwr_TravelBuffer[layer]) );
		l_pwr_TravelBuffer[layer] = (flx) l_smp;
		l_pwr_travelTop[layer] += (flx) x_diff;
		l_pwr_travelBtm[layer] += (flx) x_diff;
		if (l_smp >= l_C_0)
		{
			l_pwr_travelBtm[layer] = l_C_0;
			l_smp = (flx) l_Triode_Simple(l_smp, t_gain, l_pwr_travelTop[layer], l_pwr_Slope);
		} else
		{
			l_pwr_travelTop[layer] = l_C_0;
			l_smp = (flx) l_Triode_Simple(l_smp, t_gain, l_pwr_travelBtm[layer], l_pwr_Slope);
		}

		smpl[spNum] = flx (l_smp);
	}
}
