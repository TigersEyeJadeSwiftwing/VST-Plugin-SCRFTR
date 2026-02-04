#ifndef __SCr_Tones_C120__
#define __SCr_Tones_C120__

#include "custdata_types.h"
#include "fir_eq.h"

using namespace ScMath;

/*!
static const Impulse_EQ_Input Example[] =
{
	{
		1.20,					<-- Master output volume of generated FIR impulse, in decimal format. (fsx)
		4,						<-- Number of EQ frequency bands (Number of entries in the table below) (int)
		65,						<-- The BASE (starting) length of the generated impulse (when running at 44,100 Hz, higher rates mean longer length) (int)
		SCr_FIRQ_Decimal,		<-- The format for the Amplitude values in the table (below).  Can be "SCr_FIRQ_Decimal" for decimal values with
										(fsx) 1.0 equal to zero dB and 0.0 being slient, or "SCr_FIRQ_dB" for (fsx) values read directly as dB (may be positive or negative).
		{
			{ Frequency 1 low end,	Frequency 1 high end,		Amplitude 1 },	<--	Table of frequency points,
			{ Frequency 2 low end,	Frequency 2 high end,		Amplitude 2 },			each point in the table is a 64-bit (fsx) entry in Hz.
			{ Frequency 3 low end,	Frequency 3 high end,		Amplitude 3 },
			{ Frequency 4 low end,	Frequency 4 high end,		Amplitude 4 },
			(More entries would continue here, if more than four bands were in the table...)
		}
	},
	(Next entry goes here, for next FIR impulse...)
}
*/

//!	Basic Input when no special tone stack is enabled, two tables for "Standard" and "Bright" tones.
static const Impulse_EQ_Input Raw_Input_EQ[] =
{
	{
		0.500,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	195.0,		7200.0,		0.2500	},
			{	635.0,		3600.0,		0.5000	},
			{	195.0,		3600.0,		1.0000	},
		}
	},
	{
		0.750,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	195.0,		7200.0,		0.4000	},
			{	635.0,		3600.0,		1.2500	},
			{	195.0,		3600.0,		0.8500	},
		}
	}
};

//!	Values for the six Tone Stacks (A to F) with three tables: Regular input, Bright input, and Post-Overdrive output.
static const Impulse_EQ_Input FIRtones_A[] =
{
	{
		1.250,
		5,
		75,
		SCr_FIRQ_Decimal,
		{
			{	420.0,		7200.0,		0.2500	},
			{	725.0,		3600.0,		0.5000	},
			{	420.0,		3600.0,		1.0000	},
			{	420.0,		925.0,		0.5000	},
			{	420.0,		635.0,		0.5000	},
		}
	},
	{
		1.500,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	430.0,		7200.0,		0.4000	},
			{	725.0,		3600.0,		1.2500	},
			{	430.0,		3600.0,		0.8500	},
		}
	},
	{
		1.0,
		6,
		75,
		SCr_FIRQ_Decimal,
		{
			{	35.0,		200.0,		1.5000	},
			{	200.0,		450.0,		1.7500	},
			{	450.0,		700.0,		1.0000	},
			{	700.0,		950.0,		0.7500	},
			{	950.0,		7200.0,		1.4500	},
			{	7200.0,		22000.0,	0.7500	},
		}
	}
};
static const Impulse_EQ_Input FIRtones_B[] =
{
	{
		1.250,
		5,
		75,
		SCr_FIRQ_Decimal,
		{
			{	420.0,		7200.0,		0.2500	},
			{	725.0,		3600.0,		0.5000	},
			{	420.0,		3600.0,		1.0000	},
			{	420.0,		925.0,		0.5000	},
			{	420.0,		635.0,		0.5000	},
		}
	},
	{
		1.500,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	430.0,		7200.0,		0.4000	},
			{	725.0,		3600.0,		1.2500	},
			{	430.0,		3600.0,		0.8500	},
		}
	},
	{
		1.0,
		5,
		75,
		SCr_FIRQ_Decimal,
		{
			{	35.0,		200.0,		1.8000	},
			{	200.0,		700.0,		1.4000	},
			{	700.0,		950.0,		0.7500	},
			{	950.0,		7200.0,		1.4500	},
			{	7200.0,		22000.0,	0.7500	},
		}
	}
};
static const Impulse_EQ_Input FIRtones_C[] =
{
	{
		1.250,
		5,
		75,
		SCr_FIRQ_Decimal,
		{
			{	420.0,		7200.0,		0.2500	},
			{	725.0,		3600.0,		0.5000	},
			{	420.0,		3600.0,		1.0000	},
			{	420.0,		925.0,		0.5000	},
			{	420.0,		635.0,		0.5000	},
		}
	},
	{
		1.500,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	430.0,		7200.0,		0.4000	},
			{	725.0,		3600.0,		1.2500	},
			{	430.0,		3600.0,		0.8500	},
		}
	},
	{
		1.0,
		5,
		75,
		SCr_FIRQ_Decimal,
		{
			{	35.0,		200.0,		1.5000	},
			{	200.0,		700.0,		1.0000	},
			{	700.0,		950.0,		0.7500	},
			{	950.0,		7200.0,		1.1250	},
			{	7200.0,		22000.0,	0.7500	},
		}
	}
};
static const Impulse_EQ_Input FIRtones_D[] =
{
	{
		0.500,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	195.0,		7200.0,		0.2500	},
			{	635.0,		3600.0,		0.5000	},
			{	195.0,		3600.0,		1.0000	},
		}
	},
	{
		0.750,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	195.0,		7200.0,		0.4000	},
			{	635.0,		3600.0,		1.2500	},
			{	195.0,		3600.0,		0.8500	},
		}
	},
	{
		1.0,
		6,
		75,
		SCr_FIRQ_Decimal,
		{
			{	35.0,		200.0,		1.5000	},
			{	200.0,		450.0,		1.7500	},
			{	450.0,		700.0,		1.0000	},
			{	700.0,		950.0,		0.7500	},
			{	950.0,		7200.0,		1.4500	},
			{	7200.0,		22000.0,	0.7500	},
		}
	}
};
static const Impulse_EQ_Input FIRtones_E[] =
{
	{
		0.500,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	195.0,		7200.0,		0.2500	},
			{	635.0,		3600.0,		0.5000	},
			{	195.0,		3600.0,		1.0000	},
		}
	},
	{
		0.750,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	195.0,		7200.0,		0.4000	},
			{	635.0,		3600.0,		1.2500	},
			{	195.0,		3600.0,		0.8500	},
		}
	},
	{
		1.0,
		5,
		75,
		SCr_FIRQ_Decimal,
		{
			{	35.0,		200.0,		1.8000	},
			{	200.0,		700.0,		1.4000	},
			{	700.0,		950.0,		0.7500	},
			{	950.0,		7200.0,		1.4500	},
			{	7200.0,		22000.0,	0.7500	},
		}
	}
};
static const Impulse_EQ_Input FIRtones_F[] =
{
	{
		0.500,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	195.0,		7200.0,		0.2500	},
			{	635.0,		3600.0,		0.5000	},
			{	195.0,		3600.0,		1.0000	},
		}
	},
	{
		0.750,
		3,
		75,
		SCr_FIRQ_Decimal,
		{
			{	195.0,		7200.0,		0.4000	},
			{	635.0,		3600.0,		1.2500	},
			{	195.0,		3600.0,		0.8500	},
		}
	},
	{
		1.0,
		5,
		75,
		SCr_FIRQ_Decimal,
		{
			{	35.0,		200.0,		1.5000	},
			{	200.0,		700.0,		1.0000	},
			{	700.0,		950.0,		0.7500	},
			{	950.0,		7200.0,		1.1250	},
			{	7200.0,		22000.0,	0.7500	},
		}
	}
};

