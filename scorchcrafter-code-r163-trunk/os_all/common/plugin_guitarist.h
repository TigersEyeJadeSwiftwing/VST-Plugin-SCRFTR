#ifndef __SC_Synth_Guitar__
#define __SC_Synth_Guitar__

//! ----------------------------------------------------------------------------------------------------------------
//! ScorchCrafter Plug-in General Class Header
//!     Revision 2.1
//! ----------------------------------------------------------------------------------------------------------------

//! Note: Please look at "custdata_types.h" for details on the typdefs used throughout the software (such as "fsx" or "fhx" etc.).
#include "../common/custdata_types.h"

//! ----------------------------------------------------------------------------------------------------------------
//! These check for Global, or "Master" #define(d) values (In case of combination plug-ins made up of more than one SC plug-in)
//! ----------------------------------------------------------------------------------------------------------------
#ifndef SCr_Global_Fade_Factor
#define SCr_Global_Fade_Factor (double) 0.0
#endif

//! ----------------------------------------------------------------------------------------------------------------
//! These #define(s) can vary by plug-in
//! ----------------------------------------------------------------------------------------------------------------

//! Sets tiny bit of silence after clearing buffers or changing sample rate and such, to avoid pops, clicks, and spikes in the audio
#define SCr_C120_AmpHead_Fade_Factor (double) (fmax(double 		(256.0), double (SCr_Global_Fade_Factor)))

#define SC_Samples_Number_Of_Variations							18
#define SC_Strings_Samples_Per_Each								2
#define SC_Strings_Max_Polyphany								24
#define SC_Strings_Chord_NoteOffset								0.10
#define SC_Strings_Max_MidiInput								128
#define SC_Strings_OverSampling_Int								1
#define SC_Strings_OverSampling_Flt								1.0
#define SC_Strings_OverSampling_FltSQ							1.0
#define SC_Strings_OverSampling_Inv								1.0

#define SC_Strings_Fade_Length									0.250
#define SC_Strings_Fade_Hardness								4.0

#define SC_Strings_Shape_OpenHardness							128.0
#define SC_Strings_Shape_PalmMuteHardness						24.0
#define SC_Strings_Shape_PalmMuteToneTravel						64.0

#define SC_Strings_Open_MaxLength								16.0
#define SC_Strings_PalmMute_MaxLength							1.0

#define SC_Strings_Open_AttackLength							0.0300
#define SC_Strings_PalmMute_AttackLength						0.0240

#define SC_Strings_Open_AttackHard								32.0
#define SC_Strings_PalmMute_AttackHard							32.0

#define SC_Strings_Base_Num_SamplesPerBank						2048.0
#define SC_Strings_Base_Num_SamplesPerBankI						2048

#define SC_NUM_String_Harmonics									16

//! ----------------------------------------------------------------------------------------------------------------
class Cst_EQ;
class BTW_LP;

#include "sc_parameters.h"
#include "lp_btw.h"
#include "csts_eq.h"
#include "gtr_string.h"

struct StringNoteMap
{
	int stringNote[128];
};

class SC_Synth_Guitar
{
//! ----------------------------------------------------------------------------------------------------------------
//! NOTE: "Public" functions of the plug-in are usually the same from (ScorchCrafter) plug-in to plug-in.
//!     If a plug-in doesn't use a specific function or parameter, it will usually return a zero as int (0) or a float/double (0.0).
//! ----------------------------------------------------------------------------------------------------------------
public:
	SC_Synth_Guitar(double sRt = 44100.0);							//! Create an instance of the plug-in, setting a starting sample rate.  (Default is 44100 kHz.)
	~SC_Synth_Guitar();

