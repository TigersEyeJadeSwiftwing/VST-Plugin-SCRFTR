#ifndef __SC_ToneStack_Wavs_IMPs_E__
#define __SC_ToneStack_Wavs_IMPs_E__

#include "../custdata_types.h"

using namespace ScMath;

static const fhx wConvFac = fhx (1000000000000000000.0);
static const fhx wConvFacI = fhx ((fhx) h_C_1 / fhx (wConvFac));

// #include "wavs_amp/wav_head_g.h"
#include "wavs_amp/wav_head_c120_aa.h"

#include "wavs_amp/wav_head_c120_a.h"
#include "wavs_amp/wav_head_c120_b.h"
#include "wavs_amp/wav_head_c120_c.h"
#include "wavs_amp/wav_head_c120_d.h"
#include "wavs_amp/wav_head_c120_e.h"
#include "wavs_amp/wav_head_c120_f.h"
#include "wavs_amp/wav_head_c120_g.h"

#include "wavs_amp/wav_cab_c120_aa.h"

#include "wavs_amp/wav_cab_c.h"
#include "wavs_amp/wav_cab_c120_b.h"
#include "wavs_amp/wav_cab_c120_c.h"
#include "wavs_amp/wav_cab_c120_d.h"
#include "wavs_amp/wav_cab_c120_e.h"
#include "wavs_amp/wav_cab_c120_f.h"

static const fhx iVolumesAmp[] = {
0.90,
0.90,
0.90,
0.90,
0.90,
0.90};

static const fhx iVolumesCab[] = {
0.90,
0.90,
0.90,
0.90,
0.90,
0.90};

#endif
