#include "tonestack_h.h"
#include "tstack_wav_e.h"
#include "tstack_params.h"
#include <cstdlib>
#include <algorithm>

ToneStack_H::ToneStack_H(fsx nRate, const SC_ToneStack_Params inpParams) :
	sRate(fhx (nRate)),
	prms(inpParams),
	setShaped(true),
	fpPrec(int (1)),
	spLengthAmp(0),
	spLengthCab(0),
	dynQenable(true),
	h_DynEnvPos(fhx (0.0)),
	s_DynEnvPos(fsx (0.0)),
	l_DynEnvPos(flx (0.f)),
	CabMic(false),
	cShiftPcnt(fhx (prms.convShift)),
	cSkewPcnt(fhx (prms.DCskew)),
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

	DynamicQ = new Cst_EQ(true);
	DynamicQ->SetSmpRate(sRate);
	DynBass = new Cst_EQ();
	DynBass->SetSmpRate(sRate);
	TrebleRec = new Cst_EQ();
	TrebleRec->SetSmpRate(sRate);

	qBand[0]->SetBands(prms.qb_a_low, prms.qb_b_low);
	qBand[1]->SetBands(prms.qb_a_mid, prms.qb_b_mid);
	qBand[2]->SetBands(prms.qb_a_high, prms.qb_b_high);
	qBand[3]->SetBands(prms.qb_a_cntA, prms.qb_b_cntA);
	qBand[4]->SetBands(prms.qb_a_cntB, prms.qb_b_cntB);
	qBand[5]->SetBands(prms.qb_a_pres, prms.qb_b_pres);

	DynamicQ->SetBands(5.0, prms.DynamicToneTravelFreq);
	DynBass->SetBands(prms.DynamicToneFloorFreq, prms.DynamicTonePassFreq);
	h_DynEnvCurve = (fhx) prms.DynamicToneCurve;
	h_DynEnvOffset = (fhx) prms.DynamicToneOffset;
	h_DynEnvRange = (fhx) prms.DynamicToneRange;
	s_DynEnvCurve = (fsx) prms.DynamicToneCurve;
	s_DynEnvOffset = (fsx) prms.DynamicToneOffset;
	s_DynEnvRange = (fsx) prms.DynamicToneRange;
	l_DynEnvCurve = (flx) prms.DynamicToneCurve;
	l_DynEnvOffset = (flx) prms.DynamicToneOffset;
	l_DynEnvRange = (flx) prms.DynamicToneRange;

	TrebleRec->SetBands(60.0, 22000.0);

	CreateToneImpulses();

	ClearBuffs();
}
ToneStack_H::~ToneStack_H()
{
	for (int x = 0; x < SC_NUM_TSTACK_H_IMPULSES; x++)
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

	delete DynamicQ;
	delete DynBass;
	delete TrebleRec;
	delete rfV;
}
void ToneStack_H::setNewSampleRate(const fsx nRate)
{
	sRate = fhx (nRate);

	spLengthAmp = 0;
	spLengthCab = 0;

	for (int x = 0; x < SC_NUM_TSTACK_H_IMPULSES; x++)
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

	DynamicQ->SetSmpRate(sRate);
	DynBass->SetSmpRate(sRate);
	TrebleRec->SetSmpRate(sRate);

	ClearBuffs();
}
void ToneStack_H::setBandValue(const int bNum, fsx value)
{
	if(bNum < 0) return;
	if(bNum > 5) return;

	l_bValue[bNum] = flx (value);
	s_bValue[bNum] = fsx (value);
	h_bValue[bNum] = fhx (value);
}
void ToneStack_H::setEvenShaped(const bool isShaped)
{
	if(setShaped != isShaped) ClearBuffs();
	setShaped = isShaped;
}
fsx ToneStack_H::GetProcessedOutput()
{
	if(fpPrec == 0) return fsx (l_spl);
	if(fpPrec == 1) return fsx (s_spl);
	if(fpPrec == 2) return fsx (h_spl);

	return fsx (0.0);
}
void ToneStack_H::ClearBuffs()
{
	h_DynEnvPos = fhx (0.0);
	s_DynEnvPos = fsx (0.0);
	l_DynEnvPos = flx (0.0);

	for (int x = 0; x < 6; x++)
	{
		qBand[x]->ClearBuffers();
	}

	for (int f = 0; f < spLengthAmp; f++)
	{
		smpHamp[f] = fhx (0.0);
		smpSamp[f] = fsx (0.0);
		smpLamp[f] = flx (0.f);
	}

	for (int f = 0; f < spLengthCab; f++)
	{
		smpHcab[f] = fhx (0.0);
		smpScab[f] = fsx (0.0);
		smpLcab[f] = flx (0.f);
	}

	smpPosA = 0;
	smpPosC = 0;

	DynamicQ->ClearBuffers();
	TrebleRec->ClearBuffers();
	DynBass->ClearBuffers();
}
void ToneStack_H::SetInput(const fsx inp)
{
	h_spl = fhx (inp);
	s_spl = fsx (inp);
	l_spl = flx (inp);
}
void ToneStack_H::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
}
void ToneStack_H::RunConv()
{
	if (setShaped) RunConvolutionAmp();
	if (CabMic) RunConvolutionCab();
}
void ToneStack_H::RunEQ()
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
			l_spl += flx (DynBass->GetOutputL() * (l_DynEnvOffset - (l_DynEnvPos * l_DynEnvRange)));
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
			s_spl += fsx (DynBass->GetOutputS() * (s_DynEnvOffset - (s_DynEnvPos * s_DynEnvRange)));
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
			h_spl += fhx (DynBass->GetOutputH() * (h_DynEnvOffset - (h_DynEnvPos * h_DynEnvRange)));
		}
	}
}
void ToneStack_H::RunConvolutionAmp()
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
	} else
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
	} else
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
void ToneStack_H::RunConvolutionCab()
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
	} else
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
	} else
	if(fpPrec == 2)
	{
		smpHcab[smpPosC] = (fhx) h_spl;
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
void ToneStack_H::SetDynamicQ(const fsx inp)
{
	s_SetDynamicQ((fsx) inp);
	l_DynEnvPos = flx (s_DynEnvPos);
	h_DynEnvPos = fhx (s_DynEnvPos);
}
void ToneStack_H::l_SetDynamicQ(const flx inp)
{
	DynamicQ->l_InputSmpl(inp);
	DynamicQ->l_Run(true, false);
	l_DynEnvPos = flx (ScMath::l_abs(DynamicQ->h_GetOutput()) * (flx) 2.0f);
//	l_DynEnvPos = (flx) ScMath::l_Diode_U(l_DynEnvPos, (flx) 2.0f);
//	l_DynEnvPos = (flx) ScMath::l_inv(l_DynEnvPos);
	l_DynEnvPos = (flx) ScMath::l_Diode_U(l_DynEnvPos, l_DynEnvCurve);
//	l_DynEnvPos = (flx) ScMath::l_inv(l_DynEnvPos);
}
void ToneStack_H::s_SetDynamicQ(const fsx inp)
{
	DynamicQ->s_InputSmpl(inp);
	DynamicQ->s_Run(true, false);
	s_DynEnvPos = fsx (ScMath::s_abs(DynamicQ->h_GetOutput()) * (fsx) 2.0);
//	s_DynEnvPos = (fsx) ScMath::s_Diode_U(s_DynEnvPos, (fsx) 2.0);
//	s_DynEnvPos = (fsx) ScMath::s_inv(s_DynEnvPos);
	s_DynEnvPos = (fsx) ScMath::s_Diode_U(s_DynEnvPos, s_DynEnvCurve);
//	s_DynEnvPos = (fsx) ScMath::s_inv(s_DynEnvPos);
}
void ToneStack_H::h_SetDynamicQ(const fhx inp)
{
	DynamicQ->h_InputSmpl(inp);
	DynamicQ->h_Run(true, false);
	h_DynEnvPos = fhx (ScMath::h_abs(DynamicQ->h_GetOutput()) * (fhx) 2.0);
//	h_DynEnvPos = (fhx) ScMath::h_Diode_U(h_DynEnvPos, (fhx) 2.0);
//	h_DynEnvPos = (fhx) ScMath::h_inv(h_DynEnvPos);
	h_DynEnvPos = (fhx) ScMath::h_Diode_U(h_DynEnvPos, h_DynEnvCurve);
//	h_DynEnvPos = (fhx) ScMath::h_inv(h_DynEnvPos);
}
void ToneStack_H::CreateToneImpulses()
{
	smpPosA = 0;
	smpPosC = 0;

	impulseLamp = new flx *[SC_NUM_TSTACK_H_IMPULSES_AMP];
	impulseSamp = new fsx *[SC_NUM_TSTACK_H_IMPULSES_AMP];
	impulseHamp = new fhx *[SC_NUM_TSTACK_H_IMPULSES_AMP];
	impulseLcab = new flx *[SC_NUM_TSTACK_H_IMPULSES_CAB];
	impulseScab = new fsx *[SC_NUM_TSTACK_H_IMPULSES_CAB];
	impulseHcab = new fhx *[SC_NUM_TSTACK_H_IMPULSES_CAB];

	const int xPonent = (int) prms.ImpSyncPowerFac;
	int cnt = 1;
	for (int x = 0; x < xPonent; x++) cnt += cnt;

	const long SyncRateI = long (cnt);
	const fhx SyncRateF = fhx (cnt);
	const long impLngAmp = long (fhx (fhx (prms.ImpLenAmpF) * fhx (prms.ImpTailPadFac)));
	const long impLngCab = long (fhx (fhx (prms.ImpLenCabF) * fhx (prms.ImpTailPadFac)));
	// const int shftPosA = int (fhx (fhx (cShiftPcnt * prms.ImpLenAmpF) / (fhx) 200.0) );
	// const int shftPosC = int (fhx (fhx (cShiftPcnt * prms.ImpLenCabF) / (fhx) 200.0) );
	fhx *BankAmpA = new fhx[(SyncRateI*2) * impLngAmp];
	fhx *BankAmpB = new fhx[(SyncRateI*2) * impLngAmp];
	fhx *BankCabA = new fhx[(SyncRateI*2) * impLngCab];
	fhx *BankCabB = new fhx[(SyncRateI*2) * impLngCab];

	const fhx impRate = fhx (prms.ImpStartSmpRate);

	const fhx LGampLengthF = fhx (fhx (prms.ImpTailPadFac) *
								fhx (prms.ImpLenAmpF) * fhx (sRate) / fhx (impRate));

	const fhx LGcabLengthF = fhx (fhx (prms.ImpTailPadFac) *
								fhx (prms.ImpLenCabF) * fhx (sRate) / fhx (impRate));

	const long LGampLengthI = long (LGampLengthF);
	const long LGcabLengthI = long (LGcabLengthF);
	const fhx advRate = fhx (fhx (SyncRateF) * fhx (impRate) / fhx (sRate));

	const fhx zPoint = fhx (ScMath::h_calc_denorm());
	// const fhx zOffset = fhx (fhx ((fhx) cSkewPcnt * (fhx) 0.0000010) + (fhx) zPoint);
	const fhx zOffset = fhx (zPoint);
	// const fhx zAmpl = fhx ((fhx) 1.0 -  ScMath::h_abs(zOffset));
	const fhx zMultA = fhx ( (fhx (1.0) + zOffset) * fhx (0.50) );
	const fhx zMultB = fhx ( (fhx (1.0) - zOffset) * fhx (0.50) );
	const fhx mFac = fhx (wConvFacI);

	const fhx cVolFac = fhx (2.0);

	for (int bnd = 0; bnd < SC_NUM_TSTACK_H_IMPULSES_AMP; bnd++)
	{
		for (long x = 0; x < ((SyncRateI*2) * impLngAmp); x++)
		{
			BankAmpA[x] = (fhx) zPoint;
			BankAmpB[x] = (fhx) zPoint;
		}
		for (long x = 0; x < ((SyncRateI*2) * impLngCab); x++)
		{
			BankCabA[x] = (fhx) zPoint;
			BankCabB[x] = (fhx) zPoint;
		}

		//! Amp
		long xpCount = 1;
		long rdPoint = 0;
		for (long x = 0; x < prms.ImpLenAmpI; x++)
		{
			rdPoint = x;
//			if (rdPoint >= prms.ImpLenAmpI) rdPoint -= prms.ImpLenAmpI;

			if (bnd == 0) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120AA[x]) * fhx (mFac));
			if (bnd == 1) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120A[x]) * fhx (mFac));
			if (bnd == 2) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120C[x]) * fhx (mFac));
			if (bnd == 3) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120D[x]) * fhx (mFac));
			if (bnd == 4) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120E[x]) * fhx (mFac));
			if (bnd == 5) BankAmpA[rdPoint+1] = fhx (fhx (wavHeadC120F[x]) * fhx (mFac));

			if (BankAmpA[rdPoint+1] > fhx (0.0)) BankAmpA[rdPoint+1] *= fhx (zMultA);
			if (BankAmpA[rdPoint+1] < fhx (0.0)) BankAmpA[rdPoint+1] *= fhx (zMultB);
			// BankAmpA[rdPoint+1] += fhx (zOffset);
			// BankAmpA[rdPoint+1] *= fhx (zAmpl);
		}
		while (xpCount < (SyncRateI))
		{
			for (long x = 0; x < ((SyncRateI+2) * impLngAmp); x++)
				BankAmpB[x] = (fhx) zPoint;

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
			delete xpandFilter;

			xpCount += xpCount;
		}
		BTW_LP *xDaFilter = new BTW_LP(fsx (sRate * 0.250), fsx (44100.0 * fsx (SyncRateF)));
		for (long x = 0; x < (impLngAmp * 2 * SyncRateI); x++)
		{
				xDaFilter->inpSplH(fhx (BankAmpA[x]));
				xDaFilter->RunFilter();
				BankAmpA[x] = fhx (xDaFilter->GetOutPutH());
		}
		delete xDaFilter;


		impulseHamp[bnd] = new fhx[LGampLengthI+1];
		impulseSamp[bnd] = new fsx[LGampLengthI+1];
		impulseLamp[bnd] = new flx[LGampLengthI+1];

		for (long x = 0; x <= LGampLengthI; x++)
		{
			impulseHamp[bnd][x] = fhx (0.0);
			impulseSamp[bnd][x] = fsx (0.0);
			impulseLamp[bnd][x] = flx (0.f);
		}

		fhx bnkRead = (fhx) 0.0;
		for (long x = 0; x < LGampLengthI; x++)
		{
			const long bnkTravel = long (bnkRead);
			if (bnkTravel >= (SyncRateI * 2 * impLngAmp)) break;

			impulseHamp[bnd][x] = fhx (BankAmpA[bnkTravel]);
			impulseSamp[bnd][x] = fsx (BankAmpA[bnkTravel]);
			impulseLamp[bnd][x] = flx (BankAmpA[bnkTravel]);

			bnkRead = fhx (fhx (advRate) * fhx (x));
		}

		//! Cab
		xpCount = 1;
		rdPoint = 0;
		for (long x = 0; x < prms.ImpLenCabI; x++)
		{
			rdPoint = x;
//			if (rdPoint >= prms.ImpLenCabI) rdPoint -= prms.ImpLenCabI;

			if (bnd == 0) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120AA[x]) * fhx (mFac));
			if (bnd == 1) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120B[x]) * fhx (mFac));
			if (bnd == 2) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120C[x]) * fhx (mFac));
			if (bnd == 3) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120D[x]) * fhx (mFac));
			if (bnd == 4) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120E[x]) * fhx (mFac));
			if (bnd == 5) BankCabA[rdPoint+1] = fhx (fhx (wavCabC120F[x]) * fhx (mFac));

			if (BankCabA[rdPoint+1] > fhx (0.0)) BankCabA[rdPoint+1] *= fhx (zMultB);
			if (BankCabA[rdPoint+1] < fhx (0.0)) BankCabA[rdPoint+1] *= fhx (zMultA);
			// BankCabA[rdPoint+1] += fhx (zOffset);
			// BankCabA[rdPoint+1] *= fhx (zAmpl);
		}
		while (xpCount < (SyncRateI))
		{
			for (long x = 0; x < ((SyncRateI+2) * impLngCab); x++)
				BankCabB[x] = (fhx) zPoint;

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
			delete xpandFilter;

			xpCount += xpCount;
		}
		BTW_LP *xDbFilter = new BTW_LP(fsx (sRate * 0.250), fsx (44100.0 * fsx (SyncRateF)));
		for (long x = 0; x < (impLngCab * 2 * SyncRateI); x++)
		{
				xDbFilter->inpSplH(fhx (BankCabA[x]));
				xDbFilter->RunFilter();
				BankCabA[x] = fhx (xDbFilter->GetOutPutH());
		}
		delete xDbFilter;

		impulseHcab[bnd] = new fhx[LGcabLengthI+1];
		impulseScab[bnd] = new fsx[LGcabLengthI+1];
		impulseLcab[bnd] = new flx[LGcabLengthI+1];

		for (long x = 0; x <= LGcabLengthI; x++)
		{
			impulseHcab[bnd][x] = fhx (0.0);
			impulseScab[bnd][x] = fsx (0.0);
			impulseLcab[bnd][x] = flx (0.f);
		}

		bnkRead = (fhx) 0.0;
		for (long x = 0; x < LGcabLengthI; x++)
		{
			const long bnkTravel = long (bnkRead);
			if (bnkTravel >= (SyncRateI * 2 * impLngCab)) break;

			impulseHcab[bnd][x] = fhx (BankCabA[bnkTravel]);
			impulseScab[bnd][x] = fsx (BankCabA[bnkTravel]);
			impulseLcab[bnd][x] = flx (BankCabA[bnkTravel]);

			bnkRead = fhx (fhx (advRate) * fhx (x));
		}

		const fhx maxVa = fhx (iVolumesAmp[bnd] * prms.ImpStartSmpRate / (fhx) sRate);

		volAdjA_h[bnd] = fhx (maxVa);
		volAdjA_s[bnd] = fsx (maxVa);
		volAdjA_l[bnd] = flx (maxVa);

		const fhx maxVc = fhx (iVolumesCab[bnd] * prms.ImpStartSmpRate / (fhx) sRate);

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
