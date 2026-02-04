#include "tube_bank_d.h"

Tube_BankD::Tube_BankD(fsx spRate, fhx mdfr) : SampleBank_Cst(spRate),
	stMod(fhx (fhx (mdfr))),
	currChannel(0),
	h_distFreqHi(fhx (Dist_TBD_TubeFreqB)),
	h_distFreqLo(fhx (Dist_TBD_TubeFreqA)),
	s_distFreqHi(fsx (Dist_TBD_TubeFreqB)),
	s_distFreqLo(fsx (Dist_TBD_TubeFreqA)),
	l_distFreqHi(flx (Dist_TBD_TubeFreqB)),
	l_distFreqLo(flx (Dist_TBD_TubeFreqA)),
	l_pi(flx (M_PI)),
	s_pi(fsx (M_PI)),
	h_pi(fhx (M_PI)),
	h_tRatio(fhx (Dist_TBD_T_Ratio)),
	s_tRatio(fsx (h_tRatio)),
	l_tRatio(flx (s_tRatio)),
	l_mpHard(flx (Dist_TBD_PowerAmp_Hardness)),
	s_mpHard(fsx (Dist_TBD_PowerAmp_Hardness)),
	h_mpHard(fhx (Dist_TBD_PowerAmp_Hardness)),
	l_mprHard(flx (Dist_TBD_PreAmp_Hardness)),
	s_mprHard(fsx (Dist_TBD_PreAmp_Hardness)),
	h_mprHard(fhx (Dist_TBD_PreAmp_Hardness))
{
	h_sv = fhx (stMod);
	s_sv = fsx (stMod);
	l_sv = flx (stMod);

	FilterA = new Cst_EQ(true);
	FilterB = new Cst_EQ(true);
	FilterC = new Cst_EQ(true);

	FilterA->SetSmpRate(cRateS);
	FilterA->SetBands(100.0, (fsx) s_distFreqLo);
	FilterB->SetSmpRate(cRateS);
	FilterB->SetBands(100.0, (fsx) s_distFreqHi);
	FilterC->SetSmpRate(cRateS);
	FilterC->SetBands(Dist_TB_Distortion_ToneFilter_Low, Dist_TB_Distortion_ToneFilter_High);

	h_tSpeed = fhx (fhx (Dist_TBD_T_Speed) / cRateH);
	s_tSpeed = fsx (h_tSpeed);
	l_tSpeed = flx (h_tSpeed);

	for (int x = 0; x < Dist_TBD_PreAmp_Stages_Max; x++)
	{
		PreAmpD[x] = new SC_Diode(Dist_TBD_PreAmp_Hardness, Dist_TBD_PreAmp_Slope, Dist_TB_PreAmp_Clip_Level);
//		PreAmpD[x]->SetFreq(Dist_TBD_PreAmp_Frequency, sRateS);
	}

	for (int y = 0; y < DIST_TBD_MAX_STAGES; y++)
	{
		PowerAmpD[y] = new SC_Diode(Dist_TBD_PowerAmp_Hardness, Dist_TBD_PowerAmp_Slope, Dist_TB_PowerAmp_Clip_Level);
//		PowerAmpD[y]->SetFreq(Dist_TBD_PowerAmp_Frequency, sRateS);
	}

	clearBuffs();
}
Tube_BankD::~Tube_BankD()
{
	delete FilterA;
	delete FilterB;
	delete FilterC;

	for (int x = 0; x < Dist_TBD_PreAmp_Stages_Max; x++)
		delete PreAmpD[x];

	for (int y = 0; y < DIST_TBD_MAX_STAGES; y++)
		delete PowerAmpD[y];
}
void Tube_BankD::InitTubes()
{
	clearBuffs();
}
void Tube_BankD::setSampleRate(fsx nSRate)
{
	SampleBank_Cst::setSampleRate(nSRate);

	SampleBank_Cst::zeroAllSamples();

	clearBuffs();

	h_tSpeed = fhx (fhx (Dist_TBD_T_Speed) / cRateH);
	s_tSpeed = fsx (h_tSpeed);
	l_tSpeed = flx (h_tSpeed);

	FilterA->SetSmpRate(cRateS);
	FilterA->SetBands(100.0, (fsx) s_distFreqLo);
	FilterB->SetSmpRate(cRateS);
	FilterB->SetBands(100.0, (fsx) s_distFreqHi);
	FilterC->SetSmpRate(cRateS);
//	FilterC->SetBands(200.0, 300.0);

	h_preFreq = (fhx) expl(fhx (-2.0) * h_pi * fhx (Dist_TBD_PreAmp_Frequency) / (cRateH));
	s_preFreq = fsx (h_preFreq);
	l_preFreq = flx (h_preFreq);

	h_powerFreq = (fhx) expl(fhx (-2.0) * h_pi * fhx (Dist_TBD_PowerAmp_Frequency) / (cRateH));
	s_powerFreq = fsx (h_powerFreq);
	l_powerFreq = flx (h_powerFreq);
/*
	for (int x = 0; x < Dist_TBD_PreAmp_Stages_Max; x++)
		PreAmpD[x]->SetFreq(Dist_TBD_PreAmp_Frequency, sRateS);

	for (int y = 0; y < DIST_TBD_MAX_STAGES; y++)
		PowerAmpD[y]->SetFreq(Dist_TBD_PowerAmp_Frequency, sRateS); */
}
void Tube_BankD::SetGain(const fsx nGain)
{
	h_Gain = fhx (nGain);

	if(fhx (h_Gain) < fhx (rfV->zh)) h_Gain = fhx (rfV->zl);
	if(fhx (h_Gain) > fhx (1.0)) h_Gain = fhx (1.0);

	h_Gain *= fhx (2.0 - h_Gain);
	h_Gain *= fhx (Dist_TBD_Gain_Mult_Factor);

	s_Gain = fsx (h_Gain);
	l_Gain = flx (s_Gain);
}
void Tube_BankD::clearBuffs()
{
	h_dfltA = fhx (rfV->zh);
	h_dfltB = fhx (rfV->zh);
	s_dfltA = fsx (rfV->zs);
	s_dfltB = fsx (rfV->zs);
	l_dfltA = flx (rfV->zl);
	l_dfltB = flx (rfV->zl);

	for(int y = 0; y < DIST_TBD_MAX_STAGES; y++)
	{
		l_tPos[y] = flx (0.0f);
		s_tPos[y] = fsx (0.0);
		h_tPos[y] = fhx (0.0);

		for (int z = 0; z < Dist_TBD_TubeFreqStages; z++)
		{
			l_qFiltT[y][z] = flx (rfV->zl);
			s_qFiltT[y][z] = fsx (rfV->zs);
			h_qFiltT[y][z] = fhx (rfV->zh);
		}
	}

	for (int x = 0; x < Dist_TBD_PreAmp_Stages_Max; x++)
		PreAmpD[x]->Clear();

	for (int y = 0; y < DIST_TBD_MAX_STAGES; y++)
		PowerAmpD[y]->Clear();

	l_prePos[2] = 0.0;
	l_prePos[3] = 0.0;
	l_powerPos[2] = 0.0;
	l_powerPos[3] = 0.0;
	s_prePos[2] = 0.0;
	s_prePos[3] = 0.0;
	s_powerPos[2] = 0.0;
	s_powerPos[3] = 0.0;
	h_prePos[2] = 0.0;
	h_prePos[3] = 0.0;
	h_powerPos[2] = 0.0;
	h_powerPos[3] = 0.0;
}
void Tube_BankD::zeroAllSamples()
{
	SampleBank_Cst::zeroAllSamples();

	for (int x = 0; x < Dist_TBD_PreAmp_Stages_Max; x++)
		PreAmpD[x]->Clear();

	for (int y = 0; y < DIST_TBD_MAX_STAGES; y++)
		PowerAmpD[y]->Clear();
}
void Tube_BankD::setOverSampling(const int oSLevel)
{
	SampleBank_Cst::setOverSampling(oSLevel);

	clearBuffs();

	h_tSpeed = fhx (fhx (Dist_TBD_T_Speed) / cRateH);
	s_tSpeed = fsx (h_tSpeed);
	l_tSpeed = flx (h_tSpeed);

	FilterA->SetSmpRate(cRateS);
	FilterA->SetBands(100.0, (fsx) s_distFreqLo);
	FilterB->SetSmpRate(cRateS);
	FilterB->SetBands(100.0, (fsx) s_distFreqHi);
	FilterC->SetSmpRate(cRateS);
//	FilterC->SetBands(200.0, 300.0);

	h_preFreq = (fhx) expl(fhx (-2.0) * h_pi * fhx (Dist_TBD_PreAmp_Frequency) / (cRateH));
	s_preFreq = fsx (h_preFreq);
	l_preFreq = flx (h_preFreq);

	h_powerFreq = (fhx) expl(fhx (-2.0) * h_pi * fhx (Dist_TBD_PowerAmp_Frequency) / (cRateH));
	s_powerFreq = fsx (h_powerFreq);
	l_powerFreq = flx (h_powerFreq);
}
void Tube_BankD::RunPreAmp()
{
	if(osRate <= 0) return;
	if(osRate >= 6) return;

	int preStages = Dist_TBD_PreAmp_Stages_BoostOff;
	if((currChannel == 1) || (currChannel == 3)) preStages = Dist_TBD_PreAmp_Stages_Max;
//! ----------------------------------------------------------------------------------------------------------------
//! 32-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(precisionLevel == 0)
	{
		const flx modFiltA = (flx) expf(flx (-2.0) * l_pi * flx (l_sv) / (cRateL));
		const flx modFiltB = flx (1.0f - modFiltA);
//!		Input Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterA->SplInL(smpl[x]);
			FilterA->RunLPL();
			smpl[x] = (flx) FilterA->GetOutputL();
			smpl[x] = flx (smpl[x] - flx (l_dfltA = flx (smpl[x] * modFiltB + l_dfltA * modFiltA)));
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			l_smp = smpl[spNum];
//!		Pre-Amp
			for(int stg = 0; stg < preStages; stg++)
			{
				PreAmpD[stg]->l_in = flx (l_smp * l_Gain);
				PreAmpD[stg]->l_Run();
				l_smp = PreAmpD[stg]->l_out;
			}
			smpl[spNum] = l_smp;
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 64-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(precisionLevel == 1)
	{
		const fsx modFiltA = (fsx) exp(fsx (-2.0) * s_pi * fsx (s_sv) / (cRateS));
		const fsx modFiltB = fsx (1.0 - modFiltA);
//!		Input Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterA->SplInS(smps[x]);
			FilterA->RunLPS();
			smps[x] = (fsx) FilterA->GetOutputS();
			smps[x] = fsx (smps[x] - fsx (s_dfltA = fsx (smps[x] * modFiltB + s_dfltA * modFiltA)));
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			s_smp = smps[spNum];
//!		Pre-Amp
			for(int stg = 0; stg < preStages; stg++)
			{
				PreAmpD[stg]->s_in = fsx (s_smp * s_Gain);
				PreAmpD[stg]->s_Run();
				s_smp = PreAmpD[stg]->s_out;
			}
			smps[spNum] = s_smp;
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 128-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(precisionLevel == 2)
	{
		const fhx modFiltA = (fhx) expl(fhx (-2.0) * h_pi * fhx (h_sv) / (cRateH));
		const fhx modFiltB = fhx (1.0 - modFiltA);
//!		Input Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterA->SplInH(smph[x]);
			FilterA->RunLPH();
			smph[x] = (fhx) FilterA->GetOutputH();
			smph[x] = fhx (smph[x] - fhx (h_dfltA = fhx (smph[x] * modFiltB + h_dfltA * modFiltA)));
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			h_smp = smph[spNum];
//!		Pre-Amp
			for(int stg = 0; stg < preStages; stg++)
			{
				PreAmpD[stg]->h_in = fhx (h_smp * h_Gain);
				PreAmpD[stg]->h_Run();
				h_smp = PreAmpD[stg]->h_out;
			}
			smph[spNum] = h_smp;
		}
	}
}
void Tube_BankD::RunPowerAmp()
{
	if(osRate <= 0) return;
	if(osRate >= 6) return;

	int numPowerStgs = 0;
	if(currChannel == 0) numPowerStgs = DIST_TBD_STAGES_CLEAN;
	if(currChannel == 1) numPowerStgs = DIST_TBD_STAGES_CRUNCH;
	if(currChannel == 2) numPowerStgs = DIST_TBD_STAGES_HIGH;
	if(currChannel == 3) numPowerStgs = DIST_TBD_STAGES_MEGA;
	if(numPowerStgs == 0) return;
	if(numPowerStgs > DIST_TBD_MAX_STAGES) numPowerStgs = DIST_TBD_MAX_STAGES;

	//! TODO...
}

void Tube_BankD::ApplyDistortion()
{
	if(osRate <= 0) return;
	if(osRate >= 6) return;

	int numPowerStgs = 0;
	if(currChannel == 0) numPowerStgs = DIST_TBD_STAGES_CLEAN;
	if(currChannel == 1) numPowerStgs = DIST_TBD_STAGES_CRUNCH;
	if(currChannel == 2) numPowerStgs = DIST_TBD_STAGES_HIGH;
	if(currChannel == 3) numPowerStgs = DIST_TBD_STAGES_MEGA;
	if(numPowerStgs == 0) return;
	int preStages = Dist_TBD_PreAmp_Stages_BoostOff;
	if((currChannel == 1) || (currChannel == 3)) preStages = Dist_TBD_PreAmp_Stages_Max;
	if(numPowerStgs > DIST_TBD_MAX_STAGES) numPowerStgs = DIST_TBD_MAX_STAGES;
//! ----------------------------------------------------------------------------------------------------------------
//! 32-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(precisionLevel == 0)
	{
		const flx modFiltA = (flx) expf(flx (-2.0f) * l_pi * flx (l_sv) / (cRateL));
		const flx modFiltB = flx (1.0f - modFiltA);
		const flx mDFAa = (flx) expf(flx (-2.0f) * l_pi * l_distFreqLo / cRateL);
		const flx mDFAb = flx (1.0f - mDFAa);
//!		Input Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterA->SplInL(smpl[x]);
			FilterA->RunLPL();
			smpl[x] = (flx) FilterA->GetOutputL();
			smpl[x] = flx (smpl[x] - flx (l_dfltA = flx (smpl[x] * modFiltB + l_dfltA * modFiltA)));
			FilterC->SplInL(smpl[x]);
			FilterC->RunQL();
			smpl[x] += (flx) FilterC->GetOutputL();
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			l_smp = flx (smpl[spNum]);
//!		Pre-Amp
			l_prePos[0] = flx (l_smp * l_Gain);
			l_prePos[1] = flx (-l_smp * l_Gain);
			if (l_prePos[0] < 0.0) l_prePos[0] = 0.0f;
			if (l_prePos[1] < 0.0) l_prePos[1] = 0.0f;
			l_prePos[0] = flx (l_prePos[2] = flx ((1.0-l_preFreq) * l_prePos[0] + l_preFreq * l_prePos[2] + rfV->zl));
			l_prePos[1] = flx (l_prePos[3] = flx ((1.0-l_preFreq) * l_prePos[1] + l_preFreq * l_prePos[3] + rfV->zl));
			for(int stg = 0; stg < preStages; stg++)
			{
				PreAmpD[stg]->l_in = flx (l_smp * l_Gain);
				PreAmpD[stg]->l_spValA = l_prePos[0];
				PreAmpD[stg]->l_spValB = l_prePos[1];
				PreAmpD[stg]->l_Run();
				l_smp = PreAmpD[stg]->l_out;
			}
//!		Power-Amp
			l_powerPos[0] = flx (l_smp);
			l_powerPos[1] = flx (-l_smp);
			if (l_powerPos[0] < 0.0) l_powerPos[0] = 0.0f;
			if (l_powerPos[1] < 0.0) l_powerPos[1] = 0.0f;
			l_powerPos[0] = flx (l_powerPos[2] = flx ((1.0-l_powerFreq) * l_powerPos[0] + l_powerFreq * l_powerPos[2] + rfV->zl));
			l_powerPos[1] = flx (l_powerPos[3] = flx ((1.0-l_powerFreq) * l_powerPos[1] + l_powerFreq * l_powerPos[3] + rfV->zl));
			for(int numStages = 0; numStages < numPowerStgs; numStages++)
			{
				for (int frStage = 0; frStage < Dist_TBD_TubeFreqStages; frStage++)
						l_smp = flx (l_qFiltT[numStages][frStage] = flx (l_smp *
										mDFAb + l_qFiltT[numStages][frStage] * mDFAa));

				l_eff = l_smp;
				if (l_eff < 0.0f) l_eff = -l_eff;
				const flx travel = flx (l_tSpeed * (2.0f - l_eff));
				if (l_eff >= l_tPos[numStages]) l_tPos[numStages] += travel;
					else l_tPos[numStages] -= flx (travel * l_tRatio);

				const flx dFX = flx (l_Gain * (1.0f - (0.50f * l_tPos[numStages])));

				PowerAmpD[numStages]->l_in = flx (l_smp * dFX);
				PowerAmpD[numStages]->l_spValA = flx (l_powerPos[0]);
				PowerAmpD[numStages]->l_spValB = flx (l_powerPos[1]);
				PowerAmpD[numStages]->l_Run();
				l_smp = PowerAmpD[numStages]->l_out;
			}
			smpl[spNum] = l_smp;
		}
//!		Output Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterB->SplInL(smpl[x]);
			FilterB->RunLPL();
			smpl[x] = (flx) FilterB->GetOutputL();
			smpl[x] = flx (smpl[x] - flx (l_dfltB = flx (smpl[x] * modFiltB + l_dfltB * modFiltA)));
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 64-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(precisionLevel == 1)
	{
		const fsx modFiltA = (fsx) exp(fsx (-2.0) * s_pi * fsx (s_sv) / (cRateS));
		const fsx modFiltB = fsx (1.0 - modFiltA);
		const fsx mDFAa = (fsx) exp(fsx (-2.0) * s_pi * s_distFreqLo / cRateS);
		const fsx mDFAb = fsx (1.0 - mDFAa);
//!		Input Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterA->SplInS(smps[x]);
			FilterA->RunLPS();
			smps[x] = (fsx) FilterA->GetOutputS();
			smps[x] = fsx (smps[x] - fsx (s_dfltA = fsx (smps[x] * modFiltB + s_dfltA * modFiltA)));
			FilterC->SplInS(smps[x]);
			FilterC->RunQS();
			smps[x] += (fsx) FilterC->GetOutputS();
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			s_smp = fsx (smps[spNum]);
//!		Pre-Amp
			s_prePos[0] = fsx (s_smp * s_Gain);
			s_prePos[1] = fsx (-s_smp * s_Gain);
			if (s_prePos[0] < 0.0) s_prePos[0] = 0.0;
			if (s_prePos[1] < 0.0) s_prePos[1] = 0.0;
			s_prePos[0] = fsx (s_prePos[2] = fsx ((1.0-s_preFreq) * s_prePos[0] + s_preFreq * s_prePos[2] + rfV->zs));
			s_prePos[1] = fsx (s_prePos[3] = fsx ((1.0-s_preFreq) * s_prePos[1] + s_preFreq * s_prePos[3] + rfV->zs));
			for(int stg = 0; stg < preStages; stg++)
			{
				PreAmpD[stg]->s_in = fsx (s_smp * s_Gain);
				PreAmpD[stg]->s_spValA = s_prePos[0];
				PreAmpD[stg]->s_spValB = s_prePos[1];
				PreAmpD[stg]->s_Run();
				s_smp = PreAmpD[stg]->s_out;
			}
//!		Power-Amp
			s_powerPos[0] = fsx (s_smp);
			s_powerPos[1] = fsx (-s_smp);
			if (s_powerPos[0] < 0.0) s_powerPos[0] = 0.0;
			if (s_powerPos[1] < 0.0) s_powerPos[1] = 0.0;
			s_powerPos[0] = fsx (s_powerPos[2] = fsx ((1.0-s_powerFreq) * s_powerPos[0] + s_powerFreq * s_powerPos[2] + rfV->zs));
			s_powerPos[1] = fsx (s_powerPos[3] = fsx ((1.0-s_powerFreq) * s_powerPos[1] + s_powerFreq * s_powerPos[3] + rfV->zs));
			for(int numStages = 0; numStages < numPowerStgs; numStages++)
			{
				for (int frStage = 0; frStage < Dist_TBD_TubeFreqStages; frStage++)
						s_smp = fsx (s_qFiltT[numStages][frStage] = fsx (s_smp *
										mDFAb + s_qFiltT[numStages][frStage] * mDFAa));

				s_eff = s_smp;
				if (s_eff < 0.0) s_eff = -s_eff;
				const fsx travel = fsx (s_tSpeed * (2.0 - s_eff));
				if (s_eff >= s_tPos[numStages]) s_tPos[numStages] += travel;
					else s_tPos[numStages] -= fsx (travel * s_tRatio);

				const fsx dFX = fsx (s_Gain * (1.0 - (0.50 * s_tPos[numStages])));

				PowerAmpD[numStages]->s_in = fsx (s_smp * dFX);
				PowerAmpD[numStages]->s_spValA = fsx (s_powerPos[0]);
				PowerAmpD[numStages]->s_spValB = fsx (s_powerPos[1]);
				PowerAmpD[numStages]->s_Run();
				s_smp = PowerAmpD[numStages]->s_out;
			}
			smps[spNum] = s_smp;
		}
//!		Output Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterB->SplInS(smps[x]);
			FilterB->RunLPS();
			smps[x] = (fsx) FilterB->GetOutputS();
			smps[x] = fsx (smps[x] - fsx (s_dfltB = fsx (smps[x] * modFiltB + s_dfltB * modFiltA)));
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 128-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(precisionLevel == 2)
	{


		const fhx modFiltA = (fhx) expl(fhx (-2.0) * h_pi * fhx (h_sv) / (cRateS));
		const fhx modFiltB = fhx (1.0 - modFiltA);
		const fhx mDFAa = (fhx) expl(fhx (-2.0) * h_pi * h_distFreqLo / cRateS);
		const fhx mDFAb = fhx (1.0 - mDFAa);
//!		Input Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterA->SplInH(smph[x]);
			FilterA->RunLPH();
			smph[x] = (fhx) FilterA->GetOutputH();
			smph[x] = fhx (smph[x] - fhx (h_dfltA = fhx (smph[x] * modFiltB + h_dfltA * modFiltA)));
			FilterC->SplInH(smph[x]);
			FilterC->RunQH();
			smph[x] += (fhx) FilterC->GetOutputH();
		}
		for(int spNum = 0; spNum < rfV->vi[osRate]; spNum++)
		{
			h_smp = fhx (smph[spNum]);
//!		Pre-Amp
			h_prePos[0] = fhx (h_smp * h_Gain);
			h_prePos[1] = fhx (-h_smp * h_Gain);
			if (h_prePos[0] < 0.0) h_prePos[0] = 0.0;
			if (h_prePos[1] < 0.0) h_prePos[1] = 0.0;
			h_prePos[0] = fhx (h_prePos[2] = fhx ((1.0-h_preFreq) * h_prePos[0] + h_preFreq * h_prePos[2] + rfV->zs));
			h_prePos[1] = fhx (h_prePos[3] = fhx ((1.0-h_preFreq) * h_prePos[1] + h_preFreq * h_prePos[3] + rfV->zs));
			for(int stg = 0; stg < preStages; stg++)
			{
				PreAmpD[stg]->h_in = fhx (h_smp * h_Gain);
				PreAmpD[stg]->h_spValA = h_prePos[0];
				PreAmpD[stg]->h_spValB = h_prePos[1];
				PreAmpD[stg]->h_Run();
				h_smp = PreAmpD[stg]->h_out;
			}
//!		Power-Amp
			h_powerPos[0] = fhx (h_smp);
			h_powerPos[1] = fhx (-h_smp);
			if (h_powerPos[0] < 0.0) h_powerPos[0] = 0.0;
			if (h_powerPos[1] < 0.0) h_powerPos[1] = 0.0;
			h_powerPos[0] = fhx (h_powerPos[2] = fhx ((1.0-h_powerFreq) * h_powerPos[0] + h_powerFreq * h_powerPos[2] + rfV->zs));
			h_powerPos[1] = fhx (h_powerPos[3] = fhx ((1.0-h_powerFreq) * h_powerPos[1] + h_powerFreq * h_powerPos[3] + rfV->zs));
			for(int numStages = 0; numStages < numPowerStgs; numStages++)
			{
				for (int frStage = 0; frStage < Dist_TBD_TubeFreqStages; frStage++)
						h_smp = fhx (h_qFiltT[numStages][frStage] = fhx (h_smp *
										mDFAb + h_qFiltT[numStages][frStage] * mDFAa));

				h_eff = h_smp;
				if (h_eff < 0.0) h_eff = -h_eff;
				const fhx travel = fhx (h_tSpeed * (2.0 - h_eff));
				if (h_eff >= h_tPos[numStages]) h_tPos[numStages] += travel;
					else h_tPos[numStages] -= fhx (travel * h_tRatio);

				const fhx dFX = fhx (h_Gain * (1.0 - (0.50 * h_tPos[numStages])));

				PowerAmpD[numStages]->h_in = fhx (h_smp * dFX);
				PowerAmpD[numStages]->h_spValA = fhx (h_powerPos[0]);
				PowerAmpD[numStages]->h_spValB = fhx (h_powerPos[1]);
				PowerAmpD[numStages]->h_Run();
				h_smp = PowerAmpD[numStages]->h_out;
			}
			smph[spNum] = h_smp;
		}
//!		Output Filtering
		for(int x = 0; x < rfV->vi[osRate]; x++)
		{
			FilterB->SplInH(smph[x]);
			FilterB->RunLPH();
			smph[x] = (fhx) FilterB->GetOutputH();
			smph[x] = fhx (smph[x] - fhx (h_dfltB = fhx (smph[x] * modFiltB + h_dfltB * modFiltA)));
		}
	}
}
void Tube_BankD::SetChannel(const int nChan)
{
	if(currChannel == nChan) return;
	if(nChan < 0) return;
	if(nChan > 3) return;

	currChannel = nChan;

	zeroAllSamples();
}
void Tube_BankD::SetAmpVt(const fsx inp)
{
	return;
/*
//	const fhx ampl = fhx (inp * inp);
	const fhx ampl = (fhx) fabsl(inp);
//	const fhx ampl = (fhx) fabsl(fhx (inp * fhx (fhx (2.0) - (fhx) fabsl(inp))));
	if (ampl > mdV) mdV += fhx (fhx (192.0) / sRateH);
	if (ampl < mdV) mdV -= fhx (fhx (16.0) / sRateH);
	mdV = (fhx) fminl(fhx (1.0), fmaxl(fhx (0.0), mdV));
	l_md = flx (mdV);
	s_md = fsx (mdV);
	h_md = fhx (mdV);
*/
}
