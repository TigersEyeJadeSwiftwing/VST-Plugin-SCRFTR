#include "shapegen.h"

//! -------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new ShapeGen (audioMaster);
}
//! -------------------------------------------------------------------------------------------------------
ShapeGen::ShapeGen (audioMasterCallback audioMaster) : AudioEffectX (audioMaster, 1, kMaxNumParameters),
	inputFloat(float (0.0f)),
	inputDouble(double (0.0)),
	outputFloat(float (0.0f)),
	outputDouble(double (0.0))
{
#ifdef COMPILER_MSOFT
	setUniqueID ('0x3]');
#endif
#ifdef COMPILER_MGW
	setUniqueID ('0x3]');
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

//	//! Load and use the GUI
//	setEditor (new ScorchCEditor (this));

	params[kGain]=float (0.00f);
	params[kInput]=float (0.100f);
	params[kMasterVol]=float (1.00f);

	BuildShapeTables();
	BuildMatchTables();
	ZeroSamples();

	sWave = f9x (0.0);
}
//! -------------------------------------------------------------------------------------------------------
ShapeGen::~ShapeGen ()
{
	// nothing to do here
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
	float* in1  = inputs[0];
    float* out1 = outputs[0];

    while (--sampleFrames >= 0)
    {
    	inputFloat=float (*in1++);

		//! Safety measure, this will happily CLIP any input that is too loud
    	if(float (inputFloat) > float (1.00f)) inputFloat = float (1.00f);
		if(float (inputFloat) < float (-1.00f)) inputFloat = float (-1.00f);

		//! Here we run just about all of the plug-in code
		outputFloat=(float) RunFX(f9x (inputFloat));

		(*out1++) = float (outputFloat);
    }
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
	double* in1  = inputs[0];
    double* out1 = outputs[0];

    while (--sampleFrames >= 0)
    {
    	inputDouble=double (*in1++);

    	//! Safety measure, this will happily CLIP any input that is too loud
    	if(double (inputDouble) > double (1.00)) inputDouble = double (1.00);
		if(double (inputDouble) < double (-1.00)) inputDouble = double (-1.00);

		//! Here we run just about all of the plug-in code
		outputDouble=(double) RunFX(f9x (inputDouble));

		(*out1++) = double (outputDouble);
    }
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::setParameter (VstInt32 index, float value)
{
	float val=float (value);

	//! Safety measure, for handling illegal values
	if(float (val) > float (1.00f)) val = float (1.00f);
	if(float (val) < float (0.00f)) val = float (0.00f);

	//! If we're setting a valid parameter, update that parameter
	if(index < kMaxNumParameters) params[index] = float (val);

//	//! If the GUI is up and running, pass the value to the GUI
//	if(editor)
//	{
//		if(index < kMaxNumParameters) ((AEffGUIEditor*)editor)->setParameter (index, value);
//	}
}
//! -------------------------------------------------------------------------------------------------------
float ShapeGen::getParameter (VstInt32 index)
{
	float val=float (0.0f);

	//! If we're getting the value of a valid paramter, get that value
	if(index < kMaxNumParameters) val=float (params[index]);

	return float (val);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::getParameterLabel (VstInt32 index, char* label)
{
	if(index < kMaxNumParameters) vst_strncpy (label, " ", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::getParameterDisplay (VstInt32 index, char* text)
{
	if(index < kMaxNumParameters)
	{
		vst_strncpy (text, " ", kVstMaxParamStrLen);
	}
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::getParameterName (VstInt32 index, char* text)
{
	if(index==kGain) vst_strncpy (text, "Gain", kVstMaxParamStrLen);
	if(index==kInput) vst_strncpy (text, "Inp Sine", kVstMaxParamStrLen);
	if(index==kMasterVol) vst_strncpy (text, "M Volume", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
bool ShapeGen::getEffectName (char* name)
{
#ifdef COMPILER_MGW
	vst_strncpy (name, "ShapeGen (MinGW)", kVstMaxEffectNameLen);
#endif
#ifdef COMPILER_MSOFT
	vst_strncpy (name, "ShapeGen (MSVC)", kVstMaxEffectNameLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ShapeGen::getProductString (char* text)
{
#ifdef COMPILER_MGW
	vst_strncpy (text, "sh-gen-mgw", kVstMaxProductStrLen);
#endif
#ifdef COMPILER_MSOFT
	vst_strncpy (text, "sh-gen-ms", kVstMaxProductStrLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ShapeGen::getVendorString (char* text)
{
	//! This will eventually be changed to the dev group name
	vst_strncpy (text, "Destructavator", kVstMaxVendorStrLen);
	return true;
}
//! -------------------------------------------------------------------------------------------------------
VstInt32 ShapeGen::getVendorVersion ()
{
	return 1000;
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::setSampleRate (float smplRate)
{
	sRate = float (smplRate);

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the constructor accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);
	if(float (sRate) > float (768000.0f)) sRate=float (768000.0f);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::open()
{
	return;
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::close()
{
	return;
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::suspend()
{
	return;
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::resume()
{
	return;
}
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! Custom FX Functions -----------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
f9x ShapeGen::RunFX(f9x const inpSpl)
{
	f9x output = (f9x) sinl(f9x (f9x (sWave) * f9x (M_PI) * f9x (2.0)));
	sWave += f9x (f9x (220.0) / f9x (sRate));
	output *= f9x (params[kInput]);
	output += f9x (inpSpl);

	output = (f9x) RunTBS(f9x (output));

	return f9x (f9x (output) * f9x (params[kMasterVol]));
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::BuildShapeTables()
{
	tbWaveLGup = new f9x[TBWAVE_SIZE];
	tbWaveLGdn = new f9x[TBWAVE_SIZE];

	u9x tSize = u9x (u9x (TBWAVE_SIZE) * u9x (2));
	f9x *tbWLG = new f9x[tSize];
	f9x tFac = f9x (f9x (1.0) / f9x (tSize));
	u9x tPos = 0;

	for(tPos = 0; tPos < tSize; tPos++)
	{
		tbWLG[tPos] = (f9x) sinl(f9x (f9x (tPos) * f9x (tFac) * f9x (M_PI)));
	}

	f9x fFac = f9x (f9x (1.0) / f9x (4.0));
	for(tPos = TBWAVE_SIZE; tPos < tSize; tPos++)
	{
		tbWLG[tPos] += f9x ((f9x) sinl(f9x (f9x (tPos) * f9x (2.0) * f9x (tFac) * f9x (M_PI))) * f9x (fFac));
	}

	f9x peakMult = f9x (0.0);
	for(tPos = 0; tPos < tSize; tPos++)
	{
		if(f9x (tbWLG[tPos]) < f9x (0.0)) tbWLG[tPos] = f9x (0.0);
		if(f9x (tbWLG[tPos]) > f9x (peakMult)) peakMult = f9x (tbWLG[tPos]);
	}

	for(tPos = 0; tPos < TBWAVE_SIZE; tPos++)
	{
		tbWaveLGup[tPos] = f9x (f9x (tbWLG[tPos]) / f9x (peakMult));
	}
	for(tPos = 0; tPos < TBWAVE_SIZE; tPos++)
	{
		tbWaveLGdn[(TBWAVE_SIZE - 1) - tPos] = f9x (f9x (tbWLG[(TBWAVE_SIZE) + tPos]) / f9x (peakMult));
	}
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::BuildMatchTables()
{
	tbWaveLGmtU = new u9x[TBWAVE_SIZE];
	tbWaveLGmtD = new u9x[TBWAVE_SIZE];
	u9x tAcount = 0;
	u9x tBcount = 0;

	for(tAcount = 0; tAcount < TBWAVE_SIZE; tAcount++)
	{
		tBcount = 0;
		while(f9x (tbWaveLGup[tAcount]) > f9x (tbWaveLGdn[tBcount]))
		{
			tBcount++;
			if(tBcount >= (TBWAVE_SIZE-1)) break;
		}
		tbWaveLGmtU[tAcount] = u9x (tBcount);
	}
	/*
	for(tAcount = 0; tAcount < TBWAVE_SIZE; tAcount++)
	{
		tBcount = 0;
		while(f9x (tbWaveLGdn[tAcount]) < f9x (tbWaveLGup[tBcount]))
		{
			tBcount++;
			if(tBcount >= (TBWAVE_SIZE-1)) break;
		}
		tbWaveLGmtD[tAcount] = u9x (tBcount);
	} */
}
//! -------------------------------------------------------------------------------------------------------
f9x ShapeGen::RunTBS(f9x const inp)
{
	f9x spl = f9x (inp);

	if(f9x (spl) >= f9x (1.0)) spl = f9x (0.0);
	if(f9x (spl) <= f9x (-1.0)) spl = f9x (0.0);

	if(f9x (spl) != f9x (0.0))
	{
		if(f9x (spl) >= f9x (0.0))
		{
			if(f9x (spl) > f9x (lastSpl[0])) upTravel[0] = true;
			else upTravel[0] = false;

			lastSpl[0] = f9x (spl);
			lastSpl[1] = f9x (0.0);
			tChangePoint[1] = TBWAVE_SIZE-1;

			if(oldTravel[0] != upTravel[0])
			{
				travChange[0] = true;
				oldTravel[0] = upTravel[0];
			} else travChange[0] = false;

			u9x tmpI = u9x (f9x (f9x (spl) * f9x (TBWAVE_SIZE-1)));

			if(upTravel[0] == true)
			{
				tChangePoint[0] = u9x (tbWaveLGmtU[tmpI]);
				upPeak[0] = f9x (spl);
				spl = f9x (tbWaveLGup[tmpI]);
				distPeak[0] = f9x (spl);
			}
			else
			{
				spl /= f9x (upPeak[0]);
				tmpI = u9x (f9x (f9x (spl) * f9x (tChangePoint[0])));
				spl = f9x (tbWaveLGdn[tmpI]);
				spl /= f9x (tbWaveLGdn[tChangePoint[0]]);
				spl *= f9x (distPeak[0]);
			}
		} else
		{
			spl = f9x (-spl);

			if(f9x (spl) > f9x (lastSpl[1])) upTravel[1] = true;
			else upTravel[1] = false;

			lastSpl[1] = f9x (spl);
			lastSpl[0] = f9x (0.0);
			tChangePoint[0] = TBWAVE_SIZE-1;

			if(oldTravel[1] != upTravel[1])
			{
				travChange[1] = true;
				oldTravel[1] = upTravel[1];
			} else travChange[1] = false;

			u9x tmpI = u9x (f9x (f9x (spl) * f9x (TBWAVE_SIZE-1)));

			if(upTravel[1] == true)
			{
				tChangePoint[1] = u9x (tbWaveLGmtU[tmpI]);
				upPeak[1] = f9x (spl);
				spl = f9x (tbWaveLGup[tmpI]);
				distPeak[1] = f9x (spl);
			}
			else
			{
				spl /= f9x (upPeak[1]);
				tmpI = u9x (f9x (f9x (spl) * f9x (tChangePoint[1])));
				spl = f9x (tbWaveLGdn[tmpI]);
				spl /= f9x (tbWaveLGdn[tChangePoint[1]]);
				spl *= f9x (distPeak[1]);
			}

			spl = f9x (-spl);
		}
	} else
	{
		ZeroSamples();
	}

	return f9x (spl);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeGen::ZeroSamples()
{
	lastSpl[0] = f9x (0.0);
	lastSpl[1] = f9x (0.0);
	upPeak[0] = f9x (0.0);
	upPeak[1] = f9x (0.0);
	tChangePoint[0] = TBWAVE_SIZE-1;
	tChangePoint[1] = TBWAVE_SIZE-1;
	upTravel[0] = false;
	oldTravel[0] = false;
	travChange[0] = false;
	upTravel[1] = false;
	oldTravel[1] = false;
	travChange[1] = false;
}
//! -------------------------------------------------------------------------------------------------------
