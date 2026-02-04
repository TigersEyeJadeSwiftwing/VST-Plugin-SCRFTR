#ifndef __SC_ToneStack_Wavs_IMPs_I__
#define __SC_ToneStack_Wavs_IMPs_I__

#include "../custdata_types.h"

using namespace ScMath;

static const fhx wConvFac = fhx (1000000000000000000.0);
static const fhx wConvFacI = fhx ((fhx) h_C_1 / fhx (wConvFac));

#include "wavs_amp/wav_cab_c120_aa.h"

#include "wavs_amp/wav_cab_c.h"
#include "wavs_amp/wav_cab_c120_b.h"
#include "wavs_amp/wav_cab_c120_c.h"
#include "wavs_amp/wav_cab_c120_d.h"
#include "wavs_amp/wav_cab_c120_e.h"
#include "wavs_amp/wav_cab_c120_f.h"

static const fhx iVolumesAmp[] = {
3.950,
3.950,
3.950,
3.950,
3.950,
3.950};

static const fhx iVolumesCab[] = {
0.950,
0.950,
0.950,
0.950,
0.950,
0.950};

#define SCr_TStack_I_Max_Head_Elements		16

struct SCr_TnStack_Element_Def {
	fsx low;
	fsx high;
	fsx vol;
};

struct SCr_TnStack_Head_Def {
	int nElements;
	SCr_TnStack_Element_Def Elements[SCr_TStack_I_Max_Head_Elements];
};

static const SCr_TnStack_Head_Def TnStack_I[] =
{
	{
		5, { 	{26.0,		120.0,		1.00000},
				{200.0,		500.0,		-0.04000},
				{600.0,		3000.0,		0.12500},
				{3000.0,	7200.0,		-0.50000},
				{7200.0,	16000.0,	0.05000} }
	},
	{
		11, { 	{26.0,		120.0,		1.20000},
				{120.0,		275.0,		-0.40000},
				{275.0,		425.0,		0.01000},
				{425.0,		695.0,		-0.02000},
				{695.0,		920.0,		0.07500},
				{920.0,		5000.0,		-0.25000},
				{5000.0,	7200.0,		0.50000},
				{7200.0,	12000.0,	-0.15000},
				{12000.0,	16000.0,	0.05000},
				{16000.0,	18000.0,	-0.12000},
				{18000.0,	22000.0,	0.25000} }
	},
	{
		5, { {26.0, 120.0, 1.00000}, {200.0, 500.0, -0.12500}, {600.0, 3000.0, 0.25000}, {3000.0, 7200.0, -0.50000}, {7200.0, 16000.0, 0.00000} }
	},
	{
		5, { {26.0, 120.0, 1.00000}, {200.0, 500.0, -0.12500}, {600.0, 3000.0, 0.25000}, {3000.0, 7200.0, -0.50000}, {7200.0, 16000.0, 0.00000} }
	},
	{
		5, { {26.0, 120.0, 1.00000}, {200.0, 500.0, -0.12500}, {600.0, 3000.0, 0.25000}, {3000.0, 7200.0, -0.50000}, {7200.0, 16000.0, 0.00000} }
	},
	{
		5, { {26.0, 120.0, 1.00000}, {200.0, 500.0, -0.05000}, {600.0, 3000.0, 0.10000}, {3000.0, 7200.0, -0.50000}, {7200.0, 16000.0, 0.05000} }
	}
};

#endif
