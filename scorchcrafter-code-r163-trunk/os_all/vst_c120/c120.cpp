#include "c120.h"
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>

#ifdef BUILD_VST
//! -------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new ScC120 (audioMaster);
}
//! -------------------------------------------------------------------------------------------------------
ScC120::ScC120 (audioMasterCallback audioMaster) : AudioEffectX (audioMaster, 1, kMaxNumParameters),
#else
ScC120::ScC120 (const float smplRate),
#endif
	inputFloatL(float (0.0f)),
	inputDoubleL(double (0.0)),
	inputFloatR(float (0.0f)),
	inputDoubleR(double (0.0)),
	outputFloatL(float (0.0f)),
	outputDoubleL(double (0.0)),
	outputFloatR(float (0.0f)),
	outputDoubleR(double (0.0))
{
#ifdef BUILD_VST
#ifdef COMPILER_MGW32
	setUniqueID ('0x2[');
#endif
#ifdef COMPILER_MGW64
	setUniqueID ('0x2=');
#endif
	setNumInputs(2);
	setNumOutputs(2);
	canProcessReplacing();
	canDoubleReplacing();

	vst_strncpy (programName, "Default", kVstMaxProgNameLen);

	struct timeval tv;
	usleep(1000);
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);

	sRate = float (getSampleRate());
#else	// /BUILD_VST
	sRate = float (smplRate);
