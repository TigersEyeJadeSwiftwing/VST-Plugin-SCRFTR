#include "impulse_eq.h"

#include "custdata_types.h"

using namespace ScMath;

ImpulseQ::ImpulseQ(const fsx sRate, const Impulse_EQ_Input inpPoints) :
smpRate(sRate),
qData(inpPoints),
impulseReady(false),
l_spl(l_C_0),
s_spl(s_C_0),
h_spl(h_C_0),
actualLength(0),
smpPos(0)
{
	BuildImpulse(false);
}
ImpulseQ::~ImpulseQ()
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
void ImpulseQ::ChangeSmpRate(const fsx nRate)
{
	smpRate = nRate;
	BuildImpulse(true);
}
void ImpulseQ::BuildImpulse(const bool eraseOldBuffers)
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

	fhx *h_xBuff = new fhx[qData.impLength];
	for (int x = 0; x < qData.impLength; x++)
		h_xBuff[x] = h_C_0;

	const fhx x_rate_master = fhx (44100.0);
	const fhx x_passes = fhx (qData.numPasses);
	const fhx x_impLength = fhx (qData.impLength);
	const fhx x_pass_length_ratio = fhx (x_passes / fhx (actualLength));
	bool invSweep = true;

	fhx x_marker = h_C_0;
	for (int cnt = 0; cnt < qData.numPasses; cnt++)
	{
		const fhx x_cnt = fhx (cnt);
		const fhx x_cnt_plus = fhx (x_cnt + h_C_1);
		const fhx x_cnt_ratio = fhx (x_cnt / x_passes);
		const fhx x_cnt_ratio_p = fhx (x_cnt_plus / x_passes);
		const fhx x_sweep_start = fhx (x_cnt_ratio * x_cnt_ratio);
		const fhx x_sweep_end = fhx (x_cnt_ratio_p * x_cnt_ratio_p);
		int x_s_start = int ( fhx (x_sweep_start * (x_impLength-h_C_1)) );
		int x_s_end = int ( fhx (x_sweep_end * (x_impLength-h_C_1)) );
		if (x_s_start < 0) x_s_start = 0;
		if (x_s_start >= qData.impLength) x_s_start = qData.impLength-1;
		if (x_s_end < 0) x_s_end = 0;
		if (x_s_end >= qData.impLength) x_s_end = qData.impLength-1;

		for (int x = x_s_start; x <= x_s_end; x++)
		{
			const int x_travel_max = x_s_end - x_s_start;
			if (x_travel_max < 0) break;
			const int x_travel_point = x - x_s_start;
			const fhx x_freq_pnt = fhx (fhx (x_travel_point) / fhx (x_travel_max));
			fhx x_frq_ratio = (fhx) x_freq_pnt;
			if (invSweep) x_frq_ratio = (fhx) h_inv(x_frq_ratio);
			invSweep = !invSweep;
			const fhx x_freq = fhx (x_frq_ratio * x_rate_master);

			const fhx x_wav = (fhx) sinl(h_C_pi_doub * x_marker);
			x_marker += fhx (x_frq_ratio);

			int bndB = qData.nPoints;
			while (x_freq < qData.pnt[bndB-1].frequency)
			{
				bndB--;
				if (bndB == 0) break;
			}
			int bndA = bndB - 1;

			fhx fA = h_C_0;
			if (bndA >= 0) fA = (fhx) qData.pnt[bndA].frequency;
			fhx fB = fhx (x_rate_master);
			if (bndB < (qData.nPoints)) fB = (fhx) qData.pnt[bndB].frequency;

			fhx aA = (fhx) qData.pnt[0].amplitude;
			if (bndA >= 0) aA = (fhx) qData.pnt[bndA].amplitude;
			fhx aB = (fhx) qData.pnt[qData.nPoints - 1].amplitude;
			if (bndB < (qData.nPoints)) aB = (fhx) qData.pnt[bndB].amplitude;

			const fhx fC = fhx (fB - fA);
			const fhx fW = fhx (x_freq - fA);
			const fhx fM = fhx (fW / fC);
			const fhx aW = (fhx) h_avgx(aA, aB, fM);

			h_xBuff[x] = fhx (x_wav * aW / x_cnt_plus);
		}
	}
