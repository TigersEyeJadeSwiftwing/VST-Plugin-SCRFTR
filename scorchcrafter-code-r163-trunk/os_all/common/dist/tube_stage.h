#ifndef __dist_tube_cmp_tube_st__
#define __dist_tube_cmp_tube_st__

#include "../custdata_types.h"

using namespace ScMath;

class SC_Tube_Stage
{
public:
	flx l_in;
	flx l_out;
	flx l_spValA;
	flx l_spValB;
	flx l_auxValA;
	flx l_auxValB;
	const flx l_hard;
	const flx l_slope;
	const flx l_clip;
	flx l_freq;
	flx l_fq_X;
	flx l_fq_Y;
	flx l_buff_A;
	flx l_buff_B;
	const flx l_denorm;

	fsx s_in;
	fsx s_out;
	fsx s_spValA;
	fsx s_spValB;
	fsx s_auxValA;
	fsx s_auxValB;
	const fsx s_hard;
	const fsx s_slope;
	const flx s_clip;
	fsx s_freq;
	fsx s_fq_X;
	fsx s_fq_Y;
	fsx s_buff_A;
	fsx s_buff_B;
	const fsx s_denorm;

	fhx h_in;
	fhx h_out;
	fhx h_spValA;
	fhx h_spValB;
	fhx h_auxValA;
	fhx h_auxValB;
	const fhx h_hard;
	const fhx h_slope;
	const flx h_clip;
	fhx h_freq;
	fhx h_fq_X;
	fhx h_fq_Y;
	fhx h_buff_A;
	fhx h_buff_B;
	const fhx h_denorm;

	fsx smRate;
	fsx dFreq;

