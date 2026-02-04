#ifndef __SC_Impulse_Wavs_A__
#define __SC_Impulse_Wavs_A__

#include "../custdata_types.h"

using namespace ScMath;

#ifndef __SCrft_Impulse_Conv_Factors__
#define __SCrft_Impulse_Conv_Factors__
static const fhx wConvFac = fhx (1000000000000000000.0);
static const fhx wConvFacI = fhx ((fhx) h_C_1 / fhx (wConvFac));
#endif

#include "wavs_data/wav_cab_c120_aa.h"

#include "wavs_data/wav_cab_c.h"
#include "wavs_data/wav_cab_c120_b.h"
#include "wavs_data/wav_cab_c120_c.h"
#include "wavs_data/wav_cab_c120_d.h"
#include "wavs_data/wav_cab_c120_e.h"
#include "wavs_data/wav_cab_c120_f.h"

static const fsx iVolumesCab[] = {
0.9750,
0.9750,
0.9750,
0.9750,
0.9750,
0.9750};

#endif
