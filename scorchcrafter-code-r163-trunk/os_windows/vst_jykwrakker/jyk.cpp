#include "jyk.h"

//! -------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new JykWrakker (audioMaster);
}
//! -------------------------------------------------------------------------------------------------------
JykWrakker::JykWrakker (audioMasterCallback audioMaster) : AudioEffectX (audioMaster, 1, kMaxNumParameters),
	PI((f9x) ((f9x) 4.00 * (f9x) atanl((f9x) 1.00))),
	hPI((f9x) ((f9x) PI * (f9x) 0.50)),
#if SCPREC==1
	zeroFudge(f9x (0.0000000001)),				//! Used to avoid denormal-related issues in parts of the code
#endif
#if SCPREC==2
	zeroFudge(f9x (0.000000000000000001)),		//! Used to avoid denormal-related issues in parts of the code
#endif
#if SCPREC==3
	zeroFudge(f9x (0.000000000000000000001)),	//! Used to avoid denormal-related issues in parts of the code
#endif
	inputFloat(float (0.0f)),
	inputDouble(double (0.0)),
	outputFloat(float (0.0f)),
	outputDouble(double (0.0)),
	fadeFac(f9x (64.0))						//! Short fade to avoid pops, clicks, and spikes in output
{
#ifdef COMPILER_MGW32
	setUniqueID ('0J2[');
#endif
#ifdef COMPILER_MGW64
	setUniqueID ('0J2=');
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
	if(float (sRate) > float (768000.0f)) sRate=float (768000.0f);

	fadeMax = int (f9x (fadeFac) * f9x (sRate / 44100.0));
	fadeCount = fadeMax;

	//! Load and use the GUI
	setEditor (new JykEditor (this));

	params[kGainControl]=float (0.950f);
	params[kMasterVol]=float (0.50f);
	params[kEQstackLow]=float (0.50f);
	params[kEQstackMid]=float (0.50f);
	params[kEQstackHigh]=float (0.50f);
	params[kEQstackQkick]=float (0.50f);
	params[kAlgControlA]=float (0.0f);
	params[kAlgControlB]=float (0.0f);
	params[kAlgControlC]=float (0.0f);
	params[kAlgControlD]=float (1.0f);
	params[kEQstackPres]=float (0.50f);
	params[kMonoStereoControl]=float (0.0f);
	params[kOnline2x]=float (1.0f);
	params[kOnline4x]=float (0.0f);
	params[kOnline8x]=float (0.0f);
	params[kOnline16x]=float (0.0f);
	params[kOnlineMute]=float (0.0f);
	params[kOffline2x]=float (0.0f);
	params[kOffline4x]=float (0.0f);
	params[kOffline8x]=float (0.0f);
	params[kOffline16x]=float (1.0f);
	params[kOfflineMute]=float (0.0f);
	params[kPhaseMaster]=float (0.0f);
	params[kPhaseFloor]=float (0.0f);
	params[kPhaseLow]=float (0.0f);
	params[kPhaseMid]=float (0.0f);
	params[kPhaseHigh]=float (0.0f);
	params[kPhasePres]=float (0.0f);

	JykAmp[0] = new MdJykwrakker(float (sRate));
	TStack[0] = new ToneStack_A(float (sRate));
	JykAmp[1] = new MdJykwrakker(float (sRate));
	TStack[1] = new ToneStack_A(float (sRate));

	SampleDepthOnline=0;		//! Default 2x oversampling for live playback
	SampleDepthOnlineOld=0;
	SampleDepthOffline=0; 		//! Default 16x oversampling for off-line rendering
	SampleDepthOfflineOld=0;
	SampleDepthOld=0;
	SampleDepthCurrent=0;

	cleanGainChannelActive = false;
	crunchGainChannelActive = false;
	highGainChannelActive = true;
	megaGainChannelActive = false;
	oldChanSelect = int (3);

	isRunningStereo = false;
	isRunningStereoOld = false;
	stChan = 0;

	rateFac = f9x (2.0);

	//! Set default zero values for all filters and sample variables
//	lFOspeed = f9x (f9x (1.0) / f9x (sRate));
//	rndFP = f9x (0.0);
	JykAmp[0]->setPrecision(1);
	JykAmp[1]->setPrecision(1);
	TStack[0]->SetPrecision(1);
	TStack[1]->SetPrecision(1);
	CalcSampDepth();
	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
JykWrakker::~JykWrakker()
{
	delete TStack[0];
	delete JykAmp[0];
	delete TStack[1];
	delete JykAmp[1];
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* in1  = inputs[0];
	float* in2  = inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CalcSampDepth();

    while (--sampleFrames >= 0)
    {
    	inputFloat = float (*in1++);
    	if(isRunningStereo == false)
    	{
    		inputFloat += float (*in2++);
    		inputFloat *= float (0.50f);
    	}
    	stChan = 0;

		//! Safety measure, this will happily CLIP any input that is too loud
    	if(float (inputFloat) > float (1.00f)) inputFloat = float (1.00f);
		if(float (inputFloat) < float (-1.00f)) inputFloat = float (-1.00f);

		//! Here we run just about all of the plug-in code
		outputFloat = float (RunFX(inputFloat));

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		if(float (outputFloat) > float (4.00f)) outputFloat = float (0.00f);
		if(float (outputFloat) < float (-4.00f)) outputFloat = float (0.00f);

		outputFloat *= float (params[kMasterVol]);

		(*out1++) = float (outputFloat);
		if(isRunningStereo == false) (*out2++) = float (outputFloat);
		else
		{
			inputFloat = float (*in2++);
			stChan = 1;
			outputFloat = float (RunFX(inputFloat));
			if(float (outputFloat) > float (4.00f)) outputFloat = float (0.00f);
			if(float (outputFloat) < float (-4.00f)) outputFloat = float (0.00f);
			outputFloat *= float (params[kMasterVol]);
			(*out2++) = float (outputFloat);
		}
    }
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
	double* in1 = inputs[0];
	double* in2 = inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CalcSampDepth();

    while (--sampleFrames >= 0)
    {
    	inputDouble=double (*in1++);
    	if(isRunningStereo == false)
    	{
    		inputDouble += double (*in2++);
    		inputDouble *= double (0.50);
    	}
    	stChan = 0;

    	//! Safety measure, this will happily CLIP any input that is too loud
    	if(double (inputDouble) > double (1.00)) inputDouble = double (1.00);
		if(double (inputDouble) < double (-1.00)) inputDouble = double (-1.00);

		//! Here we run just about all of the plug-in code
		outputDouble = double (RunFX(inputDouble));

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		if(double (outputDouble) > double (4.00)) outputDouble = double (0.00);
		if(double (outputDouble) < double (-4.00)) outputDouble = double (0.00);

		outputDouble*=double (params[kMasterVol]);

		(*out1++) = double (outputDouble);
		if(isRunningStereo == false) (*out2++) = double (outputDouble);
		else
		{
			inputDouble=double (*in2++);
			stChan = 1;
			outputDouble = double (RunFX(inputDouble));
			if(double (outputDouble) > double (4.00)) outputDouble = double (0.00);
			if(double (outputDouble) < double (-4.00)) outputDouble = double (0.00);
			outputDouble*=double (params[kMasterVol]);
			(*out2++) = double (outputDouble);
		}
    }
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::setParameter (VstInt32 index, float value)
{
	float val = float (value);

	//! Safety measure, for handling illegal values
	if(float (val) > float (1.00f)) val = float (1.00f);
	if(float (val) < float (0.00f)) val = float (0.00f);

	//! If we're setting a valid parameter, update that parameter
	if(index < kMaxNumParameters)
	{
		val = float ((float) ceilf(float (val * 100.0f)) * float (0.010f));
		params[index] = float (val);
	}

	//! If the GUI is up and running, pass the value to the GUI
	if(editor)
	{
		if(index < kMaxNumParameters) ((AEffGUIEditor*)editor)->setParameter (index, val);
	}
}
//! -------------------------------------------------------------------------------------------------------
float JykWrakker::getParameter (VstInt32 index)
{
	float val=float (0.0f);

	//! If we're getting the value of a valid paramter, get that value
	if(index < kMaxNumParameters) val=float (params[index]);

	return float (val);
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::getParameterLabel (VstInt32 index, char* label)
{
	if(index < kMaxNumParameters) vst_strncpy (label, " ", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::getParameterDisplay (VstInt32 index, char* text)
{
	if(index < kMaxNumParameters)
	{
		float2string(params[index], text, kVstMaxParamStrLen);
	}
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::getParameterName (VstInt32 index, char* text)
{
	if(index==kGainControl) vst_strncpy (text, "Dr Gain", kVstMaxParamStrLen);
	if(index==kMasterVol) vst_strncpy (text, "Master", kVstMaxParamStrLen);
	if(index==kEQstackLow) vst_strncpy (text, "Q Low", kVstMaxParamStrLen);
	if(index==kEQstackMid) vst_strncpy (text, "Q Mid", kVstMaxParamStrLen);
	if(index==kEQstackHigh) vst_strncpy (text, "Q High", kVstMaxParamStrLen);
	if(index==kEQstackQkick) vst_strncpy (text, "Q Kick", kVstMaxParamStrLen);
	if(index==kEQstackPres) vst_strncpy (text, "Q Pres", kVstMaxParamStrLen);
	if(index==kAlgControlA) vst_strncpy (text, "Clean", kVstMaxParamStrLen);
	if(index==kAlgControlB) vst_strncpy (text, "Crunch", kVstMaxParamStrLen);
	if(index==kAlgControlC) vst_strncpy (text, "High", kVstMaxParamStrLen);
	if(index==kAlgControlD) vst_strncpy (text, "Mega-Bl", kVstMaxParamStrLen);
	if(index==kMonoStereoControl) vst_strncpy (text, "mn / st", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
bool JykWrakker::getEffectName (char* name)
{
#ifdef COMPILER_MGW32
	vst_strncpy (name, "SC-86 JykWrakker", kVstMaxEffectNameLen);
#endif
#ifdef COMPILER_MGW64
	vst_strncpy (name, "SC-64 JykWrakker", kVstMaxEffectNameLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool JykWrakker::getProductString (char* text)
{
#ifdef COMPILER_MGW32
	vst_strncpy (text, "JYKHmg32H", kVstMaxProductStrLen);
#endif
#ifdef COMPILER_MGW64
	vst_strncpy (text, "JYKHmg64H", kVstMaxProductStrLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool JykWrakker::getVendorString (char* text)
{
	//! This will eventually be changed to the dev group name
	vst_strncpy (text, "ScorchCrafter", kVstMaxVendorStrLen);
	return true;
}
//! -------------------------------------------------------------------------------------------------------
VstInt32 JykWrakker::getVendorVersion ()
{
	return 1000;
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::setSampleRate (float smplRate)
{
	sRate = float (smplRate);

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the constructor accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);
	if(float (sRate) > float (768000.0f)) sRate=float (768000.0f);

	fadeMax = int (f9x (fadeFac) * f9x (sRate / 44100.0f));
	fadeCount = fadeMax;

//	lFOspeed = f9x (f9x (1.0) / f9x (sRate));

	JykAmp[0]->changeSampleRate(float (sRate));
	TStack[0]->setNewSampleRate(float (sRate));
	JykAmp[1]->changeSampleRate(float (sRate));
	TStack[1]->setNewSampleRate(float (sRate));

	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::open()
{
	fadeCount = fadeMax;
	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::close()
{
	fadeCount = fadeMax;
	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::suspend()
{
	fadeCount = fadeMax;
	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::resume()
{
	fadeCount = fadeMax;
	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! Custom FX Functions -----------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::ZeroSamples()
{
	sample[0]=f9x (0.0);
	sample[1]=f9x (0.0);

	// lFOpos = f9x (f9x (int (rand() %1000)) * f9x (0.0010));

	JykAmp[0]->ClearBuffers();
	TStack[0]->ClearBuffs();
	JykAmp[1]->ClearBuffers();
	TStack[1]->ClearBuffs();
};
//! -------------------------------------------------------------------------------------------------------
fsx JykWrakker::RunFX(const fsx inpSpl)
{
	fsx output = fsx (inpSpl);

	if(SampleDepthCurrent < 6) //! If sampling control is set to "mute" then don't bother processing anything
	{
		sample[stChan] = fsx (inpSpl);

		JykAmp[stChan]->inputSample(sample[stChan]);

		if(cleanGainChannelActive == true) JykAmp[stChan]->RunInternals(0);
		if(crunchGainChannelActive == true) JykAmp[stChan]->RunInternals(1);
		if(highGainChannelActive == true) JykAmp[stChan]->RunInternals(2);
		if(megaGainChannelActive == true) JykAmp[stChan]->RunInternals(3);

		sample[stChan] = (fsx) JykAmp[stChan]->getOutSample();

		TStack[stChan]->SetInput(fsx (sample[stChan]));
		TStack[stChan]->RunInternals();
		sample[stChan] = (fsx) TStack[stChan]->GetProcessedOutput();

		output = fsx (sample[stChan]);
	}

	if(fadeCount > 0)
	{
		fadeCount--;
		output = fsx (0.0);
	}

	return fsx (output);
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::CalcSampDepth()
{
	if(SampleDepthCurrent == 0) rateFac = f9x (1.0);		//! Bypass
	if(SampleDepthCurrent == 1) rateFac = f9x (1.0);		//! 1x (no oversampling)
	if(SampleDepthCurrent == 2) rateFac = f9x (2.0);		//! 2x
	if(SampleDepthCurrent == 3) rateFac = f9x (4.0);		//! 4x
	if(SampleDepthCurrent == 4) rateFac = f9x (8.0);		//! 8x
	if(SampleDepthCurrent == 5) rateFac = f9x (16.0);		//! 16x
	if(SampleDepthCurrent == 6) rateFac = f9x (1.0);		//! Mute

	if(SampleDepthOnlineOld != 2)
	{
		if(params[kOnline2x] == float (1.0f)) ApplyOversamplingSetting(true, 2);
	}
	if(SampleDepthOnlineOld != 3)
	{
		if(params[kOnline4x] == float (1.0f)) ApplyOversamplingSetting(true, 3);
	}
	if(SampleDepthOnlineOld != 4)
	{
		if(params[kOnline8x] == float (1.0f)) ApplyOversamplingSetting(true, 4);
	}
	if(SampleDepthOnlineOld != 5)
	{
		if(params[kOnline16x] == float (1.0f)) ApplyOversamplingSetting(true, 5);
	}
	if(SampleDepthOnlineOld != 6)
	{
		if(params[kOnlineMute] == float (1.0f)) ApplyOversamplingSetting(true, 6);
	}

	if(SampleDepthOfflineOld != 2)
	{
		if(params[kOffline2x] == float (1.0f)) ApplyOversamplingSetting(false, 2);
	}
	if(SampleDepthOfflineOld != 3)
	{
		if(params[kOffline4x] == float (1.0f)) ApplyOversamplingSetting(false, 3);
	}
	if(SampleDepthOfflineOld != 4)
	{
		if(params[kOffline8x] == float (1.0f)) ApplyOversamplingSetting(false, 4);
	}
	if(SampleDepthOfflineOld != 5)
	{
		if(params[kOffline16x] == float (1.0f)) ApplyOversamplingSetting(false, 5);
	}
	if(SampleDepthOfflineOld != 6)
	{
		if(params[kOfflineMute] == float (1.0f)) ApplyOversamplingSetting(false, 6);
	}

	if(float (params[kOnline2x]) == float (0.0f))
	{
		if(SampleDepthOnline == 2) setParameterAutomated(kOnline2x, float (1.0f));
	}
	if(float (params[kOnline4x]) == float (0.0f))
	{
		if(SampleDepthOnline == 3) setParameterAutomated(kOnline4x, float (1.0f));
	}
	if(float (params[kOnline8x]) == float (0.0f))
	{
		if(SampleDepthOnline == 4) setParameterAutomated(kOnline8x, float (1.0f));
	}
	if(float (params[kOnline16x]) == float (0.0f))
	{
		if(SampleDepthOnline == 5) setParameterAutomated(kOnline16x, float (1.0f));
	}
	if(float (params[kOnlineMute]) == float (0.0f))
	{
		if(SampleDepthOnline == 6) setParameterAutomated(kOnlineMute, float (1.0f));
	}

	if(float (params[kOffline2x]) == float (0.0f))
	{
		if(SampleDepthOffline == 2) setParameterAutomated(kOffline2x, float (1.0f));
	}
	if(float (params[kOffline4x]) == float (0.0f))
	{
		if(SampleDepthOffline == 3) setParameterAutomated(kOffline4x, float (1.0f));
	}
	if(float (params[kOffline8x]) == float (0.0f))
	{
		if(SampleDepthOffline == 4) setParameterAutomated(kOffline8x, float (1.0f));
	}
	if(float (params[kOffline16x]) == float (0.0f))
	{
		if(SampleDepthOffline == 5) setParameterAutomated(kOffline16x, float (1.0f));
	}
	if(float (params[kOfflineMute]) == float (0.0f))
	{
		if(SampleDepthOffline == 6) setParameterAutomated(kOfflineMute, float (1.0f));
	}

	JykAmp[0]->setInternalParam(0, params[kGainControl]);
	JykAmp[1]->setInternalParam(0, params[kGainControl]);

	if (getCurrentProcessLevel() == 4)
	{
		isOnline = false;
		SampleDepthCurrent = SampleDepthOffline;
	} else
	{
		isOnline = true;
		SampleDepthCurrent = SampleDepthOnline;
	}

	if(SampleDepthCurrent != SampleDepthOld)
	{
		SampleDepthOld = SampleDepthCurrent;
		ZeroSamples();
		fadeCount = fadeMax;
		JykAmp[0]->setOverSampleRate(SampleDepthCurrent);
		JykAmp[1]->setOverSampleRate(SampleDepthCurrent);
		TStack[0]->setOverSampling(SampleDepthCurrent);
		TStack[1]->setOverSampling(SampleDepthCurrent);
		if(SampleDepthCurrent > 2)
		{
			JykAmp[0]->setPrecision(2);
			JykAmp[1]->setPrecision(2);
			TStack[0]->SetPrecision(2);
			TStack[1]->SetPrecision(2);
		} else
		{
			JykAmp[0]->setPrecision(1);
			JykAmp[1]->setPrecision(1);
			TStack[0]->SetPrecision(1);
			TStack[1]->SetPrecision(1);
		}
		updateDisplay();
	}

	if(float (params[kAlgControlA] + params[kAlgControlB] + params[kAlgControlC] +
			params[kAlgControlD]) < float (0.8f))
		{
			if(oldChanSelect==1) setParameterAutomated(kAlgControlA, 1.0f);
			if(oldChanSelect==2) setParameterAutomated(kAlgControlB, 1.0f);
			if(oldChanSelect==3) setParameterAutomated(kAlgControlC, 1.0f);
			if(oldChanSelect==4) setParameterAutomated(kAlgControlD, 1.0f);
		}

	if(params[kAlgControlA] == float (1.0f)) CheckChannelSelect(1);
	if(params[kAlgControlB] == float (1.0f)) CheckChannelSelect(2);
	if(params[kAlgControlC] == float (1.0f)) CheckChannelSelect(3);
	if(params[kAlgControlD] == float (1.0f)) CheckChannelSelect(4);

	if(params[kAlgControlA] == float (0.0f))
	{
		if(oldChanSelect==1) setParameterAutomated(kAlgControlA, 1.0f);
	}
	if(params[kAlgControlB] == float (0.0f))
	{
		if(oldChanSelect==2) setParameterAutomated(kAlgControlB, 1.0f);
	}
	if(params[kAlgControlC] == float (0.0f))
	{
		if(oldChanSelect==3) setParameterAutomated(kAlgControlC, 1.0f);
	}
	if(params[kAlgControlD] == float (0.0f))
	{
		if(oldChanSelect==4) setParameterAutomated(kAlgControlD, 1.0f);
	}

	if(params[kMonoStereoControl] == 1.0f) isRunningStereo = true;
	else isRunningStereo = false;

	if(isRunningStereoOld != isRunningStereo)
	{
		isRunningStereoOld = isRunningStereo;
		ZeroSamples();
		fadeCount = fadeMax;
	}

	// const f9x cutFac = f9x (f9x (0.750) / f9x (params[kEQstackLow] + params[kEQstackMid] + params[kEQstackHigh]));

	if(float (params[kPhaseLow]) == float (0.0f))
	{
		TStack[0]->setBandValue(2, fsx (params[kEQstackLow]));
		TStack[1]->setBandValue(2, fsx (params[kEQstackLow]));
	} else
	{
		TStack[0]->setBandValue(2, fsx (-params[kEQstackLow]));
		TStack[1]->setBandValue(2, fsx (-params[kEQstackLow]));
	}

	if(float (params[kPhaseMid]) == float (0.0f))
	{
		TStack[0]->setBandValue(3, fsx (params[kEQstackMid]));
		TStack[1]->setBandValue(3, fsx (params[kEQstackMid]));
	} else
	{
		TStack[0]->setBandValue(3, fsx (-params[kEQstackMid]));
		TStack[1]->setBandValue(3, fsx (-params[kEQstackMid]));
	}

	if(float (params[kPhaseHigh]) == float (0.0f))
	{
		TStack[0]->setBandValue(4, fsx (params[kEQstackHigh]));
		TStack[1]->setBandValue(4, fsx (params[kEQstackHigh]));
	} else
	{
		TStack[0]->setBandValue(4, fsx (-params[kEQstackHigh]));
		TStack[1]->setBandValue(4, fsx (-params[kEQstackHigh]));
	}

	if(float (params[kPhaseFloor]) == float (0.0f))
	{
		TStack[0]->setBandValue(1, fsx (params[kEQstackQkick]));
		TStack[1]->setBandValue(1, fsx (params[kEQstackQkick]));
	} else
	{
		TStack[0]->setBandValue(1, fsx (-params[kEQstackQkick]));
		TStack[1]->setBandValue(1, fsx (-params[kEQstackQkick]));
	}

	if(float (params[kPhasePres]) == float (0.0f))
	{
		TStack[0]->setBandValue(5, fsx (params[kEQstackPres]));
		TStack[1]->setBandValue(5, fsx (params[kEQstackPres]));
	} else
	{
		TStack[0]->setBandValue(5, fsx (-params[kEQstackPres]));
		TStack[1]->setBandValue(5, fsx (-params[kEQstackPres]));
	}

	if(float (params[kPhaseMaster]) == float (0.0f))
	{
		TStack[0]->setBandValue(0, fsx (1.0));
		TStack[1]->setBandValue(0, fsx (1.0));
	} else
	{
		TStack[0]->setBandValue(0, fsx (-1.0));
		TStack[1]->setBandValue(0, fsx (-1.0));
	}
}
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::CheckChannelSelect(const int nChan)
{
	if(oldChanSelect == nChan) return;

	cleanGainChannelActive = false;
	crunchGainChannelActive = false;
	highGainChannelActive = false;
	megaGainChannelActive = false;

	if(nChan == 1)
	{
		cleanGainChannelActive = true;
		setParameterAutomated(kAlgControlB, float (0.f));
		setParameterAutomated(kAlgControlC, float (0.f));
		setParameterAutomated(kAlgControlD, float (0.f));
	}
	if(nChan == 2)
	{
		crunchGainChannelActive = true;
		setParameterAutomated(kAlgControlA, float (0.f));
		setParameterAutomated(kAlgControlC, float (0.f));
		setParameterAutomated(kAlgControlD, float (0.f));
	}
	if(nChan == 3)
	{
		highGainChannelActive = true;
		setParameterAutomated(kAlgControlA, float (0.f));
		setParameterAutomated(kAlgControlB, float (0.f));
		setParameterAutomated(kAlgControlD, float (0.f));
	}
	if(nChan == 4)
	{
		megaGainChannelActive = true;
		setParameterAutomated(kAlgControlA, float (0.f));
		setParameterAutomated(kAlgControlB, float (0.f));
		setParameterAutomated(kAlgControlC, float (0.f));
	}

	if(oldChanSelect != nChan)
	{
		fadeCount = fadeMax;
		ZeroSamples();
		oldChanSelect = nChan;
	}

	updateDisplay();
}
//! -------------------------------------------------------------------------------------------------------
/*
void JykWrakker::RunLFO()
{
	lFOpos += f9x (lFOspeed);
#if SCPREC==1
	rndFP = (f9x) sinf(f9x (lFOpos) * f9x (PI) * f9x (2.0f));
#endif
#if SCPREC==2
	rndFP = (f9x) sin(f9x (lFOpos) * f9x (PI) * f9x (2.0));
#endif
#if SCPREC==3
	rndFP = (f9x) sinl(f9x (lFOpos) * f9x (PI) * f9x (2.0));
#endif
	if(f9x (lFOpos) > f9x (1.0)) lFOpos -= f9x (1.0);
	rndFP *= f9x (0.010);
	lFOmod[0] = f9x (f9x (0.0) + f9x (rndFP));
	lFOmod[1] = f9x (f9x (0.0) - f9x (rndFP));
}
*/
//! -------------------------------------------------------------------------------------------------------
void JykWrakker::ApplyOversamplingSetting(const bool online, const int newSetting)
{
	if(online == true)
	{
		if(newSetting != 2) setParameterAutomated(kOnline2x, float (0.0f));
		if(newSetting != 3) setParameterAutomated(kOnline4x, float (0.0f));
		if(newSetting != 4) setParameterAutomated(kOnline8x, float (0.0f));
		if(newSetting != 5) setParameterAutomated(kOnline16x, float (0.0f));
		if(newSetting != 6) setParameterAutomated(kOnlineMute, float (0.0f));
		SampleDepthOnline = newSetting;
		SampleDepthOnlineOld = newSetting;
	} else
	{
		if(newSetting != 2) setParameterAutomated(kOffline2x, float (0.0f));
		if(newSetting != 3) setParameterAutomated(kOffline4x, float (0.0f));
		if(newSetting != 4) setParameterAutomated(kOffline8x, float (0.0f));
		if(newSetting != 5) setParameterAutomated(kOffline16x, float (0.0f));
		if(newSetting != 6) setParameterAutomated(kOfflineMute, float (0.0f));
		SampleDepthOffline = newSetting;
		SampleDepthOfflineOld = newSetting;
	}
}
//! -------------------------------------------------------------------------------------------------------
