#ifndef __shapewriter_amp_head__
#define __shapewriter_amp_head__

#include "custdata_types.h"
#include <audioeffectx.h>
#include "params.h"
#include <cmath>
#include <iostream>
#include <fstream>

//!-------------------------------------------------------------------------------------------------------

class ShapeWriter : public AudioEffectX
{
public:
	ShapeWriter (audioMasterCallback audioMaster);
	~ShapeWriter ();

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
	fsx RunFX(fsx const inpSpl);
	void WavReset();
	void wavWrite();
	void wavWriteBinary();

//! Basic Stuff
	char programName[kVstMaxProgNameLen + 1];

	float inputFloat;
	double inputDouble;
	float outputFloat;
	double outputDouble;

	float sRate;
	float params[kMaxNumParameters];

	bool readReady;
	s9x smpCount;
	fsx *recSamp;
	f9x wavUp[8192];
	f9x wavDwn[8192];
	s9x rIndex;
	// f9x wavMax;
};

#endif
