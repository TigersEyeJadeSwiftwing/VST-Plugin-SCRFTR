#include "tube_bank_f.h"

using namespace ScMath;

Tube_Bank_F::Tube_Bank_F(fsx spRate, fhx mdfr) : SampleBank_Cst(spRate),
	stMod(fhx (fhx (mdfr))),
	currChannel(0),
	h_distFreqHi(fhx (Dist_TB_TubeFreqB)),
	h_distFreqLo(fhx (Dist_TB_TubeFreqA)),
	s_distFreqHi(fsx (Dist_TB_TubeFreqB)),
	s_distFreqLo(fsx (Dist_TB_TubeFreqA)),
	l_distFreqHi(flx (Dist_TB_TubeFreqB)),
	l_distFreqLo(flx (Dist_TB_TubeFreqA)),
	l_pi(flx (SC_PI)),
	s_pi(fsx (SC_PI)),
	h_pi(fhx (SC_PI)),
	h_tRatio(fhx (Dist_TB_T_Ratio)),
	s_tRatio(fsx (h_tRatio)),
	l_tRatio(flx (s_tRatio)),
	l_mpHard(flx (Dist_TB_PowerAmp_Hardness)),
	s_mpHard(fsx (Dist_TB_PowerAmp_Hardness)),
	h_mpHard(fhx (Dist_TB_PowerAmp_Hardness)),
	l_mprHard(flx (Dist_TB_PreAmp_Hardness)),
	s_mprHard(fsx (Dist_TB_PreAmp_Hardness)),
	h_mprHard(fhx (Dist_TB_PreAmp_Hardness)),
	l_Pre_Grit(flx (Dist_TB_PreAmp_Grit)),
	s_Pre_Grit(fsx (Dist_TB_PreAmp_Grit)),
	h_Pre_Grit(fhx (Dist_TB_PreAmp_Grit)),
	l_Power_Grit(flx (Dist_TB_POwerAmp_Grit)),
	s_Power_Grit(fsx (Dist_TB_POwerAmp_Grit)),
	h_Power_Grit(fhx (Dist_TB_POwerAmp_Grit)),
	h_Slope_Pre(fhx (Dist_TB_PreAmp_Slope)),
	h_Slope_Power(fhx (Dist_TB_PowerAmp_Slope)),
	s_Slope_Pre(fsx (Dist_TB_PreAmp_Slope)),
	s_Slope_Power(fsx (Dist_TB_PowerAmp_Slope)),
	l_Slope_Pre(flx (Dist_TB_PreAmp_Slope)),
	l_Slope_Power(flx (Dist_TB_PowerAmp_Slope))
//	feedBack(fhx (Dist_TB_Feedback))
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
	FilterC->SetBands(Dist_TB_Distortion_ToneFilter_Low, Dist_TB_Distortion_ToneFilter_High);

//	h_tSpeed = fhx (fhx (Dist_TB_T_Speed) / cRateH);
//	s_tSpeed = fsx (h_tSpeed);
//	l_tSpeed = flx (h_tSpeed);

