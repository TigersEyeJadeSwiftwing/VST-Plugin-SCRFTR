#include "plugin_guitarist.h"
#include <algorithm>

using namespace ScMath;

SC_Synth_Guitar::SC_Synth_Guitar(double sRt) :
	fadeFac(fsx (SCr_C120_AmpHead_Fade_Factor)),
	smpRate(fsx (sRt)),
	l_dNrmThresh(flx (powf(10.f, -28.f))),
	s_dNrmThresh(fsx (pow(10.0, -32.0))),
	h_dNrmThresh(fhx (powl(10.0, -36.0))),
	splInputMono(fsx (0.0)),
	splInputLeft(fsx (0.0)),
	splInputRight(fsx (0.0)),
	splOutputMono(fsx (0.0)),
	splOutputLeft(fsx (0.0)),
	splOutputRight(fsx (0.0)),
	l_PI(flx (SC_PI)),
	s_PI(fsx (SC_PI)),
	h_PI(fhx (SC_PI))
{
	FpParam[ScPrmF_Master_Volume] = flx (1.0f);

	IntParam[ScPrmI_Active_And_Ready] = 0;
	IntParam[ScPrmI_Error_Status] = 0;
	IntParam[ScPrmI_FP_Precision_Bit_Depth] = SCv_Bit_Depth_64;
	IntParam[ScPrmI_Phase_Control] = 0;

	fadeMax = long (fsx (fadeFac) * fsx (smpRate / 44100.0));
	fadeCount = fadeMax;

	GtStrng = new Synth_GuitarString(smpRate, 0, 0, 128, 440.0);
	GtStrng->InitSamples(0);

	mNoteCount = 0;

	PlayFilter = new BTW_LP(smpRate * 0.250, smpRate * SC_Strings_OverSampling_Flt);
/*
	for (int x = 0; x <= 56; x++)
		ntMap[0].stringNote[x] = 0;
	for (int x = 57; x <= 61; x++)
		ntMap[0].stringNote[x] = 1;
	for (int x = 62; x <= 66; x++)
		ntMap[0].stringNote[x] = 2;
	for (int x = 67; x <= 70; x++)
		ntMap[0].stringNote[x] = 3;
	for (int x = 71; x <= 75; x++)
		ntMap[0].stringNote[x] = 4;
	for (int x = 76; x <= 127; x++)
		ntMap[0].stringNote[x] = 5;

	for (int x = 0; x <= 61; x++)
		ntMap[1].stringNote[x] = 0;
	for (int x = 62; x <= 66; x++)
		ntMap[1].stringNote[x] = 1;
	for (int x = 67; x <= 71; x++)
		ntMap[1].stringNote[x] = 2;
	for (int x = 72; x <= 75; x++)
		ntMap[1].stringNote[x] = 3;
	for (int x = 76; x <= 80; x++)
		ntMap[1].stringNote[x] = 4;
	for (int x = 81; x <= 127; x++)
		ntMap[1].stringNote[x] = 5;

	for (int x = 0; x <= 68; x++)
		ntMap[2].stringNote[x] = 0;
	for (int x = 69; x <= 73; x++)
		ntMap[2].stringNote[x] = 1;
	for (int x = 74; x <= 78; x++)
		ntMap[2].stringNote[x] = 2;
	for (int x = 79; x <= 82; x++)
		ntMap[2].stringNote[x] = 3;
	for (int x = 83; x <= 87; x++)
		ntMap[2].stringNote[x] = 4;
	for (int x = 88; x <= 127; x++)
		ntMap[2].stringNote[x] = 5;
*/
	IntParam[ScPrmI_Active_And_Ready] = 1;
}
SC_Synth_Guitar::~SC_Synth_Guitar()
{
	delete GtStrng;

	delete PlayFilter;
}
void SC_Synth_Guitar::SetNewSampleRate(const double inp)
{
	smpRate = fsx (inp);
	if(fsx (smpRate) < fsx (44100.0)) smpRate = fsx (44100.0);

	fadeMax = long (fsx (fadeFac) * fsx (smpRate / 44100.0));
	fadeCount = fadeMax;

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);

	PlayFilter->setSampleRate(smpRate * SC_Strings_OverSampling_Flt);
	PlayFilter->changeFreq(smpRate * 0.250);

	GtStrng->SetNewSampleRate(smpRate);

	mNoteCount = 0;
}
void SC_Synth_Guitar::ClearAllBuffers()
{
	fadeCount = fadeMax;

	mNoteCount = 0;

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);
}
void SC_Synth_Guitar::SetFpParam(const int prm, float newValue)
{
	if(prm < 0) return;
	if(prm > ScPrmF_MAX_COUNT) return;
	if(flx (newValue) < flx (0.0f)) return;
	if(flx (newValue) > flx (1.0f)) return;

	if(prm == ScPrmF_Master_Volume) FpParam[prm] = flx (newValue);
}
void SC_Synth_Guitar::SetIntParam(const int prm, int newValue)
{
	if(prm < 0) return;
	if(prm > ScPrmI_MAX_COUNT) return;
	if(newValue < 0) return;

	if(prm == ScPrmI_Error_Status)
	{
		IntParam[ScPrmI_Error_Status] = 0;
		ClearAllBuffers();
	}
	if(prm == ScPrmI_Active_And_Ready)
	{
		if(newValue > 1) return;
		IntParam[prm] = newValue;
		ClearAllBuffers();
	}
	if(prm == ScPrmI_FP_Precision_Bit_Depth)
	{
		if(newValue >= SCv_Bit_Depth_MAX_COUNT) return;

		IntParam[prm] = newValue;
		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_Phase_Control)
		IntParam[prm] = newValue;
}
void SC_Synth_Guitar::InputSample(const double inp)
{
	splInputMono = fsx (inp);
}
void SC_Synth_Guitar::InputSampleStereo(const double inpL, const double inpR)
{
	splInputMono = fsx (fsx (fsx (inpL) + fsx (inpR)) * fsx (0.50));
	splInputLeft = inpL;
	splInputRight = inpR;
}
double SC_Synth_Guitar::GetMonoOut()
{
	return fsx (splOutputMono);
}
double SC_Synth_Guitar::GetStereoOutLeft()
{
	return fsx (splOutputLeft);
}
double SC_Synth_Guitar::GetStereoOutRight()
{
	return fsx (splOutputRight);
}
float SC_Synth_Guitar::GetFpParam(const int prm)
{
	if(prm < 0) return float (0.0f);

	if(prm == ScPrmF_Master_Volume) return float (FpParam[prm]);

	return float (0.0f);
}
int SC_Synth_Guitar::GetIntParam(const int prm)
{
	if(prm < 0) return 0;

	if(prm == ScPrmI_Active_And_Ready) return IntParam[prm];
	if(prm == ScPrmI_Error_Status) return IntParam[prm];
	if(prm == ScPrmI_FP_Precision_Bit_Depth) return IntParam[prm];

	return 0;
}
long long SC_Synth_Guitar::GetLongParam(const int prm)
{
	return 0;
}
void SC_Synth_Guitar::RunPlugin()
{
//! ----------------------------------------------------------------------------------------------------------------
//! 32-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_32)
	{
		l_dNrm = flx (l_dNrmThresh * flx (0.1f) * (flx) sinf(l_PI * l_deNormSuppress));
		l_deNormSuppress += flx (0.50f);
		if (l_deNormSuppress > flx (4.0f)) l_deNormSuppress -= flx (2.0f);

		if(fadeCount > 0)
		{
			fadeCount--;
			splOutputMono = fsx (0.0);
			splOutputLeft = fsx (0.0);
			splOutputRight = fsx (0.0);
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 64-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_64)
	{
		s_dNrm = fsx (s_dNrmThresh * fsx (0.10) * (fsx) sin(s_PI * s_deNormSuppress));
		s_deNormSuppress += fsx (0.50);
		if (s_deNormSuppress > fsx (4.0)) s_deNormSuppress -= fsx (2.0);

		s_smp[0] = 0.0;
		fsx output = s_dNrm;
		for(int x = 0; x < SC_Strings_OverSampling_Int; x++)
		{
			output = s_dNrm;
		//	for (int x = 0; x < SC_Strings_Total_Number; x++)
				output += GtStrng->s_GetSound();

			PlayFilter->inpSplS( output );
			PlayFilter->RunFilter();
		}
		s_smp[0] = s_smp[1] = s_smp[2] = PlayFilter->GetOutPutS();

		splOutputMono = splOutputLeft = splOutputRight = s_smp[0];
		if(fadeCount > 0)
		{
			fadeCount--;
			splOutputMono = fsx (0.0);
			splOutputLeft = fsx (0.0);
			splOutputRight = fsx (0.0);
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 128-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_128)
	{
		h_dNrm = fhx (h_dNrmThresh * fhx (0.10) * (fhx) sinl(h_PI * h_deNormSuppress));
		h_deNormSuppress += fhx (0.50);
		if (h_deNormSuppress > fhx (4.0)) h_deNormSuppress -= fhx (2.0);

		if(fadeCount > 0)
		{
			fadeCount--;
			splOutputMono = fsx (0.0);
			splOutputLeft = fsx (0.0);
			splOutputRight = fsx (0.0);
		}
	}
}

//! ----------------------------------------------------------------------------------------------------------------
//! MIDI Processing
//! ----------------------------------------------------------------------------------------------------------------
void SC_Synth_Guitar::noteOn(int note, int velocity, int delta, int ichan)
{
	if (ichan >= SC_Strings_Samples_Per_Each) return;

	mNotes[mNoteCount] = note;
	mDeltas[mNoteCount] = delta;
	mNoteCount++;
	if (mNoteCount > (SC_Strings_Max_MidiInput+1)) mNoteCount = SC_Strings_Max_MidiInput+1;

	if (velocity == 0)
	{
		GtStrng->Silence(note);
	} else
	{
		GtStrng->SetNotes(note, ichan, fsx (fsx (velocity) / 127.0), delta);
	}
}
void SC_Synth_Guitar::CheckForChords()
{
	return;

	if (mNoteCount == 0) return;

	if (!GtStrng) return;

	mNoteCount = 0;

	int numPoints = 0;
	for (int x = 0; x < SC_Strings_Max_Polyphany; x++)
	{
		if (GtStrng->activeMap[x])
		{
			deltaVals[x] = GtStrng->deltaMap[x];
			numPoints++;
		}
	}

	if (numPoints < 2) return;
/*
	deltaVals[numPoints] = -1;
	std::sort(deltaVals, deltaVals+numPoints);
	int numChords = 0;
	int bnkCount = 0;
	for (int x = 1; x <= numPoints; x++)
	{
		if (deltaVals[x-1] == deltaVals[x]) bnkCount++;
		else
		{
			if (bnkCount > 0)
			{
				chordVals[numChords] = deltaVals[x-1];
				numChords++;
			}
			bnkCount = 0;
		}
	}
*/
	deltaVals[numPoints] = 32767;
	std::sort(deltaVals, deltaVals+numPoints+1);
	int numChords = 0;
	int bnkCount = 0;
	for (int x = 1; x <= numPoints; x++)
	{
		if (deltaVals[x-1] == deltaVals[x])
		{
			bnkCount++;
		} else
		{
			if (bnkCount > 0)
			{
				chordVals[numChords] = deltaVals[x-1];
				numChords++;
			}
			bnkCount = 0;
		}
	}

	if (numChords == 0) return;

	int deltaDelay = int ( fhx (smpRate * SC_Strings_Chord_NoteOffset) );

	for (int x = 0; x < numChords; x++)
	{
		const int deltaPoint = chordVals[x];
		int noteCount = 0;
		for (int y = 0; y < SC_Strings_Max_Polyphany; y++)
		{
			if ((GtStrng->activeMap[y] == true) && (GtStrng->deltaMap[y] == deltaPoint))
			{
				chordOrdered[noteCount] = GtStrng->noteMap[y];
				noteCount++;
			}
		}

		if (noteCount < 2) continue;

		std::sort(chordOrdered, chordOrdered+noteCount);

		for (int z = 0; z < noteCount; z++)
			GtStrng->OffsetDeltaPos(chordOrdered[z], deltaPoint, (deltaDelay*z));
	}
}
void SC_Synth_Guitar::AllOff()
{
	GtStrng->Silence(0, true);
}
