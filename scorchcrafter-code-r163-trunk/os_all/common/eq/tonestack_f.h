#ifndef __SC_eq_tstack_f__
#define __SC_eq_tstack_f__

class RateFC;
class Cst_EQ;

#include <cmath>
#include "../eq/csts_eq.h"
#include "../custdata_types.h"

#ifdef BUILDVST_C120
#include "../plugin_c120.h"
#endif
#ifdef BUILDVST_GLZ60
#include "../plugin_glz60.h"
#endif

class ToneStack_F
{
public:
	ToneStack_F(fsx nRate = 44100.0);
	~ToneStack_F();

	void setNewSampleRate(const fsx nRate);
	void setBandValue(const int bNum, fsx value);
	void SetInputL(const flx inp) {l_spl = flx (inp);};
	void SetInputS(const fsx inp) {s_spl = fsx (inp);};
	void SetInputH(const fhx inp) {h_spl = fhx (inp);};
	flx GetProcessedOutputL() {return l_spl;};
	fsx GetProcessedOutputS() {return s_spl;};
	fhx GetProcessedOutputH() {return h_spl;};
	void ClearBuffs();
	void SetPrecision(int nLevel);
	void RunEQ();

private:
	static const fhx qBandRange_Low[2];
	static const fhx qBandRange_Mid[2];
	static const fhx qBandRange_High[2];
	static const fhx qBandRange_ContourL[2];
	static const fhx qBandRange_ContourH[2];
	static const fhx qBandRange_Presence[2];

	Cst_EQ *qBand[6];

	flx l_spl;
	fsx s_spl;
	fhx h_spl;

	fhx sRate;
	flx l_bValue[6];
	fsx s_bValue[6];
	fhx h_bValue[6];

	int fpPrec;
};

#endif