//	l_PwrFeedback = flx (s_PwrFeedback = fsx (h_PwrFeedback = fhx (feedBack)));

	PowerFiltA = new Cst_EQ(true);
	PowerFiltB = new Cst_EQ(true);
	PowerFiltA->SetBands(5.0, Dist_TB_PowerAmp_Frequency);
	PowerFiltB->SetBands(5.0, Dist_TB_PowerAmp_Frequency);
	PowerFiltA->SetSmpRate(cRateS * rfV->vs[osRate]);
	PowerFiltB->SetSmpRate(cRateS * rfV->vs[osRate]);

	PreFiltA = new Cst_EQ(true);
	PreFiltB = new Cst_EQ(true);
	PreFiltA->SetBands(5.0, Dist_TB_PreAmp_Frequency);
	PreFiltB->SetBands(5.0, Dist_TB_PreAmp_Frequency);
	PreFiltA->SetSmpRate(cRateS * rfV->vs[osRate]);
	PreFiltB->SetSmpRate(cRateS * rfV->vs[osRate]);

	l_PreAmpMult[0] = flx (s_PreAmpMult[0] = fsx (h_PreAmpMult[0] = fhx (Dist_TB_AmpMult_Pre_Low)));
	l_PreAmpMult[2] = flx (s_PreAmpMult[2] = fsx (h_PreAmpMult[2] = fhx (Dist_TB_AmpMult_Pre_Low)));
	l_PreAmpMult[1] = flx (s_PreAmpMult[1] = fsx (h_PreAmpMult[1] = fhx (Dist_TB_AmpMult_Pre_High)));
	l_PreAmpMult[3] = flx (s_PreAmpMult[3] = fsx (h_PreAmpMult[3] = fhx (Dist_TB_AmpMult_Pre_High)));

	l_PwrAmpMult[0] = flx (s_PwrAmpMult[0] = fsx (h_PwrAmpMult[0] = fhx (Dist_TB_AmpMult_Power_Low)));
	l_PwrAmpMult[1] = flx (s_PwrAmpMult[1] = fsx (h_PwrAmpMult[1] = fhx (Dist_TB_AmpMult_Power_Low)));
	l_PwrAmpMult[2] = flx (s_PwrAmpMult[2] = fsx (h_PwrAmpMult[2] = fhx (Dist_TB_AmpMult_Power_High)));
	l_PwrAmpMult[3] = flx (s_PwrAmpMult[3] = fsx (h_PwrAmpMult[3] = fhx (Dist_TB_AmpMult_Power_High)));

	h_spdFilter = (fhx) expl(fhx (-2.0) * h_pi * h_distFreqLo / cRateH);
	s_spdFilter = fsx (h_spdFilter);
	l_spdFilter = flx (s_spdFilter);

	clearBuffs();
}
Tube_Bank_F::~Tube_Bank_F()
{
	delete FilterA;
	delete FilterB;
	delete FilterC;

	delete PowerFiltA;
	delete PowerFiltB;
	delete PreFiltA;
	delete PreFiltB;
}
void Tube_Bank_F::InitTubes()
{
	clearBuffs();
}
void Tube_Bank_F::setSampleRate(fsx nSRate)
{
	SampleBank_Cst::setSampleRate(nSRate);

	SampleBank_Cst::zeroAllSamples();

	clearBuffs();

//	h_tSpeed = fhx (fhx (Dist_TB_T_Speed) / cRateH);
//	s_tSpeed = fsx (h_tSpeed);
//	l_tSpeed = flx (h_tSpeed);

	FilterA->SetSmpRate(cRateS);
	FilterA->SetBands(s_sv, (fsx) s_distFreqLo);
	FilterB->SetSmpRate(cRateS);
	FilterB->SetBands(s_sv, (fsx) s_distFreqHi);
	FilterC->SetSmpRate(cRateS);

//	l_PwrFeedback = flx (s_PwrFeedback = fsx (h_PwrFeedback = fhx (feedBack)));

	PowerFiltA->SetSmpRate(cRateS);
	PowerFiltB->SetSmpRate(cRateS);
	PreFiltA->SetSmpRate(cRateS);
	PreFiltB->SetSmpRate(cRateS);

	h_spdFilter = (fhx) expl(fhx (-2.0) * h_pi * h_distFreqLo / cRateH);
	s_spdFilter = fsx (h_spdFilter);
	l_spdFilter = flx (s_spdFilter);
}
void Tube_Bank_F::SetGain(const fsx nGain)
{
	h_Gain = fhx (nGain);

	if(fhx (h_Gain) < fhx (rfV->zh)) h_Gain = fhx (rfV->zl);
	if(fhx (h_Gain) > fhx (1.0)) h_Gain = fhx (1.0);

//	h_Gain *= fhx (2.0 - h_Gain);
	h_Gain *= fhx (Dist_TB_Gain_Mult_Factor);

	s_Gain = fsx (h_Gain);
	l_Gain = flx (s_Gain);
}
void Tube_Bank_F::clearBuffs()
{
	h_dfltA = fhx (rfV->zh);
	h_dfltB = fhx (rfV->zh);
	s_dfltA = fsx (rfV->zs);
	s_dfltB = fsx (rfV->zs);
	l_dfltA = flx (rfV->zl);
	l_dfltB = flx (rfV->zl);

//	l_tPos = flx (0.0f);
//	s_tPos = fsx (0.0);
//	h_tPos = fhx (0.0);

	for (int z = 0; z < Dist_TB_TubeFreqStages; z++)
	{
		l_qFiltT[z] = flx (rfV->zl);
		s_qFiltT[z] = fsx (rfV->zs);
		h_qFiltT[z] = fhx (rfV->zh);
	}

	waveSide = false;
}
void Tube_Bank_F::zeroAllSamples()
{
	SampleBank_Cst::zeroAllSamples();
}
void Tube_Bank_F::setOverSampling(const int oSLevel)
{
	SampleBank_Cst::setOverSampling(oSLevel);

	clearBuffs();

//	h_tSpeed = fhx (fhx (Dist_TB_T_Speed) / cRateH);
//	s_tSpeed = fsx (h_tSpeed);
//	l_tSpeed = flx (h_tSpeed);

	FilterA->SetSmpRate(cRateS);
	FilterA->SetBands(s_sv, (fsx) s_distFreqLo);
	FilterB->SetSmpRate(cRateS);
	FilterB->SetBands(s_sv, (fsx) s_distFreqHi);
	FilterC->SetSmpRate(cRateS);

	PowerFiltA->SetSmpRate(cRateS);
	PowerFiltB->SetSmpRate(cRateS);
	PreFiltA->SetSmpRate(cRateS);
	PreFiltB->SetSmpRate(cRateS);

	h_spdFilter = (fhx) expl(fhx (-2.0) * h_pi * h_distFreqLo / cRateH);
	s_spdFilter = fsx (h_spdFilter);
	l_spdFilter = flx (s_spdFilter);
}
void Tube_Bank_F::RunPreAmp()
{
	return;
}
void Tube_Bank_F::RunPowerAmp()
{
	return;
}
void Tube_Bank_F::ApplyDistortion()
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
			dLevel = flx (l_Gain * l_PreAmpMult[currChannel]);

			l_smp *= flx (dLevel);

			const flx fi = flx (l_smp * ( 1.0f + l_abs(10.0f * l_smp)) / (1.0f + (l_abs(10.0f * l_smp)) + (l_smp*l_smp)) );
			PreFiltA->l_InputSmpl(flx (fi));
			PreFiltA->l_Run();

			l_smp = flx ( l_smp * (l_mprHard + l_abs(l_smp)) / ( (1.0f - PreFiltA->l_GetOutput()) + ((l_mprHard + l_Pre_Grit) * l_abs(l_smp)) + (l_smp*l_smp) ) );
