#include "tonestack_e.h"
#include "tstack_wav_e.h"
#include <cstdlib>
#include <algorithm>

const fhx ToneStack_E::qBandRange_Low[2] = {SCr_C120_TStack_EqBand_Start_Low, SCr_C120_TStack_EqBand_End_Low};
const fhx ToneStack_E::qBandRange_Mid[2] = {SCr_C120_TStack_EqBand_Start_Mid, SCr_C120_TStack_EqBand_End_Mid};
const fhx ToneStack_E::qBandRange_High[2] = {SCr_C120_TStack_EqBand_Start_High, SCr_C120_TStack_EqBand_End_High};
const fhx ToneStack_E::qBandRange_ContourL[2] = {SCr_C120_TStack_EqBand_Start_CntrA, SCr_C120_TStack_EqBand_End_CntrA};
const fhx ToneStack_E::qBandRange_ContourH[2] = {SCr_C120_TStack_EqBand_Start_CntrB, SCr_C120_TStack_EqBand_End_CntrB};
const fhx ToneStack_E::qBandRange_Presence[2] = {SCr_C120_TStack_EqBand_Start_Pres, SCr_C120_TStack_EqBand_End_Pres};

ToneStack_E::ToneStack_E(fsx nRate, fsx CnvShift, fsx dcSkew) :
	sRate(fhx (nRate)),
	setShaped(true),
	fpPrec(int (1)),
	spLengthAmp(0),
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

	DynBass = new Cst_EQ();
	DynBass->SetSmpRate(sRate);
	TrebleRec = new Cst_EQ();
	TrebleRec->SetSmpRate(sRate);

	qBand[0]->SetBands(qBandRange_Low[0], qBandRange_Low[1]);
	qBand[1]->SetBands(qBandRange_Mid[0], qBandRange_Mid[1]);
	qBand[2]->SetBands(qBandRange_High[0], qBandRange_High[1]);
	qBand[3]->SetBands(qBandRange_ContourL[0], qBandRange_ContourL[1]);
	qBand[4]->SetBands(qBandRange_ContourH[0], qBandRange_ContourH[1]);
	qBand[5]->SetBands(qBandRange_Presence[0], qBandRange_Presence[1]);

	DynBass->SetBands(80.0, 165.0);
	TrebleRec->SetBands(65.0, 20000.0);

	CreateToneImpulses();

	ClearBuffs();
}
ToneStack_E::~ToneStack_E()
{
	for (int x = 0; x < SC_NUM_TSTACK_IMPULSES; x++)
	{
		impLength[x] = 0;

		delete [] impulseLamp[x];
		delete [] impulseSamp[x];
		delete [] impulseHamp[x];
		delete [] impulseLcab[x];
		delete [] impulseScab[x];
		delete [] impulseHcab[x];
	}
	delete [] impulseLamp;
	delete [] impulseSamp;
	delete [] impulseHamp;
	delete [] impulseLcab;
	delete [] impulseScab;
	delete [] impulseHcab;

	delete [] smpHamp;
	delete [] smpSamp;
	delete [] smpLamp;
	delete [] smpHcab;
	delete [] smpScab;
	delete [] smpLcab;

	for (int qb = 0; qb < 6; qb++)
	{
		delete qBand[qb];
	}

	delete DynBass;
	delete TrebleRec;
	delete rfV;
}
void ToneStack_E::setNewSampleRate(const fsx nRate)
{
	sRate = fhx (nRate);

	spLengthAmp = 0;
	spLengthCab = 0;

	for (int x = 0; x < SC_NUM_TSTACK_IMPULSES; x++)
	{
		impLength[x] = 0;

		delete [] impulseLamp[x];
		delete [] impulseSamp[x];
		delete [] impulseHamp[x];
		delete [] impulseLcab[x];
		delete [] impulseScab[x];
		delete [] impulseHcab[x];
	}
	delete [] impulseLamp;
	delete [] impulseSamp;
	delete [] impulseHamp;
	delete [] impulseLcab;
	delete [] impulseScab;
	delete [] impulseHcab;

	delete [] smpHamp;
	delete [] smpSamp;
	delete [] smpLamp;
	delete [] smpHcab;
	delete [] smpScab;
	delete [] smpLcab;

	impulseLamp = 0;
	impulseSamp = 0;
	impulseHamp = 0;
	impulseLcab = 0;
	impulseScab = 0;
	impulseHcab = 0;

	smpHamp = 0;
	smpSamp = 0;
	smpLamp = 0;
	smpHcab = 0;
	smpScab = 0;
	smpLcab = 0;

	CreateToneImpulses();

	for (int x = 0; x < 6; x++)
	{
		qBand[x]->SetSmpRate(sRate);
	}

	DynBass->SetSmpRate(sRate);
	TrebleRec->SetSmpRate(sRate);

	ClearBuffs();
}
void ToneStack_E::setBandValue(const int bNum, fsx value)
{
	if(bNum < 0) return;
	if(bNum > 5) return;

	l_bValue[bNum] = flx (value);
	s_bValue[bNum] = fsx (value);
	h_bValue[bNum] = fhx (value);
}
void ToneStack_E::setEvenShaped(const bool isShaped)
{
	if(setShaped != isShaped) ClearBuffs();
	setShaped = isShaped;
}
fsx ToneStack_E::GetProcessedOutput()
{
	if(fpPrec == 0) return fsx (l_spl);
	if(fpPrec == 1) return fsx (s_spl);
	if(fpPrec == 2) return fsx (h_spl);

	return fsx (0.0);
}
void ToneStack_E::ClearBuffs()
{
	h_envPos = fhx (0.0);
	s_envPos = fsx (0.0);
	l_envPos = flx (0.0);

	for (int x = 0; x < 6; x++)
	{
		qBand[x]->ClearBuffers();
	}

	smpPosA = 0;
	smpPosC = 0;
}
void ToneStack_E::SetInput(const fsx inp)
{
	h_spl = fhx (inp);
	s_spl = fsx (inp);
	l_spl = flx (inp);
}
void ToneStack_E::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
}
void ToneStack_E::RunConv()
{
	if (setShaped) RunConvolutionAmp();
	if (CabMic) RunConvolutionCab();
}
void ToneStack_E::RunEQ()
{
	if(fpPrec == 0)
	{
		TrebleRec->SplInL(l_spl);
		TrebleRec->RunQL();

		for (unsigned int x = 0; x < 6; x++)
		{
			qBand[x]->SplInL(l_spl);
			qBand[x]->RunQL();
		}
		l_spl = TrebleRec->GetOutputL();
		for (unsigned int x = 0; x < 6; x++)
			l_spl += flx (qBand[x]->GetOutputL() * flx ((l_bValue[x] * l_bValue[x]) - flx (0.25f)) * flx (4.0f));

		if (dynQenable)
		{
			DynBass->SplInL(l_spl);
			DynBass->RunQL();
			l_spl += flx (DynBass->GetOutputL() * (2.0f - (l_envPos * 4.0f)));
		}
	} else
	if(fpPrec == 1)
	{
		TrebleRec->SplInS(s_spl);
		TrebleRec->RunQS();

		for (unsigned int x = 0; x < 6; x++)
		{
			qBand[x]->SplInS(s_spl);
			qBand[x]->RunQS();
		}
		s_spl = TrebleRec->GetOutputS();
		for (unsigned int x = 0; x < 6; x++)
			s_spl += fsx (qBand[x]->GetOutputS() * fsx ((s_bValue[x] * s_bValue[x]) - fsx (0.25)) * fsx (4.0));

		if (dynQenable)
		{
			DynBass->SplInS(s_spl);
			DynBass->RunQS();
			s_spl += fsx (DynBass->GetOutputS() * (2.0 - (s_envPos * 4.0)));
		}
	} else
	if(fpPrec == 2)
	{
		TrebleRec->SplInH(h_spl);
		TrebleRec->RunQH();

		for (unsigned int x = 0; x < 6; x++)
		{
			qBand[x]->SplInH(h_spl);
			qBand[x]->RunQH();
		}
		h_spl = TrebleRec->GetOutputH();
		for (unsigned int x = 0; x < 6; x++)
			h_spl += fhx (qBand[x]->GetOutputH() * fhx ((h_bValue[x] * h_bValue[x]) - fhx (0.25)) * fhx (4.0));

		if (dynQenable)
		{
			DynBass->SplInH(h_spl);
			DynBass->RunQH();
			h_spl += fhx (DynBass->GetOutputH() * (2.0 - (h_envPos * 4.0)));
		}
	}
}
void ToneStack_E::RunConvolutionAmp()
{
	if (spLengthAmp < 3) return;

	const int impulseNum = curAmp;
	const int spLength = spLengthAmp;

	if(fpPrec == 0)
	{
		smpLamp[smpPosA] = l_spl;
		smpPosA++;
		if(smpPosA >= spLength) smpPosA = 0;

		flx mix = flx (0.0f);
		int x = 0;
		int y = (smpPosA - 1);
		if(y < 0) y = (spLength - 1);
		int z = 0;
		for (x = 0; x < spLength; x++)
		{
			mix += flx (impulseLamp[impulseNum][z] * smpLamp[y]);
			y--;
			if (y < 0) y = (spLength -1);
			z++;
			if (z >= spLength) z = 0;
		}
		l_spl = flx (mix);
		l_spl *= volAdjA_l[impulseNum];
		return;
	}
	if(fpPrec == 1)
	{
		smpSamp[smpPosA] = s_spl;
		smpPosA++;
		if(smpPosA >= spLength) smpPosA = 0;

		fsx mix = fsx (0.0);
		int x = 0;
		int y = (smpPosA - 1);
		if(y < 0) y = (spLength - 1);
		int z = 0;
		for (x = 0; x < spLength; x++)
		{
			mix += fsx (impulseSamp[impulseNum][z] * smpSamp[y]);
			y--;
			if (y < 0) y = (spLength -1);
			z++;
			if (z >= spLength) z = 0;
		}
		s_spl = fsx (mix);
		s_spl *= volAdjA_s[impulseNum];
		return;
	}
	if(fpPrec == 2)
	{
		smpHamp[smpPosA] = h_spl;
		smpPosA++;
		if(smpPosA >= spLength) smpPosA = 0;

		fhx mix = fhx (0.0);
		int x = 0;
		int y = (smpPosA - 1);
		if(y < 0) y = (spLength - 1);
		int z = 0;
		for (x = 0; x < spLength; x++)
		{
			mix += fhx (impulseHamp[impulseNum][z] * smpHamp[y]);
			y--;
			if (y < 0) y = (spLength -1);
			z++;
			if (z >= spLength) z = 0;
		}
		h_spl = fhx (mix);
		h_spl *= volAdjA_h[impulseNum];
		return;
	}
}
void ToneStack_E::RunConvolutionCab()
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
void ToneStack_E::InitToneImpulses()
{
	smpPosA = 0;
	smpPosC = 0;

	impulseLamp = new flx *[SC_NUM_TSTACK_E_IMPULSES_AMP];
	impulseSamp = new fsx *[SC_NUM_TSTACK_E_IMPULSES_AMP];
	impulseHamp = new fhx *[SC_NUM_TSTACK_E_IMPULSES_AMP];
	impulseLcab = new flx *[SC_NUM_TSTACK_E_IMPULSES_CAB];
	impulseScab = new fsx *[SC_NUM_TSTACK_E_IMPULSES_CAB];
	impulseHcab = new fhx *[SC_NUM_TSTACK_E_IMPULSES_CAB];

	for (int x = 0; x < SC_NUM_TSTACK_IMPULSES; x++)
		impLength[x] = 0;

	int bnd = 0;

	const fhx syncRate = fhx (SC_TSTACK_IMPULSE_SyncRate_FloatingPnt);
	const long syncRateL = long (SC_TSTACK_IMPULSE_SyncRate_Integer);
	if (cShiftPcnt != fsx (0.0)) currImpShift = int (fsx (fsx (cShiftPcnt * 0.010) * fsx (syncRate)));
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

	const long impLengthTamp = long (fhx (fhx (fhx (sRateM) / fhx (impulseRate)) * fhx (SC_TSTACK_IMPULSE_AMP_MAX_F)));
	const long impLengthTcab = long (fhx (fhx (fhx (sRateM) / fhx (impulseRate)) * fhx (SC_TSTACK_IMPULSE_CAB_MAX_F)));
	fhx *impulseLGamp = new fhx[impLengthTamp+syncRateL+2];
	fhx *impulseRamp = new fhx[(SC_TSTACK_IMPULSE_AMP_MAX_I+2) * syncRateL];
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
	for (bnd = 0; bnd < SC_NUM_TSTACK_E_IMPULSES_AMP; bnd++)
	{
		SplReadFilter->zeroBuffers();
		inspFilter->zeroBuffers();
		m = 0;
		cnt = 0;
		maxVtop = fhx (1.0);
		maxVbtm = fhx (-1.0);

		for(r = 0; r < ((SC_TSTACK_IMPULSE_AMP_MAX_I+2) * syncRateL); r++)
		{
			impulseRamp[r] = fhx (0.0);
		}
		for(r = 0; r < ((SC_TSTACK_IMPULSE_AMP_MAX_I) * syncRateL); r++)
		{
			if(cnt == syncRateLmidP)
			{
				impulseRamp[r] = fhx (0.0);
				if (bnd == 0) impulseRamp[r] = fhx (fhx (wavHeadC120A[m]) * fhx (mFac));
				if (bnd == 1) impulseRamp[r] = fhx (fhx (wavHeadC120B[m]) * fhx (mFac));
				if (bnd == 2) impulseRamp[r] = fhx (fhx (wavHeadC120C[m]) * fhx (mFac));
				if (bnd == 3) impulseRamp[r] = fhx (fhx (wavHeadC120D[m]) * fhx (mFac));
				if (bnd == 4) impulseRamp[r] = fhx (fhx (wavHeadC120E[m]) * fhx (mFac));
				if (bnd == 5) impulseRamp[r] = fhx (fhx (wavHeadC120F[m]) * fhx (mFac));

				if (skewImpulse == true)
				{
					impulseRamp[r] *= fhx (fhx (1.0) - fabsl(skw));
					impulseRamp[r] += fhx (skw);
				}
			}
			else impulseRamp[r] = fhx (0.0);

			cnt++;
			if(cnt >= syncRateL)
			{
				cnt = 0;
				m++;
			}
			if(m > SC_TSTACK_IMPULSE_AMP_MAX_I) break;
		}

		for(r = 0; r < ((SC_TSTACK_IMPULSE_AMP_MAX_I) * syncRateL); r++)
		{
			SplReadFilter->inpSplH(impulseRamp[r]);
			SplReadFilter->RunFilter();
			impulseRamp[r] = (fhx) SplReadFilter->GetOutPutH();
			impulseRamp[r] *= fhx (syncRate);
		}

		sMarker = fhx (0.0);
		sPartial = fhx (0.0);
		x = 0;
		a = 0;
		b = 0;
		sValA = fhx (0.0);
		sValB = fhx (0.0);
		for(x = 0; x < impLengthTamp; x++)
		{
			// a = long ((fhx) floorl(fhx (sMarker)));
			a = long (sMarker);
			if (a > (impLengthTamp)) a = (impLengthTamp);
			b = long (a + long (1));
			sPartial = fhx (fhx (sMarker) - (fhx) floorl(fhx (sMarker)));
			sPartial = (fhx) fminl(fhx (1.0), fmaxl(fhx (-1.0), (fhx) sPartial));
			sValA = fhx (impulseRamp[a]);
			sValB = fhx (impulseRamp[b]);
			sValA *= fhx (fhx (1.0) - fhx (sPartial));
			sValB *= fhx (sPartial);
			impulseLGamp[x] = fhx (fhx (fhx (sValA) + fhx (sValB)) * fhx (0.50));
			impulseLGamp[x] = (fhx) fminl(fhx (1.0), fmaxl(fhx (-1.0), (fhx) impulseLGamp[x]));

			sMarker += fhx (sAdvRate);

			if(fhx (sMarker) > fhx (fhx (SC_TSTACK_IMPULSE_AMP_MAX_F) * (fhx) syncRate))
			{
//				impLengthTamp = long (x - 2);
				break;
			}
		}

		for(x = impLengthTamp - (syncRateL * 16); x < impLengthTamp; x++) impulseLGamp[x] = fhx (0.0);

		for(x = 0; x < impLengthTamp; x++)
		{
			inspFilter->inpSplH(impulseLGamp[x]);
			inspFilter->RunFilter();
			impulseLGamp[x] = (fhx) inspFilter->GetOutPutH();
		}
		newLength = long (impLengthTamp / syncRateL);

		impulseHamp[bnd] = new fhx[newLength+1];
		impulseSamp[bnd] = new fsx[newLength+1];
		impulseLamp[bnd] = new flx[newLength+1];

		newLength -= 1;

		for(x = 0; x < newLength; x++)
		{
			tH = fhx (impulseLGamp[x*syncRateL]);
			if(tH != fhx (0.0)) maxVtop *= fhx (1.0/tH);
			if(tH != fhx (0.0)) maxVbtm *= fhx (1.0/tH);

			impulseHamp[bnd][x] = fhx (tH);
			impulseSamp[bnd][x] = fsx (tH);
			impulseLamp[bnd][x] = flx (tH);
		}
		impLength[bnd] = newLength;
/*
		if (maxVtop != fhx (0.0)) maxVtop = fhx (fhx (newLength) / maxVtop);
		else maxVtop = fhx (1.0);

		if (maxVbtm != fhx (0.0)) maxVbtm = fhx (fhx (newLength) / maxVbtm);
		else maxVbtm = fhx (1.0);
*/
		const fhx maxV = fhx (iVolumesAmp[bnd] * SC_TSTACK_IMPULSE_SAMPLE_RATE / sRate);

		volAdjA_h[bnd] = fhx (maxV);
		volAdjA_s[bnd] = fsx (maxV);
		volAdjA_l[bnd] = flx (maxV);

	}

	nLong = 65535;

	for (int mnLong = 0; mnLong < SC_NUM_TSTACK_E_IMPULSES_AMP; mnLong++)
	{
		nLong = std::min(nLong, impLength[mnLong]);
		impLength[mnLong] = (int) nLong;
	}

	smpHamp = new fhx[nLong+1];
	smpSamp = new fsx[nLong+1];
	smpLamp = new flx[nLong+1];

	for (int f = 0; f <= nLong; f++)
	{
		smpHamp[f] = fhx (0.0);
		smpSamp[f] = fsx (0.0);
		smpLamp[f] = flx (0.f);
	}

	spLengthAmp = int (nLong);
//! -------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------
	for (bnd = 0; bnd < SC_NUM_TSTACK_E_IMPULSES_CAB; bnd++)
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

	for (int mnLong = 0; mnLong < SC_NUM_TSTACK_E_IMPULSES_CAB; mnLong++)
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
	delete [] impulseLGamp;
	delete [] impulseRamp;
	delete [] impulseLGcab;
	delete [] impulseRcab;
	delete SplReadFilter;
	delete inspFilter;
}
void ToneStack_E::SetDynamicQ(const fsx inp)
{
	const fhx aSpl = fhx (inp * inp);
//	aSpl *= fhx (fhx (2.0) - aSpl);
	if (aSpl > h_envPos) h_envPos += fhx (fhx (100.0) / fhx (sRate));
	if (aSpl < h_envPos) h_envPos -= fhx (fhx (10.0) / fhx (sRate));
	h_envPos = (fhx) fminl(fhx (1.0), fmaxl(h_envPos, fhx (0.0)));
	s_envPos = fsx (h_envPos);
	l_envPos = flx (s_envPos);
}
void ToneStack_E::CreateToneImpulses()
{
	smpPosA = 0;
	smpPosC = 0;

	impulseLamp = new flx *[SC_NUM_TSTACK_E_IMPULSES_AMP];
	impulseSamp = new fsx *[SC_NUM_TSTACK_E_IMPULSES_AMP];
	impulseHamp = new fhx *[SC_NUM_TSTACK_E_IMPULSES_AMP];
	impulseLcab = new flx *[SC_NUM_TSTACK_E_IMPULSES_CAB];
	impulseScab = new fsx *[SC_NUM_TSTACK_E_IMPULSES_CAB];
	impulseHcab = new fhx *[SC_NUM_TSTACK_E_IMPULSES_CAB];

	const int xPonent = (int) SC_TSTACK_IMPULSE_SyncRate_Power_Factor;
	int cnt = 1;
	for (int x = 0; x < xPonent; x++) cnt += cnt;

	const long SyncRateI = long (cnt);
	const fhx SyncRateF = fhx (cnt);
	const long impLngAmp = long (fhx (fhx (SC_TSTACK_IMPULSE_AMP_MAX_F) * fhx (SC_TSTACK_IMPULSE_Tail_PadLength_Factor)));
	const long impLngCab = long (fhx (fhx (SC_TSTACK_IMPULSE_CAB_MAX_F) * fhx (SC_TSTACK_IMPULSE_Tail_PadLength_Factor)));
	const int shftPosA = int ((cShiftPcnt * SC_TSTACK_IMPULSE_AMP_MAX_I) / 200);
	const int shftPosC = int ((cShiftPcnt * SC_TSTACK_IMPULSE_CAB_MAX_I) / 200);
	fhx *BankAmpA = new fhx[(SyncRateI+2) * impLngAmp];
	fhx *BankAmpB = new fhx[(SyncRateI+2) * impLngAmp];
	fhx *BankCabA = new fhx[(SyncRateI+2) * impLngCab];
	fhx *BankCabB = new fhx[(SyncRateI+2) * impLngCab];

	const fhx impRate = fhx (SC_TSTACK_IMPULSE_SAMPLE_RATE);

	const fhx LGampLengthF = fhx (fhx (fhx (SyncRateF) * fhx (SC_TSTACK_IMPULSE_Tail_PadLength_Factor)) *
								fhx (SC_TSTACK_IMPULSE_AMP_MAX_F) * fhx (sRate) / fhx (impRate));

	const fhx LGcabLengthF = fhx (fhx (fhx (SyncRateF) * fhx (SC_TSTACK_IMPULSE_Tail_PadLength_Factor)) *
								fhx (SC_TSTACK_IMPULSE_CAB_MAX_F) * fhx (sRate) / fhx (impRate));

	const long LGampLengthI = long (long (LGampLengthF) / long (SyncRateI));
	const long LGcabLengthI = long (long (LGcabLengthF) / long (SyncRateI));
	const fhx advRate = fhx (fhx (SyncRateF) * fhx (impRate) / fhx (sRate));

	const fhx zPoint = fhx (ScMath::h_calc_denorm());
	const fhx zOffset = fhx (cSkewPcnt * 0.010);
	const fhx zAmpl = fhx ((fhx) 1.0 - (fhx) zOffset);
	const fhx mFac = fhx (fhx (1.0) / fhx (wConvFac));

	const fhx cVolFac = fhx (2.0);

	for (int bnd = 0; bnd < SC_NUM_TSTACK_E_IMPULSES_AMP; bnd++)
	{
		for (int x = 0; x < ((SyncRateI+2) * impLngAmp); x++)
		{
			BankAmpA[x] = (fhx) zOffset;
			BankAmpB[x] = (fhx) zPoint;
		}
		for (int x = 0; x < ((SyncRateI+2) * impLngCab); x++)
		{
			BankCabA[x] = (fhx) zOffset;
			BankCabB[x] = (fhx) zPoint;
		}

		//! Amp
		int xpCount = 1;
		int rdPoint = 0;
		for (int x = 0; x < SC_TSTACK_IMPULSE_AMP_MAX_I; x++)
		{
			rdPoint = x;
			if (rdPoint >= SC_TSTACK_IMPULSE_AMP_MAX_I) rdPoint -= SC_TSTACK_IMPULSE_AMP_MAX_I;

			if (bnd == 0) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120A[x]) * fhx (mFac));
			if (bnd == 1) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120B[x]) * fhx (mFac));
			if (bnd == 2) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120C[x]) * fhx (mFac));
			if (bnd == 3) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120D[x]) * fhx (mFac));
			if (bnd == 4) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120E[x]) * fhx (mFac));
			if (bnd == 5) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120F[x]) * fhx (mFac));

			BankAmpA[rdPoint+1] += fhx (zOffset);
			BankAmpA[rdPoint+1] *= fhx (zAmpl);
		}
		while (xpCount < (SyncRateI))
		{
			for (long x = 0; x < ((SyncRateI+2) * impLngAmp); x++)
				BankAmpB[x] = (fhx) zOffset;

			long y = 0;
			for (long x = 0; x < (impLngAmp * xpCount); x++)
			{
				BankAmpB[y] = fhx (BankAmpA[x]);
				y += 2;
			}

			BTW_LP *xpandFilter = new BTW_LP(fsx (22050.0 * fsx (xpCount)), fsx (44100.0 * fsx (xpCount*2)));
			for (long x = 0; x < (impLngAmp * 2 * xpCount); x++)
			{
				xpandFilter->inpSplH(fhx (BankAmpB[x]));
				xpandFilter->RunFilter();
				BankAmpA[x] = fhx (xpandFilter->GetOutPutH() * fhx (cVolFac));
			}

			xpCount += xpCount;
			delete xpandFilter;
		}
		BTW_LP *xDaFilter = new BTW_LP(fsx (sRate * 0.50), fsx (44100.0 * fsx (SyncRateI)));
		for (long x = 0; x < (impLngAmp * SyncRateI); x++)
		{
				xDaFilter->inpSplH(fhx (BankAmpA[x]));
				xDaFilter->RunFilter();
				BankAmpA[x] = fhx (xDaFilter->GetOutPutH());
		}
		delete xDaFilter;


		impulseHamp[bnd] = new fhx[LGampLengthI+1];
		impulseSamp[bnd] = new fsx[LGampLengthI+1];
		impulseLamp[bnd] = new flx[LGampLengthI+1];

		fhx bnkRead = 0.0;
		for (long x = 0; x <= LGampLengthI; x++)
		{
			const long bnkTravel = long (bnkRead);
			if (bnkTravel > (SyncRateI * impLngAmp)) break;

			impulseHamp[bnd][x] = fhx (BankAmpA[bnkTravel]);
			impulseSamp[bnd][x] = fsx (BankAmpA[bnkTravel]);
			impulseLamp[bnd][x] = flx (BankAmpA[bnkTravel]);

			bnkRead += fhx (advRate);
		}

		//! Cab
		xpCount = 1;
		rdPoint = 0;
		for (int x = 0; x < SC_TSTACK_IMPULSE_CAB_MAX_I; x++)
		{
			rdPoint = x;
			if (rdPoint >= SC_TSTACK_IMPULSE_CAB_MAX_I) rdPoint -= SC_TSTACK_IMPULSE_CAB_MAX_I;

			if (bnd == 0) BankCabA[rdPoint+1] = fhx (fhx (wavCabC[x]) * fhx (mFac));
			if (bnd == 1) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120B[x]) * fhx (mFac));
			if (bnd == 2) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120C[x]) * fhx (mFac));
			if (bnd == 3) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120D[x]) * fhx (mFac));
			if (bnd == 4) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120E[x]) * fhx (mFac));
			if (bnd == 5) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120F[x]) * fhx (mFac));

			BankCabA[rdPoint+1] += fhx (zOffset);
			BankCabA[rdPoint+1] *= fhx (zAmpl);
		}
		while (xpCount < (SyncRateI))
		{
			for (long x = 0; x < ((SyncRateI+2) * impLngCab); x++)
				BankCabB[x] = (fhx) zOffset;

			long y = 0;
			for (long x = 0; x < (impLngCab * xpCount); x++)
			{
				BankCabB[y] = fhx (BankCabA[x]);
				y += 2;
			}

			BTW_LP *xpandFilter = new BTW_LP(fsx (22050.0 * fsx (xpCount)), fsx (44100.0 * fsx (xpCount*2)));
			for (long x = 0; x < (impLngCab * 2 * xpCount); x++)
			{
				xpandFilter->inpSplH(fhx (BankCabB[x]));
				xpandFilter->RunFilter();
				BankCabA[x] = fhx (xpandFilter->GetOutPutH() * fhx (cVolFac));
			}

			xpCount += xpCount;
			delete xpandFilter;
		}
		BTW_LP *xDbFilter = new BTW_LP(fsx (sRate * 0.50), fsx (44100.0 * fsx (SyncRateI)));
		for (long x = 0; x < (impLngCab * SyncRateI); x++)
		{
				xDbFilter->inpSplH(fhx (BankCabA[x]));
				xDbFilter->RunFilter();
				BankCabA[x] = fhx (xDbFilter->GetOutPutH());
		}
		delete xDbFilter;

		impulseHcab[bnd] = new fhx[LGcabLengthI+1];
		impulseScab[bnd] = new fsx[LGcabLengthI+1];
		impulseLcab[bnd] = new flx[LGcabLengthI+1];

		bnkRead = 0.0;
		for (long x = 0; x <= LGcabLengthI; x++)
		{
			const long bnkTravel = long (bnkRead);
			if (bnkTravel > (SyncRateI * impLngCab)) break;

			impulseHcab[bnd][x] = fhx (BankCabA[bnkTravel]);
			impulseScab[bnd][x] = fsx (BankCabA[bnkTravel]);
			impulseLcab[bnd][x] = flx (BankCabA[bnkTravel]);

			bnkRead += fhx (advRate);
		}

		const fhx maxVa = fhx (iVolumesAmp[bnd] * fhx (0.50) * SC_TSTACK_IMPULSE_SAMPLE_RATE / sRate);

		volAdjA_h[bnd] = fhx (maxVa);
		volAdjA_s[bnd] = fsx (maxVa);
		volAdjA_l[bnd] = flx (maxVa);

		const fhx maxVc = fhx (iVolumesCab[bnd] * fhx (0.50) * SC_TSTACK_IMPULSE_SAMPLE_RATE / sRate);

		volAdjC_h[bnd] = fhx (maxVc);
		volAdjC_s[bnd] = fsx (maxVc);
		volAdjC_l[bnd] = flx (maxVc);
	}

	int nLong = int (LGampLengthI);

	smpHamp = new fhx[nLong+1];
	smpSamp = new fsx[nLong+1];
	smpLamp = new flx[nLong+1];

	for (int f = 0; f <= nLong; f++)
	{
		smpHamp[f] = fhx (0.0);
		smpSamp[f] = fsx (0.0);
		smpLamp[f] = flx (0.f);
	}

	spLengthAmp = int (nLong);

	nLong = int (LGcabLengthI);

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

	delete [] BankAmpA;
	delete [] BankCabA;
	delete [] BankAmpB;
	delete [] BankCabB;
}
