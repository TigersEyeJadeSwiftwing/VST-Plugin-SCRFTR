#include "tonestack_g.h"
#include "tstack_wav_e.h"
#include <cstdlib>
#include <algorithm>

//! These are for the output tone stack controls
//! They are frequencies in Hz for the control bands, and determine where
//!		the bands start and end (approx.).
fhx qBandRangeS_Low[] =				{20.0,		20.0,		30.0,		30.0,		30.0,		30.0};
fhx qBandRangeS_Mid[] = 			{300.0,		200.0,		300.0,		300.0,		300.0,		300.0};
fhx qBandRangeS_High[] = 			{2000.0,	1000.0,		1000.0,		1000.0,		1000.0,		1000.0};
fhx qBandRangeS_CntrA[] =			{200.0,		200.0,		200.0,		200.0,		200.0,		200.0};
fhx qBandRangeS_CntrB[]	=			{1000.0,	1000.0,		1000.0,		1000.0,		1000.0,		1000.0};
fhx qBandRangeS_Pres[] =			{3600.0,	3600.0,		3600.0,		3600.0,		3600.0,		3600.0};
fhx qBandRangeS_Fixed[] =			{30.0,		20.0,		30.0,		30.0,		30.0,		30.0};

fhx qBandRangeE_Low[] = 			{165.0,		195.0,		300.0,		200.0,		200.0,		200.0};
fhx qBandRangeE_Mid[] = 			{2000.0,	1000.0,		1000.0,		2000.0,		2000.0,		2000.0};
fhx qBandRangeE_High[] = 			{9600.0,	9600.0,		7200.0,		7200.0,		7200.0,		7200.0};
fhx qBandRangeE_CntrA[] =	 		{300.0,		300.0,		300.0,		300.0,		300.0,		300.0};
fhx qBandRangeE_CntrB[] = 			{2000.0,	2000.0,		2000.0,		2000.0,		2000.0,		2000.0};
fhx qBandRangeE_Pres[] = 			{22000.0,	20000.0,	20000.0,	20000.0,	20000.0,	20000.0};
fhx qBandRangeE_Fixed[] = 			{20000.0,	720.0,		9600.0,		200.0,		200.0,		200.0};

fhx qBandMin_Low[] =				{0.0,		0.50,		0.50,		0.0,		0.0,		0.0};
fhx qBandMin_Mid[] =				{-0.50,		-0.40,		-0.40,		0.0,		0.0,		0.0};
fhx qBandMin_High[] =				{0.0,		0.50,		0.50,		0.0,		0.0,		0.0};
fhx qBandMin_CntrA[] =				{-0.50,		-0.50,		-0.50,		-0.50,		-0.50,		-0.50};
fhx qBandMin_CntrB[] =				{-0.50,		-0.50,		-0.50,		-0.50,		-0.50,		-0.50};
fhx qBandMin_Pres[] =				{0.0,		-0.50,		-0.50,		-0.50,		-0.50,		-0.50};

fhx qBandMag_Low[] =				{2.0,		2.0,		2.40,		0.0,		0.0,		0.0};
fhx qBandMag_Mid[] =				{2.0,		2.0,		2.40,		0.0,		0.0,		0.0};
fhx qBandMag_High[] =				{2.0,		2.0,		2.40,		0.0,		0.0,		0.0};
fhx qBandMag_CntrA[] =				{2.0,		2.0,		2.0,		0.0,		0.0,		0.0};
fhx qBandMag_CntrB[] =				{2.0,		2.0,		2.0,		0.0,		0.0,		0.0};
fhx qBandMag_Pres[] =				{2.0,		2.0,		2.0,		0.0,		0.0,		0.0};

fhx qBand_FixedVol[] =				{0.250,		1.0,		1.0,		1.0,		1.0,		1.0};
fhx qBand_MasterVol[] =				{0.500,		0.0,		0.0,		0.0,		0.0,		0.0};

#define SCa_TStack_Num_Stacks		6