//!		Power-Amp
			for (int frStage = 0; frStage < Dist_TB_TubeFreqStages; frStage++)
			{
				l_smp = flx (l_qFiltT[frStage] = flx (
						mDFAb * l_smp + mDFAa * l_qFiltT[frStage] + rfV->l_dnrm));
			}

			if (l_smp >= 0.0f)
			{
				PowerFiltA->l_InputSmpl(flx (0.010f));
				if (waveSide == false)
					waveSide = true;
			} else
			{
				PowerFiltA->l_InputSmpl(flx (1.0f));
				PowerFiltA->l_Run();
				PowerFiltA->l_Run();
				PowerFiltA->l_Run();
				if (waveSide == true)
					waveSide = false;
			}
			PowerFiltA->l_Run();
			flx xPwr = flx (1.0f - PowerFiltA->l_GetOutput());
			xPwr = (flx) l_Diode_U(xPwr, l_Power_Grit);
			xPwr = flx (1.0f - xPwr);

			const flx fp = (flx) l_Diode_S(l_smp, 10.0f);
			PowerFiltB->l_InputSmpl(flx (fp));
			PowerFiltB->l_Run();

			dLevel = flx (l_Gain * l_PwrAmpMult[currChannel] * xPwr);
			l_smp *= flx (dLevel);
			const flx dA = flx (((dLevel) * l_mpHard) + 1.0f);
			l_smp = flx ( l_smp * (dA + l_abs(l_smp)) / ( (dA * (1.0f + PowerFiltB->l_GetOutput())) + (dA * l_abs(l_smp)) + (l_smp*l_smp) ) );
			if (currChannel < 2)
				l_smp *= (flx) 2.00f;
				else
				l_smp *= (flx) 0.90f;

			smpl[spNum] = -l_smp;
		}
//!		Output Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterB->l_InputSmpl(smpl[x]);
			FilterB->l_Run(true, true);
			smpl[x] = (flx) FilterB->l_GetOutput();
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
			dLevel = fsx (s_Gain * s_PreAmpMult[currChannel]);

			s_smp *= fsx (dLevel);

			const fsx fi = fsx (s_smp * ( 1.0 + s_abs(10.0 * s_smp)) / (1.0 + (s_abs(10.0 * s_smp)) + (s_smp*s_smp)) );
			PreFiltA->s_InputSmpl(fsx (fi));
			PreFiltA->s_Run();

			s_smp = fsx ( s_smp * (s_mprHard + s_abs(s_smp)) / ( (1.0 - PreFiltA->s_GetOutput()) + ((s_mprHard + s_Pre_Grit) * s_abs(s_smp)) + (s_smp*s_smp) ) );
