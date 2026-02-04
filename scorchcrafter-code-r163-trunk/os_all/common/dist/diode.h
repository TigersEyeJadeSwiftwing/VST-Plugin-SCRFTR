#ifndef __dist_tube_cmp_diode__
#define __dist_tube_cmp_diode__

#include "../custdata_types.h"
#include <cmath>

class SC_Diode
{
public:
	flx l_in;
	flx l_out;
	flx l_spValA;
	flx l_spValB;
	const flx l_hard;
	const flx l_slope;
	const flx l_clip;

	fsx s_in;
	fsx s_out;
	fsx s_spValA;
	fsx s_spValB;
	const fsx s_hard;
	const fsx s_slope;
	const flx s_clip;

	fhx h_in;
	fhx h_out;
	fhx h_spValA;
	fhx h_spValB;
	const fhx h_hard;
	const fhx h_slope;
	const flx h_clip;

	SC_Diode(const fsx hard, const fsx slope, const fsx clip = 0.50) :
		h_hard(fhx (hard)),
		h_slope(fhx (slope)),
		s_hard(fsx (hard)),
		s_slope(fsx (slope)),
		l_hard(flx (hard)),
		l_slope(flx (slope)),
		l_clip(flx (clip)),
		s_clip(fsx (clip)),
		h_clip(fhx (clip))
		{
			Clear();
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
		h_out = fsx (0.0);
	};
	void l_Run()
	{
		if (l_in >= flx (0.0f))
		{
			if (l_in > l_clip) l_in = l_clip;
			l_in *= flx (0.250f + l_spValA);
			l_spValA = flx (l_hard * (2.0f - l_spValA));
			l_out = flx (l_in * (l_spValA+l_in) / (1.0f + (l_spValA+l_slope) * l_in + (l_in*l_in)));
		} else
		{
			if (l_in < (-l_clip)) l_in = (-l_clip);
			l_in *= flx (0.250f + l_spValB);
			l_spValB = flx (l_hard * (2.0f - l_spValB));
			l_out = flx (l_in * (l_spValB-l_in) / (1.0f + (l_spValB+l_slope) * (-l_in) + (l_in*l_in)));
		}
	};
	void s_Run()
	{
		if (s_in >= fsx (0.0))
		{
			if (s_in > s_clip) s_in = s_clip;
			s_in *= fsx (0.250 + s_spValA);
			s_spValA = fsx (s_hard * (2.0 - s_spValA));
			s_out = fsx (s_in * (s_spValA+s_in) / (1.0 + (s_spValA+s_slope) * s_in + (s_in*s_in)));
		} else
		{
			if (s_in < (-s_clip)) s_in = (-s_clip);
			s_in *= fsx (0.250 + s_spValB);
			s_spValB = fsx (s_hard * (2.0 - s_spValB));
			s_out = fsx (s_in * (s_spValB-s_in) / (1.0 + (s_spValB+s_slope) * (-s_in) + (s_in*s_in)));
		}
	};
	void h_Run()
	{
		if (h_in >= fhx (0.0))
		{
			if (h_in > h_clip) h_in = h_clip;
			h_in *= fhx (0.250 + h_spValA);
			h_spValA = fhx (h_hard * (2.0 - h_spValA));
			h_out = fhx (h_in * (h_spValA+h_in) / (1.0 + (h_spValA+h_slope) * h_in + (h_in*h_in)));
		} else
		{
			if (h_in < (-h_clip)) h_in = (-h_clip);
			h_in *= fhx (0.250 + h_spValB);
			h_spValB = fhx (h_hard * (2.0 - h_spValB));
			h_out = fhx (h_in * (h_spValB-h_in) / (1.0 + (h_spValB+h_slope) * (-h_in) + (h_in*h_in)));
		}
	};
//! ---------- ---------- ----------
	void l_RunP()
	{
		if (l_in >= flx (0.0f))
		{
			if (l_in > l_clip) l_in = l_clip;
			l_in *= flx (0.750f + l_spValA + l_spValA + l_spValA);
			l_out = flx (l_in * (l_hard+l_in) / (1.0f + (l_hard+l_slope) * l_in + (l_in*l_in)));
		} else
		{
			if (l_in < (-l_clip)) l_in = (-l_clip);
			l_in *= flx (0.750f + l_spValB + l_spValB + l_spValB);
			l_out = flx (l_in * (l_hard-l_in) / (1.0f + (l_hard+l_slope) * (-l_in) + (l_in*l_in)));
		}
	};
	void s_RunP()
	{
		if (s_in >= fsx (0.0))
		{
			if (s_in > s_clip) s_in = s_clip;
			s_in *= fsx (0.750 + s_spValA + s_spValA + s_spValA);
			s_out = fsx (s_in * (s_hard+s_in) / (1.0 + (s_hard+s_slope) * s_in + (s_in*s_in)));
		} else
		{
			if (s_in < (-s_clip)) s_in = (-s_clip);
			s_in *= fsx (0.750 + s_spValB + s_spValB + s_spValB);
			s_out = fsx (s_in * (s_hard-s_in) / (1.0 + (s_hard+s_slope) * (-s_in) + (s_in*s_in)));
		}
	};
	void h_RunP()
	{
		if (h_in >= fhx (0.0))
		{
			if (h_in > h_clip) h_in = h_clip;
			h_in *= fhx (0.750 + h_spValA + h_spValA + h_spValA);
			h_out = fhx (h_in * (h_hard+h_in) / (1.0 + (h_hard+h_slope) * h_in + (h_in*h_in)));
		} else
		{
			if (h_in < (-h_clip)) h_in = (-h_clip);
			h_in *= fhx (0.750 + h_spValB + h_spValB + h_spValB);
			h_out = fhx (h_in * (h_hard-h_in) / (1.0 + (h_hard+h_slope) * (-h_in) + (h_in*h_in)));
		}
	};
//! ---------- ---------- ----------
	void l_RunX()
	{
		if (l_in >= flx (0.0f))
		{
			if (l_in > l_clip) l_in = l_clip;
			l_in *= flx (0.20f + l_spValA);
			l_spValA = flx (l_hard * (2.0f - l_spValA));
			l_out = flx (l_in * (l_spValA+l_in) / (1.0f + (l_spValA+l_slope) * l_in + (l_in*l_in)));
		} else
		{
			if (l_in < (-l_clip)) l_in = (-l_clip);
			l_in *= flx (0.20f + l_spValB);
			l_spValB = flx (l_hard * (2.0f - l_spValB));
			l_out = flx (l_in * (l_spValB-l_in) / (1.0f + (l_spValB+l_slope) * (-l_in) + (l_in*l_in)));
		}
	};
	void s_RunX()
	{
		if (s_in >= fsx (0.0))
		{
			if (s_in > s_clip) s_in = s_clip;
			s_in *= fsx (0.20 + s_spValA);
			s_spValA = fsx (s_hard * (2.0 - s_spValA));
			s_out = fsx (s_in * (s_spValA+s_in) / (1.0 + (s_spValA+s_slope) * s_in + (s_in*s_in)));
		} else
		{
			if (s_in < (-s_clip)) s_in = (-s_clip);
			s_in *= fsx (0.20 + s_spValB);
			s_spValB = fsx (s_hard * (2.0 - s_spValB));
			s_out = fsx (s_in * (s_spValB-s_in) / (1.0 + (s_spValB+s_slope) * (-s_in) + (s_in*s_in)));
		}
	};
	void h_RunX()
	{
		if (h_in >= fhx (0.0))
		{
			if (h_in > h_clip) h_in = h_clip;
			h_in *= fhx (0.20 + h_spValA);
			h_spValA = fhx (h_hard * (2.0 - h_spValA));
			h_out = fhx (h_in * (h_spValA+h_in) / (1.0 + (h_spValA+h_slope) * h_in + (h_in*h_in)));
		} else
		{
			if (h_in < (-h_clip)) h_in = (-h_clip);
			h_in *= fhx (0.20 + h_spValB);
			h_spValB = fhx (h_hard * (2.0 - h_spValB));
			h_out = fhx (h_in * (h_spValB-h_in) / (1.0 + (h_spValB+h_slope) * (-h_in) + (h_in*h_in)));
		}
	};
};

#endif
