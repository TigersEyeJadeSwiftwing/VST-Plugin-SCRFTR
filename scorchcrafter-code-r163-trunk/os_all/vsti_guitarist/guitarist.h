#ifndef __SC_Guitarist__
#define __SC_Guitarist__

#include "../common/custdata_types.h"
#include <aeffectx.h>
#include <audioeffectx.h>
#include <vstfxstore.h>
#include "../common/sc_parameters.h"
#include "../common/plugin_guitarist.h"

// #include "c120_gui.h"

#include "params.h"
#include <cstdlib>
#include <fstream>

#include "../common/plugin_guitarist.h"

class SC_Synth_Guitar;

class ScGuitarist : public AudioEffectX
{
public:
	ScGuitarist (audioMasterCallback audioMaster);
	virtual ~ScGuitarist();

//! Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames);
	virtual VstInt32 processEvents (VstEvents* ev);

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
	virtual VstInt32 canDo (char* text);

	virtual void setSampleRate (float smplRate);
	virtual void setBlockSize (VstInt32 blockSize);

	virtual VstInt32 getNumMidiInputChannels () {return 16;}
//	virtual VstInt32 getNumMidiOutputChannels ();

	virtual bool getOutputProperties (VstInt32 index, VstPinProperties* properties);

	void noteOn(VstInt32 note, VstInt32 velocity, VstInt32 delta, int ichan = 0);

private:
	void RunFX();

	SC_Synth_Guitar *Synth;

	char programName[kVstMaxProgNameLen + 1];

	float outputFloat;
	double outputDouble;

	float sRate;
	float params[kMaxNumParameters];

	bool isOnline;
	bool isOnlineOld;

	int bDepth[2];

	VstInt32 notes[128];
	VstInt32 ntDelta;
	VstInt32 ntKey;
	VstInt32 ntVel;
	int ntx;
};

#endif
