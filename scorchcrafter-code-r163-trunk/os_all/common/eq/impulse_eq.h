#ifndef __sc_impulse_multi_eq__
#define __sc_impulse_multi_eq__

#include "custdata_types.h"

using namespace ScMath;

#define SCr_ImpulseEQ_Max_Q_Points			128

struct Impulse_EQ_Element
{
	fsx frequency;
	fsx amplitude;
};

struct Impulse_EQ_Input
{
	fsx masterVol;
	int nPoints;
	int impLength;
	int numPasses;
	Impulse_EQ_Element pnt[SCr_ImpulseEQ_Max_Q_Points];
};

class ImpulseQ
{
public:
	ImpulseQ(const fsx sRate, const Impulse_EQ_Input inpPoints);
	~ImpulseQ();

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
