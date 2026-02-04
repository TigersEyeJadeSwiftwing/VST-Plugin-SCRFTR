#include "tube_bank_i.h"

using namespace ScMath;

Tube_Bank_I::Tube_Bank_I(const SC_TubeDistortion_Params *initParams, fsx spRate) : SampleBank_Cst(spRate),
	params(*initParams),
	stMod(fhx (fhx (initParams->qSpatialFactor))),
	h_distFreqHi(fhx (initParams->tFreqB)),
	h_distFreqLo(fhx (initParams->tFreqA)),
	s_distFreqHi(fsx (initParams->tFreqB)),
	s_distFreqLo(fsx (initParams->tFreqA)),
	l_distFreqHi(flx (initParams->tFreqB)),
	l_distFreqLo(flx (initParams->tFreqA)),
	l_pi(flx (SC_PI)),
	s_pi(fsx (SC_PI)),
	h_pi(fhx (SC_PI)),
	l_mpHard(flx (initParams->hrd_Power)),
	s_mpHard(fsx (initParams->hrd_Power)),
	h_mpHard(fhx (initParams->hrd_Power)),
	l_mprHard(flx (initParams->hrd_Pre)),
	s_mprHard(fsx (initParams->hrd_Pre)),
	h_mprHard(fhx (initParams->hrd_Pre)),
	l_Pre_Grit(flx (initParams->tPr_GritPre)),
	s_Pre_Grit(fsx (initParams->tPr_GritPre)),
	h_Pre_Grit(fhx (initParams->tPr_GritPre)),
	l_Power_Grit(flx (initParams->tPr_GritPower)),
	s_Power_Grit(fsx (initParams->tPr_GritPower)),
	h_Power_Grit(fhx (initParams->tPr_GritPower)),
	h_Slope_Pre(fhx (initParams->slope_Pre)),
	h_Slope_Power(fhx (initParams->slope_Power)),
	s_Slope_Pre(fsx (initParams->slope_Pre)),
	s_Slope_Power(fsx (initParams->slope_Power)),
	l_Slope_Pre(flx (initParams->slope_Pre)),
	l_Slope_Power(flx (initParams->slope_Power)),
	l_outVol(flx (initParams->volumeAdj)),
	s_outVol(fsx (initParams->volumeAdj)),
	h_outVol(fhx (initParams->volumeAdj)),
	freqStages(int (initParams->tFreqStages))
{
	h_sv = fhx (stMod);
	s_sv = fsx (stMod);
	l_sv = flx (stMod);

	FilterA = new Cst_EQ();
	FilterB = new Cst_EQ();
	FilterC = new Cst_EQ(true);

	FilterA->SetSmpRate(cRateS);
	FilterA->SetBands(s_sv, (fsx) s_distFreqLo);
	FilterB->SetSmpRate(cRateS);
	FilterB->SetBands(s_sv, (fsx) s_distFreqHi);
	FilterC->SetSmpRate(cRateS);
	FilterC->SetBands(params.tToneFilter_Low, params.tToneFilter_High);

	for (int layer = 0; layer < Dist_TB_I_LAYERS_POWER; layer++)
	{
		PowerFiltA[layer] = new Cst_EQ(true);
		PowerFiltB[layer] = new Cst_EQ(true);
		PowerFiltA[layer]->SetBands(5.0, params.tPr_FreqPower);
		PowerFiltB[layer]->SetBands(5.0, params.tPr_FreqPower);
		PowerFiltA[layer]->SetSmpRate(cRateS * rfV->vs[osRate]);
		PowerFiltB[layer]->SetSmpRate(cRateS * rfV->vs[osRate]);
	}

	for (int layer = 0; layer < Dist_TB_I_LAYERS_PRE; layer++)
	{
		PreFiltA[layer] = new Cst_EQ(true);
		PreFiltB[layer] = new Cst_EQ(true);
		PreFiltA[layer]->SetBands(5.0, params.tPr_FreqPre);
		PreFiltB[layer]->SetBands(5.0, params.tPr_FreqPre);
		PreFiltA[layer]->SetSmpRate(cRateS * rfV->vs[osRate]);
		PreFiltB[layer]->SetSmpRate(cRateS * rfV->vs[osRate]);
	}

	l_PreAmpMult = flx (s_PreAmpMult = fsx (h_PreAmpMult = fhx (params.ampMultPre)));
	l_PwrAmpMult = flx (s_PwrAmpMult = fsx (h_PwrAmpMult = fhx (params.ampMultPower)));

	h_spdFilter = (fhx) expl(fhx (-2.0) * h_pi * h_distFreqLo / cRateH);
	s_spdFilter = fsx (h_spdFilter);
	l_spdFilter = flx (s_spdFilter);

	FilterVt = new Cst_EQ(true);
	FilterVt->SetBands(5.0, 5.0);
	FilterVt->SetSmpRate(sRateS);

	outFilter = new BTW_LP(s_distFreqHi, cRateS);

	SetGain(fhx (0.10));

	clearBuffs();
}
Tube_Bank_I::~Tube_Bank_I()
{
	delete FilterA;
	delete FilterB;
	delete FilterC;
	delete FilterVt;

	delete outFilter;

	for (int layer = 0; layer < Dist_TB_I_LAYERS_POWER; layer++)
	{
		delete PowerFiltA[layer];
		delete PowerFiltB[layer];
	}
	for (int layer = 0; layer < Dist_TB_I_LAYERS_PRE; layer++)
	{
		delete PreFiltA[layer];
		delete PreFiltB[layer];
	}
}
void Tube_Bank_I::InitTubes()
{
	clearBuffs();
}
void Tube_Bank_I::setNSampleRate(fsx nSRate)
{
	SampleBank_Cst::setSampleRate(nSRate);

	// SampleBank_Cst::zeroAllSamples();

	clearBuffs();

	FilterA->SetSmpRate(cRateS);
	FilterA->SetBands(s_sv, (fsx) s_distFreqLo);
	FilterB->SetSmpRate(cRateS);
	FilterB->SetBands(s_sv, (fsx) s_distFreqHi);
	FilterC->SetSmpRate(cRateS);

	for (int layer = 0; layer < Dist_TB_I_LAYERS_POWER; layer++)
	{
		PowerFiltA[layer]->SetSmpRate(cRateS);
		PowerFiltB[layer]->SetSmpRate(cRateS);
	}
	for (int layer = 0; layer < Dist_TB_I_LAYERS_PRE; layer++)
	{
		PreFiltA[layer]->SetSmpRate(cRateS);
		PreFiltB[layer]->SetSmpRate(cRateS);
	}

	FilterVt->SetSmpRate(sRateS);
	outFilter->setSampleRate(cRateS);

	h_spdFilter = (fhx) expl(fhx (-2.0) * h_pi * h_distFreqLo / cRateH);
	s_spdFilter = fsx (h_spdFilter);
	l_spdFilter = flx (s_spdFilter);
}
void Tube_Bank_I::SetGain(const fsx nGain)
{
	h_Gain = fhx (nGain);

	if(fhx (h_Gain) < fhx (rfV->zl)) h_Gain = fhx (rfV->zl);
	if(fhx (h_Gain) > fhx (1.0)) h_Gain = fhx (1.0);

	s_Gain = fsx (h_Gain);
	l_Gain = flx (s_Gain);
}
void Tube_Bank_I::clearBuffs()
{
	h_dfltA = fhx (rfV->zh);
	h_dfltB = fhx (rfV->zh);
	s_dfltA = fsx (rfV->zs);
	s_dfltB = fsx (rfV->zs);
	l_dfltA = flx (rfV->zl);
	l_dfltB = flx (rfV->zl);

	for (int z = 0; z < params.tFreqStages; z++)
	{
		l_qFiltT[z] = flx (rfV->zl);
		s_qFiltT[z] = fsx (rfV->zs);
		h_qFiltT[z] = fhx (rfV->zh);
	}

	l_ampVo = 0.f;
	s_ampVo = 0.0;
	h_ampVo = 0.0;
	l_ampVt = 0.f;
	s_ampVt = 0.0;
	h_ampVt = 0.0;
}
void Tube_Bank_I::zeroAllSamples()
{
	SampleBank_Cst::zeroAllSamples();
}
void Tube_Bank_I::setOverSampling(const int oSLevel)
{
	SampleBank_Cst::setOverSampling(oSLevel);

	clearBuffs();

	FilterA->SetSmpRate(cRateS);
	FilterA->SetBands(s_sv, (fsx) s_distFreqLo);
	FilterB->SetSmpRate(cRateS);
	FilterB->SetBands(s_sv, (fsx) s_distFreqHi);
	FilterC->SetSmpRate(cRateS);

	for (int layer = 0; layer < Dist_TB_I_LAYERS_POWER; layer++)
	{
		PowerFiltA[layer]->SetSmpRate(cRateS);
		PowerFiltB[layer]->SetSmpRate(cRateS);
	}
	for (int layer = 0; layer < Dist_TB_I_LAYERS_PRE; layer++)
	{
		PreFiltA[layer]->SetSmpRate(cRateS);
		PreFiltB[layer]->SetSmpRate(cRateS);
	}

	outFilter->setSampleRate(cRateS);

	h_spdFilter = (fhx) expl(fhx (-2.0) * h_pi * h_distFreqLo / cRateH);
	s_spdFilter = fsx (h_spdFilter);
	l_spdFilter = flx (s_spdFilter);
}
void Tube_Bank_I::RunPreAmp()
{
	return;
}
void Tube_Bank_I::RunPowerAmp()
{
	return;
}
void Tube_Bank_I::ApplyDistortion()
{
	if(osRate <= 0) return;
	if(osRate >= 6) return;
//! ----------------------------------------------------------------------------------------------------------------
//! 32-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(precisionLevel == 0)
	{
		const flx mDFAa = l_spdFilter;
		const flx mDFAb = flx (1.0 - mDFAa);
//!		Input Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterA->l_InputSmpl(smpl[x]);
			FilterA->l_Run(true, true);
			smpl[x] = (flx) FilterA->l_GetOutput();
			FilterC->l_InputSmpl(smpl[x]);
			FilterC->l_Run(true, true);
			smpl[x] += (flx) FilterC->l_GetOutput();

			const flx dr_smp = flx (smpl[x]);
			smpl[x] *= flx (2.0f);
			smpl[x] = flx ((smpl[x] * l_abs(smpl[x])) / (flx (1.0f) + l_abs(smpl[x]) + (smpl[x]*smpl[x])));
			smpl[x] = flx (0.50f * (smpl[x] + dr_smp));
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			l_smp = flx (smpl[spNum]);
//!		Harmonics
			flx hrmFX = 3.0f;
			if (l_abs(l_smp) > 1.0f) hrmFX /= flx (l_abs(l_smp));
			flx hrmnA = flx (l_abs(l_smp) * hrmFX);
			if(hrmnA > 4.0f) hrmnA -= 4.0f;
			if(hrmnA > 4.0f) hrmnA = rfV->l_dnrm;
			flx hrmnB = rfV->l_dnrm;
			if (hrmnA < 2.0f)
			{
				hrmnB = hrmnA;
				hrmnB = flx ((hrmnB*0.50f) + 0.50f);
				hrmnB *= flx (2.0f - hrmnB);
				hrmnB -= 0.750f;
				hrmnB *= 4.0f;
			} else
			{
				hrmnB = flx (0.0f - (hrmnA - 2.0f));
				hrmnB = flx ((hrmnB*0.50f) - 0.50f);
				hrmnB *= flx (2.0f + hrmnB);
				hrmnB += 0.750f;
				hrmnB *= 4.0f;
			}
			if (l_smp < 0.0f) hrmnB = -hrmnB;
			l_smp += flx (hrmnB * 0.50f);
//!		Pre-Amp
			flx dLevel = 0.0f;
			dLevel = flx (l_Gain * l_PreAmpMult);

			for (int layer = 0; layer < Dist_TB_I_LAYERS_PRE; layer++)
			{
				if (l_smp >= 0.0f)
				{
					PreFiltA[layer]->l_InputSmpl(flx (0.0f));
				} else
				{
					PreFiltA[layer]->l_InputSmpl(flx (1.0f));
					PreFiltA[layer]->l_Run();
					PreFiltA[layer]->l_Run();
					PreFiltA[layer]->l_Run();
				}
				PreFiltA[layer]->l_Run();
				flx xPre = flx (1.0f - PreFiltA[layer]->l_GetOutput());
				xPre = (flx) l_Diode_U(xPre, flx (l_Pre_Grit * (flx (1.0f) - l_ampVt)));
				xPre = flx (1.0f - xPre);

				const flx fpp = (flx) l_Diode_S(l_smp, l_Slope_Pre);
				PreFiltB[layer]->l_InputSmpl(flx (fpp));
				PreFiltB[layer]->l_Run();

				flx drySnd = flx (xPre * l_smp);
				drySnd *= flx ( (flx (3.0f) + l_abs(drySnd)) / ((flx (1.0f)-PreFiltB[layer]->l_GetOutput()) + (flx (3.0f)*l_abs(drySnd)) + (drySnd*drySnd)) );

				dLevel = flx (l_Gain * l_PreAmpMult * xPre);
				l_smp *= flx (flx (dLevel) + flx (rfV->l_dnrm));
				const flx dAp = flx (((dLevel) * l_mprHard) + 1.0f);
				l_smp = flx ( l_smp * (dAp + l_abs(l_smp)) / ( (dAp * (1.0f - PreFiltB[layer]->l_GetOutput())) + (dAp * l_abs(l_smp)) + (l_smp*l_smp) ) );

				l_smp += flx (drySnd);
				l_smp *= flx (-0.50f);
			}
//!		Power-Amp
			for (int frStage = 0; frStage < params.tFreqStages; frStage++)
			{
				l_smp = flx (l_qFiltT[frStage] = flx (
						mDFAb * l_smp + mDFAa * l_qFiltT[frStage] + rfV->l_dnrm));
			}

			for (int layer = 0; layer < Dist_TB_I_LAYERS_POWER; layer++)
			{
				if (l_smp >= 0.0f)
				{
					PowerFiltA[layer]->l_InputSmpl(flx (0.0f));
				} else
				{
					PowerFiltA[layer]->l_InputSmpl(flx (1.0f));
					PowerFiltA[layer]->l_Run();
					PowerFiltA[layer]->l_Run();
					PowerFiltA[layer]->l_Run();
				}
				PowerFiltA[layer]->l_Run();
				flx xPwr = flx (1.0 - PowerFiltA[layer]->l_GetOutput());
				xPwr = (flx) l_Diode_U(xPwr, flx (l_Power_Grit * (flx (1.0f) - l_ampVt)));
				xPwr = flx (1.0f - xPwr);

				const flx fp = (flx) l_Diode_S(l_smp, l_Slope_Power);
				PowerFiltB[layer]->l_InputSmpl(flx (fp));
				PowerFiltB[layer]->l_Run();

				flx drySnd = flx (xPwr * l_smp);
				drySnd *= flx ( (flx (3.0f) + l_abs(drySnd)) / ((flx (1.0f)-PowerFiltB[layer]->l_GetOutput()) + (flx (3.0f)*l_abs(drySnd)) + (drySnd*drySnd)) );

				dLevel = flx (l_Gain * l_PwrAmpMult * xPwr);
				l_smp *= flx (flx (dLevel) + flx (rfV->l_dnrm));
				const flx dA = flx (((dLevel) * l_mpHard) + 1.0f);
				l_smp = flx ( l_smp * (dA + l_abs(l_smp)) / ( (dA * (1.0f - PowerFiltB[layer]->l_GetOutput())) + (dA * l_abs(l_smp)) + (l_smp*l_smp) ) );

				l_smp += flx (drySnd);
				l_smp *= flx (-0.50f);
			}
			l_smp *= (flx) l_outVol;
			smpl[spNum] = l_smp;
		}
//!		Output Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			outFilter->inpSplL(flx (smpl[x]));
			outFilter->RunFilter();
			smpl[x] = (flx) outFilter->GetOutPutL();
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 64-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(precisionLevel == 1)
	{
		const fsx mDFAa = s_spdFilter;
		const fsx mDFAb = fsx (1.0 - mDFAa);
//!		Input Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterA->s_InputSmpl(smps[x]);
			FilterA->s_Run(true, true);
			smps[x] = (fsx) FilterA->s_GetOutput();
			FilterC->s_InputSmpl(smps[x]);
			FilterC->s_Run(true, true);
			smps[x] += (fsx) FilterC->s_GetOutput();

			const fsx dr_smp = fsx (smps[x]);
			smps[x] *= fsx (2.0);
			smps[x] = fsx ((smps[x] * s_abs(smps[x])) / (fsx (1.0) + s_abs(smps[x]) + (smps[x]*smps[x])));
			smps[x] = fsx (0.50 * (smps[x] + dr_smp));
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			s_smp = fsx (smps[spNum]);
//!		Harmonics
			fsx hrmFX = 3.0;
			if (s_abs(s_smp) > 1.0) hrmFX /= fsx (s_abs(s_smp));
			fsx hrmnA = fsx (s_abs(s_smp) * hrmFX);
			if(hrmnA > 4.0) hrmnA -= 4.0;
			if(hrmnA > 4.0) hrmnA = rfV->s_dnrm;
			fsx hrmnB = rfV->s_dnrm;
			if (hrmnA < 2.0)
			{
				hrmnB = hrmnA;
				hrmnB = fsx ((hrmnB*0.50) + 0.50);
				hrmnB *= fsx (2.0 - hrmnB);
				hrmnB -= 0.750;
				hrmnB *= 4.0;
			} else
			{
				hrmnB = fsx (0.0 - (hrmnA - 2.0));
				hrmnB = fsx ((hrmnB*0.50) - 0.50);
				hrmnB *= fsx (2.0 + hrmnB);
				hrmnB += 0.750;
				hrmnB *= 4.0;
			}
			if (s_smp < 0.0) hrmnB = -hrmnB;
			s_smp += fsx (hrmnB * 0.50);
//!		Pre-Amp
			fsx dLevel = 0.0;
			dLevel = fsx (s_Gain * s_PreAmpMult);
			for (int layer = 0; layer < Dist_TB_I_LAYERS_PRE; layer++)
			{
				if (s_smp >= 0.0)
				{
					PreFiltA[layer]->s_InputSmpl(fsx (0.0));
				} else
				{
					PreFiltA[layer]->s_InputSmpl(fsx (1.0));
					PreFiltA[layer]->s_Run();
					PreFiltA[layer]->s_Run();
					PreFiltA[layer]->s_Run();
				}
				PreFiltA[layer]->s_Run();
				fsx xPre = fsx (1.0 - PreFiltA[layer]->s_GetOutput());
				xPre = (fsx) s_Diode_U(xPre, fsx (s_Pre_Grit * (fsx (1.0) - s_ampVt)));
				xPre = fsx (1.0 - xPre);

				const fsx fpp = (fsx) s_Diode_S(s_smp, s_Slope_Pre);
				PreFiltB[layer]->s_InputSmpl(fsx (fpp));
				PreFiltB[layer]->s_Run();

				fsx drySnd = fsx (xPre * s_smp);
				drySnd *= fsx ( (fsx (3.0) + s_abs(drySnd)) / ((fsx (1.0)-PreFiltB[layer]->s_GetOutput()) + (fsx (3.0)*s_abs(drySnd)) + (drySnd*drySnd)) );

				dLevel = fsx (s_Gain * s_PreAmpMult * xPre);
				s_smp *= fsx (fsx (dLevel) + fsx (rfV->s_dnrm));
				const fsx dAp = fsx (((dLevel) * s_mprHard) + 1.0);
				s_smp = fsx ( s_smp * (dAp + s_abs(s_smp)) / ( (dAp * (1.0 - PreFiltB[layer]->s_GetOutput())) + (dAp * s_abs(s_smp)) + (s_smp*s_smp) ) );

				s_smp += fsx (drySnd);
				s_smp *= fsx (-0.50);
			}
//!		Power-Amp
			for (int frStage = 0; frStage < params.tFreqStages; frStage++)
			{
				s_smp = fsx (s_qFiltT[frStage] = fsx (
						mDFAb * s_smp + mDFAa * s_qFiltT[frStage] + rfV->s_dnrm));
			}

			for (int layer = 0; layer < Dist_TB_I_LAYERS_POWER; layer++)
			{
				if (s_smp >= 0.0)
				{
					PowerFiltA[layer]->s_InputSmpl(fsx (0.0));
				} else
				{
					PowerFiltA[layer]->s_InputSmpl(fsx (1.0));
					PowerFiltA[layer]->s_Run();
					PowerFiltA[layer]->s_Run();
					PowerFiltA[layer]->s_Run();
				}
				PowerFiltA[layer]->s_Run();
				fsx xPwr = fsx (1.0 - PowerFiltA[layer]->s_GetOutput());
				xPwr = (fsx) s_Diode_U(xPwr, fsx (s_Power_Grit * (fsx (1.0) - s_ampVt)));
				xPwr = fsx (1.0 - xPwr);

				const fsx fp = (fsx) s_Diode_S(s_smp, s_Slope_Power);
				PowerFiltB[layer]->s_InputSmpl(fsx (fp));
				PowerFiltB[layer]->s_Run();

				fsx drySnd = fsx (xPwr * s_smp);
				drySnd *= fsx ( (fsx (3.0) + s_abs(drySnd)) / ((fsx (1.0)-PowerFiltB[layer]->s_GetOutput()) + (fsx (3.0)*s_abs(drySnd)) + (drySnd*drySnd)) );

				dLevel = fsx (s_Gain * s_PwrAmpMult * xPwr);
				s_smp *= fsx (fsx (dLevel) + fsx (rfV->s_dnrm));
				const fsx dA = fsx (((dLevel) * s_mpHard) + 1.0);
				s_smp = fsx ( s_smp * (dA + s_abs(s_smp)) / ( (dA * (1.0 - PowerFiltB[layer]->s_GetOutput())) + (dA * s_abs(s_smp)) + (s_smp*s_smp) ) );

				s_smp += fsx (drySnd);
				s_smp *= fsx (-0.50);
			}
			s_smp *= (fsx) s_outVol;
			smps[spNum] = s_smp;
		}
//!		Output Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			outFilter->inpSplS(fsx (smps[x]));
			outFilter->RunFilter();
			smps[x] = (fsx) outFilter->GetOutPutS();
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 128-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(precisionLevel == 2)
	{
		const fhx mDFAa = h_spdFilter;
		const fhx mDFAb = fhx (1.0 - h_spdFilter);
//!		Input Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterA->h_InputSmpl(smph[x]);
			FilterA->h_Run(true, true);
			smph[x] = (fhx) FilterA->h_GetOutput();
			FilterC->h_InputSmpl(smph[x]);
			FilterC->h_Run(true, true);
			smph[x] += (fhx) FilterC->h_GetOutput();

			const fhx dr_smp = fhx (smph[x]);
			smph[x] *= fhx (2.0);
			smph[x] = fhx ((smph[x] * h_abs(smph[x])) / (fhx (1.0) + h_abs(smph[x]) + (smph[x]*smph[x])));
			smph[x] = fhx (0.50 * (smph[x] + dr_smp));
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			h_smp = fhx (smph[spNum]);
//!		Harmonics
			fhx hrmFX = (fhx) 3.0;
			if (h_abs(h_smp) > 1.0) hrmFX /= fhx (h_abs(h_smp));
			fhx hrmnA = fhx (h_abs(h_smp) * hrmFX);
			if(hrmnA > 4.0) hrmnA -= 4.0;
			if(hrmnA > 4.0) hrmnA = rfV->h_dnrm;
			fhx hrmnB = rfV->h_dnrm;
			if (hrmnA < 2.0)
			{
				hrmnB = hrmnA;
				hrmnB = fhx ((hrmnB*0.50) + 0.50);
				hrmnB *= fhx (2.0 - hrmnB);
				hrmnB -= 0.750;
				hrmnB *= 4.0;
			} else
			{
				hrmnB = fhx (0.0 - (hrmnA - 2.0));
				hrmnB = fhx ((hrmnB*0.50) - 0.50);
				hrmnB *= fhx (2.0 + hrmnB);
				hrmnB += 0.750;
				hrmnB *= 4.0;
			}
			if (h_smp < 0.0) hrmnB = -hrmnB;
			h_smp += fhx (hrmnB * 0.50);
//!		Pre-Amp
			fhx dLevel = (fhx) 0.0;
			dLevel = fhx (h_Gain * h_PreAmpMult);

			for (int layer = 0; layer < Dist_TB_I_LAYERS_PRE; layer++)
			{
				if (h_smp >= 0.0)
				{
					PreFiltA[layer]->h_InputSmpl(fhx (0.0));
				} else
				{
					PreFiltA[layer]->h_InputSmpl(fhx (1.0));
					PreFiltA[layer]->h_Run();
					PreFiltA[layer]->h_Run();
					PreFiltA[layer]->h_Run();
				}
				PreFiltA[layer]->h_Run();
				fhx xPre = fhx (1.0 - PreFiltA[layer]->h_GetOutput());
				xPre = (fhx) h_Diode_U(xPre, fhx (h_Pre_Grit * (fhx (1.0) - h_ampVt)));
				xPre = fhx (1.0 - xPre);

				const fhx fpp = (fhx) h_Diode_S(h_smp, h_Slope_Pre);
				PreFiltB[layer]->h_InputSmpl(fhx (fpp));
				PreFiltB[layer]->h_Run();

				fhx drySnd = fhx ((fhx (1.0) - xPre) * h_smp);
				drySnd *= fhx ( (fhx (3.0) + h_abs(drySnd)) / ((fhx (1.0)-PreFiltB[layer]->h_GetOutput()) + (fhx (3.0)*h_abs(drySnd)) + (drySnd*drySnd)) );

				dLevel = fhx (h_Gain * h_PreAmpMult * xPre);
				h_smp *= fhx (fhx (dLevel) + fhx (rfV->h_dnrm));
				const fhx dAp = fhx (((dLevel) * h_mprHard) + (fhx) 1.0);
				h_smp = fhx ( h_smp * (dAp + h_abs(h_smp)) / ( (dAp * ((fhx) 1.0 - PreFiltB[layer]->h_GetOutput())) + (dAp * h_abs(h_smp)) + (h_smp*h_smp) ) );

				h_smp += fhx (drySnd);
				h_smp *= fhx (-0.50);
			}
//!		Power-Amp
			for (int frStage = 0; frStage < params.tFreqStages; frStage++)
			{
				h_smp = fhx (h_qFiltT[frStage] = fhx (
						mDFAb * h_smp + mDFAa * h_qFiltT[frStage] + rfV->h_dnrm));
			}

			for (int layer = 0; layer < Dist_TB_I_LAYERS_POWER; layer++)
			{
				if (h_smp >= 0.0)
				{
					PowerFiltA[layer]->h_InputSmpl(fhx (0.0));
				} else
				{
					PowerFiltA[layer]->h_InputSmpl(fhx (1.0));
					PowerFiltA[layer]->h_Run();
					PowerFiltA[layer]->h_Run();
					PowerFiltA[layer]->h_Run();
				}
				PowerFiltA[layer]->h_Run();
				fhx xPwr = fhx (1.0 - PowerFiltA[layer]->h_GetOutput());
				xPwr = (fhx) h_Diode_U(xPwr, fhx (h_Power_Grit * (fhx (1.0) - h_ampVt)));
				xPwr = fhx (1.0 - xPwr);

				const fhx fp = (fhx) h_Diode_S(h_smp, h_Slope_Power);
				PowerFiltB[layer]->h_InputSmpl(fhx (fp));
				PowerFiltB[layer]->h_Run();

				fhx drySnd = fhx ((fhx (1.0) - xPwr) * h_smp);
				drySnd *= fhx ( (fhx (3.0) + h_abs(drySnd)) / ((fhx (1.0)-PowerFiltB[layer]->h_GetOutput()) + (fhx (3.0)*h_abs(drySnd)) + (drySnd*drySnd)) );

				dLevel = fhx (h_Gain * h_PwrAmpMult * xPwr);
				h_smp *= fhx (fhx (dLevel) + fhx (rfV->h_dnrm));
				const fhx dA = fhx (((dLevel) * h_mpHard) + (fhx) 1.0);
				h_smp = fhx ( h_smp * (dA + h_abs(h_smp)) / ( (dA * ((fhx) 1.0 - PowerFiltB[layer]->h_GetOutput())) + (dA * h_abs(h_smp)) + (h_smp*h_smp) ) );

				h_smp += fhx (drySnd);
				h_smp *= fhx (-0.50);
			}

			h_smp *= (fhx) h_outVol;
			smph[spNum] = (fhx) h_smp;
		}
//!		Output Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			outFilter->inpSplH(fhx (smph[x]));
			outFilter->RunFilter();
			smph[x] = (fhx) outFilter->GetOutPutH();
		}
	}
}
void Tube_Bank_I::SetAmpVt(const fsx inp)
{
	s_SetAmpVt((fsx) inp);
	l_ampVt = flx (s_ampVt);
	h_ampVt = fhx (s_ampVt);
}
void Tube_Bank_I::l_SetAmpVt(const flx inp)
{
	const flx l_spl = (flx) l_abs(inp);
	FilterVt->l_InputSmpl(l_spl);
	FilterVt->l_Run(true, false);
	if (l_spl >= l_ampVo)
	{
		FilterVt->l_Run(true, false);
	}
	l_ampVt = flx (FilterVt->l_GetOutput() * 2.0f);
	l_ampVt = (flx) l_Diode_U(l_ampVt, (flx) 3.0f);

	l_ampVo = (flx) l_spl;
}
void Tube_Bank_I::s_SetAmpVt(const fsx inp)
{
	const fsx s_spl = (fsx) s_abs(inp);
	FilterVt->s_InputSmpl(s_spl);
	FilterVt->s_Run(true, false);
	if (s_spl >= s_ampVo)
	{
		FilterVt->s_Run(true, false);
	}
	s_ampVt = fsx (FilterVt->s_GetOutput() * 2.0);
	s_ampVt = (fsx) s_Diode_U(s_ampVt, (fsx) 3.0);

	s_ampVo = (fsx) s_spl;
}
void Tube_Bank_I::h_SetAmpVt(const fhx inp)
{
	const fhx h_spl = (fhx) h_abs(inp);
	FilterVt->h_InputSmpl(h_spl);
	FilterVt->h_Run(true, false);
	if (h_spl >= h_ampVo)
	{
		FilterVt->h_Run(true, false);
	}
	h_ampVt = fhx (FilterVt->h_GetOutput() * fhx (2.0));
	h_ampVt = (fhx) h_Diode_U(h_ampVt, (fhx) 3.0);

	h_ampVo = (fhx) h_spl;
}
