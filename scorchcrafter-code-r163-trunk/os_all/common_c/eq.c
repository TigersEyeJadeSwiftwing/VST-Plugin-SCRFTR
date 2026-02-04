#include "eq.h"
#include "custom_types.h"

#ifdef __cplusplus
using namespace std;
#endif

void SCr_Eq_ClearBuffers (SCr_Eq_t filt)
{
	filt.sample = 0.0;

	int cnt;
	for (cnt = 0; cnt < SCr_EQ_MaxNumPasses; cnt++) {
		filt.high_buf[cnt] = 0.0;
		filt.low_buf[cnt] = 0.0;
	}
}
void SCr_Eq_Init (SCr_Eq_t filt, const f64 bandLow, const f64 bandHigh, const f64 sRate, const int nPasses)
{
	filt.rate = sRate;
	filt.numPasses = min(nPasses, SCr_EQ_MaxNumPasses);
	filt.freqHigh = bandHigh;
	filt.freqLow = bandLow;

	f64 calc;

	calc = exp(-2.0 * SCr_PI * filt.freqHigh / filt.rate);
	filt.high_x = 1.0 - calc;
	filt.high_y = -calc;

	calc = exp(-2.0 * SCr_PI * filt.freqLow / filt.rate);
	filt.low_x = 1.0 - calc;
	filt.low_y = -calc;

	SCr_Eq_ClearBuffers(filt);
}
void SCr_Eq_ChangeRate (SCr_Eq_t filt, const f64 sRate)
{
	filt.rate = sRate;

	f64 calc;

	calc = exp(-2.0 * SCr_PI * filt.freqHigh / filt.rate);
	filt.high_x = 1.0 - calc;
	filt.high_y = -calc;

	calc = exp(-2.0 * SCr_PI * filt.freqLow / filt.rate);
	filt.low_x = 1.0 - calc;
	filt.low_y = -calc;

	SCr_Eq_ClearBuffers(filt);
}
void SCr_Eq_RunLowPass (SCr_Eq_t filt)
{
	int cnt;
	for (cnt = 0; cnt < filt.numPasses; cnt++)
		filt.sample = (filt.high_buf[cnt] = filt.high_x*filt.sample - filt.high_y*filt.high_buf[cnt] + SCr_DENORM);
}
void SCr_Eq_RunHighPass (SCr_Eq_t filt)
{
	int cnt;
	for (cnt = 0; cnt < filt.numPasses; cnt++)
		filt.sample -= (filt.low_buf[cnt] = filt.low_x*filt.sample - filt.low_y*filt.low_buf[cnt] + SCr_DENORM);
}
void SCr_Eq_RunBothPass (SCr_Eq_t filt)
{
	int cnt;
	for (cnt = 0; cnt < filt.numPasses; cnt++) {
		filt.sample -= (filt.low_buf[cnt] = filt.low_x*filt.sample - filt.low_y*filt.low_buf[cnt] + SCr_DENORM);
		filt.sample = (filt.high_buf[cnt] = filt.high_x*filt.sample - filt.high_y*filt.high_buf[cnt] + SCr_DENORM);
	}
}
