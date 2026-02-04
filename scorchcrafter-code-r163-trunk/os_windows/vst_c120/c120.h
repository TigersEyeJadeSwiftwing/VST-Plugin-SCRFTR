#ifndef __C120_amp_head__
#define __C120_amp_head__

#include "custdata_types.h"
// #include "c120_config.h"
#include <aeffectx.h>
#include <audioeffectx.h>
#include <vstfxstore.h>
#include "c120_gui.h"
#include "params.h"
#include <cstdlib>
#include <cmath>

#include "../common/plugin_c120.h"

//!-------------------------------------------------------------------------------------------------------
class ScC120 : public AudioEffectX
{
public:
	ScC120 (audioMasterCallback audioMaster);
	~ScC120();

	friend class PlgEditor;

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

	virtual void setSampleRate (float smplRate);
	virtual void open();
	virtual void close();
	virtual void suspend();
	virtual void resume();

protected:
//! Functions
	void ZeroSamples();
	void RunFX(const fsx inpSpl);
	void CheckParams();

	void SetOnlineSampling(const int inp);
	void SetOfflineSampling(const int inp);

//! Basic Stuff
	char programName[kVstMaxProgNameLen + 1];

	float inputFloat;
	double inputDouble;
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

//! Distorion
	SC_C120_Amp_Head* C120Amp;
};

#endif

