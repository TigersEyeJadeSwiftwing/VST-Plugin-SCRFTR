#include "tube_alpha.h"
#include "../custom_types.h"

#ifdef __cplusplus
using namespace std;
#endif

void SCr_Tube_A_Init (SCr_Tube_Alpha_t tb, const f64 powerLevel, const f64 sRate, const f64 hard)
{
	tb.gain = 1.0;
	tb.power = powerLevel;
	tb.travelA = 0.0;
	tb.travelB = 0.0;
	tb.travelBuffer = 0.0;
	tb.hardness = hard;
	tb.rate = sRate;

	tb.dynLow = exp(-SCr_PI_DOUB * SCr_Tube_A_DynFreq_L / tb.rate);
	tb.dynHigh = exp(-SCr_PI_DOUB * SCr_Tube_A_DynFreq_H / tb.rate);
}
void SCr_Tube_A_SetGain (SCr_Tube_Alpha_t tb, const f64 inp)
{
	tb.gain = (inp * tb.power) + 1.0;
}
void SCr_Tube_A_ZeroBuffers (SCr_Tube_Alpha_t tb)
{
	tb.travelA = 0.0;
	tb.travelB = 0.0;
	tb.travelBuffer = 0.0;
}
void SCr_Tube_A_SetRate (SCr_Tube_Alpha_t tb, const f64 nRate)
{
	tb.dynLow = exp(-SCr_PI_DOUB * SCr_Tube_A_DynFreq_L / tb.rate);
	tb.dynHigh = exp(-SCr_PI_DOUB * SCr_Tube_A_DynFreq_H / tb.rate);

	tb.travelA = 0.0;
	tb.travelB = 0.0;
	tb.travelBuffer = 0.0;
}
f64 SCr_Tube_A_Run (SCr_Tube_Alpha_t tb, const f64 inp)
{
	const f64 t_t = SCmath_Diode_Fixed(inp * tb.gain, tb.hardness);
	const f64 t_x = ( t_t >= 0.0 ? SCmath_Averagex(tb.dynLow, tb.dynHigh, t_t) :
					SCmath_Averagex(tb.dynLow, tb.dynHigh, -t_t) );

	if (inp >= 0.0) {
		tb.travelA = ( tb.travelA > t_x ? tb.travelA : t_x );
		tb.travelB = 0.0;
		tb.px = 1.0 - tb.travelA;
		tb.py = -tb.travelA;
	} else {
		tb.travelB = ( tb.travelB > t_x ? tb.travelB : t_x );
		tb.travelA = 0.0;
		tb.px = 1.0 - tb.travelB;
		tb.py = -tb.travelB;
	}

	tb.travelBuffer = tb.px*t_t + tb.py*tb.travelBuffer + SCr_DENORM;

	const f64 t_output = tb.travelBuffer * SCr_PI;

	return t_output;
}
