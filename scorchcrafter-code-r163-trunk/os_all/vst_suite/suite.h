#ifndef __SCr_Suite_VST__
#define __SCr_Suite_VST__

#include "../common_c/custom_types.h"
#include <aeffectx.h>
#include <audioeffectx.h>
#include <vstfxstore.h>
#include "../common_c/vst_c_suite.h"

#include "suite_gui.h"

#include "params_vst_suite.h"
#include <cstdlib>
#include <cmath>

class PlgEditor;

//!-------------------------------------------------------------------------------------------------------
class Suite : public AudioEffectX
{
public:
	Suite (audioMasterCallback audioMaster);
	virtual ~Suite();

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
//	virtual void open();
//	virtual void close();
//	virtual void suspend();
//	virtual void resume();

protected:
//! Functions
	void ZeroSamples();
	void RunFX(const f64 inpSplA, const f64 inpSplB);
	void CheckParams();

	void SetSampling();

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
	float params[kSuite_MaxNumParameters];
	float oldParams[kSuite_MaxNumParameters];

	bool isOnline;
	bool isOnlineOld;

	int overSmpRate[2];

//! Distorion
	SCr_Suite_t *ScPlug;
};

#endif

