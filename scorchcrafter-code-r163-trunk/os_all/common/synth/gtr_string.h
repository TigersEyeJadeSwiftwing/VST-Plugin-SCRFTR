#ifndef __SC_Synth_GuitarString__
#define __SC_Synth_GuitarString__

#include "../common/custdata_types.h"
#include "lp_btw.h"
#include "csts_eq.h"

struct SC_String_VoiceBank
{
	bool isActive;
	bool fading;
	fhx smpPlayPos;
	s9x smpPlayPosI;
	fhx smpVolume;
	fhx smpPlayRate;
	fhx fadePoint;
	fsx fadeLevel;
	int age;
	int cSample;
	int cVariationA;
	int cVariationB;
	int plyNote;
	fhx progToBankB;
	fsx decayP;
	bool isInAttackPhase;
	bool isPalmMuted;
	fhx attackProgress;
	fsx attackCurrSlope;
	fhx relativeAttack;
	int delta;
	bool chordCounted;
	fhx smpChordDelay;
	fhx humanization;
};

class Synth_GuitarString
{
public:
	Synth_GuitarString(const fsx sRt, const int stringNum, const int midStart, const int midEnd, const fsx mTune = 440.0);
	~Synth_GuitarString();

	void SetNewSampleRate(const fsx nRate);
	void InitSamples(const int baseNote = 16);
	void Clear();
	void SetNotes(const int note, const int chan = 0, const fsx vol = 1.0, const int delta = 0);
	void Silence(const int note, bool hard = false);
	// flx l_GetSound();
	fsx s_GetSound();
	// fhx h_GetSound();
	fsx GetNoteFreq(int note);
	// int GetNumberOfNotes();
	void OffsetDeltaPos(const int cNote, const int targetDelta, const int delay);

	fsx procFactor;
	fsx humanizePitchFac;

	int sampleSel;
	fsx tuneOffset;
	fhx pitchBend;
	fsx masterTune;
	bool hasAnyActive;

	int noteMap[SC_Strings_Max_Polyphany];
	int deltaMap[SC_Strings_Max_Polyphany];
	bool activeMap[SC_Strings_Max_Polyphany];

private:
	void ResetNotes();
	const int strNum;
	const flx l_PI;
	const fsx s_PI;
	const fhx h_PI;
	static const fhx noteFac[13];
	const int mStart;
	const int mEnd;

	int samplePitch;
	int chordOffset;

	fsx sRate;
	fhx sRateConvFac;
	fsx fadeSpeed;

	signed int lastActive;

	flx l_spl;
	fsx s_spl;
	fhx h_spl;
	s9x positionA;
	s9x positionB;

	fhx ***SampleA;
	fhx ***SampleB;
	bool samplesAreLoaded;

	SC_String_VoiceBank sBnk[SC_Strings_Max_Polyphany];

	s9x spLength;
	fhx spLengthF;
	fsx basePitch;
};

#endif