/*
	const s9x x_sine_size_I = s9x (qData.impLength * qData.numPasses);
	const fhx x_passes = fhx (qData.numPasses);
	const fhx x_sine_size_F = fhx (x_sine_size_I);
	const fhx x_rate_master = fhx (44100.0);
	fhx x_marker_x = h_C_0;
	fhx x_marker_y = h_C_0;

	fhx *h_xBuff = new fhx[qData.impLength];
	for (int x = 0; x < qData.impLength; x++)
		h_xBuff[x] = h_C_0;

	fhx *h_yBuff = new fhx[x_sine_size_I];
	fhx *h_zBuff = new fhx[x_sine_size_I];
	for (s9x x = 0; x < x_sine_size_I; x++)
	{
		const fhx x_x = fhx (x);
		const fhx x_ratio = fhx ((x_x + h_C_1) / x_sine_size_F);
		// const fhx x_ratio_inv = (fhx) h_inv(x_ratio);
		const fhx x_freq = fhx (x_ratio * x_rate_master);

		const fhx x_wave = (fhx) sinl(h_C_pi_doub * x_marker_x);
		x_marker_x += fhx (x_ratio);

		h_yBuff[x] = (fhx) x_wave;

		int bndB = qData.nPoints;
		while (x_freq < qData.pnt[bndB-1].frequency)
		{
			bndB--;
			if (bndB == 0) break;
		}
		int bndA = bndB - 1;

		fhx fA = h_C_0;
		if (bndA >= 0) fA = (fhx) qData.pnt[bndA].frequency;
		fhx fB = fhx (x_rate_master);
		if (bndB < (qData.nPoints)) fB = (fhx) qData.pnt[bndB].frequency;

		fhx aA = (fhx) qData.pnt[0].amplitude;
		if (bndA >= 0) aA = (fhx) qData.pnt[bndA].amplitude;
		fhx aB = (fhx) qData.pnt[qData.nPoints - 1].amplitude;
		if (bndB < (qData.nPoints)) aB = (fhx) qData.pnt[bndB].amplitude;

		const fhx fC = fhx (fB - fA);
		const fhx fW = fhx (x_freq - fA);
		const fhx fM = fhx (fW / fC);
		const fhx aW = (fhx) h_avgx(aA, aB, fM);

		h_zBuff[x] = fhx (x_wave * aW);
	}

	x_marker_x = h_C_0;
	const s9x x_sine_size_LG = s9x (x_sine_size_I + qData.impLength);
	for (s9x y = 0; y < x_sine_size_I; y++)
	{
		const s9x z = s9x (y);

		fhx x_cmb = h_C_0;
		fhx x_fac_y = h_C_0;
		fhx x_fac_z = h_C_0;
		for (s9x x = 0; x < qData.impLength; x++)
		{
			if ( ((z+x) < x_sine_size_I) && ((z+x) >= 0) ) x_fac_z = (fhx) h_zBuff[z+x];
			else x_fac_z = h_C_0;
			if ( ((y+x) < x_sine_size_I) && ((y+x) >= 0) ) x_fac_y = (fhx) h_yBuff[y+x];
			else x_fac_y = h_C_deNorm;
			if (x_fac_y == h_C_0) x_fac_y = (fhx) h_C_deNorm;

			x_cmb = fhx (x_fac_z / x_fac_y);
			h_xBuff[x] += fhx (x_cmb / x_sine_size_F);
		}
	}

	h_xBuff[qData.impLength - 1] = h_C_0;
*/
/*	const fhx x_passes = fhx (qData.numPasses);
	const fhx x_rateMaster = fhx (44100.0);
	const fhx x_impLength = fhx (baseLng);
	for (int cnt = 0; cnt < qData.numPasses; cnt++)
	{
		const fhx x_cntHigh = fhx (cnt+1);
		const fhx x_cntLow = fhx (cnt);
		const fhx x_ratioHigh = (fhx) h_C_1; // h_inv(x_cntLow / x_passes);
		const fhx x_ratioLow = (fhx) h_C_0; // h_inv(x_cntHigh / x_passes);
		const fhx x_rateHigh = fhx (x_ratioHigh * x_rateMaster);
		const fhx x_rateLow = (fhx) h_max(x_ratioLow * x_rateMaster, h_C_1);
		const int x_passLengthI = int (fhx (x_impLength * x_cntHigh / x_passes));
		const fhx x_passLengthF = fhx (x_passLengthI);

		fhx x_marker = fhx (h_C_tenth * x_cntLow / x_passes);
		fhx y_marker = (fhx) x_marker;

		for (int x = 0; x <= qData.impLength; x++)
		{
			const fhx x_x = fhx (x);
			const fhx x_travel = fhx (x_x / x_impLength);
			const fhx x_freq = (fhx) h_avgx(x_rateHigh, x_rateLow, x_travel);
			const fhx x_flip = (fhx) h_avgx(x_rateLow, x_rateHigh, x_travel);

			const fhx x_wav_q = (fhx) sinl(h_C_pi_doub * x_marker);
			x_marker += fhx (x_freq / x_rateMaster);
			const fhx x_wav_sine = (fhx) sinl(h_C_pi_doub * y_marker);
			x_marker -= fhx (x_flip / x_rateMaster);

			int bndB = qData.nPoints;
			while (x_freq < qData.pnt[bndB-1].frequency)
			{
				bndB--;
				if (bndB == 0) break;
			}
			int bndA = bndB - 1;

			fhx fA = h_C_0;
			if (bndA >= 0) fA = (fhx) qData.pnt[bndA].frequency;
			fhx fB = fhx (x_rateMaster);
			if (bndB < (qData.nPoints)) fB = (fhx) qData.pnt[bndB].frequency;

			fhx aA = (fhx) qData.pnt[0].amplitude;
			if (bndA >= 0) aA = (fhx) qData.pnt[bndA].amplitude;
			fhx aB = (fhx) qData.pnt[qData.nPoints - 1].amplitude;
			if (bndB < (qData.nPoints)) aB = (fhx) qData.pnt[bndB].amplitude;

			const fhx fC = fhx (fB - fA);
			const fhx fW = fhx (x_freq - fA);
			const fhx fM = fhx (fW / fC);

			const fhx aW = (fhx) h_avgx(aA, aB, fM);

			const fhx x_wav_q = fhx (x_sine * aW);
			const fhx x_mult = fhx ();

			if (cnt == 0) h_xBuff[x] = fhx (x_wav * aW);
			else h_xBuff[x] += fhx (x_wav * aW);
		}
	}	*/

	const s9x syncMult = 128;
	const s9x syncSize = syncMult * qData.impLength;
	fhx *h_syncBuff = new fhx[syncSize];
	for (s9x x = 0; x < syncSize; x++)
		h_syncBuff[x] = h_C_0;

	for (int x = 0; x < qData.impLength; x++)
		h_syncBuff[x*syncMult] = (fhx) h_xBuff[x];

	BTW_LP *UpFilter = new BTW_LP(fsx (fhx (x_rate_master * h_C_half)), fsx (fhx (x_rate_master * fhx (syncMult))));
	for (s9x x = 0; x < syncSize; x++)
	{
		UpFilter->inpSplH(h_syncBuff[x]);
		UpFilter->RunFilter();
		h_syncBuff[x] = fhx (UpFilter->GetOutPutH() * fhx (syncMult));
	}
	delete UpFilter;

	const fhx newLengthF = fhx (actualLength);
	for (int x = 0; x < actualLength; x++)
	{
		const fhx x_fhx = fhx (x);
		const fhx fMarker = fhx (fhx (syncSize) * x_fhx / newLengthF);
		const fhx x_fMarker = (fhx) floorl(fMarker);
		const s9x spotA = s9x (x_fMarker);
		const s9x spotB = spotA + 1;
		fhx v_A = h_C_0;
		if (spotA < syncSize) v_A = (fhx) h_syncBuff[spotA];
		fhx v_B = h_C_0;
		if (spotB < syncSize) v_B = (fhx) h_syncBuff[spotB];
		const fhx t_trav = fhx (fMarker - x_fMarker);

		const fhx targetVal = (fhx) h_avgx(v_A, v_B, t_trav);

		h_iBuff[x] = fhx (targetVal * x_pass_length_ratio);
		s_iBuff[x] = fsx (h_iBuff[x]);
		l_iBuff[x] = flx (h_iBuff[x]);
	}
