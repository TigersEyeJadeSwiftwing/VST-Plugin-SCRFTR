#include "cabmic_a.h"
#include "imp_wavs_a.h"
#include "tstack_params.h"
#include <cstdlib>
#include <algorithm>

#include "../custdata_types.h"

using namespace ScMath;

Impulse_Internal_A::Impulse_Internal_A(fsx nRate, const SC_ToneStack_Params inpParams) :
	sRate(fhx (nRate)),
	prms(inpParams),
	fpPrec(int (1)),
	spLengthCab(0),
	CabMic(false),
	cShiftPcnt(fhx (0.0)),
	cSkewPcnt(fhx (0.0)),
	currImpShift(0),
	curCab(0)
{
	rfV = new RateFC();

	CreateToneImpulses();

	ClearBuffs();
}
Impulse_Internal_A::~Impulse_Internal_A()
{
	for (int x = 0; x < SC_NUM_IMPULSE_A_IMPULSES; x++)
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

	delete rfV;
}
void Impulse_Internal_A::setNewSampleRate(const fsx nRate)
{
	sRate = fhx (nRate);

	spLengthCab = 0;

	for (int x = 0; x < SC_NUM_IMPULSE_A_IMPULSES; x++)
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

	ClearBuffs();
}
void Impulse_Internal_A::ClearBuffs()
{
	for (int f = 0; f < spLengthCab; f++)
	{
		smpHcab[f] = fhx (0.0);
		smpScab[f] = fsx (0.0);
		smpLcab[f] = flx (0.f);
	}

	smpPosC = 0;
}
void Impulse_Internal_A::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
}
void Impulse_Internal_A::RunConvolutionCab()
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
void Impulse_Internal_A::CreateToneImpulses()
{
	smpPosC = 0;

	impulseLcab = new flx *[SC_NUM_IMPULSE_A_IMPULSES_CAB];
	impulseScab = new fsx *[SC_NUM_IMPULSE_A_IMPULSES_CAB];
	impulseHcab = new fhx *[SC_NUM_IMPULSE_A_IMPULSES_CAB];

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

	for (int bnd = 0; bnd < SC_NUM_IMPULSE_A_IMPULSES_CAB; bnd++)
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
