#include "guitarist.h"

using namespace ScMath;

//! -------------------------------------------------------------------------------------------------------
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
	return new ScGuitarist (audioMaster);
}
//! -------------------------------------------------------------------------------------------------------
ScGuitarist::ScGuitarist (audioMasterCallback audioMaster) : AudioEffectX (audioMaster, 1, kMaxNumParameters),
	outputFloat(float (0.0f)),
	outputDouble(double (0.0))
{
#ifdef COMPILER_MGW32
	setUniqueID ('0x2g');
#endif
#ifdef COMPILER_MGW64
	setUniqueID ('0x2h');
#endif
	setNumInputs(0);
	setNumOutputs(2);
	canProcessReplacing();
	canDoubleReplacing();
	isSynth();

	vst_strncpy (programName, "Default", kVstMaxProgNameLen);

	sRate = float (getSampleRate());

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the setSampleRate() function accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);
	// if(float (sRate) > float (768000.0f)) sRate=float (768000.0f);

	bDepth[0] = 64;
	bDepth[1] = 128;

	for(int x = 0; x < kMaxNumParameters; x++) {params[x] = 0.0f;};

	params[kMasterVol]=float (1.0f);
	params[kSmpBitOn64] = float (1.0f);
	params[kSmpBitOff128] = float (1.0f);

	isOnline = true;
	isOnlineOld = true;

	Synth = new SC_Synth_Guitar(fsx (sRate));
}
ScGuitarist::~ScGuitarist()
{
	delete Synth;
}
VstInt32 ScGuitarist::canDo (char* text)
{
	if (!strcmp (text, "receiveVstEvents"))
		return 1;
	if (!strcmp (text, "receiveVstMidiEvent"))
		return 1;
	if (!strcmp (text, "midiProgramNames"))
		return -1;
	return -1;	// explicitly can't do; 0 => don't know
}
bool ScGuitarist::getOutputProperties(VstInt32 index, VstPinProperties* properties)
{
	if(index<2)
	{
		sprintf(properties->label, "SC Guitar");
		properties->flags = kVstPinIsActive;
		if(index<2) properties->flags |= kVstPinIsStereo; //make channel 1+2 stereo
		return true;
	}
	return false;
}
void ScGuitarist::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    float* out1 = outputs[0];
    float* out2 = outputs[1];

    Synth->CheckForChords();

    while (--sampleFrames >= 0)
    {
		//! Here we run just about all of the plug-in code
		RunFX();

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		if(float (outputFloat) > float (4.00f)) outputFloat = float (4.00f);
		if(float (outputFloat) < float (-4.00f)) outputFloat = float (-4.00f);

		outputFloat *= float (params[kMasterVol]);

		(*out1++) = float (outputFloat);
		(*out2++) = float (outputFloat);
    }
}
void ScGuitarist::processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames)
{
    double* out1 = outputs[0];
    double* out2 = outputs[1];

    Synth->CheckForChords();

    while (--sampleFrames >= 0)
    {
		//! Here we run just about all of the plug-in code
		RunFX();

		//! The next lines are for just in case something goes very wrong and a profoundly LOUD
		//! noise comes out of the FX code, this is a safety measure
		if(double (outputDouble) > double (4.00)) outputDouble = double (4.00);
		if(double (outputDouble) < double (-4.00)) outputDouble = double (-4.00);

		outputDouble*=double (params[kMasterVol]);

		(*out1++) = double (outputDouble);
		(*out2++) = double (outputDouble);
    }
}
void ScGuitarist::setProgramName (char* name)
{
	vst_strncpy (programName, name, kVstMaxProgNameLen);
}
void ScGuitarist::getProgramName (char* name)
{
	vst_strncpy (name, programName, kVstMaxProgNameLen);
}
void  ScGuitarist::setBlockSize(VstInt32 blockSize) {	AudioEffectX::setBlockSize(blockSize); }
void ScGuitarist::setParameter (VstInt32 index, float value)
{
	float val=float (value);

	//! Safety measure, for handling illegal values
	if(float (val) > float (1.00f)) val = float (1.00f);
	if(float (val) < float (0.00f)) val = float (0.00f);

	//! If we're setting a valid parameter, update that parameter
	if(index < kMaxNumParameters) params[index] = float (val);

	//! If the GUI is up and running, pass the value to the GUI
//	if(editor)
//	{
//		if(index < kMaxNumParameters) ((AEffGUIEditor*)editor)->setParameter (index, value);
//	}
}
float ScGuitarist::getParameter (VstInt32 index)
{
	float val=float (0.0f);

	//! If we're getting the value of a valid paramter, get that value
	if(index < kMaxNumParameters) val=float (params[index]);

	return float (val);
}
void ScGuitarist::getParameterLabel (VstInt32 index, char* label)
{
	if(index < kMaxNumParameters) vst_strncpy (label, " ", kVstMaxParamStrLen);
}
void ScGuitarist::getParameterDisplay (VstInt32 index, char* text)
{
	// nothing
}
void ScGuitarist::getParameterName (VstInt32 index, char* text)
{
	if(index==kMasterVol) vst_strncpy (text, "Master V", kVstMaxParamStrLen);
}
//! -------------------------------------------------------------------------------------------------------
bool ScGuitarist::getEffectName (char* name)
{
#ifdef COMPILER_MGW32
	vst_strncpy (name, "SC-x86 Guitarist", kVstMaxEffectNameLen);
#endif
#ifdef COMPILER_MGW64
	vst_strncpy (name, "SC-x64 Guitarist", kVstMaxEffectNameLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ScGuitarist::getProductString (char* text)
{
#ifdef COMPILER_MGW32
	vst_strncpy (text, "ScG32", kVstMaxProductStrLen);
#endif
#ifdef COMPILER_MGW64
	vst_strncpy (text, "ScG64", kVstMaxProductStrLen);
#endif
	return true;
}
//! -------------------------------------------------------------------------------------------------------
bool ScGuitarist::getVendorString (char* text)
{
	vst_strncpy (text, "ScorchCrafter", kVstMaxVendorStrLen);
	return true;
}
//! -------------------------------------------------------------------------------------------------------
VstInt32 ScGuitarist::getVendorVersion ()
{
	return 1000;
}
void ScGuitarist::setSampleRate (float smplRate)
{
	sRate = float (smplRate);

	//! These two lines set min and max values, the min should not be lowered to avoid
	//! mucking up the EQ and Tonestack code.  The max can be raised, it is set to a limit simply
	//! to avoid issues just in case the VST host, by mistake, tries to set a sample rate of some
	//! astronomical, absurd value.  If you want to raise the limit, please also change the values
	//! in the constructor accordingly.
	if(float (sRate) < float (44100.0f)) sRate=float (44100.0f);

	Synth->SetNewSampleRate(sRate);
}
VstInt32 ScGuitarist::processEvents(VstEvents* ev)
{
	VstInt32 npos=0;
	ntx = 0;

	for (VstInt32 i=0; i < ev->numEvents; i++)
	{
		if((ev->events[i])->type != kVstMidiType) continue;
		VstMidiEvent* event = (VstMidiEvent*)ev->events[i];
		char* midiData = event->midiData;

		switch(midiData[0] & 0xFF) //status byte (all channels)
		{
			case 0x80: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 0);
			ntx++;
			break;
			case 0x90: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 0);
			ntx++;
			break;

			case 0x81: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 1);
			ntx++;
			break;
			case 0x91: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 1);
			ntx++;
			break;

			case 0x82: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 2);
			ntx++;
			break;
			case 0x92: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 2);
			ntx++;
			break;

			case 0x83: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 3);
			ntx++;
			break;
			case 0x93: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 3);
			ntx++;
			break;

			case 0x84: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 4);
			ntx++;
			break;
			case 0x94: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 4);
			ntx++;
			break;

			case 0x85: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 5);
			ntx++;
			break;
			case 0x95: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 5);
			ntx++;
			break;

			case 0x86: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 6);
			ntx++;
			break;
			case 0x96: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 6);
			ntx++;
			break;

			case 0x87: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 7);
			ntx++;
			break;
			case 0x97: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 7);
			ntx++;
			break;

			case 0x88: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 8);
			ntx++;
			break;
			case 0x98: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 8);
			ntx++;
			break;

			case 0x89: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 9);
			ntx++;
			break;
			case 0x99: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 9);
			ntx++;
			break;

			case 0x8A: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 10);
			ntx++;
			break;
			case 0x9A: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 10);
			ntx++;
			break;

			case 0x8B: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 11);
			ntx++;
			break;
			case 0x9B: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 11);
			ntx++;
			break;

			case 0x8C: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 12);
			ntx++;
			break;
			case 0x9C: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 12);
			ntx++;
			break;

			case 0x8D: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 13);
			ntx++;
			break;
			case 0x9D: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 13);
			ntx++;
			break;

			case 0x8E: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 14);
			ntx++;
			break;
			case 0x9E: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 14);
			ntx++;
			break;

			case 0x8F: //note off
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = 0;                  //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 15);
			ntx++;
			break;
			case 0x9F: //note on
			ntDelta = notes[npos++] = event->deltaFrames; //delta
			ntKey = notes[npos++] = midiData[1] & 0x7F; //note
			ntVel = notes[npos++] = midiData[2] & 0x7F; //vel
			if(ntx < 32) Synth->noteOn(ntKey, ntVel, ntDelta, 15);
			ntx++;
			break;

			case 0xB0:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xB1:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xB2:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xB3:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xB4:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xB5:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xB6:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xB7:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xB8:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xB9:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xBA:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xBB:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xBC:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xBD:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xBE:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;
			case 0xBF:	//controller
			if(midiData[1]>0x7A) Synth->AllOff();
			break;

			default: ntx++; break;
		}

		if(npos > 120) npos -= 3; //discard events if buffer full!!
		if(ntx > 30000) ntx = 30000;
		event++;
	}

	Synth->CheckForChords();

	return 1;
}
void ScGuitarist::RunFX()
{
	Synth->RunPlugin();
	outputDouble = Synth->GetMonoOut();
	outputFloat = float (outputDouble);
}
void ScGuitarist::noteOn(VstInt32 note, VstInt32 velocity, VstInt32 delta, int ichan)
{
	Synth->noteOn(note, velocity, delta, ichan);
}
