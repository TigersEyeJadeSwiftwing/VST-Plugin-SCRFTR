#include "fir_eq.h"

#include "custdata_types.h"

using namespace ScMath;

FIR_LP_EQ::FIR_LP_EQ(fsx sRateI, fsx passFreq, int baseFIRSize) :
smpRate(sRateI),
lp_freq(passFreq),
baseSize(baseFIRSize),
impulseReady(false),
l_spl(l_C_0),
s_spl(s_C_0),
h_spl(h_C_0),
actualLength(0),
smpPos(0)
{
	if (lp_freq > (smpRate*s_C_half)) lp_freq = fsx (smpRate*s_C_half);

	BuildImpulse(false);
}
FIR_LP_EQ::~FIR_LP_EQ()
{
	actualLength = 0;
	impulseReady = false;

	delete [] l_sBuff;
	delete [] s_sBuff;
	delete [] h_sBuff;
	delete [] l_iBuff;
	delete [] s_iBuff;
	delete [] h_iBuff;
}
void FIR_LP_EQ::ChangeRates(const fsx nRate, const fsx passRate)
{
	smpRate = nRate;
	lp_freq = (fsx) s_min(passRate, nRate*s_C_half);
	BuildImpulse(true);
}
void FIR_LP_EQ::BuildImpulse(const bool eraseOldBuffers)
{
	if (eraseOldBuffers)
	{
		actualLength = 0;
		impulseReady = false;

		delete [] l_sBuff;
		delete [] s_sBuff;
		delete [] h_sBuff;
		delete [] l_iBuff;
		delete [] s_iBuff;
		delete [] h_iBuff;
	}

	smpPos = 0;

	const fhx lngFac = fhx ((fhx) smpRate / (fhx) 44100.0);
	const fhx baseLng = fhx (baseSize);
	const fhx lengthF = fhx (lngFac * baseLng);
	actualLength = int (lengthF);

	const int fir_size = 1 + (2 * actualLength);
	const int fir_M = actualLength;
	actualLength = fir_size;

	l_sBuff = new flx[actualLength];
	s_sBuff = new fsx[actualLength];
	h_sBuff = new fhx[actualLength];
	l_iBuff = new flx[actualLength];
	s_iBuff = new fsx[actualLength];
	h_iBuff = new fhx[actualLength];

	for (int x = 0; x < actualLength; x++)
	{
		h_iBuff[x] = h_C_0;
		s_iBuff[x] = s_C_0;
		l_iBuff[x] = l_C_0;
	}

	fhx *h_xBuff = new fhx[fir_size];
	for (int x = 0; x < fir_size; x++)
		h_xBuff[x] = h_C_0;

	const fhx x_master_rate = fhx (smpRate);

//!	Set up a Hanning-type FIR window for enhancing the EQ curve
	fhx *x_fWindow = new fhx[fir_size];
	for (int x = 0; x < fir_size; x++)
		x_fWindow[x] = (fhx) FIRwindow_Hanning(x, fir_size);

//!	FIR Build
	const fhx x_fir_m = fhx (fir_M);
	const fhx x_fir_size = fhx (int (fir_size - 1));
	const fhx x_freq_a = fhx (lp_freq);
	const fhx x_frq_ratio_a = fhx (x_freq_a / x_master_rate);

	for (int y = 0; y < fir_size; y++)
	{
		fhx x_point = h_C_0;
		if (y == fir_M)
		{
			x_point += fhx (h_C_2 * x_frq_ratio_a);
		} else
		{
			const fhx x_fac = fhx ( (fhx (y) - (fhx) x_fir_m) );
			const fhx x_a = fhx (sinl(h_C_pi_doub * x_frq_ratio_a * (x_fac)) / (h_C_pi * x_fac));
			x_point += fhx (x_a);
		}

		// x_point /= (fhx) fir_size;
		h_xBuff[y] = fhx (x_point * x_fWindow[y]);
	}

	for (int x = 0; x < actualLength; x++)
	{
		h_iBuff[x] = fhx (h_xBuff[x]);
		s_iBuff[x] = fsx (h_xBuff[x]);
		l_iBuff[x] = flx (h_xBuff[x]);
	}

	h_volAdj = fhx (h_C_1);
	s_volAdj = fsx (h_volAdj);
	l_volAdj = flx (h_volAdj);

	for (int x = 0; x < actualLength; x++)
	{
		h_sBuff[x] = h_C_0;
		s_sBuff[x] = s_C_0;
		l_sBuff[x] = l_C_0;
	}

	delete [] h_xBuff;
	delete [] x_fWindow;

	impulseReady = true;
}
void FIR_LP_EQ::l_RunQ()
{
	if (!impulseReady) return;
	if (actualLength < 3) return;

	smpPos += 1;
	if(smpPos >= actualLength) smpPos = 0;
	l_sBuff[smpPos] = l_spl;

	int cntI = 0;
	int cntS = smpPos;
	flx x_spl = l_C_0;

	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		x_spl += flx (l_sBuff[cntS] * l_iBuff[cntI]);
		cntI++;
		cntS--;
		if (cntS < 0) cntS = actualLength - 1;
		if (cntI >= actualLength) cntI = 0;
	}

	l_spl = flx (x_spl * l_volAdj);
}
void FIR_LP_EQ::s_RunQ()
{
	if (!impulseReady) return;
	if (actualLength < 3) return;

	smpPos += 1;
	if(smpPos >= actualLength) smpPos = 0;
	s_sBuff[smpPos] = s_spl;

	int cntI = 0;
	int cntS = smpPos;
	fsx x_spl = s_C_0;

	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		x_spl += fsx (s_sBuff[cntS] * s_iBuff[cntI]);
		cntI++;
		cntS--;
		if (cntS < 0) cntS = actualLength - 1;
		if (cntI >= actualLength) cntI = 0;
	}

	s_spl = fsx (x_spl * s_volAdj);
}
void FIR_LP_EQ::h_RunQ()
{
	if (!impulseReady) return;
	if (actualLength < 3) return;

	smpPos += 1;
	if(smpPos >= actualLength) smpPos = 0;
	h_sBuff[smpPos] = h_spl;

	int cntI = 0;
	int cntS = smpPos;
	fhx x_spl = h_C_0;

	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		x_spl += fhx (h_sBuff[cntS] * h_iBuff[cntI]);
		cntI++;
		cntS--;
		if (cntS < 0) cntS = actualLength - 1;
		if (cntI >= actualLength) cntI = 0;
	}

	h_spl = fhx (x_spl * h_volAdj);
}