ToneStack_G::ToneStack_G(fsx nRate, fsx CnvShift, fsx dcSkew) :
	sRate(fhx (nRate)),
	setShaped(true),
	fpPrec(int (1)),
	spLengthCab(0),
	dynQenable(true),
	h_envPos(fhx (0.0)),
	s_envPos(fsx (0.0)),
	l_envPos(flx (0.f)),
	CabMic(false),
	cShiftPcnt(fsx (CnvShift)),
	cSkewPcnt(fsx (dcSkew)),
	currImpShift(0),
	curAmp(0),
	curCab(0)
{
	s_bValue[0] = fsx (0.50);
	s_bValue[1] = fsx (0.50);
	s_bValue[2] = fsx (0.50);
	s_bValue[3] = fsx (0.50);
	s_bValue[4] = fsx (0.50);
	s_bValue[5] = fsx (0.50);

	rfV = new RateFC();

	for(int x = 0; x < 6; x++)
	{
		l_bValue[x] = flx (s_bValue[x]);
		h_bValue[x] = fhx (s_bValue[x]);

		qBand[x] = new Cst_EQ(true);
		qBand[x]->SetSmpRate(sRate);
	}

	FixedBand = new Cst_EQ(true);
	FixedBand->SetSmpRate(sRate);

	DynBass = new Cst_EQ();
	DynBass->SetSmpRate(sRate);

	SetAmpShapeNum(curAmp);

	DynBass->SetBands(60.0, 265.0);

	InitToneImpulses();

	ClearBuffs();
}
void ToneStack_G::SetAmpShapeNum(int inp)
{
	if (inp < 0) inp = 0;
	if (inp >= SCa_TStack_Num_Stacks) inp = SCa_TStack_Num_Stacks-1;
	curAmp = inp;

	qBand[0]->SetBands(qBandRangeS_Low[curAmp], qBandRangeE_Low[curAmp]);
	qBand[1]->SetBands(qBandRangeS_Mid[curAmp], qBandRangeE_Mid[curAmp]);
	qBand[2]->SetBands(qBandRangeS_High[curAmp], qBandRangeE_High[curAmp]);
	qBand[3]->SetBands(qBandRangeS_CntrA[curAmp], qBandRangeE_CntrA[curAmp]);
	qBand[4]->SetBands(qBandRangeS_CntrB[curAmp], qBandRangeE_CntrB[curAmp]);
	qBand[5]->SetBands(qBandRangeS_Pres[curAmp], qBandRangeE_Pres[curAmp]);

	FixedBand->SetBands(qBandRangeS_Fixed[curAmp], qBandRangeE_Fixed[curAmp]);

	h_qBmin[0] = fhx (qBandMin_Low[curAmp]);
	h_qBmin[1] = fhx (qBandMin_Mid[curAmp]);
	h_qBmin[2] = fhx (qBandMin_High[curAmp]);
	h_qBmin[3] = fhx (qBandMin_CntrA[curAmp]);
	h_qBmin[4] = fhx (qBandMin_CntrB[curAmp]);
	h_qBmin[5] = fhx (qBandMin_Pres[curAmp]);

	for (int x = 0; x < 6; x++)
	{
		s_qBmin[x] = fsx (h_qBmin[x]);
		l_qBmin[x] = flx (h_qBmin[x]);
	}

	h_qBmag[0] = fhx (qBandMag_Low[curAmp]);
	h_qBmag[1] = fhx (qBandMag_Mid[curAmp]);
	h_qBmag[2] = fhx (qBandMag_High[curAmp]);
	h_qBmag[3] = fhx (qBandMag_CntrA[curAmp]);
	h_qBmag[4] = fhx (qBandMag_CntrB[curAmp]);
	h_qBmag[5] = fhx (qBandMag_Pres[curAmp]);

	for (int y = 0; y < 6; y++)
	{
		s_qBmag[y] = fsx (h_qBmag[y]);
		l_qBmag[y] = flx (h_qBmag[y]);
	}

	for (int z = 0; z < 6; z++)
		qBand[z]->SetSmpRate(sRate);

	FixedBand->SetSmpRate(sRate);

	h_master = fhx (qBand_MasterVol[curAmp]);
	s_master = fsx (h_master);
	l_master = flx (s_master);

	h_fixedVol = fhx (qBand_FixedVol[curAmp]);
	s_fixedVol = fsx (h_fixedVol);
	l_fixedVol = flx (s_fixedVol);

	ClearBuffs();
}
ToneStack_G::~ToneStack_G()
{
	for (int x = 0; x < SC_NUM_TSTACK_IMPULSES; x++)
	{
		impLength[x] = 0;
		delete [] impulseLcab[x];
		delete [] impulseScab[x];
		delete [] impulseHcab[x];
	}
	delete [] impulseLcab;
	delete [] impulseScab;
	delete [] impulseHcab;

	delete [] smpHcab;
	delete [] smpScab;
	delete [] smpLcab;

	for (int qb = 0; qb < 6; qb++)
	{
		delete qBand[qb];
	}

	delete FixedBand;
	delete DynBass;
	delete rfV;
}
void ToneStack_G::setNewSampleRate(const fsx nRate)
{
	sRate = fhx (nRate);
	spLengthCab = 0;

	for (int x = 0; x < SC_NUM_TSTACK_IMPULSES; x++)
	{
		impLength[x] = 0;
		delete [] impulseLcab[x];
		delete [] impulseScab[x];
		delete [] impulseHcab[x];
	}
	delete [] impulseLcab;
	delete [] impulseScab;
	delete [] impulseHcab;

	delete [] smpHcab;
	delete [] smpScab;
	delete [] smpLcab;

	InitToneImpulses();

	for (int x = 0; x < 6; x++)
	{
		qBand[x]->SetSmpRate(sRate);
	}

	DynBass->SetSmpRate(sRate);
	FixedBand->SetSmpRate(sRate);

	ClearBuffs();
}
void ToneStack_G::setBandValue(const int bNum, fsx value)
{
	if(bNum < 0) return;
	if(bNum > 5) return;

	l_bValue[bNum] = flx (value);
	s_bValue[bNum] = fsx (value);
	h_bValue[bNum] = fhx (value);
}
void ToneStack_G::setEvenShaped(const bool isShaped)
{
	if(setShaped != isShaped) ClearBuffs();
	setShaped = isShaped;
}
void ToneStack_G::ClearBuffs()
{
	h_envPos = fhx (0.0);
	s_envPos = fsx (0.0);
	l_envPos = flx (0.0);

	for (int x = 0; x < 6; x++)
	{
		qBand[x]->ClearBuffers();
	}

	smpPosC = 0;
}
void ToneStack_G::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
}
void ToneStack_G::RunEQ()
{
	if (setShaped)
	{
		if(fpPrec == 0)
		{
			FixedBand->SplInL(l_spl);
			FixedBand->RunQL();

			qBand[2]->SplInL(l_spl);
			qBand[2]->RunQL();

			l_spl = FixedBand->GetOutputL();
			l_spl *= l_fixedVol;

			l_spl += flx (qBand[2]->GetOutputL() * ((l_bValue[2] + l_qBmin[2]) * l_qBmag[2]));

			qBand[1]->SplInL(l_spl);
			qBand[1]->RunQL();
			l_spl += flx (qBand[1]->GetOutputL() * ((l_bValue[1] + l_qBmin[1]) * l_qBmag[1]));
			qBand[0]->SplInL(l_spl);
			qBand[0]->RunQL();
			l_spl += flx (qBand[0]->GetOutputL() * ((l_bValue[0] + l_qBmin[0]) * l_qBmag[0]));

			l_spl *= l_master;
			qBand[3]->SplInL(l_spl);
			qBand[3]->RunQL();
			qBand[4]->SplInL(l_spl);
			qBand[4]->RunQL();
			l_spl += flx (qBand[3]->GetOutputL() * ((l_bValue[3] + l_qBmin[3]) * l_qBmag[3]));
			l_spl += flx (qBand[4]->GetOutputL() * ((l_bValue[4] + l_qBmin[4]) * l_qBmag[4]));

			qBand[5]->SplInL(l_spl);
			qBand[5]->RunQL();
			l_spl += flx (qBand[5]->GetOutputL() * ((l_bValue[5] + l_qBmin[5]) * l_qBmag[5]));

			if (dynQenable)
			{
				DynBass->SplInL(l_spl);
				DynBass->RunQL();
				l_spl += flx (DynBass->GetOutputL() * (1.0f - (l_envPos * 4.0f)));
			}
		} else
		if(fpPrec == 1)
		{
			FixedBand->SplInS(s_spl);
			FixedBand->RunQS();

			qBand[2]->SplInS(s_spl);
			qBand[2]->RunQS();
			qBand[1]->SplInS(s_spl);
			qBand[1]->RunQS();
			qBand[0]->SplInS(s_spl);
			qBand[0]->RunQS();

			s_spl = fsx (FixedBand->GetOutputS() * s_fixedVol);

			s_spl += fsx (qBand[2]->GetOutputS() * ((s_bValue[2] + s_qBmin[2]) * s_qBmag[2]));
			s_spl += fsx (qBand[1]->GetOutputS() * ((s_bValue[1] + s_qBmin[1]) * s_qBmag[1]));
			s_spl += fsx (qBand[0]->GetOutputS() * ((s_bValue[0] + s_qBmin[0]) * s_qBmag[0]));

			if (dynQenable)
			{
				DynBass->SplInS(s_spl);
				DynBass->RunQS();
				s_spl += fsx (DynBass->GetOutputS() * (1.0 - (s_envPos * 3.0)));
			}

			s_spl *= s_master;

			qBand[3]->SplInS(s_spl);
			qBand[3]->RunQS();
			qBand[4]->SplInS(s_spl);
			qBand[4]->RunQS();
			qBand[5]->SplInS(s_spl);
			qBand[5]->RunQS();
			s_spl += fsx (qBand[3]->GetOutputS() * ((s_bValue[3] + s_qBmin[3]) * s_qBmag[3]));
			s_spl += fsx (qBand[4]->GetOutputS() * ((s_bValue[4] + s_qBmin[4]) * s_qBmag[4]));
			s_spl += fsx (qBand[5]->GetOutputS() * ((s_bValue[5] + s_qBmin[5]) * s_qBmag[5]));
		} else
		if(fpPrec == 2)
		{
			FixedBand->SplInH(h_spl);
			FixedBand->RunQH();

			qBand[2]->SplInH(h_spl);
			qBand[2]->RunQH();

			h_spl = FixedBand->GetOutputH();
			h_spl *= h_fixedVol;

			h_spl += fhx (qBand[2]->GetOutputH() * ((h_bValue[2] + h_qBmin[2]) * h_qBmag[2]));

			qBand[1]->SplInH(h_spl);
			qBand[1]->RunQH();
			h_spl += fhx (qBand[1]->GetOutputH() * ((h_bValue[1] + h_qBmin[1]) * h_qBmag[1]));
			qBand[0]->SplInH(h_spl);
			qBand[0]->RunQH();
			h_spl += fhx (qBand[0]->GetOutputH() * ((h_bValue[0] + h_qBmin[0]) * h_qBmag[0]));

			h_spl *= h_master;
			qBand[3]->SplInH(h_spl);
			qBand[3]->RunQH();
			qBand[4]->SplInH(h_spl);
			qBand[4]->RunQH();
			h_spl += fhx (qBand[3]->GetOutputH() * ((h_bValue[3] + h_qBmin[3]) * h_qBmag[3]));
			h_spl += fhx (qBand[4]->GetOutputH() * ((h_bValue[4] + h_qBmin[4]) * h_qBmag[4]));

			qBand[5]->SplInH(h_spl);
			qBand[5]->RunQH();
			h_spl += fhx (qBand[5]->GetOutputH() * ((h_bValue[5] + h_qBmin[5]) * h_qBmag[5]));

			if (dynQenable)
			{
				DynBass->SplInH(h_spl);
				DynBass->RunQH();
				h_spl += fhx (DynBass->GetOutputH() * (1.0 - (h_envPos * 4.0)));
			}
		}
	}

	if (CabMic) RunConvolutionCab();
}
void ToneStack_G::RunConvolutionCab()
{
	if (spLengthCab < 3) return;

	const int impulseNum = curCab;
	const int spLength = spLengthCab;

	if(fpPrec == 0)
	{
		smpLcab[smpPosC] = l_spl;
		smpPosC++;
		if(smpPosC >= spLength) smpPosC = 0;

		flx mix = flx (0.0f);
		int x = 0;
		int y = (smpPosC - 1);
		if(y < 0) y = (spLength - 1);
		int z = 0;
		for (x = 0; x < spLength; x++)
		{
			mix += flx (impulseLcab[impulseNum][z] * smpLcab[y]);
			y--;
			if (y < 0) y = (spLength -1);
			z++;
			if (z >= spLength) z = 0;
		}
		l_spl = flx (mix);
		l_spl *= volAdjC_l[impulseNum];
		return;
	}
	if(fpPrec == 1)
	{
		smpScab[smpPosC] = s_spl;
		smpPosC++;
		if(smpPosC >= spLength) smpPosC = 0;

		fsx mix = fsx (0.0);
		int x = 0;
		int y = (smpPosC - 1);
		if(y < 0) y = (spLength - 1);
		int z = 0;
		for (x = 0; x < spLength; x++)
		{
			mix += fsx (impulseScab[impulseNum][z] * smpScab[y]);
			y--;
			if (y < 0) y = (spLength -1);
			z++;
			if (z >= spLength) z = 0;
		}
		s_spl = fsx (mix);
		s_spl *= volAdjC_s[impulseNum];
		return;
	}
	if(fpPrec == 2)
	{
		smpHcab[smpPosC] = h_spl;
		smpPosC++;
		if(smpPosC >= spLength) smpPosC = 0;

		fhx mix = fhx (0.0);
		int x = 0;
		int y = (smpPosC - 1);
		if(y < 0) y = (spLength - 1);
		int z = 0;
		for (x = 0; x < spLength; x++)
		{
			mix += fhx (impulseHcab[impulseNum][z] * smpHcab[y]);
			y--;
			if (y < 0) y = (spLength -1);
			z++;
			if (z >= spLength) z = 0;
		}
		h_spl = fhx (mix);
		h_spl *= volAdjC_h[impulseNum];
		return;
	}
}
void ToneStack_G::InitToneImpulses()
{
	smpPosC = 0;

	impulseLcab = new flx *[SC_NUM_TSTACK_IMPULSES];
	impulseScab = new fsx *[SC_NUM_TSTACK_IMPULSES];
	impulseHcab = new fhx *[SC_NUM_TSTACK_IMPULSES];

	for (int x = 0; x < SC_NUM_TSTACK_IMPULSES; x++)
		impLength[x] = 0;

	int bnd = 0;

	const fhx syncRate = fhx (SC_TSTACK_IMPULSE_SyncRate_FloatingPnt);
	const long syncRateL = long (SC_TSTACK_IMPULSE_SyncRate_Integer);
	if (cShiftPcnt != fsx (0.0)) currImpShift = int (fsx (fsx (cShiftPcnt * 0.0050) * fsx (syncRate)));
	else currImpShift = 0;
	currImpShift += syncRateL / 4;
	const long syncRateLmidP = long (syncRateL - currImpShift);
	const fhx sRateM = fhx (fhx (sRate) * fhx (syncRate));
	const fhx impulseRate = fhx (SC_TSTACK_IMPULSE_SAMPLE_RATE);
	const fhx mFac = fhx (fhx (1.0) / fhx (wConvFac));
	long r = 0;
	long m = 0;
	long cnt = 0;
	BTW_LP *SplReadFilter = new BTW_LP(fsx (fhx (impulseRate * fhx (0.250))), fsx (fhx (impulseRate * syncRate)));
	BTW_LP *inspFilter = new BTW_LP(fsx (sRate * (fsx) 0.250), fsx (sRateM));

	const long impLengthTcab = long (fhx (fhx (fhx (sRateM) / fhx (impulseRate)) * fhx (SC_TSTACK_IMPULSE_CAB_MAX_F)));
	fhx *impulseLGcab = new fhx[impLengthTcab+syncRateL+2];
	fhx *impulseRcab = new fhx[(SC_TSTACK_IMPULSE_CAB_MAX_I+2) * syncRateL];

	fhx sMarker = fhx (0.0);
	fhx sPartial = fhx (0.0);
	const fhx sAdvRate = fhx (fhx (impulseRate) / fhx (sRate));
	long x = 0;
	long a = 0;
	long b = 0;
	int newLength = 0;
	fhx sValA = fhx (0.0);
	fhx sValB = fhx (0.0);
	fhx tH = fhx (0.0);
	fhx maxVtop = fhx (1.0);
	fhx maxVbtm = fhx (-1.0);
	int nLong = 0;

	fhx skw = fhx (0.0);
	bool skewImpulse = false;
	if (cSkewPcnt != fsx (0.0))
	{
		skewImpulse = true;
		skw = fhx (fhx (cSkewPcnt) * fhx (0.010));
	}
//! -------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------
	for (bnd = 0; bnd < SC_NUM_TSTACK_IMPULSES; bnd++)
	{
		SplReadFilter->zeroBuffers();
		inspFilter->zeroBuffers();
		m = 0;
		cnt = 0;
		maxVtop = fhx (1.0);
		maxVbtm = fhx (-1.0);

		for(r = 0; r < ((SC_TSTACK_IMPULSE_CAB_MAX_I+2) * syncRateL); r++)
		{
			impulseRcab[r] = fhx (0.0);
		}
		for(r = 0; r < ((SC_TSTACK_IMPULSE_CAB_MAX_I) * syncRateL); r++)
		{
			if(cnt == syncRateLmidP)
			{
				impulseRcab[r] = fhx (0.0);
				if (bnd == 0) impulseRcab[r] = fhx (fhx (wavCabC[m]) * fhx (mFac));
				if (bnd == 1) impulseRcab[r] = fhx (fhx (wavCabC120B[m]) * fhx (mFac));
				if (bnd == 2) impulseRcab[r] = fhx (fhx (wavCabC120C[m]) * fhx (mFac));
				if (bnd == 3) impulseRcab[r] = fhx (fhx (wavCabC120D[m]) * fhx (mFac));
				if (bnd == 4) impulseRcab[r] = fhx (fhx (wavCabC120E[m]) * fhx (mFac));
				if (bnd == 5) impulseRcab[r] = fhx (fhx (wavCabC120F[m]) * fhx (mFac));

				if (skewImpulse == true)
				{
					impulseRcab[r] *= fhx (fhx (1.0) - fabsl(skw));
					impulseRcab[r] += fhx (skw);
				}
			}
			else impulseRcab[r] = fhx (0.0);

			cnt++;
			if(cnt >= syncRateL)
			{
				cnt = 0;
				m++;
			}
			if(m > SC_TSTACK_IMPULSE_CAB_MAX_I) break;
		}

		for(r = 0; r < ((SC_TSTACK_IMPULSE_CAB_MAX_I) * syncRateL); r++)
		{
			SplReadFilter->inpSplH(impulseRcab[r]);
			SplReadFilter->RunFilter();
			impulseRcab[r] = (fhx) SplReadFilter->GetOutPutH();
			impulseRcab[r] *= fhx (syncRate);
		}

		sMarker = fhx (0.0);
		sPartial = fhx (0.0);
		x = 0;
		a = 0;
		b = 0;
		sValA = fhx (0.0);
		sValB = fhx (0.0);
		for(x = 0; x < impLengthTcab; x++)
		{
			// a = long ((fhx) floorl(fhx (sMarker)));
			a = long (sMarker);
			if (a > (impLengthTcab)) a = (impLengthTcab);
			b = long (a + long (1));
			sPartial = fhx (fhx (sMarker) - (fhx) floorl(fhx (sMarker)));
			sPartial = (fhx) fminl(fhx (1.0), fmaxl(fhx (-1.0), (fhx) sPartial));
			sValA = fhx (impulseRcab[a]);
			sValB = fhx (impulseRcab[b]);
			sValA *= fhx (fhx (1.0) - fhx (sPartial));
			sValB *= fhx (sPartial);
			impulseLGcab[x] = fhx (fhx (fhx (sValA) + fhx (sValB)) * fhx (0.50));
			impulseLGcab[x] = (fhx) fminl(fhx (1.0), fmaxl(fhx (-1.0), (fhx) impulseLGcab[x]));

			sMarker += fhx (sAdvRate);

			if(fhx (sMarker) > fhx (fhx (SC_TSTACK_IMPULSE_CAB_MAX_F) * (fhx) syncRate))
			{
//				impLengthTcab = long (x - 2);
				break;
			}
		}

		for(x = impLengthTcab - (syncRateL * 16); x < impLengthTcab; x++) impulseLGcab[x] = fhx (0.0);

		for(x = 0; x < impLengthTcab; x++)
		{
			inspFilter->inpSplH(impulseLGcab[x]);
			inspFilter->RunFilter();
			impulseLGcab[x] = (fhx) inspFilter->GetOutPutH();
		}
		newLength = long (impLengthTcab / syncRateL);

		impulseHcab[bnd] = new fhx[newLength+1];
		impulseScab[bnd] = new fsx[newLength+1];
		impulseLcab[bnd] = new flx[newLength+1];

		newLength -= 1;

		for(x = 0; x < newLength; x++)
		{
			tH = fhx (impulseLGcab[x*syncRateL]);
			if(tH != fhx (0.0)) maxVtop *= fhx (1.0/tH);
			if(tH != fhx (0.0)) maxVbtm *= fhx (1.0/tH);

			impulseHcab[bnd][x] = fhx (tH);
			impulseScab[bnd][x] = fsx (tH);
			impulseLcab[bnd][x] = flx (tH);
		}
		impLength[bnd] = newLength;
/*
		if (maxVtop != fhx (0.0)) maxVtop = fhx (fhx (newLength) / maxVtop);
		else maxVtop = fhx (1.0);

		if (maxVbtm != fhx (0.0)) maxVbtm = fhx (fhx (newLength) / maxVbtm);
		else maxVbtm = fhx (1.0);
*/
		const fhx maxV = fhx (iVolumesCab[bnd] * SC_TSTACK_IMPULSE_SAMPLE_RATE / sRate);

		volAdjC_h[bnd] = fhx (maxV);
		volAdjC_s[bnd] = fsx (maxV);
		volAdjC_l[bnd] = flx (maxV);

	}

	nLong = 65535;

	for (int mnLong = 0; mnLong < SC_NUM_TSTACK_IMPULSES; mnLong++)
	{
		nLong = std::min(nLong, impLength[mnLong]);
		impLength[mnLong] = (int) nLong;
	}

	smpHcab = new fhx[nLong+1];
	smpScab = new fsx[nLong+1];
	smpLcab = new flx[nLong+1];

	for (int f = 0; f <= nLong; f++)
	{
		smpHcab[f] = fhx (0.0);
		smpScab[f] = fsx (0.0);
		smpLcab[f] = flx (0.f);
	}

	spLengthCab = int (nLong);
//! -------------------------------------------------------------------------------
	delete [] impulseLGcab;
	delete [] impulseRcab;
	delete SplReadFilter;
	delete inspFilter;
}
void ToneStack_G::SetDynamicQ(const fsx inp)
{
	const fhx aSpl = fhx (inp * inp);
//	aSpl *= fhx (fhx (2.0) - aSpl);
	if (aSpl > h_envPos) h_envPos += fhx (fhx (100.0) / fhx (sRate));
	if (aSpl < h_envPos) h_envPos -= fhx (fhx (10.0) / fhx (sRate));
	h_envPos = (fhx) fminl(fhx (1.0), fmaxl(h_envPos, fhx (0.0)));
	s_envPos = fsx (h_envPos);
	l_envPos = flx (s_envPos);
}