	SC_Tube_Stage(const fsx hard, const fsx slope, const fsx freq = 90.0, const fsx spRate = 44100.0, const fsx clip = 0.80) :
	h_hard(fhx (hard)),
	h_slope(fhx (slope)),
	s_hard(fsx (hard)),
	s_slope(fsx (slope)),
	l_hard(flx (hard)),
	l_slope(flx (slope)),
	l_clip(flx (clip)),
	s_clip(fsx (clip)),
	h_clip(fhx (clip)),
	l_denorm(flx (powf(10.0, -28.0))),
	s_denorm(fsx (pow(10.0, -32.0))),
	h_denorm(fhx (powl(10.0, -36.0))),
	smRate(fsx (spRate)),
	dFreq(fsx (freq))
	{
		SetFreq(freq);
	};
	void Clear()
	{
		l_spValA = flx (0.f);
		s_spValA = fsx (0.0);
		h_spValA = fhx (0.0);
		l_spValB = flx (0.f);
		s_spValB = fsx (0.0);
		h_spValB = fhx (0.0);
		l_out = flx (0.f);
		s_out = fsx (0.0);
		h_out = fhx (0.0);
		l_buff_A = flx (0.f);
		s_buff_A = fsx (0.0);
		h_buff_A = fhx (0.0);
		l_buff_B = flx (0.f);
		s_buff_B = fsx (0.0);
		h_buff_B = fhx (0.0);
		l_auxValA = l_auxValB = 0.f;
		s_auxValA = s_auxValB = 0.0;
		h_auxValA = h_auxValB = 0.0;
	};
	void SetRate(const fsx nRate)
	{
		smRate = nRate;
		SetFreq(dFreq);
	}
	void SetFreq(const fsx nFreq)
	{
		dFreq = nFreq;
		h_freq = (fhx) expl(fhx (-2.0) * fhx (M_PI) * fhx (nFreq) / fhx (smRate));
		s_freq = fsx (h_freq);
		l_freq = flx (h_freq);
		l_fq_Y = flx (s_fq_Y = fsx (h_fq_Y = fhx (1.0 - h_freq)));
		l_fq_X = flx (s_fq_X = fsx (h_fq_X = fhx (-h_freq)));
		Clear();
	}
//! - - - - - - - - - - - - - - - - -
	void l_Run(const flx feedBack = 0.0f)
	{
		l_spValA = l_in;
		l_spValB = -l_in;
		if (l_spValA < 0.0f) l_spValA = 0.0f;
		if (l_spValB < 0.0f) l_spValB = 0.0f;
		l_spValA += 2.0f;
		l_spValB += 2.0f;
		l_spValA = flx (l_buff_A = flx (l_fq_Y * l_spValA - l_fq_X * l_buff_A + l_denorm));
		l_spValB = flx (l_buff_B = flx (l_fq_Y * l_spValB - l_fq_X * l_buff_B + l_denorm));

		if (l_in >= 0.0f) l_in *= l_spValA;
		else l_in *= l_spValB;
		l_in *= 0.50f;

		if (l_in >= 0.0f) l_out = flx (l_in * (l_hard+l_in) / (1.0f + (l_hard+l_slope) * l_in + (l_in*l_in)));
		else l_out = flx (l_in * (l_hard-l_in) / (1.0f + (l_hard+l_slope) * (-l_in) + (l_in*l_in)));
	};
	void s_Run(const fsx feedBack = 0.0)
	{
		s_spValA = s_in;
		s_spValB = -s_in;
		if (s_spValA < 0.0) s_spValA = 0.0;
		if (s_spValB < 0.0) s_spValB = 0.0;
		s_spValA += 2.0;
		s_spValB += 2.0;
		s_spValA = fsx (s_buff_A = fsx (s_fq_Y * s_spValA - s_fq_X * s_buff_A + s_denorm));
		s_spValB = fsx (s_buff_B = fsx (s_fq_Y * s_spValB - s_fq_X * s_buff_B + s_denorm));

		if (s_in >= 0.0) s_in *= s_spValA;
		else s_in *= s_spValB;
		s_in *= 0.50;

		if (s_in >= 0.0) s_out = fsx (s_in * (s_hard+s_in) / (1.0 + (s_hard+s_slope) * s_in + (s_in*s_in)));
		else s_out = fsx (s_in * (s_hard-s_in) / (1.0 + (s_hard+s_slope) * (-s_in) + (s_in*s_in)));
	};
	void h_Run(const fhx feedBack = 0.0)
	{
		h_spValA = h_in;
		h_spValB = -h_in;
		if (h_spValA < 0.0) h_spValA = 0.0;
		if (h_spValB < 0.0) h_spValB = 0.0;
		h_spValA += 2.0;
		h_spValB += 2.0;
		h_spValA = fhx (h_buff_A = fhx (h_fq_Y * h_spValA - h_fq_X * h_buff_A + h_denorm));
		h_spValB = fhx (h_buff_B = fhx (h_fq_Y * h_spValB - h_fq_X * h_buff_B + h_denorm));

		if (h_in >= 0.0) h_in *= h_spValA;
		else h_in *= h_spValB;
		h_in *= 0.50;

		if (h_in >= 0.0) h_out = fhx (h_in * (h_hard+h_in) / (1.0 + (h_hard+h_slope) * h_in + (h_in*h_in)));
		else h_out = fhx (h_in * (h_hard-h_in) / (1.0 + (h_hard+h_slope) * (-h_in) + (h_in*h_in)));
	};
//! - - - - - - - - - - - - - - - - -
	void l_RunA(const flx feedBack = 0.0f)
	{
		l_in = l_avgx(l_in, -l_out, feedBack);
		l_spValA = l_in;
		l_spValB = -l_in;
		if (l_spValA < 0.0f) l_spValA = 0.0f;
		if (l_spValB < 0.0f) l_spValB = 0.0f;
		l_spValA += 0.250f;
		l_spValB += 0.250f;
		l_spValA = flx (l_buff_A = flx (l_fq_Y * l_spValA - l_fq_X * l_buff_A + l_denorm));
		l_spValB = flx (l_buff_B = flx (l_fq_Y * l_spValB - l_fq_X * l_buff_B + l_denorm));

		if (l_in >= 0.0f) l_in *= l_spValA;
		else l_in *= l_spValB;
		l_in *= 2.0f;

		if (l_in >= 0.0f) l_out = flx (l_in * (l_hard+l_in) / (1.0f + (l_hard+l_slope) * l_in + (l_in*l_in)));
		else l_out = flx (l_in * (l_hard-l_in) / (1.0f + (l_hard+l_slope) * (-l_in) + (l_in*l_in)));
	};
	void s_RunA(const fsx feedBack = 0.0)
	{
		s_in = s_avgx(s_in, -s_out, feedBack);
		s_spValA = s_in;
		s_spValB = -s_in;
		if (s_spValA < 0.0) s_spValA = 0.0;
		if (s_spValB < 0.0) s_spValB = 0.0;
		s_spValA += 0.250;
		s_spValB += 0.250;
		s_spValA = fsx (s_buff_A = fsx (s_fq_Y * s_spValA - s_fq_X * s_buff_A + s_denorm));
		s_spValB = fsx (s_buff_B = fsx (s_fq_Y * s_spValB - s_fq_X * s_buff_B + s_denorm));

		if (s_in >= 0.0) s_in *= s_spValA;
		else s_in *= s_spValB;
		s_in *= 2.0;

		if (s_in >= 0.0) s_out += fsx (s_in * (s_hard+s_in) / (1.0 + (s_hard+s_slope) * s_in + (s_in*s_in)));
		else s_out = fsx (s_in * (s_hard-s_in) / (1.0 + (s_hard+s_slope) * (-s_in) + (s_in*s_in)));
	};
	void h_RunA(const fhx feedBack = 0.0)
	{
		h_in = h_avgx(h_in, -h_out, feedBack);
		h_spValA = h_in;
		h_spValB = -h_in;
		if (h_spValA < 0.0) h_spValA = 0.0;
		if (h_spValB < 0.0) h_spValB = 0.0;
		h_spValA += 0.250;
		h_spValB += 0.250;
		h_spValA = fhx (h_buff_A = fhx (h_fq_Y * h_spValA - h_fq_X * h_buff_A + h_denorm));
		h_spValB = fhx (h_buff_B = fhx (h_fq_Y * h_spValB - h_fq_X * h_buff_B + h_denorm));

		if (h_in >= 0.0) h_in *= h_spValA;
		else h_in *= h_spValB;
		h_in *= 2.0;

		if (h_in >= 0.0) h_out = fhx (h_in * (h_hard+h_in) / (1.0 + (h_hard+h_slope) * h_in + (h_in*h_in)));
		else h_out = fhx (h_in * (h_hard-h_in) / (1.0 + (h_hard+h_slope) * (-h_in) + (h_in*h_in)));
	};
};

#endif