	void SetNewSampleRate(const double inp);							//! Use this whenever changing the standard sampling rate (NOT oversampling).
	void ClearAllBuffers();												//! Should be used when starting or stopping playback, or to silence effect "tail" tones.
																		//! Note:  This is automatically called when chagning certain other settings, such as sample rate, etc.,
																		//! So you do NOT need to call it when setting parameters, the plug-in will clear buffers as needed automatically in such cases.
	void SetFpParam(const int prm, float newValue);					//! Sets internal parameter (from the sc_parameters.h list) to a new 32-bit float value from 0.0f to 1.0f.
	void SetIntParam(const int prm, int newValue);					//! Sets internal parameter to new integer or boolean value, valid values can differ.
	void InputSample(const double inp);								//! Send an audio sample to the plugin (64-bit floating point).  Usually (but not always) should be from -1.0 to +1.0
	void InputSampleStereo(const double inpL, const double inpR);	//! Just like InputSample(), except for sending stereo sound to the plugin.
	void RunPlugin();													//! Always run this ONCE in the program loop for the plugin, to run all internals.  JUST ONCE, even when oversampling!
	double GetMonoOut();												//! Returns processed and/or generated audio of the plugin, in one mono channel.
	double GetStereoOutLeft();											//! Gets the LEFT channel audio output proecessed and/or generated by the plugin.
	double GetStereoOutRight();										//! Gets the RIGHT channel audio output proecessed and/or generated by the plugin.
	float GetFpParam(const int prm);									//! Returns 32-bit float internal value.  If the plugin doesn't have such a paramter, it returns 0.0f.
	int GetIntParam(const int prm);									//! Returns internal integer value.  If it is an invalid paramteter, it returns zero.
	long long GetLongParam(const int prm);							//! Returns internal integer value of great size.  If it is an invalid paramteter, it returns zero.  Not all plug-ins use this.
	long GetPluginInterfaceVersion() { return long (2100); };			//! Returns Version of Plug-in General Class Header (this file).  Major revisions are x1000, so rev. 2.0 is "2000"

	void noteOn(int note, int velocity, int delta, int ichan = 0);
	void AllOff();
	void CheckForChords();

	//! These aren't used in single-plugin VSTs, but are for a standalone app or plug-in suite.
	float l_GetOutMono() { return l_smp[0]; };
	float l_GetOutLeft() { return l_smp[1]; };
	float l_GetOutRight() { return l_smp[2]; };

	double s_GetOutMono() { return s_smp[0]; };
	double s_GetOutLeft() { return s_smp[1]; };
	double s_GetOutRight() { return s_smp[2]; };

	long double h_GetOutMono() { return h_smp[0]; };
	long double h_GetOutLeft() { return h_smp[1]; };
	long double h_GetOutRight() { return h_smp[2]; };

//! ----------------------------------------------------------------------------------------------------------------
//! NOTE: "Private" functions, variables, objects, etc. can vary by plug-in.
//! ----------------------------------------------------------------------------------------------------------------
private:
	long fadeCount;					// Counts down and must be zero before audio is sent to the output.  Used to avoid pops and clicks.  Jumps up to the max when buffers are cleared or the plugin is reset.
	long fadeMax;					// This is the maximum for the fadecount, and is changed when the sample rate is updated.
	const double fadeFac;			// Determines how long the fade should be when buffers are cleared, this value can be different in various plugins but is fixed and does not change.
	double smpRate;				// Current sample rate that the plugin recognizes and runs at.
	double splInputMono;			// The input value the plug-in uses to preocess any audio from, 64-bit floating point.  Use the InputSample() function to pass audio in a stream to it.
	double splInputLeft;			// Same as splInputMono, except as the left channel of stereo input.
	double splInputRight;			// Same as splInputMono, except as the right channel of stereo input.
	double splOutputMono;			// Output audio sample after any processing, 64-bit floating point.  Use GetMonoOut() to retrieve its value.
	double splOutputLeft;			// Output audio sample after any processing, 64-bit floating point.  Use GetMonoOut() to retrieve its value.
	double splOutputRight;			// Output audio sample after any processing, 64-bit floating point.  Use GetMonoOut() to retrieve its value.

	flx l_smp[3];					// Internal sample data, 32 bit (float)
	fsx s_smp[3];					// Internal sample data, 64 bit (double)
	fhx h_smp[3];					// Internal sample data, 128 bit (long double)

	float FpParam[ScPrmF_MAX_COUNT];
	int IntParam[ScPrmI_MAX_COUNT];

	Synth_GuitarString *GtStrng;

	int mNotes[SC_Strings_Max_MidiInput];
	int mDeltas[SC_Strings_Max_MidiInput];
	int mNoteCount;
	int numNotesSame[SC_Strings_Max_Polyphany];
	int deltaVals[SC_Strings_Max_Polyphany];
	int chordVals[SC_Strings_Max_Polyphany];
	int chordOrdered[SC_Strings_Max_Polyphany];

	BTW_LP *PlayFilter;

	const flx l_PI;
	const fsx s_PI;
	const fhx h_PI;

	flx l_deNormSuppress;
	fsx s_deNormSuppress;
	fhx h_deNormSuppress;
	const flx l_dNrmThresh;
	const fsx s_dNrmThresh;
	const fhx h_dNrmThresh;
	flx l_dNrm;
	fsx s_dNrm;
	fhx h_dNrm;
};

#endif