//!		Power-Amp
			for (int frStage = 0; frStage < Dist_TB_TubeFreqStages; frStage++)
			{
				s_smp = fsx (s_qFiltT[frStage] = fsx (
						mDFAb * s_smp + mDFAa * s_qFiltT[frStage] + rfV->s_dnrm));
			}

			if (s_smp >= 0.0)
			{
				PowerFiltA->s_InputSmpl(fsx (0.010));
				if (waveSide == false)
					waveSide = true;
			} else
			{
				PowerFiltA->s_InputSmpl(fsx (1.0));
				PowerFiltA->s_Run();
				PowerFiltA->s_Run();
				PowerFiltA->s_Run();
				if (waveSide == true)
					waveSide = false;
			}
			PowerFiltA->s_Run();
			fsx xPwr = fsx (1.0 - PowerFiltA->s_GetOutput());
			xPwr = (fsx) s_Diode_U(xPwr, s_Power_Grit);
			xPwr = fsx (1.0 - xPwr);

			const fsx fp = (fsx) s_Diode_S(s_smp, 10.0);
			PowerFiltB->s_InputSmpl(fsx (fp));
			PowerFiltB->s_Run();

			dLevel = fsx (s_Gain * s_PwrAmpMult[currChannel] * xPwr);
			s_smp *= fsx (dLevel);
			const fsx dA = fsx (((dLevel) * s_mpHard) + 1.0);
			s_smp = fsx ( s_smp * (dA + s_abs(s_smp)) / ( (dA * (1.0 + PowerFiltB->s_GetOutput())) + (dA * s_abs(s_smp)) + (s_smp*s_smp) ) );
			if (currChannel < 2)
				s_smp *= (fsx) 2.00;
				else
				s_smp *= (fsx) 0.90;

			smps[spNum] = -s_smp;
		}
//!		Output Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterB->s_InputSmpl(smps[x]);
			FilterB->s_Run(true, true);
			smps[x] = (fsx) FilterB->s_GetOutput();
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
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			h_smp = fhx (smph[spNum]);
//!		Harmonics
			fhx hrmFX = 3.0;
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
			fhx dLevel = 0.0;
			dLevel = fhx (h_Gain * h_PreAmpMult[currChannel]);

			h_smp *= fhx (dLevel);

			const fhx fi = fhx (h_smp * ( 1.0 + h_abs(10.0 * h_smp)) / (1.0 + (h_abs(10.0 * h_smp)) + (h_smp*h_smp)) );
			PreFiltA->h_InputSmpl(fhx (fi));
			PreFiltA->h_Run();

			h_smp = fhx ( h_smp * (h_mprHard + h_abs(h_smp)) / ( (1.0 - PreFiltA->h_GetOutput()) + ((h_mprHard + h_Pre_Grit) * h_abs(h_smp)) + (h_smp*h_smp) ) );
//!		Power-Amp
			for (int frStage = 0; frStage < Dist_TB_TubeFreqStages; frStage++)
			{
				h_smp = fhx (h_qFiltT[frStage] = fhx (
						mDFAb * h_smp + mDFAa * h_qFiltT[frStage] + rfV->h_dnrm));
			}

			if (h_smp >= 0.0)
			{
				PowerFiltA->h_InputSmpl(fhx (0.010));
				if (waveSide == false)
					waveSide = true;
			} else
			{
				PowerFiltA->h_InputSmpl(fhx (1.0));
				PowerFiltA->h_Run();
				PowerFiltA->h_Run();
				PowerFiltA->h_Run();
				if (waveSide == true)
					waveSide = false;
			}
			PowerFiltA->h_Run();
			fhx xPwr = fhx (1.0 - PowerFiltA->h_GetOutput());
			xPwr = (fhx) h_Diode_U(xPwr, h_Power_Grit);
			xPwr = fhx (1.0 - xPwr);

			const fhx fp = (fhx) h_Diode_S(h_smp, 10.0);
			PowerFiltB->h_InputSmpl(fhx (fp));
			PowerFiltB->h_Run();

			dLevel = fhx (h_Gain * h_PwrAmpMult[currChannel] * xPwr);
			h_smp *= fhx (dLevel);
			const fhx dA = fhx (((dLevel) * h_mpHard) + 1.0);
			h_smp = fhx ( h_smp * (dA + h_abs(h_smp)) / ( (dA * (1.0 + PowerFiltB->h_GetOutput())) + (dA * h_abs(h_smp)) + (h_smp*h_smp) ) );
			if (currChannel < 2)
				h_smp *= (fhx) 2.00;
				else
				h_smp *= (fhx) 0.90;

			smph[spNum] = (fhx) -h_smp;
		}
//!		Output Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterB->h_InputSmpl(smph[x]);
			FilterB->h_Run(true, true);
			smph[x] = (fhx) FilterB->h_GetOutput();
		}
	}
}
void Tube_Bank_F::SetChannel(const int nChan)
{
	if(currChannel == nChan) return;
	if(nChan < 0) return;
	if(nChan > 3) return;

	currChannel = nChan;

	zeroAllSamples();
}
void Tube_Bank_F::SetAmpVt(const fsx inp)
{
	return;
}
