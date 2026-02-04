#ifndef __C120_amp_head__
#define __C120_amp_head__

// #include "../common/custdata_types.h"
#ifdef BUILD_VST

#include <aeffectx.h>
#include <audioeffectx.h>
#include <vstfxstore.h>

#else	// /BUILD_VST

#include <scorchcraft.h>

#ifndef NO_GUI
#include <plugguieditor.h>
#endif

#endif	// /BUILD_VST

#include "../common/sc_parameters.h"
#include "../common/plugin_c120.h"

#ifndef NO_GUI
#include "c120_gui.h"
#endif

#include "params_vst_c120.h"
#include <cstdlib>
#include <cmath>

#include "../common/plugin_c120.h"

// class SC_C120_Amp_Head;
// class PlgEditor_C120;

//!-------------------------------------------------------------------------------------------------------
#ifdef BUILD_VST
class ScC120 : public AudioEffectX
#else
class ScC120 : public ScorchCraftPlugin
#endif
{
public:
#ifdef BUILD_VST
	ScC120 (audioMasterCallback audioMaster);
#else
	ScC120 (const float smplRate = 44100.0f);
#endif
	virtual ~ScC120();
#ifndef USE_IRR
	friend class PlgEditor_C120;
#endif

#ifdef BUILD_VST
//! Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames);

//! Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);

//! Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();
/*
	virtual void open();
	virtual void close();
	virtual void suspend();
	virtual void resume();
*/
#else	// /BUILD_VST
	virtual void l_processAudio (float** inputs, float** outputs, long sampleFrames);
	virtual void s_processAudio (double** inputs, double** outputs, long sampleFrames);

	virtual void setParameter (int index, float value);
	virtual float getParameter (int index);
#endif	// /BUILD_VST
	virtual void setSampleRate (float smplRate);

protected:
//! Functions
	void ZeroSamples();
	void InitPlug();
	void RunFX(const fsx inpSplA, const fsx inpSplB);
	void CheckParams();

	void SetOnlineSampling(const int inp);
	void SetOfflineSampling(const int inp);

	void CycleCabsOff(int prm);

//! Basic Stuff
	char programName[kVstMaxProgNameLen + 1];

	float inputFloatL;
	float inputFloatR;
	double inputDoubleL;
	double inputDoubleR;
	float outputFloatL;
	float outputFloatR;
	double outputDoubleL;
	double outputDoubleR;

	float sRate;
	float params[kMaxNumParameters];
	float oldParams[kMaxNumParameters];

	bool isOnline;
	bool isOnlineOld;

	int overSmpRate[2];
	int bDepth[2];
	int ampChan;
	int ampChanOld;

//!	Channel Controls
	bool isRunningStereo;
	bool isRunningStereoOld;

//! Actual Plug-in
	SC_C120_Amp_Head* C120Amp;
};

#endif

