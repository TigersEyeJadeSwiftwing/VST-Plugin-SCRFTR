#ifndef __SCr_FIR_EQ__
#define __SCr_FIR_EQ__

#include "custdata_types.h"

using namespace ScMath;

#ifndef SCr_ImpulseEQ_Max_Q_Points
#define SCr_ImpulseEQ_Max_Q_Points			32
#endif

enum SCr_FIR_EQ_Amplitude_Format
{
	SCr_FIRQ_Decimal,
	SCr_FIRQ_dB
};

struct Impulse_EQ_Element
{
	fsx frequencyLow;
	fsx frequencyHigh;
	fsx amplitude;
};

struct Impulse_EQ_Input
{
	fsx masterVol;
	int nPoints;
	int impLength;
	SCr_FIR_EQ_Amplitude_Format volFormat;
	Impulse_EQ_Element pnt[SCr_ImpulseEQ_Max_Q_Points];
};

class FIR_LP_EQ
{
public:
	FIR_LP_EQ(fsx sRateI, fsx passFreq, int baseFIRSize = 24);
	~FIR_LP_EQ();

	void ChangeRates(const fsx nRate, const fsx passRate);
	flx l_GetOutput() { return l_spl; };
	fsx s_GetOutput() { return s_spl; };
	fhx h_GetOutput() { return h_spl; };
	void l_Input(const flx inp) { l_spl = inp; };
	void s_Input(const fsx inp) { s_spl = inp; };
	void h_Input(const fhx inp) { h_spl = inp; };
	void l_RunQ();
	void s_RunQ();
	void h_RunQ();

private:
	void BuildImpulse(const bool eraseOldBuffers = true);

	const int baseSize;
	fsx lp_freq;
	bool impulseReady;
	fsx smpRate;

	int actualLength;

	int smpPos;

	flx l_spl;
	fsx s_spl;
	fhx h_spl;

	flx l_volAdj;
	fsx s_volAdj;
	fhx h_volAdj;

	flx *l_sBuff;
	fsx *s_sBuff;
	fhx *h_sBuff;
	flx *l_iBuff;
	fsx *s_iBuff;
	fhx *h_iBuff;
};

class FIR_Band_EQ
{
public:
	FIR_Band_EQ(fsx sRateI, fsx passFreqA, fsx passFreqB, int baseFIRSize = 12);
	~FIR_Band_EQ();

	void ChangeRates(const fsx nRate, const fsx passRate);
	flx l_GetOutput() { return l_spl; };
	fsx s_GetOutput() { return s_spl; };
	fhx h_GetOutput() { return h_spl; };
	void l_Input(const flx inp) { l_spl = inp; };
	void s_Input(const fsx inp) { s_spl = inp; };
	void h_Input(const fhx inp) { h_spl = inp; };
	void l_RunQ();
	void s_RunQ();
	void h_RunQ();

private:
	void BuildImpulse(const bool eraseOldBuffers = true);

	const int baseSize;
	fsx lp_freq;
	fsx hp_freq;
	bool impulseReady;
	fsx smpRate;

	int actualLength;

	int smpPos;

	flx l_spl;
	fsx s_spl;
	fhx h_spl;

	flx l_volAdj;
	fsx s_volAdj;
	fhx h_volAdj;

	flx *l_sBuff;
	fsx *s_sBuff;
	fhx *h_sBuff;
	flx *l_iBuff;
	fsx *s_iBuff;
	fhx *h_iBuff;
};

class FIR_Multi_EQ
{
public:
	FIR_Multi_EQ(fsx sRateI, const Impulse_EQ_Input qDataInp);
	~FIR_Multi_EQ();

	void ChangeSmpRate(const fsx nRate);
	flx l_GetOutput() { return l_spl; };
	fsx s_GetOutput() { return s_spl; };
	fhx h_GetOutput() { return h_spl; };
	void l_Input(const flx inp) { l_spl = inp; };
	void s_Input(const fsx inp) { s_spl = inp; };
	void h_Input(const fhx inp) { h_spl = inp; };
	void l_RunQ();
	void s_RunQ();
	void h_RunQ();

private:
	void BuildImpulse(const bool eraseOldBuffers = true);

	const Impulse_EQ_Input qData;
	bool impulseReady;
	fsx smpRate;

	int actualLength;
	int multiLength;

	int smpPos;

	flx l_spl;
	fsx s_spl;
	fhx h_spl;

	flx l_volAdj;
	fsx s_volAdj;
	fhx h_volAdj;

	flx *l_sBuff;
	fsx *s_sBuff;
	fhx *h_sBuff;
	flx *l_iBuff;
	fsx *s_iBuff;
	fhx *h_iBuff;
};

#endif