#endif

	InitPlug();
}
//! -------------------------------------------------------------------------------------------------------
ScC120::~ScC120()
{
	delete C120Amp;
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* in1  = inputs[0];
	float* in2  = inputs[1];
    float* out1 = outputs[0];
    float* out2 = outputs[1];

    inputFloatL = 0.f;
    inputFloatR = 0.f;
    outputFloatL = 0.f;
    outputFloatR = 0.f;

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CheckParams();

    while (--sampleFrames >= 0)
    {
    	if (in1) inputFloatL = float (*in1++);
    	if (in2) inputFloatR = float (*in2++);

		//! Safety measure, this will happily CLIP any input that is too loud
    	inputFloatL = (float) fminf(4.0f, fmaxf(-4.0f, inputFloatL));
		inputFloatR = (float) fminf(4.0f, fmaxf(-4.0f, inputFloatR));

		//! Here we run just about all of the plug-in code
		RunFX(inputFloatL, inputFloatR);

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		outputFloatL = (float) fminf(4.0f, fmaxf(-4.0f, outputFloatL));
		outputFloatR = (float) fminf(4.0f, fmaxf(-4.0f, outputFloatR));

		if (out1) (*out1++) = float (outputFloatL);
		if (out2) (*out2++) = float (outputFloatR);
    }
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* out1 = outputs[0];
    double* out2 = outputs[1];

    inputDoubleL = 0.0;
    inputDoubleR = 0.0;
    outputDoubleL = 0.0;
    outputDoubleR = 0.0;

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CheckParams();

    while (--sampleFrames >= 0)
    {
    	if (in1) inputDoubleL = double (*in1++);
    	if (in2) inputDoubleR = double (*in2++);

		//! Safety measure, this will happily CLIP any input that is too loud
    	inputDoubleL = (double) fmin(4.0, fmax(-4.0, inputDoubleL));
		inputDoubleR = (double) fmin(4.0, fmax(-4.0, inputDoubleR));

		//! Here we run just about all of the plug-in code
		RunFX(inputDoubleL, inputDoubleR);

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		outputDoubleL = (double) fmin(4.0, fmax(-4.0, outputDoubleL));
		outputDoubleR = (double) fmin(4.0, fmax(-4.0, outputDoubleR));

		if (out1) (*out1++) = double (outputDoubleL);
		if (out2) (*out2++) = double (outputDoubleR);
    }
}
#ifdef BUILD_VST
//! -------------------------------------------------------------------------------------------------------
void ScC120::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::setParameter (VstInt32 index, float value)
{
	float val = float (value);

	//! Safety measure, for handling illegal values
	if(float (val) > float (1.00f)) val = float (1.00f);
	if(float (val) < float (0.00f)) val = float (0.00f);

	//! If we're setting a valid parameter, update that parameter
	if(index < kMaxNumParameters)
	{
		params[index] = float (val);
	}
#ifndef NO_GUI
	//! If the GUI is up and running, pass the value to the GUI
	if(editor)
	{
#ifdef USE_IRR
		if(index < kMaxNumParameters) ((SC_C120_Editor*)editor)->setParameter (index, val);
#else
		if(index < kMaxNumParameters) ((PlgEditor_C120*)editor)->setParameter (index, val);
#endif
	}
#endif
}
//! -------------------------------------------------------------------------------------------------------
float ScC120::getParameter (VstInt32 index)
{
	float val = float (0.0f);

	//! If we're getting the value of a valid paramter, get that value
	if(index < kMaxNumParameters) val=float (params[index]);

	return float (val);
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::getParameterLabel (VstInt32 index, char* label)
{
	if(index < kMaxNumParameters) vst_strncpy (label, " ", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::getParameterDisplay (VstInt32 index, char* text)
{
	if(index < kMaxNumParameters)
	{
		float2string(params[index], text, kVstMaxParamStrLen);
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::getParameterName (VstInt32 index, char* text)
{
	vst_strncpy (text, " ", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
bool ScC120::getEffectName (char* name)
{
#ifdef COMPILER_MGW32
	vst_strncpy (name, "SC-x86 C-120 Amp Head", kVstMaxEffectNameLen);
#endif
#ifdef COMPILER_MGW64
	vst_strncpy (name, "SC-x64 C-120 Amp Head", kVstMaxEffectNameLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ScC120::getProductString (char* text)
{
#ifdef COMPILER_MGW32
	vst_strncpy (text, "C120-mg32H", kVstMaxProductStrLen);
#endif
#ifdef COMPILER_MGW64
	vst_strncpy (text, "C120-mg64H", kVstMaxProductStrLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ScC120::getVendorString (char* text)
{
	vst_strncpy (text, "ScorchCrafter", kVstMaxVendorStrLen);
	return true;
}
//! -------------------------------------------------------------------------------------------------------
VstInt32 ScC120::getVendorVersion ()
{
	return 3210;
}
//! -------------------------------------------------------------------------------------------------------
/*
void ScC120::open()
{
//	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::close()
{
//	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::suspend()
{
//	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::resume()
{
//	ZeroSamples();
}
//! -------------------------------------------------------------------------------------------------------
*/
#else	// /BUILD_VST
//! -------------------------------------------------------------------------------------------------------
void ScC120::l_processAudio (float** inputs, float** outputs, long sampleFrames)
{
	float* in1 = inputs[0];
	float* in2 = inputs[1];
	float* out1 = outputs[0];
    float* out2 = outputs[1];

    inputFloatL = 0.0f;
    inputFloatR = 0.0f;
    outputFloatL = 0.0f;
    outputFloatR = 0.0f;

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CheckParams();

    while (--sampleFrames >= 0)
    {
    	inputFloatL = float (*in1++);
    	inputFloatR = float (*in2++);

		//! Safety measure, this will happily CLIP any input that is too loud
    	inputFloatL = (float) fminf(1.0f, fmaxf(-1.0f, inputFloatL));
		inputFloatR = (float) fminf(1.0f, fmaxf(-1.0f, inputFloatR));

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
void ScC120::s_processAudio (double** inputs, double** outputs, long sampleFrames)
{
	double* in1 = inputs[0];
	double* in2 = inputs[1];
	double* out1 = outputs[0];
    double* out2 = outputs[1];

    inputDoubleL = 0.0;
    inputDoubleR = 0.0;
    outputDoubleL = 0.0;
    outputDoubleR = 0.0;

	//! Placed out-side the main FX code, to speed things up - We don't need to update this
	//! for every friggin sample, placed here it should be often enough.
    CheckParams();

    while (--sampleFrames >= 0)
    {
    	inputDoubleL = double (*in1++);
    	inputDoubleR = double (*in2++);

		//! Safety measure, this will happily CLIP any input that is too loud
    	inputDoubleL = (double) fmin(1.0, fmax(-1.0, inputDoubleL));
		inputDoubleR = (double) fmin(1.0, fmax(-1.0, inputDoubleR));

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
void ScC120::setParameter (int index, float value)
{
	float val = float (value);

	//! Safety measure, for handling illegal values
	if(float (val) > float (1.00f)) val = float (1.00f);
	if(float (val) < float (0.00f)) val = float (0.00f);

	//! If we're setting a valid parameter, update that parameter
	if(index < kMaxNumParameters)
	{
		params[index] = float (val);
	}
#ifndef NO_GUI
	//! If the GUI is up and running, pass the value to the GUI
	if(editor)
	{
#ifdef USE_IRR
		if(index < kMaxNumParameters) ((SC_C120_Editor*)editor)->setParameter (index, val);
#else
		if(index < kMaxNumParameters) ((PlgEditor_C120*)editor)->setParameter (index, val);
#endif
	}
#endif
}
//! -------------------------------------------------------------------------------------------------------
float ScC120::getParameter (int index)
{
	float val = float (0.0f);

	//! If we're getting the value of a valid paramter, get that value
	if(index < kMaxNumParameters) val = float (params[index]);

	return float (val);
}
//! -------------------------------------------------------------------------------------------------------
#endif	// /BUILD_VST
//! -------------------------------------------------------------------------------------------------------
void ScC120::setSampleRate (float smplRate)
{
	sRate = float (smplRate);

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the constructor accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);
//	if(float (sRate) > float (768000.0f)) sRate=float (768000.0f);

	C120Amp->SetNewSampleRate(fsx (sRate));
}
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! - - -
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! Custom FX Functions -----------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! - - -
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
void ScC120::ZeroSamples()
{
	C120Amp->ClearAllBuffers();
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::InitPlug()
{
	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the setSampleRate() function accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);
	// if(float (sRate) > float (768000.0f)) sRate=float (768000.0f);

	bDepth[0] = 64;
	bDepth[1] = 64;

	overSmpRate[0] = 16;
	overSmpRate[1] = 2;

	ampChan = 2;
	ampChanOld = ampChan;

	for(int x = 0; x < kMaxNumParameters; x++) {params[x] = 0.0f;};

	params[kGainControl]=float (0.50f);
	params[kMasterVol]=float (0.50f);
	params[kEQstackLow]=float (0.50f);
	params[kEQstackMid]=float (0.50f);
	params[kEQstackHigh]=float (0.50f);
	params[kEQstackContour]=float (0.50f);
	params[kEQstackPres]=float (0.50f);
	params[kBtnHeavy] = float (1.0f);
	params[kBtnBoost] = float (0.0f);

	params[kSmpBitOn64] = float (1.0f);
	params[kSmpBitOff64] = float (1.0f);
	params[kSmpRenderOn2x] = float (1.0f);
	params[kSmpRenderOff16x] = float (1.0f);
	params[kToneStShapedA] = float (1.0f);
	params[kToneStFlatEven] = float (1.0f);
	params[kInternalCabA] = float (1.0f);
	params[kDynamicEnabled] = float (1.0f);

	params[kGuiPanelButtonA] = float (1.0f);

	C120Amp = new SC_C120_Amp_Head(fsx (sRate));

	for(int x = 0; x < kMaxNumParameters; x++) {oldParams[x] = params[x];};

	isOnline = true;
	isOnlineOld = true;

	isRunningStereo = false;
	isRunningStereoOld = false;

#ifndef NO_GUI
	//! Load and use the GUI
#ifdef USE_IRR
	setEditor (new SC_C120_Editor (this));
#else
	setEditor (new PlgEditor_C120 (this));
#endif
#endif

	C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_2x);
	C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_64);
	C120Amp->SetFpParam(ScPrmF_Drive_Gain, params[kGainControl]);
	C120Amp->SetFpParam(ScPrmF_Master_Volume, params[kMasterVol]);
	C120Amp->SetFpParam(ScPrmF_EQ_Contour, params[kEQstackContour]);
	C120Amp->SetFpParam(ScPrmF_EQ_Low, params[kEQstackLow]);
	C120Amp->SetFpParam(ScPrmF_EQ_Mid, params[kEQstackMid]);
	C120Amp->SetFpParam(ScPrmF_EQ_High, params[kEQstackHigh]);
	C120Amp->SetFpParam(ScPrmF_EQ_Presence, params[kEQstackPres]);
	C120Amp->SetIntParam(ScPrmI_Drive_Channel, ampChan);
	C120Amp->SetIntParam(ScPrmI_Stereo, 0);
	C120Amp->SetIntParam(ScPrmI_CabMic_Active, 0);
	C120Amp->SetIntParam(ScPrmI_CabMic_Num, 0);
	C120Amp->SetIntParam(ScPrmI_TStack_Shape, 1);
	C120Amp->SetIntParam(ScPrmI_TStack_ShapeNum, 0);
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::RunFX(const fsx inpSplA, const fsx inpSplB)
{
	C120Amp->InputSampleStereo((fsx) inpSplA, (fsx) inpSplB);

	C120Amp->RunPlugin();

	outputDoubleL = (fsx) C120Amp->GetStereoOutLeft();
	outputDoubleR = (fsx) C120Amp->GetStereoOutRight();
	outputFloatL = flx (outputDoubleL);
	outputFloatR = flx (outputDoubleR);
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::CheckParams()
{
	if (getCurrentProcessLevel() == 4) isOnline = false;
	else isOnline = true;

	if(isOnlineOld != isOnline)
	{
		isOnlineOld = isOnline;

		if(isOnline == true)
		{
			if(overSmpRate[1] == 1) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_1x);
			if(overSmpRate[1] == 2) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_2x);
			if(overSmpRate[1] == 4) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_4x);
			if(overSmpRate[1] == 8) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_8x);
			if(overSmpRate[1] == 16) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_16x);
			if(overSmpRate[1] == 0) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_OFF);
			if(bDepth[1] == 32) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_32);
			if(bDepth[1] == 64) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_64);
			if(bDepth[1] == 128) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_128);
		} else
		{
			if(overSmpRate[0] == 1) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_1x);
			if(overSmpRate[0] == 2) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_2x);
			if(overSmpRate[0] == 4) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_4x);
			if(overSmpRate[0] == 8) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_8x);
			if(overSmpRate[0] == 16) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_16x);
			if(overSmpRate[0] == 0) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_OFF);
			if(bDepth[0] == 32) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_32);
			if(bDepth[0] == 64) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_64);
			if(bDepth[0] == 128) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_128);
		}
	}

	for(int x = 0; x < kMaxNumParameters; x++)
	{
		if(oldParams[x] != params[x])
		{
			oldParams[x] = float (params[x]);

			if(x == kMasterVol) C120Amp->SetFpParam(ScPrmF_Master_Volume, params[x]);
			if(x == kGainControl) C120Amp->SetFpParam(ScPrmF_Drive_Gain, params[x]);
			if(x == kEQstackLow) C120Amp->SetFpParam(ScPrmF_EQ_Low, params[x]);
			if(x == kEQstackMid) C120Amp->SetFpParam(ScPrmF_EQ_Mid, params[x]);
			if(x == kEQstackHigh) C120Amp->SetFpParam(ScPrmF_EQ_High, params[x]);
			if(x == kEQstackContour) C120Amp->SetFpParam(ScPrmF_EQ_Contour, params[x]);
			if(x == kEQstackPres) C120Amp->SetFpParam(ScPrmF_EQ_Presence, params[x]);

			if(x == kGuiPanelButtonA)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kGuiPanelButtonB, 0.0f);
					setParameterAutomated(kGuiPanelButtonC, 0.0f);
					oldParams[kGuiPanelButtonB] = 0.f;
					oldParams[kGuiPanelButtonC] = 0.f;
					updateDisplay();
				}
			}
			if(x == kGuiPanelButtonB)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kGuiPanelButtonA, 0.0f);
					setParameterAutomated(kGuiPanelButtonC, 0.0f);
					oldParams[kGuiPanelButtonA] = 0.f;
					oldParams[kGuiPanelButtonC] = 0.f;
					updateDisplay();
				}
			}
			if(x == kGuiPanelButtonC)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kGuiPanelButtonB, 0.0f);
					setParameterAutomated(kGuiPanelButtonA, 0.0f);
					oldParams[kGuiPanelButtonB] = 0.f;
					oldParams[kGuiPanelButtonA] = 0.f;
					updateDisplay();
				}
			}
			if(x == kBtnBright)
			{
				if(params[x] == 1.f) C120Amp->SetIntParam(ScPrmI_Bright, 1);
				if(params[x] == 0.f) C120Amp->SetIntParam(ScPrmI_Bright, 0);
			}
			if(x == kInternalCabOff)
			{
				if(params[x] == 1.f) C120Amp->SetIntParam(ScPrmI_CabMic_Active, 1);
				if(params[x] == 0.f) C120Amp->SetIntParam(ScPrmI_CabMic_Active, 0);
			}
			if((x >= kInternalCabA) && (x <= kInternalCabF))
			{
				if(params[x] == 1.f)
				{
					C120Amp->SetIntParam(ScPrmI_CabMic_Num, x-kInternalCabA);
					CycleCabsOff(x);
				} else
				{
					setParameterAutomated(x, 1.f);
					updateDisplay();
				}
			}
			if(x == kToneStFlatEven)
			{
				if(params[x] == 1.f) C120Amp->SetIntParam(ScPrmI_TStack_Shape, 1);
				if(params[x] == 0.f) C120Amp->SetIntParam(ScPrmI_TStack_Shape, 0);
			}
			if(x == kDynamicEnabled)
			{
				if(params[x] == 1.f) C120Amp->SetIntParam(ScPrmI_TStack_Dynamic, 1);
				if(params[x] == 0.f) C120Amp->SetIntParam(ScPrmI_TStack_Dynamic, 0);
			}
			if(x == kToneStShapedA)
			{
				if(params[x] == 1.f)
				{
					C120Amp->SetIntParam(ScPrmI_TStack_ShapeNum, 0);
					setParameterAutomated(kToneStShapedB, 0.f);
					setParameterAutomated(kToneStShapedC, 0.f);
					setParameterAutomated(kToneStShapedD, 0.f);
					setParameterAutomated(kToneStShapedE, 0.f);
					setParameterAutomated(kToneStShapedF, 0.f);
					oldParams[kToneStShapedB] = 0.f;
					oldParams[kToneStShapedC] = 0.f;
					oldParams[kToneStShapedD] = 0.f;
					oldParams[kToneStShapedE] = 0.f;
					oldParams[kToneStShapedF] = 0.f;
					updateDisplay();
				} else
				{
					setParameterAutomated(kToneStShapedA, 1.f);
					updateDisplay();
				}
			}
			if(x == kToneStShapedB)
			{
				if(params[x] == 1.f)
				{
					C120Amp->SetIntParam(ScPrmI_TStack_ShapeNum, 1);
					setParameterAutomated(kToneStShapedA, 0.f);
					setParameterAutomated(kToneStShapedC, 0.f);
					setParameterAutomated(kToneStShapedD, 0.f);
					setParameterAutomated(kToneStShapedE, 0.f);
					setParameterAutomated(kToneStShapedF, 0.f);
					oldParams[kToneStShapedA] = 0.f;
					oldParams[kToneStShapedC] = 0.f;
					oldParams[kToneStShapedD] = 0.f;
					oldParams[kToneStShapedE] = 0.f;
					oldParams[kToneStShapedF] = 0.f;
					updateDisplay();
				} else
				{
					setParameterAutomated(kToneStShapedB, 1.f);
					updateDisplay();
				}
			}
			if(x == kToneStShapedC)
			{
				if(params[x] == 1.f)
				{
					C120Amp->SetIntParam(ScPrmI_TStack_ShapeNum, 2);
					setParameterAutomated(kToneStShapedB, 0.f);
					setParameterAutomated(kToneStShapedA, 0.f);
					setParameterAutomated(kToneStShapedD, 0.f);
					setParameterAutomated(kToneStShapedE, 0.f);
					setParameterAutomated(kToneStShapedF, 0.f);
					oldParams[kToneStShapedB] = 0.f;
					oldParams[kToneStShapedA] = 0.f;
					oldParams[kToneStShapedD] = 0.f;
					oldParams[kToneStShapedE] = 0.f;
					oldParams[kToneStShapedF] = 0.f;
					updateDisplay();
				} else
				{
					setParameterAutomated(kToneStShapedC, 1.f);
					updateDisplay();
				}
			}
			if(x == kToneStShapedD)
			{
				if(params[x] == 1.f)
				{
					C120Amp->SetIntParam(ScPrmI_TStack_ShapeNum, 3);
					setParameterAutomated(kToneStShapedB, 0.f);
					setParameterAutomated(kToneStShapedC, 0.f);
					setParameterAutomated(kToneStShapedA, 0.f);
					setParameterAutomated(kToneStShapedE, 0.f);
					setParameterAutomated(kToneStShapedF, 0.f);
					oldParams[kToneStShapedB] = 0.f;
					oldParams[kToneStShapedC] = 0.f;
					oldParams[kToneStShapedA] = 0.f;
					oldParams[kToneStShapedE] = 0.f;
					oldParams[kToneStShapedF] = 0.f;
					updateDisplay();
				} else
				{
					setParameterAutomated(kToneStShapedD, 1.f);
					updateDisplay();
				}
			}
			if(x == kToneStShapedE)
			{
				if(params[x] == 1.f)
				{
					C120Amp->SetIntParam(ScPrmI_TStack_ShapeNum, 4);
					setParameterAutomated(kToneStShapedB, 0.f);
					setParameterAutomated(kToneStShapedC, 0.f);
					setParameterAutomated(kToneStShapedD, 0.f);
					setParameterAutomated(kToneStShapedA, 0.f);
					setParameterAutomated(kToneStShapedF, 0.f);
					oldParams[kToneStShapedB] = 0.f;
					oldParams[kToneStShapedC] = 0.f;
					oldParams[kToneStShapedD] = 0.f;
					oldParams[kToneStShapedA] = 0.f;
					oldParams[kToneStShapedF] = 0.f;
					updateDisplay();
				} else
				{
					setParameterAutomated(kToneStShapedE, 1.f);
					updateDisplay();
				}
			}
			if(x == kToneStShapedF)
			{
				if(params[x] == 1.f)
				{
					C120Amp->SetIntParam(ScPrmI_TStack_ShapeNum, 5);
					setParameterAutomated(kToneStShapedB, 0.f);
					setParameterAutomated(kToneStShapedC, 0.f);
					setParameterAutomated(kToneStShapedD, 0.f);
					setParameterAutomated(kToneStShapedE, 0.f);
					setParameterAutomated(kToneStShapedA, 0.f);
					oldParams[kToneStShapedB] = 0.f;
					oldParams[kToneStShapedC] = 0.f;
					oldParams[kToneStShapedD] = 0.f;
					oldParams[kToneStShapedE] = 0.f;
					oldParams[kToneStShapedA] = 0.f;
					updateDisplay();
				} else
				{
					setParameterAutomated(kToneStShapedF, 1.f);
					updateDisplay();
				}
			}

			if(x == kBtnBoost)
			{
				if(params[x] == 1.f)
				{
					if(params[kBtnClean] == 1.f) ampChan = 1;
					if(params[kBtnHeavy] == 1.f) ampChan = 3;
				}
				if(params[x] == 0.f)
				{
					if(params[kBtnClean] == 1.f) ampChan = 0;
					if(params[kBtnHeavy] == 1.f) ampChan = 2;
				}
			}
			if(x == kBtnClean)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kBtnHeavy, 0.f);
					updateDisplay();
					if(params[kBtnBoost] == 0.f) ampChan = 0;
					if(params[kBtnBoost] == 1.f) ampChan = 1;
				} else
				{
					if(params[kBtnHeavy] == 0.f)
					{
						setParameterAutomated(kBtnClean, 1.f);
						updateDisplay();
					}
				}
			}
			if(x == kBtnHeavy)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kBtnClean, 0.f);
					updateDisplay();
					if(params[kBtnBoost] == 0.f) ampChan = 2;
					if(params[kBtnBoost] == 1.f) ampChan = 3;
				} else
				{
					if(params[kBtnClean] == 0.f)
					{
						setParameterAutomated(kBtnHeavy, 1.f);
						updateDisplay();
					}
				}
			}

			if(x == kSmpBitOn32)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kSmpBitOn64, 0.f);
					setParameterAutomated(kSmpBitOn128, 0.f);
					oldParams[kSmpBitOn64] = 0.f;
					oldParams[kSmpBitOn128] = 0.f;
					updateDisplay();
					bDepth[1] = 32;
					if(isOnline) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_32);
				} else
				{
					if((params[kSmpBitOn64] + params[kSmpBitOn128]) < 0.5f)
					{
						setParameterAutomated(kSmpBitOn32, 1.f);
						updateDisplay();
					}
				}
			}
			if(x == kSmpBitOn64)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kSmpBitOn32, 0.f);
					setParameterAutomated(kSmpBitOn128, 0.f);
					oldParams[kSmpBitOn32] = 0.f;
					oldParams[kSmpBitOn128] = 0.f;
					updateDisplay();
					bDepth[1] = 64;
					if(isOnline) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_64);
				} else
				{
					if((params[kSmpBitOn32] + params[kSmpBitOn128]) < 0.5f)
					{
						setParameterAutomated(kSmpBitOn64, 1.f);
						updateDisplay();
					}
				}
			}
			if(x == kSmpBitOn128)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kSmpBitOn64, 0.f);
					setParameterAutomated(kSmpBitOn32, 0.f);
					oldParams[kSmpBitOn64] = 0.f;
					oldParams[kSmpBitOn32] = 0.f;
					updateDisplay();
					bDepth[1] = 128;
					if(isOnline) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_128);
				} else
				{
					if((params[kSmpBitOn64] + params[kSmpBitOn32]) < 0.5f)
					{
						setParameterAutomated(kSmpBitOn128, 1.f);
						updateDisplay();
					}
				}
			}

			if(x == kSmpBitOff32)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kSmpBitOff64, 0.f);
					setParameterAutomated(kSmpBitOff128, 0.f);
					oldParams[kSmpBitOff64] = 0.f;
					oldParams[kSmpBitOff128] = 0.f;
					updateDisplay();
					bDepth[0] = 32;
					if(!isOnline) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_32);
				} else
				{
					if((params[kSmpBitOff64] + params[kSmpBitOff128]) < 0.5f)
					{
						setParameterAutomated(kSmpBitOff32, 1.f);
						updateDisplay();
					}
				}
			}
			if(x == kSmpBitOff64)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kSmpBitOff32, 0.f);
					setParameterAutomated(kSmpBitOff128, 0.f);
					oldParams[kSmpBitOff32] = 0.f;
					oldParams[kSmpBitOff128] = 0.f;
					updateDisplay();
					bDepth[0] = 64;
					if(!isOnline) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_64);
				} else
				{
					if((params[kSmpBitOff32] + params[kSmpBitOff128]) < 0.5f)
					{
						setParameterAutomated(kSmpBitOff64, 1.f);
						updateDisplay();
					}
				}
			}
			if(x == kSmpBitOff128)
			{
				if(params[x] == 1.f)
				{
					setParameterAutomated(kSmpBitOff64, 0.f);
					setParameterAutomated(kSmpBitOff32, 0.f);
					oldParams[kSmpBitOff64] = 0.f;
					oldParams[kSmpBitOff32] = 0.f;
					updateDisplay();
					bDepth[0] = 128;
					if(!isOnline) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_128);
				} else
				{
					if((params[kSmpBitOff64] + params[kSmpBitOff32]) < 0.5f)
					{
						setParameterAutomated(kSmpBitOff128, 1.f);
						updateDisplay();
					}
				}
			}

			if((x >= kSmpRenderOn1x) && (x <= kSmpRenderOnOFF))
			{
				if(params[x] == 1.f) SetOnlineSampling(x);
				else
				{
					flx tmp = flx (0.f);
					for(int y = kSmpRenderOn1x; y <= kSmpRenderOnOFF; y++)
					{
						tmp += params[y];
					}
					if(tmp < 0.5f)
					{
						setParameterAutomated(x, 1.f);
						updateDisplay();
					}
				}
			}
			if((x >= kSmpRenderOff1x) && (x <= kSmpRenderOffOFF))
			{
				if(params[x] == 1.f) SetOfflineSampling(x);
				else
				{
					flx tmpB = flx (0.f);
					for(int y = kSmpRenderOff1x; y <= kSmpRenderOffOFF; y++)
					{
						tmpB += params[y];
					}
					if(tmpB < 0.5f)
					{
						setParameterAutomated(x, 1.f);
						updateDisplay();
					}
				}
			}
			if(x == kTStackBeforePower)
			{
				if(params[x] == 1.f)
				{
					C120Amp->SetIntParam(ScPrmI_TStack_Before_Power, 1);
					setParameterAutomated(kTStackAfterPower, 0.f);
					updateDisplay();
				} else
				{
					setParameterAutomated(kTStackBeforePower, 1.f);
					updateDisplay();
				}
			}
			if(x == kTStackAfterPower)
			{
				if(params[x] == 1.f)
				{
					C120Amp->SetIntParam(ScPrmI_TStack_Before_Power, 0);
					setParameterAutomated(kTStackBeforePower, 0.f);
					updateDisplay();
				} else
				{
					setParameterAutomated(kTStackAfterPower, 1.f);
					updateDisplay();
				}
			}
		}
	}

	if(ampChan != ampChanOld)
	{
		ampChanOld = ampChan;
		C120Amp->SetIntParam(ScPrmI_Drive_Channel, ampChan);
	}

	if(params[kMonoStereoControl] == 1.0f) isRunningStereo = true;
	else isRunningStereo = false;

	if(isRunningStereoOld != isRunningStereo)
	{
		isRunningStereoOld = isRunningStereo;
		if(isRunningStereo == true) C120Amp->SetIntParam(ScPrmI_Stereo, 1);
		else C120Amp->SetIntParam(ScPrmI_Stereo, 0);
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::CycleCabsOff(int prm)
{
	for (int y = kInternalCabA; y <= kInternalCabF; y++)
	{
		if (y != prm)
		{
			setParameterAutomated(y, 0.f);
			oldParams[y] = 0.f;
		}
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::SetOnlineSampling(const int inp)
{
	for(int x = kSmpRenderOn1x; x <= kSmpRenderOnOFF; x++)
	{
		if (inp != x)
		{
			setParameterAutomated(x, 0.0f);
			oldParams[x] = 0.0f;
		}
	}
	updateDisplay();

	if(inp == kSmpRenderOn1x) overSmpRate[1] = 1;
	if(inp == kSmpRenderOn2x) overSmpRate[1] = 2;
	if(inp == kSmpRenderOn4x) overSmpRate[1] = 4;
	if(inp == kSmpRenderOn8x) overSmpRate[1] = 8;
	if(inp == kSmpRenderOn16x) overSmpRate[1] = 16;
	if(inp == kSmpRenderOnOFF) overSmpRate[1] = 0;

	if(isOnline == true)
	{
		if(overSmpRate[1] == 1) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_1x);
		if(overSmpRate[1] == 2) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_2x);
		if(overSmpRate[1] == 4) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_4x);
		if(overSmpRate[1] == 8) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_8x);
		if(overSmpRate[1] == 16) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_16x);
		if(overSmpRate[1] == 0) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_OFF);
		if(bDepth[1] == 32) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_32);
		if(bDepth[1] == 64) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_64);
		if(bDepth[1] == 128) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_128);
	} else
	{
		if(overSmpRate[0] == 1) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_1x);
		if(overSmpRate[0] == 2) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_2x);
		if(overSmpRate[0] == 4) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_4x);
		if(overSmpRate[0] == 8) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_8x);
		if(overSmpRate[0] == 16) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_16x);
		if(overSmpRate[0] == 0) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_OFF);
		if(bDepth[0] == 32) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_32);
		if(bDepth[0] == 64) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_64);
		if(bDepth[0] == 128) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_128);
	}
}
//! -------------------------------------------------------------------------------------------------------
void ScC120::SetOfflineSampling(const int inp)
{
	for(int x = kSmpRenderOff1x; x <= kSmpRenderOffOFF; x++)
	{
		if (inp != x)
		{
			setParameterAutomated(x, 0.0f);
			oldParams[x] = 0.0f;
		}
	}
	updateDisplay();

	if(inp == kSmpRenderOff1x) overSmpRate[0] = 1;
	if(inp == kSmpRenderOff2x) overSmpRate[0] = 2;
	if(inp == kSmpRenderOff4x) overSmpRate[0] = 4;
	if(inp == kSmpRenderOff8x) overSmpRate[0] = 8;
	if(inp == kSmpRenderOff16x) overSmpRate[0] = 16;
	if(inp == kSmpRenderOffOFF) overSmpRate[0] = 0;

	if(isOnline == true)
	{
		if(overSmpRate[1] == 1) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_1x);
		if(overSmpRate[1] == 2) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_2x);
		if(overSmpRate[1] == 4) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_4x);
		if(overSmpRate[1] == 8) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_8x);
		if(overSmpRate[1] == 16) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_16x);
		if(overSmpRate[1] == 0) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_OFF);
		if(bDepth[1] == 32) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_32);
		if(bDepth[1] == 64) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_64);
		if(bDepth[1] == 128) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_128);
	} else
	{
		if(overSmpRate[0] == 1) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_1x);
		if(overSmpRate[0] == 2) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_2x);
		if(overSmpRate[0] == 4) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_4x);
		if(overSmpRate[0] == 8) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_8x);
		if(overSmpRate[0] == 16) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_16x);
		if(overSmpRate[0] == 0) C120Amp->SetIntParam(ScPrmI_OverSampling_Rate, SCv_OverSampling_OFF);
		if(bDepth[0] == 32) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_32);
		if(bDepth[0] == 64) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_64);
		if(bDepth[0] == 128) C120Amp->SetIntParam(ScPrmI_FP_Precision_Bit_Depth, SCv_Bit_Depth_128);
	}
}
//! -------------------------------------------------------------------------------------------------------
