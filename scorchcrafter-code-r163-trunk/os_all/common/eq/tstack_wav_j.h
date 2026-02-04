#ifndef __SC_ToneStack_Wavs_IMPs_J__
#define __SC_ToneStack_Wavs_IMPs_J__

#include "../custdata_types.h"

using namespace ScMath;

static const fsx iVolumesInput[] = {
2.00,
2.00};

static const fsx iVolumesAmp[] = {
16.00,
16.00,
16.00,
16.00,
16.00,
16.00};

#define SCr_TStack_J_Max_Head_Elements		6

static const int TStack_Max_Elements = SCr_TStack_J_Max_Head_Elements;

struct SCr_TnStack_Element_Def {
	fsx low;
	fsx high;
	fsx vol;
};

struct SCr_TnStack_Head_Def {
	int nElements;
	SCr_TnStack_Element_Def Elements[TStack_Max_Elements];
};

static const SCr_TnStack_Head_Def TnStack_In[] =
{
	{
		1, { 	{300.0,		7200.0,		1.00000},
				{70.0,		7200.0,		0.12500},
				{90.0,		210.0,		0.40000},
				{420.0,		3600.0,		3.00000},
				{3600.0,	7200.0,		0.12500}}
	},
	{
		1, { 	{430.0,		7200.0,		1.00000},
				{70.0,		7200.0,		0.12500},
				{90.0,		210.0,		0.05000},
				{695.0,		3600.0,		3.00000},
				{3600.0,	7200.0,		0.12500}}
	}
};

static const SCr_TnStack_Head_Def TnStack_Out[] =
{
	{
		4, { 	{20.0,		120.0,		1.000000},
				{250.0,		400.0,		0.250000},
				{2000.0,	12500.0,	0.125000},
				{7200.0,	12500.0,	0.353553} }
	},
	{
		5, { 	{20.0,		120.0,		2.00000},
				{200.0,		900.0,		0.50000},
				{1800.0,	6000.0,		1.00000},
				{2000.0,	3000.0,		-0.50000},
				{6000.0,	20000.0,	-0.25000} }
	},
	{
		4, { 	{20.0,		160.0,		2.00000},
				{200.0,		1200.0,		0.50000},
				{1500.0,	6000.0,		1.00000},
				{6000.0,	20000.0,	-0.25000} }
	},
	{
		4, { 	{20.0,		300.0,		2.00000},
				{300.0,		1000.0,		0.50000},
				{1000.0,	6500.0,		1.00000},
				{6500.0,	20000.0,	-0.25000} }
	},
	{
		4, { 	{20.0,		180.0,		2.00000},
				{180.0,		1200.0,		0.50000},
				{1800.0,	6400.0,		1.00000},
				{6400.0,	20000.0,	-0.25000} }
	},
	{
		4, { 	{20.0,		120.0,		2.00000},
				{200.0,		900.0,		0.50000},
				{1800.0,	6400.0,		1.00000},
				{12000.0,	20000.0,	0.25000} }
	}
};

#endif
