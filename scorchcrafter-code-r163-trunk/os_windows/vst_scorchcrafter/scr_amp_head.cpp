#include "scr_amp_head.h"
#include "w_dat_dn.h"
#include "w_dat_up.h"
// #include "cfloat"

//! -------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new ScorchCraft (audioMaster);
}
//! -------------------------------------------------------------------------------------------------------
ScorchCraft::ScorchCraft (audioMasterCallback audioMaster) : AudioEffectX (audioMaster, 1, kMaxNumParameters),
	PI((f9x) ((f9x) 4.00 * (f9x) atanl((f9x) 1.00))),
	hPI((f9x) ((f9x) PI * (f9x) 0.50)),
#if SCPREC==1
	zeroFudge(f9x (0.000001)),				//! Used to avoid denormal-related issues in parts of the code
#endif
#if SCPREC==2
	zeroFudge(f9x (0.0000000000001)),		//! Used to avoid denormal-related issues in parts of the code
#endif
#if SCPREC==3
	zeroFudge(f9x (0.0000000000001)),		//! Used to avoid denormal-related issues in parts of the code
#endif
	inputFloat(float (0.0f)),
	inputDouble(double (0.0)),
	outputFloat(float (0.0f)),
	outputDouble(double (0.0)),
	fadeFac(f9x (32.0)),					//! Short fade to avoid pops, clicks, and spikes in output
	tbFacF(f9x (u7x (COMPONENT_TABLE_SIZE - 1))),
	tbFacI(u7x (COMPONENT_TABLE_SIZE - 1)),
	dstRead(f9x (0.0))
{
#ifdef COMPILER_MSOFT32
#if SCPREC==1
	setUniqueID ('0x2-');
#else
	setUniqueID ('0y2-');
#endif
#endif

#ifdef COMPILER_MSOFT64
#if SCPREC==1
	setUniqueID ('0x2$');
#else
	setUniqueID ('0y2$');
#endif
#endif

#ifdef COMPILER_MGW32
#if SCPREC==1
	setUniqueID ('0x2[');
#endif
#if SCPREC==2
	setUniqueID ('0y2[');
#endif
#if SCPREC==3
	setUniqueID ('0z2[');
#endif
#endif

#ifdef COMPILER_MGW64
#if SCPREC==1
	setUniqueID ('0x2=');
#endif
#if SCPREC==2
	setUniqueID ('0y2=');
#endif
#if SCPREC==3
	setUniqueID ('0z2=');
#endif
#endif
	setNumInputs (1);
	setNumOutputs (1);
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

	qFade = f9x (f9x (5.0) / f9x (sRate));

	//! Load and use the GUI
	setEditor (new ScorchCEditor (this));

	params[kSamplingOnline]=float (0.00f);
	params[kSamplingRender]=float (0.750f);
	params[kGainControl]=float (0.750f);
	params[kMasterVol]=float (0.50f);
	params[kEQstackLow]=float (0.50f);
	params[kEQstackMid]=float (0.50f);
	params[kEQstackHigh]=float (0.50f);
	params[kFilterI]=float (1.0f);
	params[kEQstackQkick]=float (0.50f);
	params[kAlgControlA]=float (0.0f);
	params[kAlgControlB]=float (0.0f);
	params[kAlgControlC]=float (1.0f);
	params[kAlgControlD]=float (0.0f);
	params[kFilterX]=float (1.0f);
	params[kEQstackPres]=float (0.50f);
	params[kToneStackType]=float (1.0f);

	LoadLPassCutOffs();

	SampleDepthOnline=1; //! Default 1x (no oversampling) for live playback
	SampleDepthOffline=5; //! Default 16x oversampling for off-line rendering
	SampleDepthOld=0;
	SampleDepthCurrent=0;

	cleanGainChannelActive = false;
	crunchGainChannelActive = false;
	highGainChannelActive = true;
	megaGainChannelActive = false;
	oldChanSelect = int (3);

	rateFac = f9x (1.0);

	//! Set default zero values for all filters and sample variables
	ZeroSamples();
	SetupLowPassFs();
	ZeroPassBuffers();
	LoadTubeTables();
}
//! -------------------------------------------------------------------------------------------------------
ScorchCraft::~ScorchCraft ()
{
	if(driveTubeHGdn)
	{
		delete[] driveTubeHGdn;
		driveTubeHGdn = NULL;
	}
	if(driveTubeHGup)
	{
		delete[] driveTubeHGup;
		driveTubeHGup = NULL;
	}
	if(driveTubeLGdn)
	{
		delete[] driveTubeLGdn;
		driveTubeLGdn = NULL;
	}
	if(driveTubeLGup)
	{
		delete[] driveTubeLGup;
		driveTubeLGup = NULL;
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* in1  = inputs[0];
    float* out1 = outputs[0];

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CalcSampDepth();

    while (--sampleFrames >= 0)
    {
    	inputFloat=float (*in1++);

		//! Safety measure, this will happily CLIP any input that is too loud
    	if(float (inputFloat) > float (1.00f)) inputFloat = float (1.00f);
		if(float (inputFloat) < float (-1.00f)) inputFloat = float (-1.00f);

		//! Here we run just about all of the plug-in code
		outputFloat=(float) RunFX(f9x (inputFloat));

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		if(float (outputFloat) > float (4.00f)) outputFloat = float (0.00f);
		if(float (outputFloat) < float (-4.00f)) outputFloat = float (0.00f);

		outputFloat *= float (params[kMasterVol]);

		(*out1++) = float (outputFloat);
    }
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
	double* in1  = inputs[0];
    double* out1 = outputs[0];

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CalcSampDepth();

    while (--sampleFrames >= 0)
    {
    	inputDouble=double (*in1++);

    	//! Safety measure, this will happily CLIP any input that is too loud
    	if(double (inputDouble) > double (1.00)) inputDouble = double (1.00);
		if(double (inputDouble) < double (-1.00)) inputDouble = double (-1.00);

		//! Here we run just about all of the plug-in code
		outputDouble=(double) RunFX(f9x (inputDouble));

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		if(double (outputDouble) > double (4.00)) outputDouble = double (0.00);
		if(double (outputDouble) < double (-4.00)) outputDouble = double (0.00);

		outputDouble*=double (params[kMasterVol]);

		(*out1++) = double (outputDouble);
    }
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::setParameter (VstInt32 index, float value)
{
	float val=float (value);

	//! Safety measure, for handling illegal values
	if(float (val) > float (1.00f)) val = float (1.00f);
	if(float (val) < float (0.00f)) val = float (0.00f);

	//! If we're setting a valid parameter, update that parameter
	if(index < kMaxNumParameters) params[index] = float (val);

	//! If the GUI is up and running, pass the value to the GUI
	if(editor)
	{
		if(index < kMaxNumParameters) ((AEffGUIEditor*)editor)->setParameter (index, value);
	}
}
//! -------------------------------------------------------------------------------------------------------
float ScorchCraft::getParameter (VstInt32 index)
{
	float val=float (0.0f);

	//! If we're getting the value of a valid paramter, get that value
	if(index < kMaxNumParameters) val=float (params[index]);

	return float (val);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::getParameterLabel (VstInt32 index, char* label)
{
	if(index < kMaxNumParameters) vst_strncpy (label, " ", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::getParameterDisplay (VstInt32 index, char* text)
{
	if(index < kMaxNumParameters)
	{
		if(index==0)
		{
			if(SampleDepthOnline==0) vst_strncpy (text, " Bypass", kVstMaxParamStrLen);
			if(SampleDepthOnline==1) vst_strncpy (text, " 1x", kVstMaxParamStrLen);
			if(SampleDepthOnline==2) vst_strncpy (text, " 2x", kVstMaxParamStrLen);
			if(SampleDepthOnline==3) vst_strncpy (text, " 4x", kVstMaxParamStrLen);
			if(SampleDepthOnline==4) vst_strncpy (text, " 8x", kVstMaxParamStrLen);
			if(SampleDepthOnline==5) vst_strncpy (text, " 16x", kVstMaxParamStrLen);
			if(SampleDepthOnline==6) vst_strncpy (text, " Mute", kVstMaxParamStrLen);
		}
		if(index==1)
		{
			if(SampleDepthOffline==0) vst_strncpy (text, " Bypass", kVstMaxParamStrLen);
			if(SampleDepthOffline==1) vst_strncpy (text, " 1x", kVstMaxParamStrLen);
			if(SampleDepthOffline==2) vst_strncpy (text, " 2x", kVstMaxParamStrLen);
			if(SampleDepthOffline==3) vst_strncpy (text, " 4x", kVstMaxParamStrLen);
			if(SampleDepthOffline==4) vst_strncpy (text, " 8x", kVstMaxParamStrLen);
			if(SampleDepthOffline==5) vst_strncpy (text, " 16x", kVstMaxParamStrLen);
			if(SampleDepthOffline==6) vst_strncpy (text, " Mute", kVstMaxParamStrLen);
		}
		if(index==2) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==3) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==4) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==5) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==6) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==7) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==8) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==9)
		{
			if(highGainChannelActive == true) vst_strncpy (text, " High", kVstMaxParamStrLen);
			else vst_strncpy (text, "Low", kVstMaxParamStrLen);
		}
		if(index==10) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==11) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==12)
		{
			if(tStackTypeSelection == 0) vst_strncpy (text, " Even", kVstMaxParamStrLen);
			else vst_strncpy (text, "Shaped", kVstMaxParamStrLen);
		}
		if(index==13) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==14) float2string(params[index], text, kVstMaxParamStrLen);
		if(index==15) float2string(params[index], text, kVstMaxParamStrLen);
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::getParameterName (VstInt32 index, char* text)
{
	if(index==0) vst_strncpy (text, "S OnLn", kVstMaxParamStrLen);
	if(index==1) vst_strncpy (text, "S OffLn", kVstMaxParamStrLen);
	if(index==2) vst_strncpy (text, "Dr Gain", kVstMaxParamStrLen);
	if(index==3) vst_strncpy (text, "Master", kVstMaxParamStrLen);
	if(index==4) vst_strncpy (text, "Q Low", kVstMaxParamStrLen);
	if(index==5) vst_strncpy (text, "Q Mid", kVstMaxParamStrLen);
	if(index==6) vst_strncpy (text, "Q High", kVstMaxParamStrLen);
	if(index==7) vst_strncpy (text, "Flt I", kVstMaxParamStrLen);
	if(index==8) vst_strncpy (text, "Q Kick", kVstMaxParamStrLen);
	if(index==9) vst_strncpy (text, "Clean", kVstMaxParamStrLen);
	if(index==10) vst_strncpy (text, "Flt X", kVstMaxParamStrLen);
	if(index==11) vst_strncpy (text, "Q Pres", kVstMaxParamStrLen);
	if(index==12) vst_strncpy (text, "T Stack", kVstMaxParamStrLen);
	if(index==13) vst_strncpy (text, "Crunch", kVstMaxParamStrLen);
	if(index==14) vst_strncpy (text, "High", kVstMaxParamStrLen);
	if(index==15) vst_strncpy (text, "Mega-Bl", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
bool ScorchCraft::getEffectName (char* name)
{
#ifdef COMPILER_MGW32
#if SCPREC==1
	vst_strncpy (name, "ScorchCrafter C120 LD (MGW32)", kVstMaxEffectNameLen);
#endif
#if SCPREC==2
	vst_strncpy (name, "ScorchCrafter C120 SD (MGW32)", kVstMaxEffectNameLen);
#endif
#if SCPREC==3
	vst_strncpy (name, "ScorchCrafter C120 HD (MGW32)", kVstMaxEffectNameLen);
#endif
#endif

#ifdef COMPILER_MGW64
#if SCPREC==1
	vst_strncpy (name, "ScorchCrafter C120 LD (MGW64)", kVstMaxEffectNameLen);
#endif
#if SCPREC==2
	vst_strncpy (name, "ScorchCrafter C120 SD (MGW64)", kVstMaxEffectNameLen);
#endif
#if SCPREC==3
	vst_strncpy (name, "ScorchCrafter C120 HD (MGW64)", kVstMaxEffectNameLen);
#endif
#endif

#ifdef COMPILER_MSOFT32
#if SCPREC==1
	vst_strncpy (name, "ScorchCrafter C120 LD (MSVC32)", kVstMaxEffectNameLen);
#else
	vst_strncpy (name, "ScorchCrafter C120 SD (MSVC32)", kVstMaxEffectNameLen);
#endif
#endif

#ifdef COMPILER_MSOFT64
#if SCPREC==1
	vst_strncpy (name, "ScorchCrafter C120 LD (MSVC64)", kVstMaxEffectNameLen);
#else
	vst_strncpy (name, "ScorchCrafter C120 SD (MSVC64)", kVstMaxEffectNameLen);
#endif
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ScorchCraft::getProductString (char* text)
{
#ifdef COMPILER_MGW32
#if SCPREC==1
	vst_strncpy (text, "S-C120Hmg32L", kVstMaxProductStrLen);
#endif
#if SCPREC==2
	vst_strncpy (text, "S-C120Hmg32S", kVstMaxProductStrLen);
#endif
#if SCPREC==3
	vst_strncpy (text, "S-C120Hmg32H", kVstMaxProductStrLen);
#endif
#endif

#ifdef COMPILER_MGW64
#if SCPREC==1
	vst_strncpy (text, "S-C120Hmg64L", kVstMaxProductStrLen);
#endif
#if SCPREC==2
	vst_strncpy (text, "S-C120Hmg64S", kVstMaxProductStrLen);
#endif
#if SCPREC==3
	vst_strncpy (text, "S-C120Hmg64H", kVstMaxProductStrLen);
#endif
#endif

#ifdef COMPILER_MSOFT32
#if SCPREC==1
	vst_strncpy (text, "S-C120Hms32L", kVstMaxProductStrLen);
#else
	vst_strncpy (text, "S-C120Hms32S", kVstMaxProductStrLen);
#endif
#endif

#ifdef COMPILER_MSOFT64
#if SCPREC==1
	vst_strncpy (text, "S-C120Hms64L", kVstMaxProductStrLen);
#else
	vst_strncpy (text, "S-C120Hms64S", kVstMaxProductStrLen);
#endif
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ScorchCraft::getVendorString (char* text)
{
	//! This will eventually be changed to the dev group name
	vst_strncpy (text, "Destructavator", kVstMaxVendorStrLen);
	return true;
}
//! -------------------------------------------------------------------------------------------------------
VstInt32 ScorchCraft::getVendorVersion ()
{
	return 3000;
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::setSampleRate (float smplRate)
{
	sRate = float (smplRate);

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the constructor accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);
	if(float (sRate) > float (768000.0f)) sRate=float (768000.0f);

	fadeMax = int (f9x (fadeFac) * f9x (sRate / 44100.0));
	fadeCount = fadeMax;

	qFade = f9x (f9x (5.0) / f9x (sRate));

	ZeroSamples();
	SetupLowPassFs();
	ZeroPassBuffers();
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::open()
{
	fadeCount = fadeMax;
	ZeroSamples();
	// SetupLowPassFs();
	ZeroPassBuffers();
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::close()
{
	fadeCount = fadeMax;
	ZeroSamples();
	// SetupLowPassFs();
	ZeroPassBuffers();
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::suspend()
{
	fadeCount = fadeMax;
	ZeroSamples();
	// SetupLowPassFs();
	ZeroPassBuffers();
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::resume()
{
	fadeCount = fadeMax;
	ZeroSamples();
	// SetupLowPassFs();
	ZeroPassBuffers();
}
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! Custom FX Functions -----------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::ZeroSamples()
{
	sample[0]=f9x (0.0);
	sample[1]=f9x (0.0);
	sample[2]=f9x (0.0);
	sample[3]=f9x (0.0);
	sample[4]=f9x (0.0);
	sample[5]=f9x (0.0);
	sample[6]=f9x (0.0);
	sample[7]=f9x (0.0);
	sample[8]=f9x (0.0);
	sample[9]=f9x (0.0);
	sample[10]=f9x (0.0);
	sample[11]=f9x (0.0);
	sample[12]=f9x (0.0);
	sample[13]=f9x (0.0);
	sample[14]=f9x (0.0);
	sample[15]=f9x (0.0);
}
//! -------------------------------------------------------------------------------------------------------
f9x ScorchCraft::RunFX(const f9x inpSpl)
{
	f9x output=f9x (0.0);
#if SCPREC==1
	f9x dstReadB = (f9x) fabsf(f9x (inpSpl));
#endif
#if SCPREC==2
	f9x dstReadB = (f9x) fabs(f9x (inpSpl));
#endif
#if SCPREC==3
	f9x dstReadB = (f9x) fabsl(f9x (inpSpl));
#endif
	if(f9x (dstReadB) > f9x (dstRead)) dstRead = f9x (dstReadB);
	else dstRead -= f9x (qFade);
	if(f9x (dstRead) < f9x (0.0)) dstRead = f9x (0.0);

	if(SampleDepthCurrent < 6) //! If sampling control is set to "mute" then don't bother processing anything
	{
		sample[0] = f9x (inpSpl);

		RunUpSampling();

		RunInputFilterSystem();

		if(SampleDepthCurrent > 0) RunDistortion(SampleDepthCurrent);

		RunDownSampling();

		RunOutputEqFilter();

		output = f9x (f9x (sample[0]) * f9x (6.0));
	}

	if(fadeCount > 0)
	{
		fadeCount--;
		output = f9x (0.0);
	}

	return f9x (output);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::CalcSampDepth()
{
	if(SampleDepthCurrent == 0) rateFac = f9x (1.0);		//! Bypass
	if(SampleDepthCurrent == 1) rateFac = f9x (1.0);		//! 1x (no oversampling)
	if(SampleDepthCurrent == 2) rateFac = f9x (2.0);		//! 2x
	if(SampleDepthCurrent == 3) rateFac = f9x (4.0);		//! 4x
	if(SampleDepthCurrent == 4) rateFac = f9x (8.0);		//! 8x
	if(SampleDepthCurrent == 5) rateFac = f9x (16.0);		//! 16x
	if(SampleDepthCurrent == 6) rateFac = f9x (1.0);		//! Mute

	SampleDepthOnline = int ((params[kSamplingOnline] * 6.f) + 1.f);
		if((int) SampleDepthOnline == (int) 7) SampleDepthOnline=(int) 0;
	SampleDepthOffline = int ((params[kSamplingRender] * 6.f) + 1.f);
		if((int) SampleDepthOffline == (int) 7) SampleDepthOffline=(int) 0;
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
		ZeroPassBuffers();
		ZeroSamples();
		fadeCount = fadeMax;
	}

	if(float (params[kAlgControlA] + params[kAlgControlB] + params[kAlgControlC] +
			params[kAlgControlD]) < float (0.8f)) params[kAlgControlB] = float (1.0f);

	if(params[kAlgControlA] == float (1.0f)) CheckChannelSelect(1);
	if(params[kAlgControlB] == float (1.0f)) CheckChannelSelect(2);
	if(params[kAlgControlC] == float (1.0f)) CheckChannelSelect(3);
	if(params[kAlgControlD] == float (1.0f)) CheckChannelSelect(4);

	tStackTypeSelection=1;
	if(float (params[kToneStackType]) < float (1.0f)) tStackTypeSelection=0;
	if(tStackTypeSelection != oldTStackSel)
	{
		oldTStackSel=tStackTypeSelection;
		ZeroPassBuffers();
		ZeroSamples();
		fadeCount = fadeMax;
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::ZeroPassBuffers()
{
	int x=0;
	int y=0;
	for(y=0;y<LPF_MAX_COUNT;y++)
	{
		for(x=0;x<=4;x++)
		{
			procA[y][x]=f9x (0.0);
			procB[y][x]=f9x (0.0);
			procC[y][x]=f9x (0.0);
		}
	}
	for(y=0;y<EQ_MAX_COUNT;y++)
	{
		EQprocA[y]=f9x (0.0);
		EQprocB[y]=f9x (0.0);
		EQprocC[y]=f9x (0.0);
	}

	for(x=0;x<4;x++)
	{
		for(y=0;y<5;y++)
		{
			suHist[x][y] = f9x (0.0);
			sdHist[x][y] = f9x (0.0);
		}
	}

	for(x=0;x<MAX_STAGES;x++)
	{
		dsFilter[x] = (f9x) 0.0;
		lastPoint[x][0] = f9x (0.0);
		lastPoint[x][1] = f9x (0.0);
		distP[x][0] = f9x (0.0);
		distP[x][1] = f9x (0.0);
	}

	dstRead = f9x (0.0);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::SetupLowPassFs()
{
	if(SampleDepthCurrent == 0) rateFac = f9x (1.0);
	if(SampleDepthCurrent == 1) rateFac = f9x (1.0);
	if(SampleDepthCurrent == 2) rateFac = f9x (2.0);
	if(SampleDepthCurrent == 3) rateFac = f9x (4.0);
	if(SampleDepthCurrent == 4) rateFac = f9x (8.0);
	if(SampleDepthCurrent == 5) rateFac = f9x (16.0);
	if(SampleDepthCurrent == 6) rateFac = f9x (1.0);

	int x=1;
	int y=1;
	for(y=0;y<LPF_MAX_COUNT;y++)
	{
		x=0;
			c[y][x]=f9x (f9x (1.0) / f9x ((f9x) tanl(f9x (PI) * f9x (cutOff[y][x]) / f9x (sRate * (1.0)))));
			procXX[y][x] =f9x (f9x (1.0) / f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) + f9x (damp[y][x])))));
			proc1[y][x]=f9x (f9x (2.0) * f9x (f9x (1.0) - f9x (f9x (c[y][x]) * f9x (c[y][x]))) * f9x (procXX[y][x]));
			proc0[y][x]=f9x (f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) - f9x (damp[y][x])))) * f9x (procXX[y][x]));
		x=1;
			c[y][x]=f9x (f9x (1.0) / f9x ((f9x) tanl(f9x (PI) * f9x (cutOff[y][x]) / f9x (sRate * (2.0)))));
			procXX[y][x] =f9x (f9x (1.0) / f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) + f9x (damp[y][x])))));
			proc1[y][x]=f9x (f9x (2.0) * f9x (f9x (1.0) - f9x (f9x (c[y][x]) * f9x (c[y][x]))) * f9x (procXX[y][x]));
			proc0[y][x]=f9x (f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) - f9x (damp[y][x])))) * f9x (procXX[y][x]));
		x=2;
			c[y][x]=f9x (f9x (1.0) / f9x ((f9x) tanl(f9x (PI) * f9x (cutOff[y][x]) / f9x (sRate * (4.0)))));
			procXX[y][x] =f9x (f9x (1.0) / f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) + f9x (damp[y][x])))));
			proc1[y][x]=f9x (f9x (2.0) * f9x (f9x (1.0) - f9x (f9x (c[y][x]) * f9x (c[y][x]))) * f9x (procXX[y][x]));
			proc0[y][x]=f9x (f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) - f9x (damp[y][x])))) * f9x (procXX[y][x]));
		x=3;
			c[y][x]=f9x (f9x (1.0) / f9x ((f9x) tanl(f9x (PI) * f9x (cutOff[y][x]) / f9x (sRate * (8.0)))));
			procXX[y][x] =f9x (f9x (1.0) / f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) + f9x (damp[y][x])))));
			proc1[y][x]=f9x (f9x (2.0) * f9x (f9x (1.0) - f9x (f9x (c[y][x]) * f9x (c[y][x]))) * f9x (procXX[y][x]));
			proc0[y][x]=f9x (f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) - f9x (damp[y][x])))) * f9x (procXX[y][x]));
		x=4;
			c[y][x]=f9x (f9x (1.0) / f9x ((f9x) tanl(f9x (PI) * f9x (cutOff[y][x]) / f9x (sRate * (16.0)))));
			procXX[y][x] =f9x (f9x (1.0) / f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) + f9x (damp[y][x])))));
			proc1[y][x]=f9x (f9x (2.0) * f9x (f9x (1.0) - f9x (f9x (c[y][x]) * f9x (c[y][x]))) * f9x (procXX[y][x]));
			proc0[y][x]=f9x (f9x (f9x (1.0) + f9x (f9x (c[y][x]) * f9x (f9x (c[y][x]) - f9x (damp[y][x])))) * f9x (procXX[y][x]));
	}

	for(y=0;y<EQ_MAX_COUNT;y++)
	{
		EQc[y]=f9x (f9x (1.0) / f9x ((f9x) tanl(f9x (PI) * f9x (EQcutOff[y]) / f9x (sRate))));
		EQprocXX[y] =f9x (f9x (1.0) / f9x (f9x (1.0) + f9x (f9x (EQc[y]) * f9x (f9x (EQc[y]) + f9x (EQdamp[y])))));
		EQproc1[y]=f9x (f9x (2.0) * f9x (f9x (1.0) - f9x (f9x (EQc[y]) * f9x (EQc[y]))) * f9x (EQprocXX[y]));
		EQproc0[y]=f9x (f9x (f9x (1.0) + f9x (f9x (EQc[y]) * f9x (f9x (EQc[y]) - f9x (EQdamp[y])))) * f9x (EQprocXX[y]));
	}

	suT[0][0] = f9x (f9x (4.0) * f9x (sRate) * f9x (sRate));
	suT[0][1] = f9x (f9x (4.0) * f9x (sRate * 2.0) * f9x (sRate * 2.0));
	suT[0][2] = f9x (f9x (4.0) * f9x (sRate * 4.0) * f9x (sRate * 4.0));
	suT[0][3] = f9x (f9x (4.0) * f9x (sRate * 8.0) * f9x (sRate * 8.0));
	suT[0][4] = f9x (f9x (4.0) * f9x (sRate * 16.0) * f9x (sRate * 16.0));

	suT[1][0] = f9x (f9x (8.0) * f9x (sRate) * f9x (sRate));
	suT[1][1] = f9x (f9x (8.0) * f9x (sRate * 2.0) * f9x (sRate * 2.0));
	suT[1][2] = f9x (f9x (8.0) * f9x (sRate * 4.0) * f9x (sRate * 4.0));
	suT[1][3] = f9x (f9x (8.0) * f9x (sRate * 8.0) * f9x (sRate * 8.0));
	suT[1][4] = f9x (f9x (8.0) * f9x (sRate * 16.0) * f9x (sRate * 16.0));

	suT[2][0] = f9x (f9x (2.0) * f9x (sRate));
	suT[2][1] = f9x (f9x (2.0) * f9x (sRate * 2.0));
	suT[2][2] = f9x (f9x (2.0) * f9x (sRate * 4.0));
	suT[2][3] = f9x (f9x (2.0) * f9x (sRate * 8.0));
	suT[2][4] = f9x (f9x (2.0) * f9x (sRate * 16.0));

	suT[3][0] = f9x (f9x (PI) / f9x (sRate));
	suT[3][1] = f9x (f9x (PI) / f9x (sRate * 2.0));
	suT[3][2] = f9x (f9x (PI) / f9x (sRate * 4.0));
	suT[3][3] = f9x (f9x (PI) / f9x (sRate * 8.0));
	suT[3][4] = f9x (f9x (PI) / f9x (sRate * 16.0));

	sCutoff[0] = f9x (f9x (sRate) * f9x (0.2250));
	sCutoff[1] = f9x (f9x (sRate) * f9x (0.2250));
	sCutoff[2] = f9x (f9x (sRate) * f9x (0.2250));
	sCutoff[3] = f9x (f9x (sRate) * f9x (0.2250));
	sCutoff[4] = f9x (f9x (sRate) * f9x (0.2250));

	wp[0] = f9x (f9x (suT[2][0]) * (f9x) tanl(f9x (suT[3][0]) * f9x (sCutoff[0])));
	wp[1] = f9x (f9x (suT[2][1]) * (f9x) tanl(f9x (suT[3][1]) * f9x (sCutoff[1])));
	wp[2] = f9x (f9x (suT[2][2]) * (f9x) tanl(f9x (suT[3][2]) * f9x (sCutoff[2])));
	wp[3] = f9x (f9x (suT[2][3]) * (f9x) tanl(f9x (suT[3][3]) * f9x (sCutoff[3])));
	wp[4] = f9x (f9x (suT[2][4]) * (f9x) tanl(f9x (suT[3][4]) * f9x (sCutoff[4])));

	f9x b1, b2, bd, bd_tmp;

	for(x = 0; x < 5; x++)
	{
		b1 = f9x (f9x (0.7653670) / f9x (wp[x]));
		b2 = f9x (f9x (1.0) / f9x (f9x (wp[x]) * f9x (wp[x])));
		bd_tmp = f9x (f9x (f9x (suT[0][x]) * f9x (b2)) + f9x (1.0));
		bd = f9x (f9x (1.0) / f9x (f9x (bd_tmp) + f9x (f9x (suT[2][x]) * f9x (b1))));
		scGain[x] = f9x (f9x (bd) * f9x (1.0));
		suCoef[2][x] = f9x (f9x (2.0) - f9x (f9x (suT[1][x]) * f9x (b2)));
		suCoef[0][x] = f9x (f9x (suCoef[2][x]) * f9x (bd));
		suCoef[1][x] = f9x (f9x (f9x (bd_tmp) - f9x (f9x (suT[2][x]) * f9x (b1))) * f9x (bd));
		b1 = f9x (f9x (1.8477590) / f9x (wp[x]));
		bd = f9x (f9x (1.0) / f9x (f9x (bd_tmp) + f9x (f9x (suT[2][x]) * f9x (b1))));
		scGain[x] *= f9x (bd);
		suCoef[2][x] *= f9x (bd);
		suCoef[3][x] = f9x (f9x (f9x (bd_tmp) - f9x (f9x (suT[2][x]) * f9x (b1))) * f9x (bd));
	}

	for(x = 0; x < 4; x++)
	{
		for(y = 0; y < 5; y++)
		{
			sdT[x][y] = f9x (suT[x][y]);
			sdCoef[x][y] = f9x (suCoef[x][y]);
		}
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::LoadLPassCutOffs()
{
	int x=0;
	for(x=0;x<=4;x++)
	{	//! Values in comments are old values that have been tried before
		//! "damp" refers to how sharp an LP filter is, higher is more dull, low is sharp
		//! "cutoff" is the frequency in Hz.
		//! -----------------------------------------------------------------------------
		//! This first set of filters is for the pre-distortion input.
		damp[LPF_Bottom_In][x]=f9x (0.8750); // 1.0
		cutOff[LPF_Bottom_In][x]=f9x (135.0); // 115.0, 75.0, 95.0, 125.0
		damp[LPF_Filter_I][x]=f9x (1.0); // 1.0, 0.9250
		cutOff[LPF_Filter_I][x]=f9x (420.0); // 495.0, 545.0, 450.0, 475.0, 465.0
		damp[LPF_Filter_Ib][x]=f9x (4.0);
		cutOff[LPF_Filter_Ib][x]=f9x (20000.0); // 20000.0, 16000.0
		damp[LPF_Filter_Xlow][x]=f9x (0.750); // 1.0
		cutOff[LPF_Filter_Xlow][x]=f9x (420.0); // 650.0, 625.0, 475.0
		damp[LPF_Filter_Xhigh][x]=f9x (4.0); // 6.0, 4.0
		cutOff[LPF_Filter_Xhigh][x]=f9x (650.0); // 3000.0, 5000.0, 3600.0
		damp[LPF_Top_Ceil][x]=f9x (4.0); // 4.0, 16.0, 8.0, 18.0, 12.0
		cutOff[LPF_Top_Ceil][x]=f9x (7200.0); // 3600.0
	}
	//! These are output EQ controls, adjusted with the five knobs
	EQdamp[EQ_ToneStack_Low]=f9x (4.0);
	EQcutOff[EQ_ToneStack_Low]=f9x (425.0); // 850.0, 650.0, 450.0
	EQdamp[EQ_ToneStack_High]=f9x (4.0);
	EQcutOff[EQ_ToneStack_High]=f9x (1800.0); // 2000.0
	EQdamp[EQ_ToneStack_Pres]=f9x (4.0); // 16.0
	EQcutOff[EQ_ToneStack_Pres]=f9x (6600.0); // 7200.0
	EQdamp[EQ_ToneStack_BottQ]=f9x (1.0); // 1.0
	EQcutOff[EQ_ToneStack_BottQ]=f9x (165.0); // 165.0
	EQdamp[EQ_ToneStack_CeilA]=f9x (3.0);
	EQcutOff[EQ_ToneStack_CeilA]=f9x (18000.0);
	EQdamp[EQ_ToneStack_CeilB]=f9x (3.0);
	EQcutOff[EQ_ToneStack_CeilB]=f9x (18000.0);
}
//! -------------------------------------------------------------------------------------------------------
f9x ScorchCraft::RunLowPass(const LOWPASS_LIST LowPassID, int smpLev, f9x inputS)
{
	if(smpLev < 0) smpLev = 0;
	if(smpLev > 4) smpLev = 4;
	if(f9x (inputS) == f9x (0.0)) inputS = f9x (zeroFudge);

	procA[LowPassID][smpLev]=f9x (f9x (f9x (procXX[LowPassID][smpLev]) * f9x (inputS)) - f9x (f9x (proc1[LowPassID][smpLev]) * f9x (procB[LowPassID][smpLev])) - f9x (f9x (proc0[LowPassID][smpLev]) * f9x (procC[LowPassID][smpLev])));
	inputS=f9x (f9x (procA[LowPassID][smpLev]) + f9x (procB[LowPassID][smpLev]) + f9x (procB[LowPassID][smpLev]) + f9x (procC[LowPassID][smpLev]));

	procC[LowPassID][smpLev]=f9x (procB[LowPassID][smpLev]);
	procB[LowPassID][smpLev]=f9x (procA[LowPassID][smpLev]);

	return f9x (inputS);
}
//! -------------------------------------------------------------------------------------------------------
f9x ScorchCraft::RunEQLPass(const EQ_LIST EqID, f9x inputS)
{
	if(f9x (inputS) == f9x (0.0)) inputS = f9x (zeroFudge);

	EQprocA[EqID]=f9x (f9x (f9x (EQprocXX[EqID]) * f9x (inputS)) - f9x (f9x (EQproc1[EqID]) * f9x (EQprocB[EqID])) - f9x (f9x (EQproc0[EqID]) * f9x (EQprocC[EqID])));
	inputS=f9x (f9x (EQprocA[EqID]) + f9x (EQprocB[EqID]) + f9x (EQprocB[EqID]) + f9x (EQprocC[EqID]));

	EQprocC[EqID]=f9x (EQprocB[EqID]);
	EQprocB[EqID]=f9x (EQprocA[EqID]);

	return f9x (inputS);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::RunUpSampling()
{
	if(SampleDepthCurrent==6) return; //! If (sampling == mute) then (don't bother to continue from here);
	if(SampleDepthCurrent==0) return; //! If (sampling == bypass) then (don't bother to continue from here);
	if(SampleDepthCurrent>=2)
	{
		sample[1]=f9x (0.0);
		sample[0]=(f9x) RunUpSampleFilter(0, f9x (sample[0]));
		sample[1]=(f9x) RunUpSampleFilter(0, f9x (sample[1]));
	}
	if(SampleDepthCurrent>=3)
	{
		sample[2]=f9x (sample[1]);
		sample[1]=f9x (0.0);
		sample[3]=f9x (0.0);
		sample[0]=(f9x) RunUpSampleFilter(1, f9x (sample[0]));
		sample[1]=(f9x) RunUpSampleFilter(1, f9x (sample[1]));
		sample[2]=(f9x) RunUpSampleFilter(1, f9x (sample[2]));
		sample[3]=(f9x) RunUpSampleFilter(1, f9x (sample[3]));
	}
	if(SampleDepthCurrent>=4)
	{
		sample[6]=f9x (sample[3]);
		sample[4]=f9x (sample[2]);
		sample[2]=f9x (sample[1]);
		sample[1]=f9x (0.0);
		sample[3]=f9x (0.0);
		sample[5]=f9x (0.0);
		sample[7]=f9x (0.0);
		sample[0]=(f9x) RunUpSampleFilter(2, f9x (sample[0]));
		sample[1]=(f9x) RunUpSampleFilter(2, f9x (sample[1]));
		sample[2]=(f9x) RunUpSampleFilter(2, f9x (sample[2]));
		sample[3]=(f9x) RunUpSampleFilter(2, f9x (sample[3]));
		sample[4]=(f9x) RunUpSampleFilter(2, f9x (sample[4]));
		sample[5]=(f9x) RunUpSampleFilter(2, f9x (sample[5]));
		sample[6]=(f9x) RunUpSampleFilter(2, f9x (sample[6]));
		sample[7]=(f9x) RunUpSampleFilter(2, f9x (sample[7]));
	}
	if(SampleDepthCurrent==5)
	{
		sample[14]=f9x (sample[7]);
		sample[12]=f9x (sample[6]);
		sample[10]=f9x (sample[5]);
		sample[8]=f9x (sample[4]);
		sample[6]=f9x (sample[3]);
		sample[4]=f9x (sample[2]);
		sample[2]=f9x (sample[1]);
		sample[1]=f9x (0.0);
		sample[3]=f9x (0.0);
		sample[5]=f9x (0.0);
		sample[7]=f9x (0.0);
		sample[9]=f9x (0.0);
		sample[11]=f9x (0.0);
		sample[13]=f9x (0.0);
		sample[15]=f9x (0.0);
		sample[0]=(f9x) RunUpSampleFilter(3, f9x (sample[0]));
		sample[1]=(f9x) RunUpSampleFilter(3, f9x (sample[1]));
		sample[2]=(f9x) RunUpSampleFilter(3, f9x (sample[2]));
		sample[3]=(f9x) RunUpSampleFilter(3, f9x (sample[3]));
		sample[4]=(f9x) RunUpSampleFilter(3, f9x (sample[4]));
		sample[5]=(f9x) RunUpSampleFilter(3, f9x (sample[5]));
		sample[6]=(f9x) RunUpSampleFilter(3, f9x (sample[6]));
		sample[7]=(f9x) RunUpSampleFilter(3, f9x (sample[7]));
		sample[8]=(f9x) RunUpSampleFilter(3, f9x (sample[8]));
		sample[9]=(f9x) RunUpSampleFilter(3, f9x (sample[9]));
		sample[10]=(f9x) RunUpSampleFilter(3, f9x (sample[10]));
		sample[11]=(f9x) RunUpSampleFilter(3, f9x (sample[11]));
		sample[12]=(f9x) RunUpSampleFilter(3, f9x (sample[12]));
		sample[13]=(f9x) RunUpSampleFilter(3, f9x (sample[13]));
		sample[14]=(f9x) RunUpSampleFilter(3, f9x (sample[14]));
		sample[15]=(f9x) RunUpSampleFilter(3, f9x (sample[15]));
	}
}
//! -------------------------------------------------------------------------------------------------------
f9x ScorchCraft::RunUpSampleFilter(const int sLevel, f9x inputS)
{
	if(f9x (inputS) == f9x (0.0)) inputS = f9x (zeroFudge);

	f9x outputS = f9x (f9x (inputS) * f9x (scGain[sLevel+1]));
	outputS -= f9x (f9x (suHist[0][sLevel+1]) * f9x (suCoef[0][sLevel+1]));
	f9x new_hist = f9x (f9x (outputS) - f9x (f9x (suHist[1][sLevel+1]) * f9x (suCoef[1][sLevel+1])));

	outputS = f9x (f9x (new_hist) + f9x (f9x (suHist[0][sLevel+1]) * f9x (2.0)));
	outputS += f9x (suHist[1][sLevel+1]);

	suHist[1][sLevel+1] = f9x (suHist[0][sLevel+1]);
	suHist[0][sLevel+1] = f9x (new_hist);

	outputS -= f9x (f9x (suHist[2][sLevel+1]) * f9x (suCoef[2][sLevel+1]));
	new_hist = f9x (f9x (outputS) - f9x (f9x (suHist[3][sLevel+1]) * f9x (suCoef[3][sLevel+1])));

	outputS = f9x (f9x (new_hist) + f9x (f9x (suHist[2][sLevel+1]) * f9x (2.0)));
	outputS += f9x (suHist[3][sLevel+1]);

	suHist[3][sLevel+1] = f9x (suHist[2][sLevel+1]);
	suHist[2][sLevel+1] = f9x (new_hist);

	return f9x (f9x (outputS) * f9x (2.0));
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::RunDownSampling()
{
	if(SampleDepthCurrent==6) return; //! If (sampling == mute) then (don't bother to continue from here);
	if(SampleDepthCurrent==0) return; //! If (sampling == bypass) then (don't bother to continue from here);
	if(SampleDepthCurrent>=5)
	{
		sample[0]=(f9x) RunDownSampleFilter(3, f9x (sample[0]));
		sample[1]=(f9x) RunDownSampleFilter(3, f9x (sample[1]));
		sample[2]=(f9x) RunDownSampleFilter(3, f9x (sample[2]));
		sample[3]=(f9x) RunDownSampleFilter(3, f9x (sample[3]));
		sample[4]=(f9x) RunDownSampleFilter(3, f9x (sample[4]));
		sample[5]=(f9x) RunDownSampleFilter(3, f9x (sample[5]));
		sample[6]=(f9x) RunDownSampleFilter(3, f9x (sample[6]));
		sample[7]=(f9x) RunDownSampleFilter(3, f9x (sample[7]));
		sample[8]=(f9x) RunDownSampleFilter(3, f9x (sample[8]));
		sample[9]=(f9x) RunDownSampleFilter(3, f9x (sample[9]));
		sample[10]=(f9x) RunDownSampleFilter(3, f9x (sample[10]));
		sample[11]=(f9x) RunDownSampleFilter(3, f9x (sample[11]));
		sample[12]=(f9x) RunDownSampleFilter(3, f9x (sample[12]));
		sample[13]=(f9x) RunDownSampleFilter(3, f9x (sample[13]));
		sample[14]=(f9x) RunDownSampleFilter(3, f9x (sample[14]));
		sample[15]=(f9x) RunDownSampleFilter(3, f9x (sample[15]));
/*		sample[14] = f9x (f9x (f9x (sample[14]) + f9x (sample[15])) * f9x (0.50));
		sample[12] = f9x (f9x (f9x (sample[12]) + f9x (sample[13])) * f9x (0.50));
		sample[10] = f9x (f9x (f9x (sample[10]) + f9x (sample[11])) * f9x (0.50));
		sample[8] = f9x (f9x (f9x (sample[8]) + f9x (sample[9])) * f9x (0.50));
		sample[6] = f9x (f9x (f9x (sample[6]) + f9x (sample[7])) * f9x (0.50));
		sample[4] = f9x (f9x (f9x (sample[4]) + f9x (sample[5])) * f9x (0.50));
		sample[2] = f9x (f9x (f9x (sample[2]) + f9x (sample[3])) * f9x (0.50));
		sample[0] = f9x (f9x (f9x (sample[0]) + f9x (sample[1])) * f9x (0.50)); */
		sample[1]=f9x (sample[2]);
		sample[2]=f9x (sample[4]);
		sample[3]=f9x (sample[6]);
		sample[4]=f9x (sample[8]);
		sample[5]=f9x (sample[10]);
		sample[6]=f9x (sample[12]);
		sample[7]=f9x (sample[14]);
	}
	if(SampleDepthCurrent>=4)
	{
		sample[0]=(f9x) RunDownSampleFilter(2, f9x (sample[0]));
		sample[1]=(f9x) RunDownSampleFilter(2, f9x (sample[1]));
		sample[2]=(f9x) RunDownSampleFilter(2, f9x (sample[2]));
		sample[3]=(f9x) RunDownSampleFilter(2, f9x (sample[3]));
		sample[4]=(f9x) RunDownSampleFilter(2, f9x (sample[4]));
		sample[5]=(f9x) RunDownSampleFilter(2, f9x (sample[5]));
		sample[6]=(f9x) RunDownSampleFilter(2, f9x (sample[6]));
		sample[7]=(f9x) RunDownSampleFilter(2, f9x (sample[7]));
/*		sample[6] = f9x (f9x (f9x (sample[6]) + f9x (sample[7])) * f9x (0.50));
		sample[4] = f9x (f9x (f9x (sample[4]) + f9x (sample[5])) * f9x (0.50));
		sample[2] = f9x (f9x (f9x (sample[2]) + f9x (sample[3])) * f9x (0.50));
		sample[0] = f9x (f9x (f9x (sample[0]) + f9x (sample[1])) * f9x (0.50)); */
		sample[1]=f9x (sample[2]);
		sample[2]=f9x (sample[4]);
		sample[3]=f9x (sample[6]);
	}
	if(SampleDepthCurrent>=3)
	{
		sample[0]=(f9x) RunDownSampleFilter(1, f9x (sample[0]));
		sample[1]=(f9x) RunDownSampleFilter(1, f9x (sample[1]));
		sample[2]=(f9x) RunDownSampleFilter(1, f9x (sample[2]));
		sample[3]=(f9x) RunDownSampleFilter(1, f9x (sample[3]));
//		sample[2] = f9x (f9x (f9x (sample[2]) + f9x (sample[3])) * f9x (0.50));
//		sample[0] = f9x (f9x (f9x (sample[0]) + f9x (sample[1])) * f9x (0.50));
		sample[1]=f9x (sample[2]);
	}
	if(SampleDepthCurrent>=2)
	{
		sample[0]=(f9x) RunDownSampleFilter(0, f9x (sample[0]));
		sample[1]=(f9x) RunDownSampleFilter(0, f9x (sample[1]));
//		sample[0] = f9x (f9x (f9x (sample[0]) + f9x (sample[1])) * f9x (0.50));
	}
	// sample[0]=(f9x) RunDownSampleFilter(-1, f9x (sample[0]));
}
//! -------------------------------------------------------------------------------------------------------
f9x ScorchCraft::RunDownSampleFilter(const int sLevel, f9x inputS)
{
	if(f9x (inputS) == f9x (0.0)) inputS=f9x (zeroFudge);

	f9x outputS = f9x (f9x (inputS) * f9x (scGain[sLevel+1]));
	outputS -= f9x (f9x (sdHist[0][sLevel+1]) * f9x (sdCoef[0][sLevel+1]));
	f9x new_hist = f9x (f9x (outputS) - f9x (f9x (sdHist[1][sLevel+1]) * f9x (sdCoef[1][sLevel+1])));

	outputS = f9x (f9x (new_hist) + f9x (f9x (sdHist[0][sLevel+1]) * f9x (2.0)));
	outputS += f9x (sdHist[1][sLevel+1]);

	sdHist[1][sLevel+1] = f9x (sdHist[0][sLevel+1]);
	sdHist[0][sLevel+1] = f9x (new_hist);

	outputS -= f9x (f9x (sdHist[2][sLevel+1]) * f9x (sdCoef[2][sLevel+1]));
	new_hist = f9x (f9x (outputS) - f9x (f9x (sdHist[3][sLevel+1]) * f9x (sdCoef[3][sLevel+1])));

	outputS = f9x (f9x (new_hist) + f9x (f9x (sdHist[2][sLevel+1]) * f9x (2.0)));
	outputS += f9x (sdHist[3][sLevel+1]);

	sdHist[3][sLevel+1] = f9x (sdHist[2][sLevel+1]);
	sdHist[2][sLevel+1] = f9x (new_hist);

	return f9x (outputS);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::RunInputFilterSystem()
{
	switch(SampleDepthCurrent)
	{
		case 0:
		RunInputFilter(0,0);
		break;
		case 1:
		RunInputFilter(0,0);
		break;
		case 2:
		RunInputFilter(1,0);
		RunInputFilter(1,1);
		break;
		case 3:
		RunInputFilter(2,0);
		RunInputFilter(2,1);
		RunInputFilter(2,2);
		RunInputFilter(2,3);
		break;
		case 4:
		RunInputFilter(3,0);
		RunInputFilter(3,1);
		RunInputFilter(3,2);
		RunInputFilter(3,3);
		RunInputFilter(3,4);
		RunInputFilter(3,5);
		RunInputFilter(3,6);
		RunInputFilter(3,7);
		break;
		case 5:
		RunInputFilter(4,0);
		RunInputFilter(4,1);
		RunInputFilter(4,2);
		RunInputFilter(4,3);
		RunInputFilter(4,4);
		RunInputFilter(4,5);
		RunInputFilter(4,6);
		RunInputFilter(4,7);
		RunInputFilter(4,8);
		RunInputFilter(4,9);
		RunInputFilter(4,10);
		RunInputFilter(4,11);
		RunInputFilter(4,12);
		RunInputFilter(4,13);
		RunInputFilter(4,14);
		RunInputFilter(4,15);
		break;
		case 6:
		break;
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::RunInputFilter(const int sLev, const int sampNum)
{
	f9x splow = (f9x) RunLowPass(LPF_Bottom_In, sLev, f9x (sample[sampNum]));
	f9x sphigh = (f9x) RunLowPass(LPF_Filter_Ib, sLev, f9x (sample[sampNum]));
	sample[sampNum] = f9x (f9x (sphigh) - f9x (splow));

	splow = (f9x) RunLowPass(LPF_Filter_I, sLev, f9x (sample[sampNum]));
	sphigh = (f9x) RunLowPass(LPF_Top_Ceil, sLev, f9x (sample[sampNum]));
	sample[sampNum] = f9x (f9x (sphigh) - f9x (f9x (splow) * f9x (params[kFilterI])));

	splow = (f9x) RunLowPass(LPF_Filter_Xlow, sLev, f9x (sample[sampNum]));
	f9x spmid = (f9x) RunLowPass(LPF_Filter_Xhigh, sLev, f9x (sample[sampNum]));
	spmid -= f9x (splow);
	spmid *= f9x (params[kFilterX] * dstRead);
	sample[sampNum] += f9x (spmid);
	sample[sampNum] *= f9x (f9x (1.0) - f9x (f9x (0.50) * f9x (params[kFilterX])));
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::RunDistortion(const int sLev)
{
	if(SampleDepthCurrent==6) return; //! If (sampling == mute) then (don't bother to continue from here);
	if(SampleDepthCurrent==0) return; //! If (sampling == bypass) then (don't bother to continue from here);

	if(cleanGainChannelActive == true)
	{
		DistortTypeClean(sLev, 0);
		if(sLev > 1) DistortTypeClean(sLev, 1);
		if(sLev > 2)
		{
			DistortTypeClean(sLev, 2);
			DistortTypeClean(sLev, 3);
		}
		if(sLev > 3)
		{
			DistortTypeClean(sLev, 4);
			DistortTypeClean(sLev, 5);
			DistortTypeClean(sLev, 6);
			DistortTypeClean(sLev, 7);
		}
		if(sLev > 4)
		{
			DistortTypeClean(sLev, 8);
			DistortTypeClean(sLev, 9);
			DistortTypeClean(sLev, 10);
			DistortTypeClean(sLev, 11);
			DistortTypeClean(sLev, 12);
			DistortTypeClean(sLev, 13);
			DistortTypeClean(sLev, 14);
			DistortTypeClean(sLev, 15);
		}
		return;
	}
	if(crunchGainChannelActive == true)
	{
		DistortTypeCrunch(sLev, 0);
		if(sLev > 1) DistortTypeCrunch(sLev, 1);
		if(sLev > 2)
		{
			DistortTypeCrunch(sLev, 2);
			DistortTypeCrunch(sLev, 3);
		}
		if(sLev > 3)
		{
			DistortTypeCrunch(sLev, 4);
			DistortTypeCrunch(sLev, 5);
			DistortTypeCrunch(sLev, 6);
			DistortTypeCrunch(sLev, 7);
		}
		if(sLev > 4)
		{
			DistortTypeCrunch(sLev, 8);
			DistortTypeCrunch(sLev, 9);
			DistortTypeCrunch(sLev, 10);
			DistortTypeCrunch(sLev, 11);
			DistortTypeCrunch(sLev, 12);
			DistortTypeCrunch(sLev, 13);
			DistortTypeCrunch(sLev, 14);
			DistortTypeCrunch(sLev, 15);
		}
		return;
	}
	if(highGainChannelActive == true)
	{
		DistortTypeHigh(sLev, 0);
		if(sLev > 1) DistortTypeHigh(sLev, 1);
		if(sLev > 2)
		{
			DistortTypeHigh(sLev, 2);
			DistortTypeHigh(sLev, 3);
		}
		if(sLev > 3)
		{
			DistortTypeHigh(sLev, 4);
			DistortTypeHigh(sLev, 5);
			DistortTypeHigh(sLev, 6);
			DistortTypeHigh(sLev, 7);
		}
		if(sLev > 4)
		{
			DistortTypeHigh(sLev, 8);
			DistortTypeHigh(sLev, 9);
			DistortTypeHigh(sLev, 10);
			DistortTypeHigh(sLev, 11);
			DistortTypeHigh(sLev, 12);
			DistortTypeHigh(sLev, 13);
			DistortTypeHigh(sLev, 14);
			DistortTypeHigh(sLev, 15);
		}
		return;
	}
	if(megaGainChannelActive == true)
	{
		DistortTypeMega(sLev, 0);
		if(sLev > 1) DistortTypeMega(sLev, 1);
		if(sLev > 2)
		{
			DistortTypeMega(sLev, 2);
			DistortTypeMega(sLev, 3);
		}
		if(sLev > 3)
		{
			DistortTypeMega(sLev, 4);
			DistortTypeMega(sLev, 5);
			DistortTypeMega(sLev, 6);
			DistortTypeMega(sLev, 7);
		}
		if(sLev > 4)
		{
			DistortTypeMega(sLev, 8);
			DistortTypeMega(sLev, 9);
			DistortTypeMega(sLev, 10);
			DistortTypeMega(sLev, 11);
			DistortTypeMega(sLev, 12);
			DistortTypeMega(sLev, 13);
			DistortTypeMega(sLev, 14);
			DistortTypeMega(sLev, 15);
		}
		return;
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::DistortTypeClean(const int sLevI, const int sNum)
{
	//! Set up input
	f9x spl = (f9x) (f9x (sample[sNum]));

	//! Check for illegal, out-of-range values
	if((f9x) spl > (f9x) 1.0) spl = (f9x) 1.0;
	if((f9x) spl < (f9x) -1.0) spl = (f9x) -1.0;

	//! Set up how much distortion is applied (according to user-controlled parameters)
	const f9x gain = f9x (f9x ((f9x) params[kGainControl] * f9x (0.9999950)) + f9x (0.00000250));
	const f9x dGain = f9x (f9x (1.0) - f9x (f9x (f9x (1.0) - f9x (gain)) * f9x (f9x (1.0) - f9x (gain))));

	//! Drive Stage
	const f9x filterFac= (f9x) ((f9x) 1.0 - (f9x) expl((f9x) -2.0 * (f9x) PI *
							(f9x) 18000.0 / (f9x) ((f9x) sRate * (f9x) rateFac)));

	for(int x = 0; x < DIST_STAGES_CLEAN; x++)
	{
		if((f9x) spl != (f9x) 0.00)
		{
			spl *= f9x (dGain);

			if((f9x) spl > (f9x) 0.00)
			{
				lastPoint[x][1] = f9x (0.0);

				spl = (f9x) RunTubeA(x, 0, f9x (spl));
			}
			if((f9x) spl < (f9x) 0.00)
			{
				lastPoint[x][0] = f9x (0.0);

				spl = (f9x) -spl;
				spl = (f9x) RunTubeA(x, 1, f9x (spl));
				spl = (f9x) -spl;
			}
		} else
		{
			spl = (f9x) zeroFudge;
			lastPoint[x][0] = f9x (0.0);
			lastPoint[x][1] = f9x (0.0);
		}
		spl = (f9x) (dsFilter[x] += (f9x) ((f9x) ((f9x) spl - (f9x) dsFilter[x]) * (f9x) filterFac));
	}

	//! Give a little "breathing room" for output filters
	spl *= (f9x) 0.4950; // 0.450, 0.250

	//! Output of completed sample.  We're done!
	sample[sNum] = f9x (spl);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::DistortTypeCrunch(const int sLevI, const int sNum)
{
	//! Set up input
	f9x spl = (f9x) (f9x (sample[sNum]));

	//! Check for illegal, out-of-range values
	if((f9x) spl > (f9x) 1.0) spl = (f9x) 1.0;
	if((f9x) spl < (f9x) -1.0) spl = (f9x) -1.0;

	//! Set up how much distortion is applied (according to user-controlled parameters)
	const f9x gain = f9x (f9x ((f9x) params[kGainControl] * f9x (0.9999950)) + f9x (0.00000250));
	const f9x dGain = f9x (f9x (1.0) - f9x (f9x (f9x (1.0) - f9x (gain)) * f9x (f9x (1.0) - f9x (gain))));

	//! Drive Stage
	const f9x filterFac= (f9x) ((f9x) 1.0 - (f9x) expl((f9x) -2.0 * (f9x) PI *
							(f9x) 16000.0 / (f9x) ((f9x) sRate * (f9x) rateFac)));

	for(int x = 0; x < DIST_STAGES_CRUNCH; x++)
	{
		if((f9x) spl != (f9x) 0.00)
		{
			spl *= f9x (dGain);

			if((f9x) spl > (f9x) 0.00)
			{
				lastPoint[x][1] = f9x (0.0);

				spl = (f9x) RunTubeA(x, 0, f9x (spl));
			}
			if((f9x) spl < (f9x) 0.00)
			{
				lastPoint[x][0] = f9x (0.0);

				spl = (f9x) -spl;
				spl = (f9x) RunTubeA(x, 1, f9x (spl));
				spl = (f9x) -spl;
			}
		} else
		{
			spl = (f9x) zeroFudge;
			lastPoint[x][0] = f9x (0.0);
			lastPoint[x][1] = f9x (0.0);
		}
		spl = (f9x) (dsFilter[x] += (f9x) ((f9x) ((f9x) spl - (f9x) dsFilter[x]) * (f9x) filterFac));
	}

	//! Give a little "breathing room" for output filters
	spl *= (f9x) 0.4950; // 0.450, 0.250

	//! Output of completed sample.  We're done!
	sample[sNum] = f9x (spl);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::DistortTypeHigh(const int sLevI, const int sNum)
{
	//! Set up input
	f9x spl = (f9x) (f9x (sample[sNum]));

	//! Check for illegal, out-of-range values
	if((f9x) spl > (f9x) 1.0) spl = (f9x) 1.0;
	if((f9x) spl < (f9x) -1.0) spl = (f9x) -1.0;

	//! Set up how much distortion is applied (according to user-controlled parameters)
	const f9x gain = f9x (f9x ((f9x) params[kGainControl] * f9x (0.9999950)) + f9x (0.00000250));
	const f9x dGain = f9x (f9x (1.0) - f9x (f9x (f9x (1.0) - f9x (gain)) * f9x (f9x (1.0) - f9x (gain))));

	//! Drive Stage
	const f9x filterFac= (f9x) ((f9x) 1.0 - (f9x) expl((f9x) -2.0 * (f9x) PI *
							(f9x) 14000.0 / (f9x) ((f9x) sRate * (f9x) rateFac)));

	for(int x = 0; x < DIST_STAGES_HIGH; x++)
	{
		if((f9x) spl != (f9x) 0.00)
		{
			spl *= f9x (dGain);

			if((f9x) spl > (f9x) 0.00)
			{
				lastPoint[x][1] = f9x (0.0);

				spl = (f9x) RunTubeA(x, 0, f9x (spl));
			}
			if((f9x) spl < (f9x) 0.00)
			{
				lastPoint[x][0] = f9x (0.0);

				spl = (f9x) -spl;
				spl = (f9x) RunTubeA(x, 1, f9x (spl));
				spl = (f9x) -spl;
			}
		} else
		{
			spl = (f9x) zeroFudge;
			lastPoint[x][0] = f9x (0.0);
			lastPoint[x][1] = f9x (0.0);
		}
		spl = (f9x) (dsFilter[x] += (f9x) ((f9x) ((f9x) spl - (f9x) dsFilter[x]) * (f9x) filterFac));
	}

	//! Give a little "breathing room" for output filters
	spl *= (f9x) 0.4950; // 0.450, 0.250

	//! Output of completed sample.  We're done!
	sample[sNum] = f9x (spl);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::DistortTypeMega(const int sLevI, const int sNum)
{
	//! Set up input
	f9x spl = (f9x) (f9x (sample[sNum]));

	//! Check for illegal, out-of-range values
	if((f9x) spl > (f9x) 1.0) spl = (f9x) 1.0;
	if((f9x) spl < (f9x) -1.0) spl = (f9x) -1.0;

	//! Set up how much distortion is applied (according to user-controlled parameters)
	const f9x gain = f9x (f9x ((f9x) params[kGainControl] * f9x (0.9999950)) + f9x (0.00000250));
	const f9x dGain = f9x (f9x (1.0) - f9x (f9x (f9x (1.0) - f9x (gain)) * f9x (f9x (1.0) - f9x (gain))));

	//! Drive Stage
	const f9x filterFac= (f9x) ((f9x) 1.0 - (f9x) expl((f9x) -2.0 * (f9x) PI *
							(f9x) 12000.0 / (f9x) ((f9x) sRate * (f9x) rateFac)));

	for(int x = 0; x < DIST_STAGES_MEGA; x++)
	{
		if((f9x) spl != (f9x) 0.00)
		{
			spl *= f9x (dGain);

			if((f9x) spl > (f9x) 0.00)
			{
				lastPoint[x][1] = f9x (0.0);

				spl = (f9x) RunTubeA(x, 0, f9x (spl));
			}
			if((f9x) spl < (f9x) 0.00)
			{
				lastPoint[x][0] = f9x (0.0);

				spl = (f9x) -spl;
				spl = (f9x) RunTubeA(x, 1, f9x (spl));
				spl = (f9x) -spl;
			}
		} else
		{
			spl = (f9x) zeroFudge;
			lastPoint[x][0] = f9x (0.0);
			lastPoint[x][1] = f9x (0.0);
		}
		spl = (f9x) (dsFilter[x] += (f9x) ((f9x) ((f9x) spl - (f9x) dsFilter[x]) * (f9x) filterFac));
	}

	//! Give a little "breathing room" for output filters
	spl *= (f9x) 0.4950; // 0.450, 0.250

	//! Output of completed sample.  We're done!
	sample[sNum] = f9x (spl);
}
//! -------------------------------------------------------------------------------------------------------
f9x ScorchCraft::RunTubeA(const int x, const int y, f9x smpl)
{
	if(f9x (smpl) > f9x (1.0)) return f9x (0.0);
	if(f9x (smpl) < f9x (0.0)) return f9x (0.0);

	f9x vTravel = f9x (f9x (smpl) - f9x (lastPoint[x][y]));
	lastPoint[x][y] = f9x (smpl);

	f9x splx = f9x (0.0);
	f9x sply = f9x (0.0);
	u7x i = 0;

#if SCPREC==1
	splx = (f9x) modff(f9x (f9x (smpl) * f9x (tbFacF)), &sply);
#endif
#if SCPREC==2
	splx = (f9x) modf(f9x (f9x (smpl) * f9x (tbFacF)), &sply);
#endif
#if SCPREC==3
	splx = (f9x) modfl(f9x (f9x (smpl) * f9x (tbFacF)), &sply);
#endif
	i = u7x (sply);

	if(f9x (vTravel) >= f9x (0.0))
	{
		if(i >= tbFacI)
		{
			smpl = f9x (driveTubeLGup[tbFacI]);
			peakP[x][y] = f9x (driveTubeLGdn[tbFacI]);
		}
		else
		{
			smpl = f9x (f9x (f9x (driveTubeLGup[i]) * f9x (f9x (1.0) - f9x (splx))) + f9x (f9x (driveTubeLGup[i+1]) * f9x (splx)));
			peakP[x][y] = f9x (f9x (f9x (driveTubeLGdn[i]) * f9x (f9x (1.0) - f9x (splx))) + f9x (f9x (driveTubeLGdn[i+1]) * f9x (splx)));
		}
		distP[x][y] = f9x (smpl);
	}
	else
	{
		if(i >= tbFacI) smpl = f9x (driveTubeLGdn[tbFacI]);
		else smpl = f9x (f9x (f9x (driveTubeLGdn[i]) * f9x (f9x (1.0) - f9x (splx))) + f9x (f9x (driveTubeLGdn[i+1]) * f9x (splx)));

		if(f9x (peakP[x][y]) > f9x (0.0)) smpl /= f9x (peakP[x][y]);
		smpl *= f9x (distP[x][y]);
	}

	return f9x (smpl);
}
//! -------------------------------------------------------------------------------------------------------
f9x ScorchCraft::RunTubeB(const int x, const int y, f9x smpl)
{
	if(f9x (smpl) > f9x (1.0)) return f9x (0.0);
	if(f9x (smpl) < f9x (0.0)) return f9x (0.0);

	f9x vTravel = f9x (f9x (smpl) - f9x (lastPoint[x][y]));
	lastPoint[x][y] = f9x (smpl);

	f9x splx = f9x (0.0);

	int i = 0;

	if(f9x (vTravel) >= f9x (0.0))
	{
		splx = f9x (f9x (smpl) * f9x (tbFacF));
		i = int (f9x ((f9x) floorl(f9x (splx))));
		if(i >= tbFacI)
		{
			smpl = f9x (driveTubeHGup[tbFacI]);
			peakP[x][y] = f9x (driveTubeHGdn[tbFacI]);
		}
		else
		{
			splx -= (f9x) floorl(f9x (splx));
			smpl = f9x (f9x (f9x (driveTubeHGup[i]) * f9x (f9x (1.0) - f9x (splx))) + f9x (f9x (driveTubeHGup[i+1]) * f9x (splx)));
			peakP[x][y] = f9x (f9x (f9x (driveTubeHGdn[i]) * f9x (f9x (1.0) - f9x (splx))) + f9x (f9x (driveTubeHGdn[i+1]) * f9x (splx)));
		}
		distP[x][y] = f9x (smpl);
	}
	else
	{
		splx = f9x (f9x (smpl) * f9x (tbFacF));
		i = int (f9x ((f9x) floorl(f9x (splx))));
		if(i >= tbFacI) smpl = f9x (driveTubeHGdn[tbFacI]);
		else
		{
//			splx -= f9x (i);
			splx -= (f9x) floorl(f9x (splx));
			smpl = f9x (f9x (f9x (driveTubeHGdn[i]) * f9x (f9x (1.0) - f9x (splx))) + f9x (f9x (driveTubeHGdn[i+1]) * f9x (splx)));
		}
		if(f9x (peakP[x][y]) > f9x (0.0)) smpl /= f9x (peakP[x][y]);
		smpl *= f9x (distP[x][y]);
	}

	if((f9x) smpl > (f9x) 1.0) smpl = (f9x) 1.0;

	return f9x (smpl);
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::RunOutputEqFilter()
{
	if(fadeCount > 0) sample[0] = f9x (zeroFudge);

	f9x splow = (f9x) RunEQLPass(EQ_ToneStack_Low, f9x (sample[0]));
	f9x spmid = (f9x) RunEQLPass(EQ_ToneStack_High, f9x (sample[0]));
	f9x sphigh = (f9x) RunEQLPass(EQ_ToneStack_CeilA, f9x (sample[0]));
	sphigh -= f9x (spmid);
	spmid -= f9x (splow);

	spmid *= f9x (params[kEQstackMid]);
	if(tStackTypeSelection == 0)
	{
		splow *= f9x (params[kEQstackLow]);
		sphigh *= f9x (params[kEQstackHigh]);
	}
	if(tStackTypeSelection == 1)
	{
		splow += f9x (f9x (f9x (splow) * f9x (params[kEQstackLow])) * f9x (1.0));
		sphigh *= f9x (f9x (2.0) * f9x (params[kEQstackHigh]));
		// splow *= f9x (f9x (1.0) + f9x (dstRead));
		// spmid *= f9x (f9x (1.250) - f9x (dstRead));
		// sphigh *= f9x (f9x (0.250) + f9x (dstRead));
	}

	sample[0] = f9x (f9x (splow) + f9x (spmid) + f9x (sphigh));

	if(tStackTypeSelection == 1) sample[0] *= f9x (0.250);

	splow = (f9x) RunEQLPass(EQ_ToneStack_BottQ, f9x (sample[0]));
	spmid = (f9x) RunEQLPass(EQ_ToneStack_Pres, f9x (sample[0]));
	sphigh = (f9x) RunEQLPass(EQ_ToneStack_CeilB, f9x (sample[0]));
	sphigh -= f9x (spmid);
	spmid -= f9x (splow);

	// if(tStackTypeSelection == 1) splow *= f9x (f9x (1.0) + f9x (dstRead));

	splow*=f9x (params[kEQstackQkick]);
	spmid*=f9x (0.50);
	sphigh*=f9x (params[kEQstackPres]);

	// if(tStackTypeSelection == 1) splow *= f9x (f9x (1.0) + f9x (dstRead));

	sample[0] = f9x (f9x (splow) + f9x (spmid) + f9x (sphigh));
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::CheckChannelSelect(const int nChan)
{
	if(oldChanSelect == nChan) return;

	cleanGainChannelActive = false;
	crunchGainChannelActive = false;
	highGainChannelActive = false;
	megaGainChannelActive = false;

	if(nChan == 1)
	{
		cleanGainChannelActive = true;
		// setParameterAutomated(kAlgControlA, float (0.9f));
		setParameterAutomated(kAlgControlB, float (0.f));
		setParameterAutomated(kAlgControlC, float (0.f));
		setParameterAutomated(kAlgControlD, float (0.f));
	}
	if(nChan == 2)
	{
		crunchGainChannelActive = true;
		setParameterAutomated(kAlgControlA, float (0.f));
		// setParameterAutomated(kAlgControlB, float (0.9f));
		setParameterAutomated(kAlgControlC, float (0.f));
		setParameterAutomated(kAlgControlD, float (0.f));
	}
	if(nChan == 3)
	{
		highGainChannelActive = true;
		setParameterAutomated(kAlgControlA, float (0.f));
		setParameterAutomated(kAlgControlB, float (0.f));
		// setParameterAutomated(kAlgControlC, float (0.9f));
		setParameterAutomated(kAlgControlD, float (0.f));
	}
	if(nChan == 4)
	{
		megaGainChannelActive = true;
		setParameterAutomated(kAlgControlA, float (0.f));
		setParameterAutomated(kAlgControlB, float (0.f));
		setParameterAutomated(kAlgControlC, float (0.f));
		// setParameterAutomated(kAlgControlD, float (0.9f));
	}

	if(oldChanSelect != nChan)
	{
		fadeCount = fadeMax;
		ZeroSamples();
		ZeroPassBuffers();

		oldChanSelect = nChan;

		updateDisplay();
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScorchCraft::LoadTubeTables()
{
/*	if(driveTubeHGdn)
	{
		delete[] driveTubeHGdn;
		// driveTubeHGdn = NULL;
	}
	if(driveTubeHGup)
	{
		delete[] driveTubeHGup;
		// driveTubeHGup = NULL;
	}
	if(driveTubeLGdn)
	{
		delete[] driveTubeLGdn;
		// driveTubeLGdn = NULL;
	}
	if(driveTubeLGup)
	{
		delete[] driveTubeLGup;
		// driveTubeLGup = NULL;
	} */
	driveTubeHGdn = new f9x[COMPONENT_TABLE_SIZE];
	driveTubeHGup = new f9x[COMPONENT_TABLE_SIZE];
	driveTubeLGdn = new f9x[COMPONENT_TABLE_SIZE];
	driveTubeLGup = new f9x[COMPONENT_TABLE_SIZE];

	u9x i = 0;
	double fred = double (0.0);
	char *ttabRead = new char[8];

	for(i = 0; i < (COMPONENT_TABLE_SIZE); i++)
	{
		ttabRead[0] = wavdataUp[(i * 8)];
		ttabRead[1] = wavdataUp[((i * 8) + 1)];
		ttabRead[2] = wavdataUp[((i * 8) + 2)];
		ttabRead[3] = wavdataUp[((i * 8) + 3)];
		ttabRead[4] = wavdataUp[((i * 8) + 4)];
		ttabRead[5] = wavdataUp[((i * 8) + 5)];
		ttabRead[6] = wavdataUp[((i * 8) + 6)];
		ttabRead[7] = wavdataUp[((i * 8) + 7)];
		std::stringstream sst(ttabRead);
		fred = double (0.0);
		sst >> fred;
		if(double (fred) > double (1.0)) fred = double (1.0);
		if(double (fred) < double (-1.0)) fred = double (-1.0);
		driveTubeLGup[i] = (f9x) fabsl(f9x (fred));
		driveTubeHGup[i] = (f9x) fabsl(f9x (fred));
	}

	for(i = 0; i < (COMPONENT_TABLE_SIZE); i++)
	{
		ttabRead[0] = wavdataDn[(i * 8)];
		ttabRead[1] = wavdataDn[((i * 8) + 1)];
		ttabRead[2] = wavdataDn[((i * 8) + 2)];
		ttabRead[3] = wavdataDn[((i * 8) + 3)];
		ttabRead[4] = wavdataDn[((i * 8) + 4)];
		ttabRead[5] = wavdataDn[((i * 8) + 5)];
		ttabRead[6] = wavdataDn[((i * 8) + 6)];
		ttabRead[7] = wavdataDn[((i * 8) + 7)];
		std::stringstream sst(ttabRead);
		fred = double (0.0);
		sst >> fred;
		if(double (fred) > double (1.0)) fred = double (1.0);
		if(double (fred) < double (-1.0)) fred = double (-1.0);
		driveTubeLGdn[i] = (f9x) fabsl(f9x (fred));
		driveTubeHGdn[i] = (f9x) fabsl(f9x (fred));
	}

	/*
	std::ifstream aa("C:/tmp/wavlow_up.dat", std::ios::in | std::ios::binary);
	for(i = 0; i < (COMPONENT_TABLE_SIZE); i++)
	{
		aa.read((char*)&fred, sizeof (double));
		driveTubeLGup[i] = (f9x) fabsl(f9x (fred));
	}
	aa.close();

	std::ifstream ab("C:/tmp/wavlow_dn.dat", std::ios::in | std::ios::binary);
	for(i = 0; i < (COMPONENT_TABLE_SIZE); i++)
	{
		ab.read((char*)&fred, sizeof (double));
		driveTubeLGdn[i] = (f9x) fabsl(f9x (fred));
	}
	ab.close();

	std::ifstream ac("C:/tmp/wavhg_up.dat", std::ios::in | std::ios::binary);
	for(i = 0; i < (COMPONENT_TABLE_SIZE); i++)
	{
		ac.read((char*)&fred, sizeof (double));
		driveTubeHGup[i] = (f9x) fabsl(f9x (fred));
	}
	ac.close();

	std::ifstream ad("C:/tmp/wavhg_dn.dat", std::ios::in | std::ios::binary);
	for(i = 0; i < (COMPONENT_TABLE_SIZE); i++)
	{
		ad.read((char*)&fred, sizeof (double));
		driveTubeHGdn[i] = (f9x) fabsl(f9x (fred));
	}
	ad.close();
	*/

	driveTubeHGdn[0] = f9x (0.0);
	driveTubeHGup[0] = f9x (0.0);
	driveTubeLGdn[0] = f9x (0.0);
	driveTubeLGup[0] = f9x (0.0);
}
//! -------------------------------------------------------------------------------------------------------