FIR_Band_EQ::FIR_Band_EQ(fsx sRateI, fsx passFreqA, fsx passFreqB, int baseFIRSize) :
smpRate(sRateI),
lp_freq(passFreqB),
hp_freq(passFreqA),
baseSize(baseFIRSize),
impulseReady(false),
l_spl(l_C_0),
s_spl(s_C_0),
h_spl(h_C_0),
actualLength(0),
smpPos(0)
{
	if (lp_freq > (smpRate*s_C_half)) lp_freq = fsx (smpRate*s_C_half);
	if (hp_freq > (smpRate*s_C_half)) hp_freq = fsx (smpRate*s_C_half);

	BuildImpulse(false);
}
FIR_Band_EQ::~FIR_Band_EQ()
{
	actualLength = 0;
	impulseReady = false;

	delete [] l_sBuff;
	delete [] s_sBuff;
	delete [] h_sBuff;
	delete [] l_iBuff;
	delete [] s_iBuff;
	delete [] h_iBuff;
}
void FIR_Band_EQ::ChangeRates(const fsx nRate, const fsx passRate)
{
	smpRate = nRate;
	lp_freq = (fsx) s_min(passRate, nRate*s_C_half);
	BuildImpulse(true);
}
void FIR_Band_EQ::BuildImpulse(const bool eraseOldBuffers)
{
	if (eraseOldBuffers)
	{
		actualLength = 0;
		impulseReady = false;

		delete [] l_sBuff;
		delete [] s_sBuff;
		delete [] h_sBuff;
		delete [] l_iBuff;
		delete [] s_iBuff;
		delete [] h_iBuff;
	}

	smpPos = 0;

	const fhx lngFac = fhx ((fhx) smpRate / (fhx) 44100.0);
	const fhx baseLng = fhx (baseSize);
	const fhx lengthF = fhx (lngFac * baseLng);
	actualLength = int (lengthF);

	const int fir_size = 1 + (2 * actualLength);
	const int fir_M = actualLength;
	actualLength = fir_size;

	l_sBuff = new flx[actualLength];
	s_sBuff = new fsx[actualLength];
	h_sBuff = new fhx[actualLength];
	l_iBuff = new flx[actualLength];
	s_iBuff = new fsx[actualLength];
	h_iBuff = new fhx[actualLength];

	for (int x = 0; x < actualLength; x++)
	{
		h_iBuff[x] = h_C_0;
		s_iBuff[x] = s_C_0;
		l_iBuff[x] = l_C_0;
	}

	fhx *h_xBuff = new fhx[fir_size];
	for (int x = 0; x < fir_size; x++)
		h_xBuff[x] = h_C_0;

	const fhx x_master_rate = fhx (smpRate);

//!	Set up a Hanning-type FIR window for enhancing the EQ curve
	fhx *x_fWindow = new fhx[fir_size];
	for (int x = 0; x < fir_size; x++)
		x_fWindow[x] = (fhx) FIRwindow_Hanning(x, fir_size);

//!	FIR Build
	const fhx x_fir_m = fhx (fir_M);
	const fhx x_fir_size = fhx (int (fir_size - 1));
	const fhx x_freq_a = fhx (hp_freq);
	const fhx x_freq_b = fhx (lp_freq);
	const fhx x_frq_ratio_a = fhx (x_freq_a / x_master_rate);
	const fhx x_frq_ratio_b = fhx (x_freq_b / x_master_rate);

	for (int y = 0; y < fir_size; y++)
	{
		fhx x_point = h_C_0;
		if (y == fir_M)
		{
			x_point += fhx (h_C_2 * (x_frq_ratio_b - x_frq_ratio_a));
		} else
		{
			const fhx x_fac = fhx ( (fhx (y) - (fhx) x_fir_m) );
			const fhx x_a = fhx (sinl(h_C_pi_doub * x_frq_ratio_a * (x_fac)) / (h_C_pi * x_fac));
			const fhx x_b = fhx (sinl(h_C_pi_doub * x_frq_ratio_b * (x_fac)) / (h_C_pi * x_fac));
			x_point += fhx (x_b - x_a);
		}

		// x_point /= (fhx) fir_size;
		h_xBuff[y] = fhx (x_point * x_fWindow[y]);
	}

	for (int x = 0; x < actualLength; x++)
	{
		h_iBuff[x] = fhx (h_xBuff[x]);
		s_iBuff[x] = fsx (h_xBuff[x]);
		l_iBuff[x] = flx (h_xBuff[x]);
	}

	h_volAdj = fhx (h_C_1);
	s_volAdj = fsx (h_volAdj);
	l_volAdj = flx (h_volAdj);

	for (int x = 0; x < actualLength; x++)
	{
		h_sBuff[x] = h_C_0;
		s_sBuff[x] = s_C_0;
		l_sBuff[x] = l_C_0;
	}

	delete [] h_xBuff;
	delete [] x_fWindow;

	impulseReady = true;
}
void FIR_Band_EQ::l_RunQ()
{
	if (!impulseReady) return;
	if (actualLength < 3) return;

	smpPos += 1;
	if(smpPos >= actualLength) smpPos = 0;
	l_sBuff[smpPos] = l_spl;

	int cntI = 0;
	int cntS = smpPos;
	flx x_spl = l_C_0;

	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		x_spl += flx (l_sBuff[cntS] * l_iBuff[cntI]);
		cntI++;
		cntS--;
		if (cntS < 0) cntS = actualLength - 1;
		if (cntI >= actualLength) cntI = 0;
	}

	l_spl = flx (x_spl * l_volAdj);
}
void FIR_Band_EQ::s_RunQ()
{
	if (!impulseReady) return;
	if (actualLength < 3) return;

	smpPos += 1;
	if(smpPos >= actualLength) smpPos = 0;
	s_sBuff[smpPos] = s_spl;

	int cntI = 0;
	int cntS = smpPos;
	fsx x_spl = s_C_0;

	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		x_spl += fsx (s_sBuff[cntS] * s_iBuff[cntI]);
		cntI++;
		cntS--;
		if (cntS < 0) cntS = actualLength - 1;
		if (cntI >= actualLength) cntI = 0;
	}

	s_spl = fsx (x_spl * s_volAdj);
}
void FIR_Band_EQ::h_RunQ()
{
	if (!impulseReady) return;
	if (actualLength < 3) return;

	smpPos += 1;
	if(smpPos >= actualLength) smpPos = 0;
	h_sBuff[smpPos] = h_spl;

	int cntI = 0;
	int cntS = smpPos;
	fhx x_spl = h_C_0;

	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		x_spl += fhx (h_sBuff[cntS] * h_iBuff[cntI]);
		cntI++;
		cntS--;
		if (cntS < 0) cntS = actualLength - 1;
		if (cntI >= actualLength) cntI = 0;
	}

	h_spl = fhx (x_spl * h_volAdj);
}

