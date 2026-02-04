#include "suite.h"
#include "../common_c/vst_c_suite.h"

//! -------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new Suite (audioMaster);
}
//! -------------------------------------------------------------------------------------------------------
Suite::Suite (audioMasterCallback audioMaster) : AudioEffectX (audioMaster, 1, kSuite_MaxNumParameters),
	inputFloatL(float (0.0f)),
	inputDoubleL(double (0.0)),
	inputFloatR(float (0.0f)),
	inputDoubleR(double (0.0)),
	outputFloatL(float (0.0f)),
	outputDoubleL(double (0.0)),
	outputFloatR(float (0.0f)),
	outputDoubleR(double (0.0))
{
#ifdef COMPILER_MGW32
	setUniqueID ('0xf@');
#endif
#ifdef COMPILER_MGW64
	setUniqueID ('0xf#');
#endif
	setNumInputs(2);
	setNumOutputs(2);
	canProcessReplacing();
	canDoubleReplacing();

	vst_strncpy (programName, "Default", kVstMaxProgNameLen);

	sRate = float (getSampleRate());

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the setSampleRate() function accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);

	overSmpRate[0] = 16;
	overSmpRate[1] = 1;

	for(int x = 0; x < kSuite_MaxNumParameters; x++)
		params[x] = 0.0f;

	params[kSuite_MasterVol] = 1.0f;

	ScPlug = (SCr_Suite_t*)malloc(sizeof(SCr_Suite_t) + sizeof(f64));

	SCr_Suite_Init(ScPlug, (f64) sRate);

	for(int x = 0; x < kSuite_MaxNumParameters; x++)
		oldParams[x] = params[x];

	isOnline = true;
	isOnlineOld = true;

#ifndef NO_GUI
	//! Load and use the GUI
	setEditor (new PlgEditor (this));
