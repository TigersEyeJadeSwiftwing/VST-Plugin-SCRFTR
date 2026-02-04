#include "tonestack_i.h"
#include "tstack_wav_i.h"
#include "tstack_params.h"
#include <cstdlib>
#include <algorithm>

#include "../custdata_types.h"

using namespace ScMath;

ToneStack_I::ToneStack_I(fsx nRate, const SC_ToneStack_Params inpParams) :
	sRate(fhx (nRate)),
	prms(inpParams),
	setShaped(true),
	fpPrec(int (1)),
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

	for(int x = 0; x < SC_NUM_TSTACK_I_IMPULSES_AMP; x++)
	{
		for(int y = 0; y < TnStack_I[x].nElements; y++)
		{
			AmpHeadEQ[x][y] = new Cst_EQ();
			AmpHeadEQ[x][y]->SetSmpRate(sRate);
			AmpHeadEQ[x][y]->SetBands(TnStack_I[x].Elements[y].low, TnStack_I[x].Elements[y].high);
		}
	}
	for(int y = 0; y < SCr_TStack_I_Max_Head_Elements; y++)
	{
		l_ampHead_smp[y] = l_C_0;
		s_ampHead_smp[y] = s_C_0;
		h_ampHead_smp[y] = h_C_0;
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
ToneStack_I::~ToneStack_I()
{
	for(int x = 0; x < SC_NUM_TSTACK_I_IMPULSES_AMP; x++)
	{
		for(int y = 0; y < TnStack_I[x].nElements; y++)
		{
			delete AmpHeadEQ[x][y];
		}
	}

	for (int x = 0; x < SC_NUM_TSTACK_I_IMPULSES; x++)
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

	delete DynamicQ;
	delete DynBass;
	delete TrebleRec;
	delete rfV;
}
void ToneStack_I::setNewSampleRate(const fsx nRate)
{
	sRate = fhx (nRate);

	spLengthCab = 0;

	for(int x = 0; x < SC_NUM_TSTACK_I_IMPULSES_AMP; x++)
	{
		for(int y = 0; y < TnStack_I[x].nElements; y++)
		{
			AmpHeadEQ[x][y]->SetSmpRate(sRate);
		}
	}

	for (int x = 0; x < SC_NUM_TSTACK_I_IMPULSES; x++)
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

	impulseLcab = 0;
	impulseScab = 0;
	impulseHcab = 0;

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
void ToneStack_I::setBandValue(const int bNum, fsx value)
{
	if(bNum < 0) return;
	if(bNum > 5) return;

	l_bValue[bNum] = flx (value);
	s_bValue[bNum] = fsx (value);
	h_bValue[bNum] = fhx (value);
}
void ToneStack_I::setEvenShaped(const bool isShaped)
{
	if(setShaped != isShaped) ClearBuffs();
	setShaped = isShaped;
}
fsx ToneStack_I::GetProcessedOutput()
{
	if(fpPrec == 0) return fsx (l_spl);
	if(fpPrec == 1) return fsx (s_spl);
	if(fpPrec == 2) return fsx (h_spl);

	return fsx (0.0);
}
void ToneStack_I::ClearBuffs()
{
	h_DynEnvPos = fhx (0.0);
	s_DynEnvPos = fsx (0.0);
	l_DynEnvPos = flx (0.0);

	for (int x = 0; x < 6; x++)
	{
		qBand[x]->ClearBuffers();
	}

	for (int f = 0; f < spLengthCab; f++)
	{
		smpHcab[f] = fhx (0.0);
		smpScab[f] = fsx (0.0);
		smpLcab[f] = flx (0.f);
	}

	smpPosC = 0;

	for(int x = 0; x < SC_NUM_TSTACK_I_IMPULSES_AMP; x++)
	{
		for(int y = 0; y < TnStack_I[x].nElements; y++)
		{
			AmpHeadEQ[x][y]->ClearBuffers();
		}
	}

	for(int y = 0; y < SCr_TStack_I_Max_Head_Elements; y++)
	{
		l_ampHead_smp[y] = l_C_0;
		s_ampHead_smp[y] = s_C_0;
		h_ampHead_smp[y] = h_C_0;
	}

	DynamicQ->ClearBuffers();
	TrebleRec->ClearBuffers();
	DynBass->ClearBuffers();
}
void ToneStack_I::SetInput(const fsx inp)
{
	h_spl = fhx (inp);
	s_spl = fsx (inp);
	l_spl = flx (inp);
}
void ToneStack_I::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
}
void ToneStack_I::RunConv()
{
	if (setShaped) RunConvolutionAmp();
	if (CabMic) RunConvolutionCab();
}
void ToneStack_I::RunEQ()
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
void ToneStack_I::RunConvolutionAmp()
{
	const int nFX = TnStack_I[curAmp].nElements;

	if(fpPrec == 0)
	{
		for (int x = 0; x < nFX; x++)
			AmpHeadEQ[curAmp][x]->l_InputSmpl(l_spl);

		l_spl = l_C_0;

		for (int x = 0; x < nFX; x++)
		{
			AmpHeadEQ[curAmp][x]->l_Run(true, true);
			l_spl += flx (AmpHeadEQ[curAmp][x]->l_GetOutput() * TnStack_I[curAmp].Elements[x].vol);
		}

		l_spl *= (flx) iVolumesAmp[curAmp];
	} else
	if(fpPrec == 1)
	{
		for (int x = 0; x < nFX; x++)
			AmpHeadEQ[curAmp][x]->s_InputSmpl(s_spl);

		s_spl = s_C_0;

		for (int x = 0; x < nFX; x++)
		{
			AmpHeadEQ[curAmp][x]->s_Run(true, true);
			s_spl += fsx (AmpHeadEQ[curAmp][x]->s_GetOutput() * TnStack_I[curAmp].Elements[x].vol);
		}

		s_spl *= (fsx) iVolumesAmp[curAmp];
	} else
	if(fpPrec == 2)
	{
		for (int x = 0; x < nFX; x++)
			AmpHeadEQ[curAmp][x]->h_InputSmpl(h_spl);

		h_spl = h_C_0;

		for (int x = 0; x < nFX; x++)
		{
			AmpHeadEQ[curAmp][x]->h_Run(true, true);
			h_spl += fhx (AmpHeadEQ[curAmp][x]->h_GetOutput() * TnStack_I[curAmp].Elements[x].vol);
		}

		h_spl *= (fhx) iVolumesAmp[curAmp];
	}
}
void ToneStack_I::RunConvolutionCab()
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
void ToneStack_I::SetDynamicQ(const fsx inp)
{
	s_SetDynamicQ((fsx) inp);
	l_DynEnvPos = flx (s_DynEnvPos);
	h_DynEnvPos = fhx (s_DynEnvPos);
}
void ToneStack_I::l_SetDynamicQ(const flx inp)
{
	DynamicQ->l_InputSmpl(inp);
	DynamicQ->l_Run(true, false);
	l_DynEnvPos = flx (ScMath::l_abs(DynamicQ->h_GetOutput()) * (flx) 2.0f);
//	l_DynEnvPos = (flx) ScMath::l_Diode_U(l_DynEnvPos, (flx) 2.0f);
//	l_DynEnvPos = (flx) ScMath::l_inv(l_DynEnvPos);
	l_DynEnvPos = (flx) ScMath::l_Diode_U(l_DynEnvPos, l_DynEnvCurve);
//	l_DynEnvPos = (flx) ScMath::l_inv(l_DynEnvPos);
}
void ToneStack_I::s_SetDynamicQ(const fsx inp)
{
	DynamicQ->s_InputSmpl(inp);
	DynamicQ->s_Run(true, false);
	s_DynEnvPos = fsx (ScMath::s_abs(DynamicQ->h_GetOutput()) * (fsx) 2.0);
//	s_DynEnvPos = (fsx) ScMath::s_Diode_U(s_DynEnvPos, (fsx) 2.0);
//	s_DynEnvPos = (fsx) ScMath::s_inv(s_DynEnvPos);
	s_DynEnvPos = (fsx) ScMath::s_Diode_U(s_DynEnvPos, s_DynEnvCurve);
//	s_DynEnvPos = (fsx) ScMath::s_inv(s_DynEnvPos);
}
void ToneStack_I::h_SetDynamicQ(const fhx inp)
{
	DynamicQ->h_InputSmpl(inp);
	DynamicQ->h_Run(true, false);
	h_DynEnvPos = fhx (ScMath::h_abs(DynamicQ->h_GetOutput()) * (fhx) 2.0);
//	h_DynEnvPos = (fhx) ScMath::h_Diode_U(h_DynEnvPos, (fhx) 2.0);
//	h_DynEnvPos = (fhx) ScMath::h_inv(h_DynEnvPos);
	h_DynEnvPos = (fhx) ScMath::h_Diode_U(h_DynEnvPos, h_DynEnvCurve);
//	h_DynEnvPos = (fhx) ScMath::h_inv(h_DynEnvPos);
}
void ToneStack_I::CreateToneImpulses()
{
	smpPosC = 0;

	impulseLcab = new flx *[SC_NUM_TSTACK_I_IMPULSES_CAB];
	impulseScab = new fsx *[SC_NUM_TSTACK_I_IMPULSES_CAB];
	impulseHcab = new fhx *[SC_NUM_TSTACK_I_IMPULSES_CAB];

	const int xPonent = (int) prms.ImpSyncPowerFac;
	int cnt = 1;
	for (int x = 0; x < xPonent; x++) cnt += cnt;

	const long SyncRateI = long (cnt);
	const fhx SyncRateF = fhx (cnt);
	const long impLngCab = long (fhx (fhx (prms.ImpLenCabF) * fhx (prms.ImpTailPadFac)));
	fhx *BankCabA = new fhx[(SyncRateI*2) * impLngCab];
	fhx *BankCabB = new fhx[(SyncRateI*2) * impLngCab];

	const fhx impRate = fhx (prms.ImpStartSmpRate);

	const fhx LGcabLengthF = fhx (fhx (prms.ImpTailPadFac) *
								fhx (prms.ImpLenCabF) * fhx (sRate) / fhx (impRate));

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

	for (int bnd = 0; bnd < SC_NUM_TSTACK_I_IMPULSES_CAB; bnd++)
	{
		for (long x = 0; x < ((SyncRateI*2) * impLngCab); x++)
		{
			BankCabA[x] = (fhx) zPoint;
			BankCabB[x] = (fhx) zPoint;
		}

		//! Cab
		long xpCount = 1;
		long rdPoint = 0;
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

		fhx bnkRead = (fhx) 0.0;
		for (long x = 0; x < LGcabLengthI; x++)
		{
			const long bnkTravel = long (bnkRead);
			if (bnkTravel >= (SyncRateI * 2 * impLngCab)) break;

			impulseHcab[bnd][x] = fhx (BankCabA[bnkTravel]);
			impulseScab[bnd][x] = fsx (BankCabA[bnkTravel]);
			impulseLcab[bnd][x] = flx (BankCabA[bnkTravel]);

			bnkRead = fhx (fhx (advRate) * fhx (x));
		}

		const fhx maxVc = fhx (iVolumesCab[bnd] * prms.ImpStartSmpRate / (fhx) sRate);

		volAdjC_h[bnd] = fhx (maxVc);
		volAdjC_s[bnd] = fsx (maxVc);
		volAdjC_l[bnd] = flx (maxVc);
	}

	int nLong = int (LGcabLengthI);

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

	delete [] BankCabA;
	delete [] BankCabB;
}
