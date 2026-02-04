#include "shapewriter.h"
#include "cfloat"
#include "climits"

//! -------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new ShapeWriter (audioMaster);
}
//! -------------------------------------------------------------------------------------------------------
ShapeWriter::ShapeWriter (audioMasterCallback audioMaster) : AudioEffectX (audioMaster, 1, kMaxNumParameters),
	inputFloat(float (0.0f)),
	inputDouble(double (0.0)),
	outputFloat(float (0.0f)),
	outputDouble(double (0.0))
{
#ifdef COMPILER_MSOFT
	setUniqueID ('0x3-');
#endif
#ifdef COMPILER_MGW
	setUniqueID ('0x3=');
#endif
	setNumInputs (1);
	setNumOutputs (1);
	canProcessReplacing();
	canDoubleReplacing();

	vst_strncpy (programName, "Default", kVstMaxProgNameLen);

	recSamp = new fsx[8192];

	sRate = float (getSampleRate());

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the setSampleRate() function accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);
//	if(float (sRate) > float (768000.0f)) sRate=float (768000.0f);

//	//! Load and use the GUI
//	setEditor (new ScorchCEditor (this));

	params[kReset]=float (0.00f);
	params[kStart]=float (0.00f);
	params[kWriteFile]=float (0.00f);
	params[kWriteBinary]=float (0.00f);

	WavReset();
}
//! -------------------------------------------------------------------------------------------------------
ShapeWriter::~ShapeWriter ()
{
	// nothing to do here
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
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
		outputFloat=(float) RunFX(fsx (inputFloat));

		(*out1++) = float (outputFloat);
    }
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
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
		outputDouble=(double) RunFX(fsx (inputDouble));

		(*out1++) = double (outputDouble);
    }
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::setParameter (VstInt32 index, float value)
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
float ShapeWriter::getParameter (VstInt32 index)
{
	float val=float (0.0f);

	//! If we're getting the value of a valid paramter, get that value
	if(index < kMaxNumParameters) val=float (params[index]);

	return float (val);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::getParameterLabel (VstInt32 index, char* label)
{
	if(index < kMaxNumParameters) vst_strncpy (label, " ", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::getParameterDisplay (VstInt32 index, char* text)
{
	if(index < kMaxNumParameters)
	{
		vst_strncpy (text, " ", kVstMaxParamStrLen);
	}
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::getParameterName (VstInt32 index, char* text)
{
	if(index==kReset) vst_strncpy (text, "Reset", kVstMaxParamStrLen);
	if(index==kStart) vst_strncpy (text, "Read", kVstMaxParamStrLen);
	if(index==kWriteFile) vst_strncpy (text, "Write->T", kVstMaxParamStrLen);
	if(index==kWriteBinary) vst_strncpy (text, "Write->B", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
bool ShapeWriter::getEffectName (char* name)
{
#ifdef COMPILER_MGW
	vst_strncpy (name, "ShapeWriter (MinGW)", kVstMaxEffectNameLen);
#endif
#ifdef COMPILER_MSOFT
	vst_strncpy (name, "ShapeWriter (MSVC)", kVstMaxEffectNameLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ShapeWriter::getProductString (char* text)
{
#ifdef COMPILER_MGW
	vst_strncpy (text, "swrtMG", kVstMaxProductStrLen);
#endif
#ifdef COMPILER_MSOFT
	vst_strncpy (text, "swrtMS", kVstMaxProductStrLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ShapeWriter::getVendorString (char* text)
{
	//! This will eventually be changed to the dev group name
	vst_strncpy (text, "Destructavator", kVstMaxVendorStrLen);
	return true;
}
//! -------------------------------------------------------------------------------------------------------
VstInt32 ShapeWriter::getVendorVersion ()
{
	return 1000;
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::setSampleRate (float smplRate)
{
	sRate = float (smplRate);

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the constructor accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);
//	if(float (sRate) > float (768000.0f)) sRate=float (768000.0f);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::open()
{
	return;
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::close()
{
	return;
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::suspend()
{
	return;
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::resume()
{
	return;
}
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! Custom FX Functions -----------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
fsx ShapeWriter::RunFX(fsx const inpSpl)
{
	f9x output = f9x (inpSpl);

	if(params[kReset] > float (0.5f))
	{
		setParameterAutomated(kReset, float (0.0f));
		setParameterAutomated(kWriteFile, float (0.0f));
		setParameterAutomated(kWriteBinary, float (0.0f));
		setParameterAutomated(kStart, float (0.0f));
		if(readReady == false)
		{
			WavReset();
		}
		updateDisplay();
	}

	if(params[kStart] > float (0.5f))
	{
		if(readReady == true)
		{
			if(fsx (inpSpl) != fsx (0.0000000))
			{
				recSamp[rIndex] = fsx (inpSpl);
				rIndex++;

				if(rIndex >= 8192) readReady = false;

				// if(f9x (wavMax) < f9x (inpSpl)) wavMax = f9x (inpSpl);
			} else
			{
				if(rIndex > 2)
				{
					recSamp[rIndex] = fsx (inpSpl);
					rIndex++;

					if(rIndex >= 8192) readReady = false;
				} else
				{
//					readReady = false;
//					setParameterAutomated(kStart, float (0.0f));
//					updateDisplay();
				}
			}
		} else
		{
			setParameterAutomated(kStart, float (0.0f));
			updateDisplay();
		}
	}

	if(params[kWriteFile] > float (0.5f))
	{
		if(readReady == true)
		{
			setParameterAutomated(kWriteFile, float (0.0f));
			setParameterAutomated(kWriteBinary, float (0.0f));
			updateDisplay();
		} else
		{
			wavWrite();
		}
	}
	if(params[kWriteBinary] > float (0.5f))
	{
		if(readReady == true)
		{
			setParameterAutomated(kWriteFile, float (0.0f));
			setParameterAutomated(kWriteBinary, float (0.0f));
			updateDisplay();
		} else
		{
			wavWriteBinary();
		}
	}

	return fsx (output);
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::WavReset()
{
	rIndex = 0;
	readReady = true;

	for(s9x x = 0; x < 8192; x++)
	{
		recSamp[x] = fsx (0.0);
	}
//	for(int y = 0; y < 2000; y++)
//	{
//		wavUp[y] = f9x (0.0);
//		wavDwn[y] = f9x (0.0);
//	}
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::wavWrite()
{
/*
	f9x peak = f9x (f9x (1.0) / f9x (wavMax));
	VstInt32 halfPoint = VstInt32 (rIndex / 2);

	VstInt32 a = 0;
	for(a = 0; a <= halfPoint; a++)
	{
		wavUp[a] = f9x (f9x (recSamp[a] * f9x (peak)));
		// wavUp[a] /= f9x (M_PI_4);
	}
	for(a = halfPoint; a <= rIndex; a++)
	{
		wavDwn[a-halfPoint] = f9x (f9x (recSamp[a] * f9x (peak)));
		// wavDwn[a-halfPoint] /= f9x (M_PI_4);
	}

	f9x const mFac = f9x (10000000000000000000.0);
	int b = 0;
	std::ofstream tf;
	tf.open("C:/tmp/wav.h");
	tf << " \n";
	tf << "{\n";
	tf << "0.000,			//! index = 0\n";
	for(a = 0; a <= halfPoint; a++)
	{
		tf << "0." << u9x (f9x (f9x (wavUp[a]) * f9x (mFac))) << ",  //! index = " << a+1 << "\n";
	}
	tf << "};\n";
	tf << "\n";
	tf << "{\n";
	for(a = rIndex; a > halfPoint; a--)
	{
		tf << "0." << u9x (f9x (f9x (wavDwn[a-halfPoint]) * f9x (mFac))) << ",  //! index = " << b << "\n";
		b++;
	}
	tf << "0." << u9x (f9x (f9x (wavDwn[0]) * f9x (mFac))) << "   //! index = " << b << "\n";
	tf << "};\n";
	tf << "\n";
	tf << "HalfPoint is: " << halfPoint << "\n";
	tf << "Total is: " << rIndex-1 << "\n";
	tf << "Conversion Factor/Precision is: " << u9x (mFac) << "\n";
	tf << "\n";
	tf.close();
*/
	fhx const mFac = fhx (1000000000000000000.0);
	s9x a = 0;
	std::ofstream tf;
	tf.open("C:/tmp/wav.h");
	tf << " \n";
	tf << "static const long double wConvFac = long double (" << s9x (mFac) << ");\n";
	tf << " \n";
	tf << "s9x wav[] = {\n";
	// tf << "0.000,			//! index = 0\n";
	for(a = 0; a < 8192; a++)
	{
		tf << s9x (fhx (fhx (recSamp[a]) * fhx (mFac))) << "LL,  // index = " << a << "\n";
	}
	tf << "};\n";
	tf << "\n";
	tf << "// Conversion Factor / Precision is: " << s9x (mFac) << "\n";
	tf << "// Sample Rate is: " << s9x (sRate) << "\n";
	tf << "\n";
	tf.close();

	setParameterAutomated(kWriteFile, float (0.0f));
	setParameterAutomated(kWriteBinary, float (0.0f));
	updateDisplay();
}
//! -------------------------------------------------------------------------------------------------------
void ShapeWriter::wavWriteBinary()
{
/*	f9x peak = f9x (f9x (1.0) / f9x (wavMax));
	VstInt32 halfPoint = VstInt32 (rIndex / 2);

	VstInt32 a = 0;
	for(a = 0; a <= halfPoint; a++)
	{
		wavUp[a] = f9x (f9x (recSamp[a] * f9x (peak)));
		// wavUp[a] /= f9x (M_PI_4);
	}
	for(a = halfPoint; a <= rIndex; a++)
	{
		wavDwn[a-halfPoint] = f9x (f9x (recSamp[a] * f9x (peak)));
		// wavDwn[a-halfPoint] /= f9x (M_PI_4);
	} */
/*
	std::ofstream tf("C:/tmp/wav_raw.dat", std::ios::out | std::ios::binary | std::ios::app);
	int i = 0;
	double fred;
	for(i = 0; i < 8000; i++)
	{
		fred = double (recSamp[i]);
		tf.write((char*)&(fred), sizeof (double));
	}
	tf.close();
*/
	setParameterAutomated(kWriteFile, float (0.0f));
	setParameterAutomated(kWriteBinary, float (0.0f));
	updateDisplay();
}
//! -------------------------------------------------------------------------------------------------------
//! -------------------------------------------------------------------------------------------------------
