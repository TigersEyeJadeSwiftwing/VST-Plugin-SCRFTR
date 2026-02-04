#include "tonestack_c.h"
#include "tstack_wav_c.h"
#include <cstdlib>
#include <algorithm>

ToneStack_C::ToneStack_C(fsx nRate) :
	sRate(fhx (nRate)),
	setShaped(true),
	fpPrec(int (1)),
	spLength(0),
	h_envPos(fhx (0.0)),
	s_envPos(fsx (0.0)),
	l_envPos(flx (0.f)),
	CabMic(false)
{
	s_bValue[0] = fsx (0.50);
	s_bValue[1] = fsx (0.50);
	s_bValue[2] = fsx (0.50);
	s_bValue[3] = fsx (0.00);
	s_bValue[4] = fsx (0.00);
	s_bValue[5] = fsx (0.00);

	rfV = new RateFC();

	for(int x = 0; x < 6; x++)
	{
		l_bValue[x] = flx (s_bValue[x]);
		h_bValue[x] = fhx (s_bValue[x]);
	}

	InitToneImpulses();

	h_EqBand_Freq[0][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 20.0 / fhx (sRate)));
	h_EqBand_Freq[1][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 600.0 / fhx (sRate)));
	h_EqBand_Freq[2][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1600.0 / fhx (sRate)));
	h_EqBand_Freq[3][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 300.0 / fhx (sRate)));
	h_EqBand_Freq[4][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1000.0 / fhx (sRate)));
	h_EqBand_Freq[5][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 3000.0 / fhx (sRate)));

	h_EqBand_Freq[0][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 150.0 / fhx (sRate)));
	h_EqBand_Freq[1][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1800.0 / fhx (sRate)));
	h_EqBand_Freq[2][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 3000.0 / fhx (sRate)));
	h_EqBand_Freq[3][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 500.0 / fhx (sRate)));
	h_EqBand_Freq[4][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 2000.0 / fhx (sRate)));
	h_EqBand_Freq[5][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 5000.0 / fhx (sRate)));

	for (int x = 0; x < 6; x++)
	{
		l_EqBand_Pos[x][0] = flx (0.0);
		l_EqBand_Pos[x][1] = flx (0.0);
		s_EqBand_Pos[x][0] = fsx (0.0);
		s_EqBand_Pos[x][1] = fsx (0.0);
		h_EqBand_Pos[x][0] = fhx (0.0);
		h_EqBand_Pos[x][1] = fhx (0.0);

		l_EqBand_Freq[x][0] = flx (h_EqBand_Freq[x][0]);
		l_EqBand_Freq[x][1] = flx (h_EqBand_Freq[x][1]);
		s_EqBand_Freq[x][0] = fsx (h_EqBand_Freq[x][0]);
		s_EqBand_Freq[x][1] = fsx (h_EqBand_Freq[x][1]);
	}

	ClearBuffs();
}
ToneStack_C::~ToneStack_C()
{
	for (int x = 0; x < SC_NUM_TSTACK_C_IMPULSES; x++)
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
void ToneStack_C::setNewSampleRate(const fsx nRate)
{
	sRate = fhx (nRate);

	spLength = 0;

	for (int x = 0; x < SC_NUM_TSTACK_C_IMPULSES; x++)
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

	h_EqBand_Freq[0][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 20.0 / fhx (sRate)));
	h_EqBand_Freq[1][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 600.0 / fhx (sRate)));
	h_EqBand_Freq[2][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1600.0 / fhx (sRate)));
	h_EqBand_Freq[3][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 300.0 / fhx (sRate)));
	h_EqBand_Freq[4][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1000.0 / fhx (sRate)));
	h_EqBand_Freq[5][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 3000.0 / fhx (sRate)));

	h_EqBand_Freq[0][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 150.0 / fhx (sRate)));
	h_EqBand_Freq[1][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1800.0 / fhx (sRate)));
	h_EqBand_Freq[2][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 3000.0 / fhx (sRate)));
	h_EqBand_Freq[3][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 500.0 / fhx (sRate)));
	h_EqBand_Freq[4][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 2000.0 / fhx (sRate)));
	h_EqBand_Freq[5][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 5000.0 / fhx (sRate)));

	for (int x = 0; x < 6; x++)
	{
		l_EqBand_Pos[x][0] = flx (0.0);
		l_EqBand_Pos[x][1] = flx (0.0);
		s_EqBand_Pos[x][0] = fsx (0.0);
		s_EqBand_Pos[x][1] = fsx (0.0);
		h_EqBand_Pos[x][0] = fhx (0.0);
		h_EqBand_Pos[x][1] = fhx (0.0);

		l_EqBand_Freq[x][0] = flx (h_EqBand_Freq[x][0]);
		l_EqBand_Freq[x][1] = flx (h_EqBand_Freq[x][1]);
		s_EqBand_Freq[x][0] = fsx (h_EqBand_Freq[x][0]);
		s_EqBand_Freq[x][1] = fsx (h_EqBand_Freq[x][1]);
	}

	ClearBuffs();
}
void ToneStack_C::setBandValue(const int bNum, fsx value)
{
	if(bNum < 0) return;
	if(bNum > 5) return;

	l_bValue[bNum] = flx (value);
	s_bValue[bNum] = fsx (value);
	h_bValue[bNum] = fhx (value);
}
void ToneStack_C::setEvenShaped(const bool isShaped)
{
	if(setShaped != isShaped) ClearBuffs();
	setShaped = isShaped;
}
fsx ToneStack_C::GetProcessedOutput()
{
	if(fpPrec == 0) return fsx (l_spl);
	if(fpPrec == 1) return fsx (s_spl);
	if(fpPrec == 2) return fsx (h_spl);

	return fsx (0.0);
}
void ToneStack_C::ClearBuffs()
{
	h_envPos = fhx (0.0);
	s_envPos = fsx (0.0);
	l_envPos = flx (0.0);

	for (int x = 0; x < 6; x++)
	{
		l_EqBand_Pos[x][0] = flx (0.0);
		l_EqBand_Pos[x][1] = flx (0.0);
		s_EqBand_Pos[x][0] = fsx (0.0);
		s_EqBand_Pos[x][1] = fsx (0.0);
		h_EqBand_Pos[x][0] = fhx (0.0);
		h_EqBand_Pos[x][1] = fhx (0.0);
	}
}
void ToneStack_C::SetInput(const fsx inp)
{
	h_spl = fhx (inp);
	s_spl = fsx (inp);
	l_spl = flx (inp);
}
void ToneStack_C::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
}
void ToneStack_C::RunEQ()
{
	if(fpPrec == 0)
	{
		l_spl = (flx) fminf(flx (1.0f), fmaxf(l_spl, flx (-1.0f)));

		if(flx (l_spl) == flx (0.0f)) return;
		l_subSpl[0] = flx (l_spl);
		l_subSpl[1] = flx (l_spl);
		l_subSpl[2] = flx (l_spl);
		l_subSpl[3] = flx (l_spl);
		l_subSpl[4] = flx (l_spl);
		l_subSpl[5] = flx (l_spl);

		for(int z = 0; z < 6; z++)
		{
			l_subSpl[z] -= flx (l_EqBand_Pos[z][0] += flx (flx ((flx) l_subSpl[z] - (flx) l_EqBand_Pos[z][0]) * flx (l_EqBand_Freq[z][0])));
			l_subSpl[z] = flx (l_EqBand_Pos[z][1] += flx (flx ((flx) l_subSpl[z] - (flx) l_EqBand_Pos[z][1]) * flx (l_EqBand_Freq[z][1])));
		}

		const flx plBack = flx (flx (flx (2.0f) - l_envPos) * flx (2.0f));

		l_subSpl[0] *= flx (flx (l_bValue[0] * l_bValue[0] * flx (plBack)) - flx (0.50f));
		l_subSpl[1] *= flx (flx (l_bValue[1] * l_bValue[1] * flx (2.0f)) - flx (0.50f));
		l_subSpl[2] *= flx (flx (l_bValue[2] * l_bValue[2] * flx (plBack)) - flx (0.50f));
		l_subSpl[3] *= flx (flx (l_bValue[3]) * flx (2.0f));
		l_subSpl[4] *= flx (flx (l_bValue[4]) * flx (2.0f));
		l_subSpl[5] *= flx (l_bValue[5] * l_bValue[5] * flx (8.0f));

		l_spl *= flx (0.50f);
		l_spl += flx (flx (l_subSpl[0]) + flx (l_subSpl[1]) + flx (l_subSpl[2]) + flx (l_subSpl[3]) + flx (l_subSpl[4]) + flx (l_subSpl[5]));
//		l_spl *= flx (flx (1.0f) - flx (l_envPos * flx (0.250f)));
//		l_spl *= flx (1.50f);

		return;
	}
	if(fpPrec == 1)
	{
		s_spl = (fsx) fmin(fsx (1.0), fmax(s_spl, fsx (-1.0)));

		if(fsx (s_spl) == fsx (0.0)) return;
		s_subSpl[0] = fsx (s_spl);
		s_subSpl[1] = fsx (s_spl);
		s_subSpl[2] = fsx (s_spl);
		s_subSpl[3] = fsx (s_spl);
		s_subSpl[4] = fsx (s_spl);
		s_subSpl[5] = fsx (s_spl);

		for(int z = 0; z < 6; z++)
		{
			s_subSpl[z] -= fsx (s_EqBand_Pos[z][0] += fsx (fsx ((fsx) s_subSpl[z] - (fsx) s_EqBand_Pos[z][0]) * fsx (s_EqBand_Freq[z][0])));
			s_subSpl[z] = fsx (s_EqBand_Pos[z][1] += fsx (fsx ((fsx) s_subSpl[z] - (fsx) s_EqBand_Pos[z][1]) * fsx (s_EqBand_Freq[z][1])));
		}

		const fsx plBack = fsx (fsx (fsx (2.0) - s_envPos) * fsx (2.0));

		s_subSpl[0] *= fsx (fsx (s_bValue[0] * s_bValue[0] * fsx (plBack)) - fsx (0.50));
		s_subSpl[1] *= fsx (fsx (s_bValue[1] * s_bValue[1] * fsx (2.0)) - fsx (0.50));
		s_subSpl[2] *= fsx (fsx (s_bValue[2] * s_bValue[2] * fsx (plBack)) - fsx (0.50));
		s_subSpl[3] *= fsx (fsx (s_bValue[3]) * fsx (2.0));
		s_subSpl[4] *= fsx (fsx (s_bValue[4]) * fsx (2.0));
		s_subSpl[5] *= fsx (s_bValue[5] * s_bValue[5] * fsx (8.0));

		s_spl *= fsx (0.50);
		s_spl += fsx (fsx (s_subSpl[0]) + fsx (s_subSpl[1]) + fsx (s_subSpl[2]) + fsx (s_subSpl[3]) + fsx (s_subSpl[4]) + fsx (s_subSpl[5]));
//		s_spl *= fsx (fsx (1.0) - fsx (s_envPos * fsx (0.250)));
//		s_spl *= fsx (1.50);

		return;
	}
	if(fpPrec == 2)
	{
		h_spl = (fhx) fminl(fhx (1.0), fmaxl(h_spl, fhx (-1.0)));

		if(fhx (h_spl) == fhx (0.0)) return;
		h_subSpl[0] = fhx (h_spl);
		h_subSpl[1] = fhx (h_spl);
		h_subSpl[2] = fhx (h_spl);
		h_subSpl[3] = fhx (h_spl);
		h_subSpl[4] = fhx (h_spl);
		h_subSpl[5] = fhx (h_spl);

		for(int z = 0; z < 6; z++)
		{
			h_subSpl[z] -= fhx (h_EqBand_Pos[z][0] += fhx (fhx ((fhx) h_subSpl[z] - (fhx) h_EqBand_Pos[z][0]) * fhx (h_EqBand_Freq[z][0])));
			h_subSpl[z] = fhx (h_EqBand_Pos[z][1] += fhx (fhx ((fhx) h_subSpl[z] - (fhx) h_EqBand_Pos[z][1]) * fhx (h_EqBand_Freq[z][1])));
		}

		const fhx plBack = fhx (fhx (fhx (2.0) - h_envPos) * fhx (2.0));

		h_subSpl[0] *= fhx (fhx (h_bValue[0] * h_bValue[0] * fhx (plBack)) - fhx (0.50));
		h_subSpl[1] *= fhx (fhx (h_bValue[1] * h_bValue[1] * fhx (2.0)) - fhx (0.50));
		h_subSpl[2] *= fhx (fhx (h_bValue[2] * h_bValue[2] * fhx (plBack)) - fhx (0.50));
		h_subSpl[3] *= fhx (fhx (h_bValue[3]) * fhx (2.0));
		h_subSpl[4] *= fhx (fhx (h_bValue[4]) * fhx (2.0));
		h_subSpl[5] *= fhx (h_bValue[5] * h_bValue[5] * fhx (8.0));

		h_spl *= fhx (0.50);
		h_spl += fhx (fhx (h_subSpl[0]) + fhx (h_subSpl[1]) + fhx (h_subSpl[2]) + fhx (h_subSpl[3]) + fhx (h_subSpl[4]) + fhx (h_subSpl[5]));
//		h_spl *= fhx (fhx (1.0) - fhx (h_envPos * fhx (0.250)));
//		h_spl *= fhx (1.50);

		return;
	}
}
void ToneStack_C::RunConv()
{
	if (spLength < 3) return;
	if(setShaped == false) return;

	if(fpPrec == 0)
	{
		smpL[smpPos] = l_spl;
		smpPos++;
		if(smpPos >= spLength) smpPos = 0;

		int crPatch = 1;
		if(CabMic == false) crPatch = 0;

		flx mix = flx (0.0f);
		int x = 0;
		int y = (smpPos - 1);
		if(y < 0) y = (spLength - 1);
		int z = 0;
		for (x = 0; x < spLength; x++)
		{
			mix += flx (impulseL[crPatch][z] * smpL[y]);
			y--;
			if (y < 0) y = (spLength -1);
			z++;
			if (z >= spLength) z = 0;
		}
		l_spl = flx (mix);
		return;
	}
	if(fpPrec == 1)
	{
		smpS[smpPos] = s_spl;
		smpPos++;
		if(smpPos >= spLength) smpPos = 0;

		int crPatch = 1;
		if(CabMic == false) crPatch = 0;

		fsx mix = fsx (0.0);
		int x = 0;
		int y = (smpPos - 1);
		if(y < 0) y = (spLength - 1);
		int z = 0;
		for (x = 0; x < spLength; x++)
		{
			mix += fsx (impulseS[crPatch][z] * smpS[y]);
			y--;
			if (y < 0) y = (spLength -1);
			z++;
			if (z >= spLength) z = 0;
		}
		s_spl = fsx (mix);
		return;
	}
	if(fpPrec == 2)
	{
		smpH[smpPos] = h_spl;
		smpPos++;
		if(smpPos >= spLength) smpPos = 0;

		int crPatch = 1;
		if(CabMic == false) crPatch = 0;

		fhx mix = fhx (0.0);
		int x = 0;
		int y = (smpPos - 1);
		if(y < 0) y = (spLength - 1);
		int z = 0;
		for (x = 0; x < spLength; x++)
		{
			mix += fhx (impulseH[crPatch][z] * smpH[y]);
			y--;
			if (y < 0) y = (spLength -1);
			z++;
			if (z >= spLength) z = 0;
		}
		h_spl = fhx (mix);
		return;
	}
}
void ToneStack_C::InitToneImpulses()
{
	smpPos = 0;

	impulseL = new flx *[SC_NUM_TSTACK_C_IMPULSES];
	impulseS = new fsx *[SC_NUM_TSTACK_C_IMPULSES];
	impulseH = new fhx *[SC_NUM_TSTACK_C_IMPULSES];

	for (int x = 0; x < SC_NUM_TSTACK_C_IMPULSES; x++)
	{
		impLength[x] = 0;
	}

	int bnd = 0;

	const fhx syncRate = fhx (64.0);
	const long syncRateL = long (64);
	const long syncRateLmidP = long (0);
	const fhx sRateM = fhx (fhx (sRate) * fhx (syncRate));
	const fhx impulseRate = fhx (44100.0);
	const fhx mFac = fhx (fhx (1.0) / fhx (wConvFac));
	long r = 0;
	long m = 0;
	long cnt = 0;
	BTWorthLP_HD *SplReadFilter = new BTWorthLP_HD(fsx (fhx (impulseRate * fhx (0.250))), fsx (fhx (impulseRate * syncRate)));
	BTWorthLP_HD *inspFilter = new BTWorthLP_HD(fsx (sRate * (fsx) 0.250), fsx (sRateM));

	const long impLengthT = long (fhx (fhx (fhx (sRateM) / fhx (impulseRate)) * fhx (SC_TSTACK_C_IMPULSE_MAX_F)));
	fhx *impulseLG = new fhx[impLengthT+syncRateL+2];
	fhx *impulseR = new fhx[(SC_TSTACK_C_IMPULSE_MAX_I+2) * syncRateL];

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
	fhx maxV = fhx (0.0);

	for (bnd = 0; bnd < SC_NUM_TSTACK_C_IMPULSES; bnd++)
	{
		SplReadFilter->zeroBuffers();
		inspFilter->zeroBuffers();

		m = 0;
		cnt = 0;
		maxV = fhx (0.0);

		for(r = 0; r < ((SC_TSTACK_C_IMPULSE_MAX_I+2) * syncRateL); r++)
		{
			impulseR[r] = fhx (0.0);
		}

		for(r = 0; r < ((SC_TSTACK_C_IMPULSE_MAX_I) * syncRateL); r++)
		{
			if(cnt == syncRateLmidP)
			{
				impulseR[r] = fhx (0.0);
				if (bnd == 0) impulseR[r] = fhx (fhx (wavAmpV[m]) * fhx (mFac));
				if (bnd == 1) impulseR[r] = fhx (fhx (wavAmpV[m]) * fhx (mFac));
			}
			else impulseR[r] = fhx (0.0);

			cnt++;
			if(cnt >= syncRateL)
			{
				cnt = 0;
				m++;
			}
			if(m > SC_TSTACK_C_IMPULSE_MAX_I) break;
		}

		for(r = 0; r < ((SC_TSTACK_C_IMPULSE_MAX_I) * syncRateL); r++)
		{
			impulseR[r] = (fhx) SplReadFilter->runFilterH(impulseR[r]);
			impulseR[r] *= fhx (syncRate * fhx (0.9750));
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
			if (a > (impLengthT)) a = (impLengthT);
			b = long (a + long (1));
			sPartial = fhx (fhx (sMarker) - (fhx) floorl(fhx (sMarker)));
			sPartial = (fhx) fminl(fhx (1.0), fmaxl(fhx (-1.0), (fhx) sPartial));
			sValA = fhx (impulseR[a]);
			sValB = fhx (impulseR[b]);
			sValA *= fhx (fhx (1.0) - fhx (sPartial));
			sValB *= fhx (sPartial);
			impulseLG[x] = fhx (fhx (fhx (sValA) + fhx (sValB)) * fhx (0.50));
			impulseLG[x] = (fhx) fminl(fhx (1.0), fmaxl(fhx (-1.0), (fhx) impulseLG[x]));

			sMarker += fhx (sAdvRate);

			if(fhx (sMarker) > fhx (fhx (SC_TSTACK_C_IMPULSE_MAX_F) * (fhx) syncRate))
			{
//				impLengthT = long (x - 2);
				break;
			}
		}
		for(x = 0; x < impLengthT; x++)
		{
			impulseLG[x] = (fhx) inspFilter->runFilterH(fhx (impulseLG[x]));
		//	impulseLG[x] *= fhx (syncRate);
			// if (impulseLG[x] > maxV) maxV = impulseLG[x];
			// if ((-impulseLG[x]) > maxV) maxV = (-impulseLG[x]);
			maxV += (fhx) impulseLG[x];
		}
		newLength = long (impLengthT / syncRateL);

		if (maxV != fhx (0.0)) maxV = fhx (fhx (1.0) / maxV);
		else maxV = fhx (1.0);

		// maxV *= fhx (syncRate);

		volAdj_h[bnd] = fhx (maxV);
		volAdj_s[bnd] = fsx (maxV);
		volAdj_l[bnd] = flx (maxV);

		impulseH[bnd] = new fhx[newLength+1];
		impulseS[bnd] = new fsx[newLength+1];
		impulseL[bnd] = new flx[newLength+1];

		newLength -= 1;

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

	int nLong = 65535;

	for (int mnLong = 0; mnLong < SC_NUM_TSTACK_C_IMPULSES; mnLong++)
	{
		nLong = std::min(nLong, impLength[mnLong]);
		impLength[mnLong] = (int) nLong;
	}

	smpH = new fhx[nLong+1];
	smpS = new fsx[nLong+1];
	smpL = new flx[nLong+1];

	for (int f = 0; f <= nLong; f++)
	{
		smpH[f] = fhx (0.0);
		smpS[f] = fsx (0.0);
		smpL[f] = flx (0.f);
	}

	spLength = int (nLong);

	delete [] impulseLG;
	delete [] impulseR;
	delete SplReadFilter;
	delete inspFilter;
}
void ToneStack_C::SetDynamicQ(const fsx inp)
{
	fhx aSpl = (fhx) fabsl(fhx (inp));
//	aSpl *= fhx (fhx (2.0) - aSpl);
	if (aSpl > h_envPos) h_envPos += fhx (fhx (64.0) / fhx (sRate));
	if (aSpl < h_envPos) h_envPos -= fhx (fhx (16.0) / fhx (sRate));
	h_envPos = (fhx) fminl(fhx (1.0), fmaxl(h_envPos, fhx (0.0)));
	s_envPos = fsx (h_envPos);
	l_envPos = flx (s_envPos);
}