/*
	long ext_length = long (long (qData.impLength) * long (qData.numPasses));
	fhx *h_xBuff = new fhx[ext_length];
	fhx fMarker = h_C_0;
	const fhx x_LengthFac = fhx (ext_length);
	const fhx x_Curve = fhx (h_C_2);
	const fhx x_rate = fhx (fhx (44100.0) * fhx (qData.numPasses));
	for (long x = 0; x < ext_length; x++)
	{
		const fhx x_FreqPoint = fhx (x);
		const fhx x_FreqRatio = fhx (x_FreqPoint / (x_LengthFac));
		const fhx x_FreqBase = fhx (h_inv(x_FreqRatio) * h_inv(x_FreqRatio) / fhx (qData.numPasses));
		// const fhx x_FreqBase = fhx (expl(x_FreqRatio * -h_C_pi_doub) / fhx (qData.numPasses));
		const fhx x_VolCorrect = fhx ( h_C_1 + (h_inv(x_FreqRatio) - (h_inv(x_FreqRatio)*h_inv(x_FreqRatio))) );
		// const fhx x_VolCorrect = fhx ( (h_C_1 / x_LengthFac) + (h_inv(x_FreqRatio) * h_inv(x_FreqRatio)) );
		const fhx x_Freq = fhx (x_rate * x_FreqBase);

		const fhx fWav = (fhx) sinl(h_C_pi_doub * fMarker);
		fMarker += fhx (x_FreqBase);

		int bndB = qData.nPoints;
		while (x_Freq < qData.pnt[bndB-1].frequency)
		{
			bndB--;
			if (bndB == 0) break;
		}
		int bndA = bndB - 1;

		fhx fA = h_C_0;
		if (bndA >= 0) fA = (fhx) qData.pnt[bndA].frequency;
		fhx fB = fhx (x_rate);
		if (bndB < (qData.nPoints)) fB = (fhx) qData.pnt[bndB].frequency;

		fhx aA = (fhx) qData.pnt[0].amplitude;
		if (bndA >= 0) aA = (fhx) qData.pnt[bndA].amplitude;
		fhx aB = (fhx) qData.pnt[qData.nPoints - 1].amplitude;
		if (bndB < (qData.nPoints)) aB = (fhx) qData.pnt[bndB].amplitude;

		const fhx fC = fhx (fB - fA);
		const fhx fW = fhx (x_Freq - fA);
		const fhx fM = fhx (fW / fC);

		const fhx aW = (fhx) h_avgx(aA, aB, fM);

		h_xBuff[x] = fhx (fWav * aW * x_VolCorrect);
	}	*/