#endif
}
//! -------------------------------------------------------------------------------------------------------
Suite::~Suite()
{
	free(ScPlug);
}
//! -------------------------------------------------------------------------------------------------------
void Suite::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* in1  = inputs[0];
	float* in2  = inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CheckParams();

    while (--sampleFrames >= 0)
    {
    	inputFloatL = float (*in1++);
    	inputFloatR = float (*in2++);

		//! Safety measure, this will happily CLIP any input that is too loud
    	inputFloatL = (float) fminf(4.0f, fmaxf(-4.0f, inputFloatL));
		inputFloatR = (float) fminf(4.0f, fmaxf(-4.0f, inputFloatR));

		//! Here we run just about all of the plug-in code
		RunFX(inputFloatL, inputFloatR);

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		outputFloatL = (float) fminf(4.0f, fmaxf(-4.0f, outputFloatL));
		outputFloatR = (float) fminf(4.0f, fmaxf(-4.0f, outputFloatR));

		(*out1++) = float (outputFloatL);
		(*out2++) = float (outputFloatR);
    }
}
//! -------------------------------------------------------------------------------------------------------
void Suite::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* out1 = outputs[0];
    double* out2 = outputs[1];

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CheckParams();

    while (--sampleFrames >= 0)
    {
    	inputDoubleL = double (*in1++);
    	inputDoubleR = double (*in2++);

		//! Safety measure, this will happily CLIP any input that is too loud
    	inputDoubleL = (double) fmin(4.0, fmax(-4.0, inputDoubleL));
		inputDoubleR = (double) fmin(4.0, fmax(-4.0, inputDoubleR));

		//! Here we run just about all of the plug-in code
		RunFX(inputDoubleL, inputDoubleR);

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		outputDoubleL = (double) fmin(4.0, fmax(-4.0, outputDoubleL));
		outputDoubleR = (double) fmin(4.0, fmax(-4.0, outputDoubleR));

		(*out1++) = double (outputDoubleL);
		(*out2++) = double (outputDoubleR);
    }
}
//! -------------------------------------------------------------------------------------------------------
void Suite::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void Suite::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void Suite::setParameter (VstInt32 index, float value)
{
	float val = float (value);

	//! Safety measure, for handling illegal values
	if(val > 1.00f) val = 1.00f;
	if(val < 0.00f) val = 0.00f;

	//! If we're setting a valid parameter, update that parameter
	if(index < kSuite_MaxNumParameters)
	{
		params[index] = val;
	}
#ifndef NO_GUI
	//! If the GUI is up and running, pass the value to the GUI
	if(editor)
	{
		if(index < kSuite_MaxNumParameters) ((PlgEditor*)editor)->setParameter (index, val);
	}
#endif
}
//! -------------------------------------------------------------------------------------------------------
float Suite::getParameter (VstInt32 index)
{
	float val = 0.0f;

	//! If we're getting the value of a valid paramter, get that value
	if(index < kSuite_MaxNumParameters) val= params[index];

	return val;
}
//! -------------------------------------------------------------------------------------------------------
void Suite::getParameterLabel (VstInt32 index, char* label)
{
	if(index >= kSuite_MaxNumParameters) return;
	vst_strncpy (label, " ", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
void Suite::getParameterDisplay (VstInt32 index, char* text)
{
	if(index < kSuite_MaxNumParameters)
	{
		float2string(params[index], text, kVstMaxParamStrLen);
	}
}
//! -------------------------------------------------------------------------------------------------------
void Suite::getParameterName (VstInt32 index, char* text)
{
	if(index >= kSuite_MaxNumParameters) return;
	vst_strncpy (text, " ", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
bool Suite::getEffectName (char* name)
{
#ifdef COMPILER_MGW32
	vst_strncpy (name, "ScorchCrafter VST Suite x32", kVstMaxEffectNameLen);
#endif
#ifdef COMPILER_MGW64
	vst_strncpy (name, "ScorchCrafter VST Suite x64", kVstMaxEffectNameLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool Suite::getProductString (char* text)
{
#ifdef COMPILER_MGW32
	vst_strncpy (text, "ScS-mg32", kVstMaxProductStrLen);
#endif
#ifdef COMPILER_MGW64
	vst_strncpy (text, "ScS-mg64", kVstMaxProductStrLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool Suite::getVendorString (char* text)
{
	vst_strncpy (text, "ScorchCrafter", kVstMaxVendorStrLen);
	return true;
}
//! -------------------------------------------------------------------------------------------------------
VstInt32 Suite::getVendorVersion ()
{
	return 1000;
}
//! -------------------------------------------------------------------------------------------------------
void Suite::setSampleRate (float smplRate)
{
	sRate = float (smplRate);

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the constructor accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);

	SCr_Suite_ChangeSampleRate(ScPlug, (f64) sRate);
}
/*
//! -------------------------------------------------------------------------------------------------------
void Suite::open()
{
//	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void Suite::close()
{
//	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void Suite::suspend()
{
//	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void Suite::resume()
{
//	ZeroSamples();
}
*/
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! Custom FX Functions -----------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
void Suite::ZeroSamples()
{
	// JykAmp->ClearAllBuffers();
};
//! -------------------------------------------------------------------------------------------------------
void Suite::RunFX(const f64 inpSplA, const f64 inpSplB)
{
	SCr_Suite_InputStereo(ScPlug, (f64) inpSplA, (f64) inpSplB);

	SCr_Suite_Run(ScPlug);

	outputDoubleL = (f64) ScPlug->smp[0];
	outputDoubleR = (f64) ScPlug->smp[1];
	outputFloatL = f32 (outputDoubleL);
	outputFloatR = f32 (outputDoubleR);
}
//! -------------------------------------------------------------------------------------------------------
void Suite::CheckParams()
{
	if (getCurrentProcessLevel() == 4) isOnline = false;
	else isOnline = true;

	if(isOnlineOld != isOnline)
	{
		isOnlineOld = isOnline;
		SetSampling();
	}

	for(int x = 0; x < kSuite_MaxNumParameters; x++)
	{
		if(oldParams[x] != params[x])
		{
			oldParams[x] = params[x];
			if (x == kSuite_MasterVol) SCr_Suite_Set_MasterVol(ScPlug, (f64) params[x]);
			if (x == kSuite_InputBoost) SCr_Suite_Set_InputBoost(ScPlug, (f64) params[x]);
			if (x == kSuite_Sampling_OnLine) {
				const f64 tmp = params[x] * (f64) SCr_OverSampling_MaxFactor;
				overSmpRate[1] = (int) tmp;
				if (overSmpRate[1] < 0) overSmpRate[1] = 0;
				if (overSmpRate[1] > SCr_OverSampling_MaxFactor) overSmpRate[1] = SCr_OverSampling_MaxFactor;
				SetSampling();
			}
			if (x == kSuite_Sampling_OffLine) {
				const f64 tmp = params[x] * (f64) SCr_OverSampling_MaxFactor;
				overSmpRate[0] = (int) tmp;
				if (overSmpRate[0] < 0) overSmpRate[0] = 0;
				if (overSmpRate[0] > SCr_OverSampling_MaxFactor) overSmpRate[0] = SCr_OverSampling_MaxFactor;
				SetSampling();
			}
		}
	}
}
//! -------------------------------------------------------------------------------------------------------
void Suite::SetSampling()
{
	if (isOnline)
		SCr_Suite_Set_OverSampling(ScPlug, overSmpRate[1]);
	else
		SCr_Suite_Set_OverSampling(ScPlug, overSmpRate[0]);
}
//! -------------------------------------------------------------------------------------------------------
