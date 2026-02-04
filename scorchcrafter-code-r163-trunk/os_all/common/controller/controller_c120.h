#ifndef __SC_Controller_C120__
#define __SC_Controller_C120__

#include "custdata_types.h"
#include "sc_parameters.h"
#include "plugin_c120.h"
#include "csts_eq.h"

#ifdef BUILD_VST
#include "params_vst_c120.h"
#endif

#ifndef SC_HAVE_PARAMS_C120
#error (ScorchCrafter) No parameters available for controller (for C-120)
#endif

class SC_Controller_C120
{
public:
	SC_Controller_C120(const bool smoothControls = true);
	~SC_Controller_C120();

	void SetControlSmoothRate(const fsx inp);
	void SetControlCheckRate(const fsx inp);
	flx GetParam(const int index);
	void SetParam(const int index, const flx val);
	void SetParamHard(const int index, const flx val);
	void UpdatePlugin(SC_C120_Amp_Head **Plg);
	void TickControls();
	void ResetParams();
	void ClearBuffers();

	bool displayNeedsUpdate;

private:
	void ForceParam(const int index, const flx val);
	void ForceParamRange(const int indexA, const int indexB, const flx val);
	void ForceParamRangeExcludingX(const int indexA, const int indexB, const int indexX, const flx val);

	bool controlSmooth;
	bool isOnline;
	Cst_EQ *kSmoother[kMaxNumParameters_C120];
	flx params_cur[kMaxNumParameters_C120];
	flx params_set[kMaxNumParameters_C120];
	flx params_old[kMaxNumParameters_C120];
	int bitDepth_On;
	int bitDepth_off;
	int overSmp_On;
	int overSmp_Off;
};

#endif