FIR_Multi_EQ::FIR_Multi_EQ(fsx sRateI, const Impulse_EQ_Input qDataInp) :
smpRate(sRateI),
qData(qDataInp),
impulseReady(false),
l_spl(l_C_0),
s_spl(s_C_0),
h_spl(h_C_0),
actualLength(0),
smpPos(0)
{
	BuildImpulse(false);
}
FIR_Multi_EQ::~FIR_Multi_EQ()
{
	actualLength = 0;
	impulseReady = false;

	delete [] l_sBuff;
	delete [] s_sBuff;
	delete [] h_sBuff;
	delete [] l_iBuff;
	delete [] s_iBuff;
	delete [] h_iBuff;
}
void FIR_Multi_EQ::ChangeSmpRate(const fsx nRate)
{
	smpRate = nRate;
	BuildImpulse(true);
}
void FIR_Multi_EQ::BuildImpulse(const bool eraseOldBuffers)
{
	if (eraseOldBuffers)
	{
		actualLength = 0;
		impulseReady = false;

		delete [] l_sBuff;
		delete [] s_sBuff;
		delete [] h_sBuff;
		delete [] l_iBuff;
		delete [] s_iBuff;
		delete [] h_iBuff;
	}

	smpPos = 0;

	const fhx lngFac = fhx ((fhx) smpRate / (fhx) 44100.0);
	const fhx baseLng = fhx (qData.impLength);
	const fhx lengthF = fhx (lngFac * baseLng);
	actualLength = int (lengthF);

	const int fir_size = 1 + (2 * actualLength);
	const int fir_M = actualLength;
	actualLength = fir_size;
	multiLength = actualLength;

	l_sBuff = new flx[actualLength];
	s_sBuff = new fsx[actualLength];
	h_sBuff = new fhx[actualLength];
	l_iBuff = new flx[multiLength];
	s_iBuff = new fsx[multiLength];
	h_iBuff = new fhx[multiLength];

	for (int x = 0; x < multiLength; x++)
	{
		h_iBuff[x] = h_C_0;
		s_iBuff[x] = s_C_0;
		l_iBuff[x] = l_C_0;
	}

	fhx *h_xBuff = new fhx[multiLength];
	for (int x = 0; x < multiLength; x++)
		h_xBuff[x] = h_C_0;

	const fhx x_master_rate = fhx (smpRate);

//!	Set up a Hanning-type FIR window for enhancing the EQ curve
	fhx *x_fWindow = new fhx[fir_size];
	for (int x = 0; x < fir_size; x++)
		x_fWindow[x] = (fhx) FIRwindow_Hanning(x, fir_size);

//!	FIR Build
	const fhx x_fir_m = fhx (fir_M);
	const fhx x_fir_size = fhx (int (fir_size-1));

	for (int x = 0; x < qData.nPoints; x++)
	{
		const fhx x_freq_low = (fhx) qData.pnt[x].frequencyLow;
		const fhx x_freq_high = (fhx) qData.pnt[x].frequencyHigh;
		const fhx x_frq_ratio_l = fhx (x_freq_low / x_master_rate);
		const fhx x_frq_ratio_h = fhx (x_freq_high / x_master_rate);

		fhx x_ampl = fhx (qData.pnt[x].amplitude);
		if(qData.volFormat == SCr_FIRQ_dB)
			x_ampl = (fhx) powl(h_C_2, fhx (x_ampl / fhx (6.0)));

		const int x_offset = 0;
		for (int y = 0; y < fir_size; y++)
		{
			fhx x_point = h_C_0;
			if (y == fir_M)
			{
				x_point += fhx (h_C_2 * (x_frq_ratio_h - x_frq_ratio_l));
			} else
			{
				const fhx x_fac = fhx ( (fhx (y) - (fhx) x_fir_m) );
				const fhx x_l = fhx (sinl(h_C_pi_doub * x_frq_ratio_l * x_fac) / (h_C_pi * x_fac));
				const fhx x_h = fhx (sinl(h_C_pi_doub * x_frq_ratio_h * x_fac) / (h_C_pi * x_fac));
				x_point += fhx (x_h - x_l);
			}

			x_point *= (fhx) x_fWindow[y];

			h_xBuff[y+x_offset] += fhx (x_point * x_ampl);
		}
	}

	for (int x = 0; x < multiLength; x++)
	{
		h_iBuff[x] = fhx (h_xBuff[x]);
		s_iBuff[x] = fsx (h_xBuff[x]);
		l_iBuff[x] = flx (h_xBuff[x]);
	}

	h_volAdj = fhx (qData.masterVol);
	s_volAdj = fsx (h_volAdj);
	l_volAdj = flx (h_volAdj);

	for (int x = 0; x < actualLength; x++)
	{
		h_sBuff[x] = h_C_0;
		s_sBuff[x] = s_C_0;
		l_sBuff[x] = l_C_0;
	}

	delete [] h_xBuff;
	delete [] x_fWindow;

	impulseReady = true;
}
void FIR_Multi_EQ::l_RunQ()
{
	if (!impulseReady) return;
	if (actualLength < 3) return;

	smpPos += 1;
	if(smpPos >= actualLength) smpPos = 0;
	l_sBuff[smpPos] = l_spl;

	int cntI = 0;
	int cntS = smpPos;
	flx x_spl = l_C_0;

	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		x_spl += flx (l_sBuff[cntS] * l_iBuff[cntI]);
		cntI++;
		cntS--;
		if (cntS < 0) cntS = actualLength - 1;
		if (cntI >= actualLength) cntI = 0;
	}

	l_spl = flx (x_spl * l_volAdj);
}
void FIR_Multi_EQ::s_RunQ()
{
	if (!impulseReady) return;
	if (actualLength < 3) return;

	smpPos += 1;
	if(smpPos >= actualLength) smpPos = 0;
	s_sBuff[smpPos] = s_spl;

	int cntI = 0;
	int cntS = smpPos;
	fsx x_spl = s_C_0;

	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		x_spl += fsx (s_sBuff[cntS] * s_iBuff[cntI]);
		cntI++;
		cntS--;
		if (cntS < 0) cntS = actualLength - 1;
		if (cntI >= actualLength) cntI = 0;
	}

	s_spl = fsx (x_spl * s_volAdj);
}
void FIR_Multi_EQ::h_RunQ()
{
	if (!impulseReady) return;
	if (actualLength < 3) return;

	smpPos += 1;
	if(smpPos >= actualLength) smpPos = 0;
	h_sBuff[smpPos] = h_spl;

	int cntI = 0;
	int cntS = smpPos;
	fhx x_spl = h_C_0;

	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		x_spl += fhx (h_sBuff[cntS] * h_iBuff[cntI]);
		cntI++;
		cntS--;
		if (cntS < 0) cntS = actualLength - 1;
		if (cntI >= actualLength) cntI = 0;
	}

	h_spl = fhx (x_spl * h_volAdj);
}
