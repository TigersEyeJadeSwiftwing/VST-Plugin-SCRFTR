#ifndef __GLZ60_Bass_amp_head__
#define __GLZ60_Bass_amp_head__

#include "../common/custdata_types.h"
#include <aeffectx.h>
#include <audioeffectx.h>
#include <vstfxstore.h>
#include "../common/sc_parameters.h"
#include "../common/plugin_glz60.h"

#include "glz60_gui.h"

#include "params_glz60.h"
#include <cstdlib>
#include <cmath>

#include "../common/plugin_glz60.h"

class SC_GLZ60_Amp_Head;
class PlgEditor_Glz;

//!-------------------------------------------------------------------------------------------------------
class ScGLZ60 : public AudioEffectX
{
public:
	ScGLZ60 (audioMasterCallback audioMaster);
	virtual ~ScGLZ60();
#ifndef USE_IRR
	friend class PlgEditor_Glz;
#endif

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

//! Distorion
	SC_GLZ60_Amp_Head* GlzAmp;
};

#endif