//!	Old Settings (not used)
static const Impulse_EQ_Input ToneSettings[] =
{
	{
		1.0,
		5,
		60,
		SCr_FIRQ_Decimal,
		{
			{	20.0,		200.0,		1.2000	},
			{	200.0,		700.0,		1.0000	},
			{	700.0,		950.0,		0.7500	},
			{	950.0,		7200.0,		1.1250	},
			{	7200.0,		22000.0,	0.9000	},
		}
	},
	{
		1.0,
		5,
		60,
		SCr_FIRQ_Decimal,
		{
			{	20.0,		200.0,		1.5000	},
			{	200.0,		700.0,		1.4000	},
			{	700.0,		950.0,		0.7500	},
			{	950.0,		7200.0,		1.2500	},
			{	7200.0,		22000.0,	0.8000	},
		}
	},
	{
		1.0,
		6,
		60,
		SCr_FIRQ_Decimal,
		{
			{	20.0,		200.0,		1.2000	},
			{	200.0,		450.0,		1.5000	},
			{	450.0,		700.0,		1.0000	},
			{	700.0,		950.0,		0.8500	},
			{	950.0,		7200.0,		1.2500	},
			{	7200.0,		22000.0,	0.8500	},
		}
	},
	{
		0.50,
		5,
		60,
		SCr_FIRQ_Decimal,
		{
			{	20.0,		200.0,		3.2000	},
			{	200.0,		1200.0,		1.0000	},
			{	1200.0,		4200.0,		3.0000	},
			{	4200.0,		7200.0,		1.0000	},
			{	7200.0,		22000.0,	0.2500	},
		}
	},
	{
		0.50,
		5,
		60,
		SCr_FIRQ_Decimal,
		{
			{	20.0,		200.0,		3.2000	},
			{	200.0,		1200.0,		1.0000	},
			{	1200.0,		4200.0,		3.0000	},
			{	4200.0,		7200.0,		1.0000	},
			{	7200.0,		22000.0,	0.2500	},
		}
	},
	{
		0.50,
		5,
		60,
		SCr_FIRQ_Decimal,
		{
			{	20.0,		200.0,		3.2000	},
			{	200.0,		1200.0,		1.0000	},
			{	1200.0,		4200.0,		3.0000	},
			{	4200.0,		7200.0,		1.0000	},
			{	7200.0,		22000.0,	0.2500	},
		}
	},
};

#endif
