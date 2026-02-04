#include "gtr_string.h"
#include "gtr_strings/string_gibson_xplr_emg.h"
#include "gtr_strings/string_ibanez_rg_evo.h"

using namespace ScMath;

const fhx Synth_GuitarString::noteFac[] = {1.0,
	powl(2.0, fhx (1.0 / 12.0)),
	powl(2.0, fhx (2.0 / 12.0)),
	powl(2.0, fhx (3.0 / 12.0)),
	powl(2.0, fhx (4.0 / 12.0)),
	powl(2.0, fhx (5.0 / 12.0)),
	powl(2.0, fhx (6.0 / 12.0)),
	powl(2.0, fhx (7.0 / 12.0)),
	powl(2.0, fhx (8.0 / 12.0)),
	powl(2.0, fhx (9.0 / 12.0)),
	powl(2.0, fhx (10.0 / 12.0)),
	powl(2.0, fhx (11.0 / 12.0)),
	2.0
};

Synth_GuitarString::Synth_GuitarString(const fsx sRt, const int stringNum, const int midStart, const int midEnd, const fsx mTune) :
	sRate(sRt),
	l_PI(flx (SC_PI)),
	s_PI(fsx (SC_PI)),
	h_PI(fhx (SC_PI)),
	strNum(stringNum),
	pitchBend(fhx (1.0)),
	masterTune(fsx (mTune)),
	mStart(midStart),
	mEnd(midEnd)
{
	samplesAreLoaded = false;
	ResetNotes();
	sampleSel = 0;
	tuneOffset = 1.0;
	sRateConvFac = fhx (fhx (sRate / SC_Strings_Base_Num_SamplesPerBank));
	lastActive = -1;
	humanizePitchFac = 0.010;
	fadeSpeed = fsx (SC_Strings_OverSampling_Inv / (SC_Strings_Fade_Length * sRate));
	hasAnyActive = false;

	chordOffset = int (fhx (SC_Strings_Chord_NoteOffset * sRate) );
}
Synth_GuitarString::~Synth_GuitarString()
{
	if (samplesAreLoaded)
	{
		for (int x = 0; x < SC_Strings_Samples_Per_Each; x++)
		{
			for (int y = 0; y < SC_Samples_Number_Of_Variations; y++)
			{
				delete [] SampleA[x][y];
				delete [] SampleB[x][y];
			}
		}
		for (int x = 0; x < SC_Strings_Samples_Per_Each; x++)
		{
			delete [] SampleA[x];
			delete [] SampleB[x];
		}
		delete [] SampleA;
		delete [] SampleB;
	}
}
void Synth_GuitarString::InitSamples(const int baseNote)
{
	basePitch = fsx (GetNoteFreq(baseNote));
	samplePitch = baseNote;
	spLength = s9x (SC_Strings_Base_Num_SamplesPerBankI*2) + 16;
	spLengthF = fhx (SC_Strings_Base_Num_SamplesPerBank);
	fhx *wavSample = new fhx[spLength];
	fhx *hrmn = new fhx[SC_NUM_String_Harmonics];

	SampleA = new fhx**[SC_Strings_Samples_Per_Each];
	for (int x = 0; x < SC_Strings_Samples_Per_Each; x++)
		SampleA[x] = new fhx*[SC_Samples_Number_Of_Variations];

	SampleB = new fhx**[SC_Strings_Samples_Per_Each];
	for (int x = 0; x < SC_Strings_Samples_Per_Each; x++)
		SampleB[x] = new fhx*[SC_Samples_Number_Of_Variations];

	for (int x = 0; x < SC_Strings_Samples_Per_Each; x++)
	{
		//! Sample bank A
		for (int y = 0; y < SC_Samples_Number_Of_Variations; y++)
		{
			if (x == 0)
			{
				for (int h = 0; h < SC_NUM_String_Harmonics; h++)
					hrmn[h] = fhx (strGibsonXplrEMG_A[h]);
			}
			if (x >= 1)
			{
				for (int h = 0; h < SC_NUM_String_Harmonics; h++)
					hrmn[h] = fhx (strGibsonXplrEMG_A[h]);
			}
			for (int h = 0; h < SC_NUM_String_Harmonics; h++)
			{
				int rFac = rand() & 2000;
				rFac -= 1000;
				fhx rRnd = fhx ((fhx (rFac) * fhx (0.000010)) + fhx (1.0));
				hrmn[h] *= fhx (rRnd);
				hrmn[h] = (fhx) powl(2.0, fhx (hrmn[h] / 6.0));
			}

			SampleA[x][y] = new fhx[spLength];

			fhx pos = 0.0;
			fhx hm = 0.0;
			fhx pFac = fhx (1.0 / spLengthF);
			for (s9x z = 0; z < spLength; z++)
			{
				wavSample[z] = 0.0;
				for (int h = 0; h < SC_NUM_String_Harmonics; h++)
					wavSample[z] += fhx ( (sinl(fhx (2.0 * h_PI * pos * fhx (h+1)))) * hrmn[h] );

				pos += pFac;
				SampleA[x][y][z] = fhx (wavSample[z]);
			}
		}
		//! Sample bank B
		for (int y = 0; y < SC_Samples_Number_Of_Variations; y++)
		{
			if (x == 0)
			{
				for (int h = 0; h < SC_NUM_String_Harmonics; h++)
					hrmn[h] = fhx (strGibsonXplrEMG_B[h]);
			}
			if (x >= 1)
			{
				for (int h = 0; h < SC_NUM_String_Harmonics; h++)
					hrmn[h] = fhx (strGibsonXplrEMG_B[h]);
			}
			for (int h = 0; h < SC_NUM_String_Harmonics; h++)
			{
				int rFac = rand() & 2000;
				rFac -= 1000;
				fhx rRnd = fhx ((fhx (rFac) * fhx (0.000010)) + fhx (1.0));
				hrmn[h] *= fhx (rRnd);
				hrmn[h] = (fhx) powl(2.0, fhx (hrmn[h] / 6.0));
			}

			SampleB[x][y] = new fhx[spLength];

			fhx pos = 0.0;
			fhx hm = 0.0;
			fhx pFac = fhx (1.0 / spLengthF);
			for (s9x z = 0; z < spLength; z++)
			{
				wavSample[z] = 0.0;
				for (int h = 0; h < SC_NUM_String_Harmonics; h++)
					wavSample[z] += fhx ( (sinl(fhx (2.0 * h_PI * pos * fhx (h+1)))) * hrmn[h] );

				pos += pFac;
				SampleB[x][y][z] = fhx (wavSample[z]);
			}
		}
	}
	delete [] wavSample;
	delete [] hrmn;

	samplesAreLoaded = true;
	Silence(true);
}
void Synth_GuitarString::Clear()
{
	for (int x = 0; x < SC_Strings_Samples_Per_Each; x++)
	{
		for (int y = 0; y < SC_Samples_Number_Of_Variations; y++)
		{
			delete [] SampleA[x][y];
			delete [] SampleB[x][y];
		}
	}
	for (int x = 0; x < SC_Strings_Samples_Per_Each; x++)
	{
		delete [] SampleA[x];
		delete [] SampleB[x];
	}
	delete [] SampleA;
	delete [] SampleB;

	samplesAreLoaded = false;
}
void Synth_GuitarString::ResetNotes()
{
	lastActive = -1;
	for (int x = 0; x < SC_Strings_Max_Polyphany; x++)
	{
		sBnk[x].age = 0;
		sBnk[x].isActive = false;
		sBnk[x].fadePoint = 0.0;
		sBnk[x].fading = false;
		sBnk[x].smpPlayPos = 0.0;
		sBnk[x].smpPlayPosI = 0;

		activeMap[x] = false;
	}
}
void Synth_GuitarString::SetNotes(const int note, const int chan, const fsx vol, const int delta)
{
	const int spChan = chan;

	int voiceSel = -1;
	for (int x = 0; x < SC_Strings_Max_Polyphany; x++)
	{
		if (sBnk[x].isActive == false)
		{
			voiceSel = x;
			break;
		}
	}
	if (voiceSel == -1) return;
/*
	{
		int ageSel = -1;
		for (int x = 0; x < SC_Strings_Max_Polyphany; x++)
		{
			if (sBnk[x].age > ageSel)
			{
				ageSel = sBnk[x].age;
				voiceSel = x;
			}
		}
	}
	if (voiceSel == -1) voiceSel = 0;

	sBnk[voiceSel].isActive = true;
	sBnk[voiceSel].plyNote = note;

	for (int x = 0; x < SC_Strings_Max_Polyphany; x++)
	{
		if (sBnk[x].isActive)
		{
			sBnk[x].age += 1;
//			if (x != voiceSel)
//			{
//				sBnk[x].fading = true;
//			}
		}
	}
*/
	sBnk[voiceSel].isActive = true;
	sBnk[voiceSel].plyNote = note;
	sBnk[voiceSel].smpVolume = vol;
	sBnk[voiceSel].smpPlayRate = fhx (SC_Strings_OverSampling_Inv * GetNoteFreq(note-12) / sRateConvFac);
	sBnk[voiceSel].smpPlayPos = fhx (-SC_Strings_OverSampling_FltSQ * fhx (delta) * sBnk[voiceSel].smpPlayRate);
	sBnk[voiceSel].smpPlayPosI = 0;
	sBnk[voiceSel].humanization = 1.0;
	if (humanizePitchFac >= fsx (0.0))
	{
		int deviate = (std::rand() % 1000) - 500;
		sBnk[voiceSel].humanization = fhx (1.0 + (0.00050 * fhx (humanizePitchFac) * fhx (deviate)));
		sBnk[voiceSel].smpPlayRate *= (fhx) sBnk[voiceSel].humanization;
	}
	sBnk[voiceSel].fadePoint = 0.0;
	sBnk[voiceSel].fadeLevel = 1.0;
	sBnk[voiceSel].fading = false;
	sBnk[voiceSel].cSample = 0;
	int newVar = rand() % (SC_Samples_Number_Of_Variations - 1);
	if (newVar >= sampleSel) newVar++;
	sampleSel = newVar;
	sBnk[voiceSel].cVariationA = sampleSel;
	newVar = rand() % (SC_Samples_Number_Of_Variations - 1);
	if (newVar >= sampleSel) newVar++;
	sampleSel = newVar;
	sBnk[voiceSel].cVariationB = sampleSel;
	sBnk[voiceSel].progToBankB = fhx (0.0);
	sBnk[voiceSel].decayP = 0.0;
	sBnk[voiceSel].attackProgress = 0.0;
	sBnk[voiceSel].attackCurrSlope = 0.0;
	sBnk[voiceSel].isInAttackPhase = true;
	if (spChan < 1) sBnk[voiceSel].isPalmMuted = false;
	else sBnk[voiceSel].isPalmMuted = true;
	lastActive = voiceSel;
	sBnk[voiceSel].plyNote = note;
	sBnk[voiceSel].relativeAttack = fhx (SC_Strings_OverSampling_Inv * GetNoteFreq(note) / GetNoteFreq(44));
	sBnk[voiceSel].delta = delta;

	sBnk[voiceSel].smpChordDelay = 0.0;
	sBnk[voiceSel].chordCounted = false;

	noteMap[voiceSel] = note;
	deltaMap[voiceSel] = delta;
	activeMap[voiceSel] = true;
}
void Synth_GuitarString::OffsetDeltaPos(const int cNote, const int targetDelta, const int delay)
{
	for (int bank = 0; bank < SC_Strings_Max_Polyphany; bank++)
	{
		if ( (sBnk[bank].plyNote == cNote) && (sBnk[bank].isActive) && (activeMap[bank] == true) )
		{
			if (sBnk[bank].delta == targetDelta)
			{
				sBnk[bank].delta += delay;
				sBnk[bank].smpPlayPos = fhx (-SC_Strings_OverSampling_FltSQ * sBnk[bank].humanization * fhx (sBnk[bank].delta) * sBnk[bank].smpPlayRate);
				sBnk[bank].smpPlayPos += sBnk[bank].smpChordDelay;
				// activeMap[bank] == false;
			}
		}
	}
}
fsx Synth_GuitarString::s_GetSound()
{
	s_spl = 0.0;
	if (!samplesAreLoaded) return s_spl;
	hasAnyActive = false;

	for (int x = 0; x < SC_Strings_Max_Polyphany; x++)
	{
		if (sBnk[x].isActive)
		{
			if (sBnk[x].smpPlayPos < 0.0)
			{
				sBnk[x].smpPlayPos += sBnk[x].smpPlayRate;
				sBnk[x].smpChordDelay += sBnk[x].smpPlayRate;
			} else
			{
				activeMap[x] = false;

				positionA = sBnk[x].smpPlayPosI;
				positionB = positionA + 1;
				fsx spotA = 0.0;
				fsx spotB = 0.0;
				fsx pointB = (fsx) sBnk[x].progToBankB;

				if (sBnk[x].isPalmMuted)
					pointB = s_Diode_U(pointB, SC_Strings_Shape_PalmMuteToneTravel);

				if (positionB >= 0)
					spotB = s_avgx(SampleA[sBnk[x].cSample][sBnk[x].cVariationA][positionB],
									SampleB[sBnk[x].cSample][sBnk[x].cVariationB][positionB],
									pointB);
				if (positionA >= 0)
					spotA = s_avgx(SampleA[sBnk[x].cSample][sBnk[x].cVariationA][positionA],
									SampleB[sBnk[x].cSample][sBnk[x].cVariationB][positionA],
									pointB);

				fsx cmp_spl = s_avgx(spotA, spotB, (fsx) sBnk[x].smpPlayPos);

				if (sBnk[x].isPalmMuted == false)
					cmp_spl *= fsx (
								sBnk[x].smpVolume *
								sBnk[x].fadeLevel *
								sBnk[x].attackCurrSlope *
								(1.0 - (sBnk[x].decayP * 0.950)) *
								(1.0 - (sBnk[x].decayP * 0.950)) *
								(1.0 - sBnk[x].progToBankB)
								);
				else cmp_spl *= fsx (
								sBnk[x].smpVolume *
								sBnk[x].fadeLevel *
								sBnk[x].attackCurrSlope *
								(1.0 - sBnk[x].decayP) *
								(1.0 - sBnk[x].decayP) *
								(1.0 - sBnk[x].progToBankB)
								);

				sBnk[x].smpPlayPos += fsx (sBnk[x].smpPlayRate * fsx (pitchBend));

				if (sBnk[x].isInAttackPhase == true)
				{
					fhx atAdj = fhx (1.0 - h_min(sBnk[x].attackProgress, 1.0));

					if (sBnk[x].isPalmMuted == true)
						atAdj = (fhx) h_Diode_U(atAdj, SC_Strings_PalmMute_AttackHard);
					else atAdj = (fhx) h_Diode_U(atAdj, SC_Strings_Open_AttackHard);

					sBnk[x].attackCurrSlope = fsx (1.0 - (fsx) atAdj);
					sBnk[x].attackCurrSlope *= (fsx) sBnk[x].attackCurrSlope;

					if (sBnk[x].isPalmMuted == true) sBnk[x].attackProgress += fhx (sBnk[x].relativeAttack / (sRate*SC_Strings_PalmMute_AttackLength));
					else sBnk[x].attackProgress += fhx (sBnk[x].relativeAttack / (sRate*SC_Strings_Open_AttackLength));

					if (sBnk[x].attackProgress >= 1.0)
						sBnk[x].isInAttackPhase = false;
				} else
				{
					if (sBnk[x].isPalmMuted)
					{
						sBnk[x].progToBankB += fhx (SC_Strings_OverSampling_Inv / (sRate*SC_Strings_PalmMute_MaxLength));
						sBnk[x].decayP = (fsx) s_Diode_U(sBnk[x].progToBankB, SC_Strings_Shape_PalmMuteHardness);
					} else
					{
						sBnk[x].progToBankB += fhx (SC_Strings_OverSampling_Inv / (sRate*SC_Strings_Open_MaxLength));
						sBnk[x].decayP = (fsx) s_Diode_U(sBnk[x].progToBankB, SC_Strings_Shape_OpenHardness);
					}
					if (sBnk[x].progToBankB >= 0.99990) sBnk[x].fading = true;
					sBnk[x].progToBankB = s_min( sBnk[x].progToBankB, 1.00);
				}

				while (sBnk[x].smpPlayPos > fhx (1.0))
				{
					sBnk[x].smpPlayPos -= fhx (1.0);
					sBnk[x].smpPlayPosI += 1;
				}

				while (sBnk[x].smpPlayPosI > SC_Strings_Base_Num_SamplesPerBankI)
					sBnk[x].smpPlayPosI -= SC_Strings_Base_Num_SamplesPerBankI;

				s_spl += cmp_spl;
			}
			if (sBnk[x].fading)
			{
				sBnk[x].fadePoint += fsx (fadeSpeed);
				sBnk[x].fadeLevel = fsx (1.0 - s_Diode_U((fsx) sBnk[x].fadePoint, SC_Strings_Fade_Hardness) );
				sBnk[x].fadeLevel *= (fsx) sBnk[x].fadeLevel;

				if (sBnk[x].fadePoint >= 1.0)
				{
					sBnk[x].isActive = false;
					sBnk[x].age = 0;
					sBnk[x].smpPlayPos = 0.0;
					sBnk[x].smpPlayPosI = 0;
				}
			} else
			{
				hasAnyActive = true;
			}
		}
	}

	return s_spl;
}
fsx Synth_GuitarString::GetNoteFreq(int note)
{
	fsx ffac = fsx (masterTune * tuneOffset);
	int nfac = 69;
	if (note >= nfac)
	{
		while (note > (nfac+12))
		{
			nfac += 12;
			ffac *= 2.0;
		}
		ffac *= noteFac[note-nfac];
	} else
	{
		while (note < (nfac))
		{
			nfac -= 12;
			ffac *= 0.50;
		}
		ffac *= noteFac[note-nfac];
	}
	return ffac;
}
void Synth_GuitarString::SetNewSampleRate(const fsx nRate)
{
	sRate = nRate;
	sRateConvFac = fhx (fhx (sRate / SC_Strings_Base_Num_SamplesPerBank));
	fadeSpeed = fsx (SC_Strings_OverSampling_Inv / (SC_Strings_Fade_Length * sRate));
	chordOffset = int (fhx (SC_Strings_Chord_NoteOffset * sRate) );

	ResetNotes();
}
void Synth_GuitarString::Silence(const int note, bool hard)
{
	if (hard)
	{
		lastActive = -1;
		ResetNotes();
		return;
	}
	for (int x = 0; x < SC_Strings_Max_Polyphany; x++)
	{
		if (sBnk[x].isActive)
		{
			if (note == sBnk[x].plyNote)
				sBnk[x].fading = true;
				sBnk[x].isInAttackPhase = false;
				activeMap[x] = false;
		}
	}
	lastActive = -1;
}
