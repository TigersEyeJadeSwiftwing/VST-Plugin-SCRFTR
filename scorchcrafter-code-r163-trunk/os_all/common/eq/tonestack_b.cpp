#include "tonestack_b.h"
#include "tstack_wavs.h"

ToneStack_A::ToneStack_A(fsx nRate) :
	sRate(fhx (nRate)),
	setShaped(false),
	fpPrec(int (1)),
	spLength(0)
{
	s_bValue[0] = fsx (0.50);
	s_bValue[1] = fsx (0.50);
	s_bValue[2] = fsx (0.50);
	s_bValue[3] = fsx (0.00);
	s_bValue[4] = fsx (0.00);
	s_bValue[5] = fsx (0.00);

	for(int x = 0; x < 6; x++)
	{
		l_bValue[x] = flx (s_bValue[x]);
		h_bValue[x] = fhx (s_bValue[x]);
	}

	InitToneImpulses();

	ClearBuffs();
}
ToneStack_A::~ToneStack_A()
{
	for (int x = 0; x < 6; x++)
	{
		impLength[x] = 0;

		delete [] impulseL[x];
		delete [] impulseS[x];
		delete [] impulseH[x];
	}
	delete [] impulseL;
	delete [] impulseS;
	delete [] impulseH;

	delete [] smpH;
	delete [] smpS;
	delete [] smpL;
}
void ToneStack_A::setNewSampleRate(const fsx nRate)
{
	sRate = fhx (nRate);

	spLength = 0;

	for (int x = 0; x < 6; x++)
	{
		impLength[x] = 0;

		delete [] impulseL[x];
		delete [] impulseS[x];
		delete [] impulseH[x];
	}
	delete [] impulseL;
	delete [] impulseS;
	delete [] impulseH;

	delete [] smpH;
	delete [] smpS;
	delete [] smpL;

	InitToneImpulses();

	ClearBuffs();
}
void ToneStack_A::setBandValue(const int bNum, fsx value)
{
	if(bNum < 0) return;
	if(bNum > 5) return;

	value += fsx (0.250);

	if(bNum == 0) value *= fsx (16.0);
	if(bNum == 1) value *= fsx (-0.250);
	if(bNum == 2) value *= fsx (12.0);
	if(bNum == 3) value *= fsx (0.750);
	if(bNum == 4) value *= fsx (0.750);
	if(bNum == 5) value *= fsx (16.0);

	l_bValue[bNum] = flx (value);
	s_bValue[bNum] = fsx (value);
	h_bValue[bNum] = fhx (value);
}
void ToneStack_A::setEvenShaped(const bool isShaped)
{
	if(isShaped != setShaped) ClearBuffs();
	setShaped = isShaped;
}
fsx ToneStack_A::GetProcessedOutput()
{
	if(fpPrec == 0) return fsx (l_spl);
	if(fpPrec == 1) return fsx (s_spl);
	if(fpPrec == 2) return fsx (h_spl);

	return fsx (0.0);
}
void ToneStack_A::ClearBuffs()
{
	smpPos = 0;
}
void ToneStack_A::SetInput(const fsx inp)
{
	h_spl = fhx (inp);
	s_spl = fsx (inp);
	l_spl = flx (inp);
}
void ToneStack_A::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
}
void ToneStack_A::RunEQ()
{
	if (spLength == 0) return;

	if(fpPrec == 0)
	{
		return;
	}
	if(fpPrec == 1)
	{
		return;
	}
	if(fpPrec == 2)
	{
		smpH[smpPos] = h_spl;
		smpPos++;
		if(smpPos >= spLength) smpPos = 0;

		fhx mix = fhx (0.0);
		fhx out = fhx (0.0);

		int x = 0;
		int y = smpPos - 1;
		if(y < 0) y = spLength - 1;
		int z = 0;
		int bnd = 0;
		for (x = 0; x < spLength; x++)
		{
			mix = fhx (0.0);
			for (bnd = 0; bnd < 6; bnd++)
			{
				mix += fhx (impulseH[bnd][z] * h_bValue[bnd]);
			}
			out += fhx (mix * smpH[y]);

			y--;
			if (y < 0) y = spLength -1;
			z++;
			if (z >= spLength) z = 0;
		}

		h_spl = fhx (out);
		// h_spl *= fhx (0.50);

		return;
	}
}
void ToneStack_A::InitToneImpulses()
{
	impulseL = new flx *[6];
	impulseS = new fsx *[6];
	impulseH = new fhx *[6];

	for (int x = 0; x < 6; x++)
	{
		impLength[x] = 0;
	}

	int bnd = 0;

	const fhx syncRate = fhx (64.0);
	const long syncRateL = long (64);
	const fhx sRateM = fhx (fhx (sRate) * fhx (syncRate));
	const fsx impulseRate = fsx (44100.0);
	const fhx mFac = fhx (fhx (1.0) / fhx (100000000000000000.0));
	long r = 0;
	long m = 0;
	long cnt = 0;
	BTWorthLP_HD *SplReadFilter = new BTWorthLP_HD(fsx (fhx (impulseRate * fhx (0.50))), fsx (fhx (impulseRate * syncRate)));
	BTWorthLP_HD *inspFilter = new BTWorthLP_HD(fsx (sRate), fsx (sRateM));

	long impLengthT = long (fhx (fhx (fhx (sRateM) / fhx (impulseRate)) * fhx (32.0)));
	fhx *impulseLG = new fhx[impLengthT];
	fhx *impulseR = new fhx[200 * syncRateL];

	fhx sMarker = fhx (0.0);
	fhx sPartial = fhx (0.0);
	const fhx sAdvRate = fhx (fhx (impulseRate) / fhx (sRate));
	long x = 0;
	long a = 0;
	long b = 0;
	long newLength = 0;
	fhx sValA = fhx (0.0);
	fhx sValB = fhx (0.0);
	fhx tH = fhx (0.0);
	fhx maxV = fhx (0.0);

	for (bnd = 0; bnd < 6; bnd++)
	{
		SplReadFilter->zeroBuffers();
		inspFilter->zeroBuffers();

		m = 0;
		cnt = 0;
		maxV = fhx (0.0);

		for(r = 0; r < (195 * syncRateL); r++)
		{
			if(cnt == 0)
			{
				impulseR[r] = fhx (0.0);
				if (bnd == 0) impulseR[r] = fhx (fhx (QwavA[m]) * fhx (mFac));
				if (bnd == 1) impulseR[r] = fhx (fhx (QwavB[m]) * fhx (mFac));
				if (bnd == 2) impulseR[r] = fhx (fhx (QwavC[m]) * fhx (mFac));
				if (bnd == 3) impulseR[r] = fhx (fhx (QwavD[m]) * fhx (mFac));
				if (bnd == 4) impulseR[r] = fhx (fhx (QwavE[m]) * fhx (mFac));
				if (bnd == 5) impulseR[r] = fhx (fhx (QwavF[m]) * fhx (mFac));
			}
			else impulseR[r] = fhx (0.0);
			cnt++;
			if(cnt >= syncRateL)
			{
				cnt = 0;
				m++;
			}
			if(m >= 194) break;
		}

		for(r = 0; r < (193 * syncRateL); r++)
		{
			impulseR[r] *= fhx (syncRate * fhx (0.250));
			impulseR[r] = (fhx) SplReadFilter->runFilterH(impulseR[r]);
		}
		sMarker = fhx (0.0);
		sPartial = fhx (0.0);
		x = 0;
		a = 0;
		b = 0;
		sValA = fhx (0.0);
		sValB = fhx (0.0);
		for(x = 0; x < impLengthT; x++)
		{
			a = long ((fhx) floorl(fhx (sMarker)));
			b = long (a + long (1));
			sPartial = fhx (fhx (sMarker) - (fhx) floorl(fhx (sMarker)));
			sValA = fhx (impulseR[a]);
			sValB = fhx (impulseR[b]);
			sValA *= fhx (fhx (1.0) - fhx (sPartial));
			sValB *= fhx (sPartial);
			impulseLG[x] = fhx (fhx (fhx (sValA) + fhx (sValB)) * fhx (0.50));

			sMarker += fhx (sAdvRate);

			if(fhx (sMarker) > fhx (fhx (190.0) * syncRate))
			{
				impLengthT = long (fhx (fhx (x) / fhx (sAdvRate))) - 1;
				break;
			}
		}
		for(x = 0; x < impLengthT; x++)
		{
			impulseLG[x] = (fhx) inspFilter->runFilterH(fhx (impulseLG[x]));
			// if (impulseLG[x] > maxV) maxV = impulseLG[x];
			// if ((-impulseLG[x]) > maxV) maxV = (-impulseLG[x]);
			maxV += (fhx) fabsl(impulseLG[x]);
		}
		newLength = long (impLengthT / syncRateL);

		if (maxV != fhx (0.0)) maxV = fhx (fhx (1.0) / maxV);
		else maxV = fhx (1.0);

		impulseH[bnd] = new fhx[newLength];
		impulseS[bnd] = new fsx[newLength];
		impulseL[bnd] = new flx[newLength];

		for(x = 0; x < newLength; x++)
		{
			tH = fhx (impulseLG[x*syncRateL]);
//			tH *= fhx (maxV);

			impulseH[bnd][x] = fhx (tH);
			impulseS[bnd][x] = fsx (tH);
			impulseL[bnd][x] = flx (tH);
		}
		impLength[bnd] = newLength;
	}

	newLength = 99999;

	for (x = 0; x < 6; x++)
	{
		if(newLength > impLength[x]) newLength = impLength[x];
	}

	smpH = new fhx[newLength];
	smpS = new fsx[newLength];
	smpL = new flx[newLength];

	spLength = int (newLength);

	delete [] impulseLG;
	delete [] impulseR;
	delete SplReadFilter;
	delete inspFilter;
}