/*
	BTW_LP *ImpulseFilter = new BTW_LP(smpRate / 2.0, x_rate);
	for (long x = 0; x < ext_length; x++)
	{
		ImpulseFilter->inpSplH(h_xBuff[x]);
		ImpulseFilter->RunFilter();
		h_xBuff[x] = (fhx) ImpulseFilter->GetOutPutH();
	}
	delete ImpulseFilter;
*/
/*
	fMarker = h_C_0;
	const fhx newLengthF = fhx (actualLength);
	for (int x = 0; x < actualLength; x++)
	{
		const fhx x_fhx = fhx (x);
		fMarker = fhx (x_LengthFac * x_fhx / newLengthF);
		const fhx x_fMarker = (fhx) floorl(fMarker);
		const long spotA = long (x_fMarker);
		const long spotB = spotA + 1;
		fhx v_A = h_C_0;
		if (spotA < ext_length) v_A = (fhx) h_xBuff[spotA];
		fhx v_B = h_C_0;
		if (spotB < ext_length) v_B = (fhx) h_xBuff[spotB];
		const fhx t_trav = fhx (fMarker - x_fMarker);

		const fhx targetVal = (fhx) h_avgx(v_A, v_B, t_trav);

		h_iBuff[x] = fhx (targetVal / newLengthF);
		s_iBuff[x] = fsx (h_iBuff[x]);
		l_iBuff[x] = flx (h_iBuff[x]);
	}
*/
/*	for (int cnt = 0; cnt < actualLength; cnt++)
	{
		fhx x_Marker = h_C_0;
		const int cntp = cnt + 1;
		const fhx x_FreqBase = fhx (fhx (cntp) / fhx (smpRate));
		const fhx x_WavBase = fhx (fhx (cntp) / fhx (actualLength));

		int bndB = qData.nPoints;
		while (x_FreqBase < qData.pnt[bndB-1].frequency)
		{
			bndB--;
			if (bndB == 0) break;
		}
		int bndA = bndB - 1;

		fhx fA = h_C_0;
		if (bndA >= 0) fA = (fhx) qData.pnt[bndA].frequency;
		fhx fB = fhx (smpRate);
		if (bndB < (qData.nPoints)) fB = (fhx) qData.pnt[bndB].frequency;

		fhx aA = (fhx) qData.pnt[0].amplitude;
		if (bndA >= 0) aA = (fhx) qData.pnt[bndA].amplitude;
		fhx aB = (fhx) qData.pnt[qData.nPoints - 1].amplitude;
		if (bndB < (qData.nPoints)) aB = (fhx) qData.pnt[bndB].amplitude;

		const fhx fC = fhx (fB - fA);
		const fhx fW = fhx (x_FreqBase - fA);
		const fhx fM = fhx (fW / fC);

		const fhx x_amp = (fhx) h_avgx(aA, aB, fM);

		for (int x = 0; x < cntp; x++)
		{
			const fhx x_Wav = (fhx) sinl(h_C_pi_doub * x_Marker);
			x_Marker = fhx (fhx (x) / fhx (cntp));

			h_iBuff[x] += fhx (x_Wav * x_amp * (fhx) qData.masterVol);
			s_iBuff[x] = fsx (h_iBuff[x]);
			l_iBuff[x] = flx (h_iBuff[x]);
		}
	} */

