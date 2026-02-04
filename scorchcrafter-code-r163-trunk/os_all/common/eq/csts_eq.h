#ifndef __sc_shared_eq_custom_S__
#define __sc_shared_eq_custom_S__

class RateFC;

#include "custdata_types.h"

class Cst_EQ
{
public:
	Cst_EQ(bool setSingle = false);
	~Cst_EQ() { delete rateV; };

	void l_InputSmpl(const flx inp);
	void s_InputSmpl(const fsx inp);
	void h_InputSmpl(const fhx inp);
	void SplInL(const flx inp) { l_InputSmpl(inp); };
	void SplInS(const fsx inp) { s_InputSmpl(inp); };
	void SplInH(const fhx inp) { h_InputSmpl(inp); };

	void SetBands(fsx lo, fsx hi);
	void SetSmpRate(const fsx nRate);
	void ClearBuffers();

	void RunQL();
	void RunQS();
	void RunQH();
	void RunLPL();
	void RunLPS();
	void RunLPH();
	void RunHPL();
	void RunHPS();
	void RunHPH();

	void l_Run(const bool lowP = true, const bool highP = false);
	void s_Run(const bool lowP = true, const bool highP = false);
	void h_Run(const bool lowP = true, const bool highP = false);

	void l_ForceBufs(const flx inp);
	void s_ForceBufs(const fsx inp);
	void h_ForceBufs(const fhx inp);

	void SetRunSingle(const bool inp);

	flx l_GetOutput();
	fsx s_GetOutput();
	fhx h_GetOutput();
	flx GetOutputL() { return l_GetOutput(); };
	fsx GetOutputS() { return s_GetOutput(); };
	fhx GetOutputH() { return h_GetOutput(); };

private:
	RateFC *rateV;
	bool runSingle;
	fsx freqHigh;
	fsx freqLow;

	flx sampL;
	fsx sampS;
	fhx sampH;

	flx sRateL;
	fsx sRateS;
	fhx sRateH;

	flx qPaL[2][3];
	fsx qPaS[2][3];
	fhx qPaH[2][3];

	flx pValLowL;
	flx pValHiL;
	fsx pValLowS;
	fsx pValHiS;
	fhx pValLowH;
	fhx pValHiH;

	flx bValLowL;
	flx bValHiL;
	fsx bValLowS;
	fsx bValHiS;
	fhx bValLowH;
	fhx bValHiH;
};

#endif
