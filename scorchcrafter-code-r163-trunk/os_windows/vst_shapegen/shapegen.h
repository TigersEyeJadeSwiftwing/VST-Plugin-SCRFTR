#ifndef __shapewriter_amp_head__
#define __shapewriter_amp_head__

#include "custdata_types.h"
#include <audioeffectx.h>
#include "params.h"
#include <cmath>

#define TBWAVE_SIZE 25000
//!-------------------------------------------------------------------------------------------------------

class ShapeGen : public AudioEffectX
{
public:
	ShapeGen (audioMasterCallback audioMaster);
	~ShapeGen ();

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
	f9x RunFX(f9x const inpSpl);
	void BuildShapeTables();
	void BuildMatchTables();
	f9x RunTBS(f9x const inp);

//! Basic Stuff
	char programName[kVstMaxProgNameLen + 1];

	float inputFloat;
	double inputDouble;
	float outputFloat;
	double outputDouble;

	float sRate;
	float params[kMaxNumParameters];

	f9x sWave;
	f9x *tbWaveLGup;
	f9x *tbWaveLGdn;
	u9x *tbWaveLGmtU;
	u9x *tbWaveLGmtD;

	bool upTravel[2];
	f9x upPeak[2];
	bool oldTravel[2];
	bool travChange[2];
	u9x tChangePoint[2];
	f9x lastSpl[2];
	f9x distPeak[2];
};

#endif
