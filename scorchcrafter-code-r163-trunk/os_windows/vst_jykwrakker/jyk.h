#ifndef __jykwrakker_amp_head__
#define __jykwrakker_amp_head__

#include "custdata_types.h"
#include "jyk_config.h"
#include "../lib_graphics_src/zlib.h"
#include "../lib_graphics_src/png.h"
#include <audioeffectx.h>
#include "jyk_gui.h"
#include "params.h"
#include "resource.h"
#include <cstdlib>
#include <cmath>

#include "module_jyk.h"
#include "tonestack_a.h"
//!-------------------------------------------------------------------------------------------------------
class JykWrakker : public AudioEffectX
{
public:
	JykWrakker (audioMasterCallback audioMaster);
	~JykWrakker();

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

private:
//! Functions
	void ZeroSamples();
	fsx RunFX(const fsx inpSpl);
	void CalcSampDepth();
	void CheckChannelSelect(const int nChan);
	void ApplyOversamplingSetting(const bool online, const int newSetting);
//	void RunLFO();

//! Basic Stuff
	char programName[kVstMaxProgNameLen + 1];

	float inputFloat;
	double inputDouble;
	float outputFloat;
	double outputDouble;

	float sRate;
	float params[kMaxNumParameters];
	fsx sample[2];
	bool isOnline;
	int SampleDepthCurrent;
	int SampleDepthOld;
	int SampleDepthOnline;
	int SampleDepthOnlineOld;
	int SampleDepthOffline;
	int SampleDepthOfflineOld;
	const f9x PI;
	const f9x hPI;
	const f9x zeroFudge;

//!	Channel Controls
	bool cleanGainChannelActive;
	bool crunchGainChannelActive;
	bool highGainChannelActive;
	bool megaGainChannelActive;
	int oldChanSelect;

	bool isRunningStereo;
	bool isRunningStereoOld;
	int stChan;

//! Tone Stack Stuff
	int tStackTypeSelection, oldTStackSel;

	ToneStack_A* TStack[2];

//! Special
	f9x rateFac;
	int fadeCount;
	int fadeMax;
	const f9x fadeFac;

//	f9x rndFP;
//	f9x lFOpos;
//	f9x lFOspeed;
//	f9x lFOmod[2];

//! Distorion
	MdJykwrakker* JykAmp[2];
};

#endif