/*	fhx fMarker = h_C_0;
	for (int cnt = 0; cnt < qData.numPasses; cnt++)
	{
		const fhx x_PassPoint = fhx (cnt);
		const fhx x_LengthFac = fhx (actualLength);
		const fhx x_NumPasses = fhx (qData.numPasses);
		const fhx x_Curve = fhx (x_NumPasses + (x_LengthFac * h_C_hundredth));
		const fhx x_rate = fhx (smpRate);
		// fMarker = h_C_0;

		for (int x = 0; x < actualLength; x++)
		{
			const fhx x_FreqPoint = fhx (x);
			const fhx x_FreqRatio = fhx ( (x_FreqPoint / (x_LengthFac*x_NumPasses)) + (x_PassPoint/x_NumPasses) );
			const fhx x_FreqBase = fhx (powl(h_inv(x_FreqRatio), x_Curve) / lngFac);
			// const fhx xFreqBase = fhx (h_inv(xFreqRatio) * h_inv(xFreqRatio) * h_inv(xFreqRatio));
			// const fhx xFreqBase = (fhx) expl(-h_C_pi_doub * h_inv(xFreqRatio));
			const fhx x_Freq = fhx (x_rate * x_FreqBase);

			// if (fMarker >= h_C_1) break;
			const fhx fWav = (fhx) sinl(h_C_pi_doub * fMarker);
			fMarker += fhx (x_FreqBase);

			int bndB = qData.nPoints;
			while (x_Freq < qData.pnt[bndB-1].frequency)
			{
				bndB--;
				if (bndB == 0) break;
			}
			int bndA = bndB - 1;

			fhx fA = h_C_0;
			if (bndA >= 0) fA = (fhx) qData.pnt[bndA].frequency;
			fhx fB = fhx (x_rate);
			if (bndB < (qData.nPoints)) fB = (fhx) qData.pnt[bndB].frequency;

			fhx aA = (fhx) qData.pnt[0].amplitude;
			if (bndA >= 0) aA = (fhx) qData.pnt[bndA].amplitude;
			fhx aB = (fhx) qData.pnt[qData.nPoints - 1].amplitude;
			if (bndB < (qData.nPoints)) aB = (fhx) qData.pnt[bndB].amplitude;

			const fhx fC = fhx (fB - fA);
			const fhx fW = fhx (x_Freq - fA);
			const fhx fM = fhx (fW / fC);

			const fhx aW = (fhx) h_avgx(aA, aB, fM);

			if (cnt == 0) h_iBuff[x] = fhx (fWav * aW);
			else h_iBuff[x] += fhx (fWav * aW);
			s_iBuff[x] = fsx (h_iBuff[x]);
			l_iBuff[x] = flx (h_iBuff[x]);
		}	*/
/*		if (cnt > 0)
		{
			for (int x = 0; x < actualLength; x++)
			{
				fhx x_mix = h_C_0;
				int cntI = x;
				int cntX = 0;
				const fhx t_div = fhx (actualLength);
				for (int y = 0; y < actualLength; y++)
				{
					x_mix += fhx (h_iBuff[cntI] * h_xBuffA[cntX]);
					cntX++;
					cntI++;
					if (cntI >= actualLength) cntI = 0;
				}
				h_xBuffB[x] = fhx (x_mix);
			}
			for (int x = 0; x < actualLength; x++)
			{
				h_iBuff[x] = fhx (h_xBuffB[x]);
				s_iBuff[x] = fsx (h_iBuff[x]);
				l_iBuff[x] = flx (h_iBuff[x]);
			}
		}
	} */

	h_volAdj = fhx (qData.masterVol / lngFac);
	s_volAdj = fsx (h_volAdj);
	l_volAdj = flx (h_volAdj);

	for (int x = 0; x < actualLength; x++)
	{
		h_sBuff[x] = h_C_0;
		s_sBuff[x] = s_C_0;
		l_sBuff[x] = l_C_0;
	}

	delete [] h_xBuff;
//	delete [] h_yBuff;
//	delete [] h_zBuff;
	delete [] h_syncBuff;

	impulseReady = true;
}
void ImpulseQ::l_RunQ()
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
void ImpulseQ::s_RunQ()
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
void ImpulseQ::h_RunQ()
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
