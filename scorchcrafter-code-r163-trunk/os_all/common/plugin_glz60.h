#ifndef __ScorchCrafter_GLZ60__
#define __ScorchCrafter_GLZ60__

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

//! These control input filter settings.
//! (Frequencies in Hz.)
#define SCr_C120_AmpHead_InputQ_Low_Standard (double)			80.0
#define SCr_C120_AmpHead_InputQ_Low_Bright (double)				420.0
#define SCr_C120_AmpHead_InputQ_Ceiling (double)				7200.0

//! These settings affect how a stereo effect is created in stereo mode.
//! Please only change them if you really know what you are doing!
#define SCr_C120_AmpHead_QSpatialFactor_Mono (long double)		60.0
#define SCr_C120_AmpHead_QSpatialFactor_Left (long double) 		60.0
#define SCr_C120_AmpHead_QSpatialFactor_Right (long double) 	60.0
#define SCr_C120_AmpHead_CnvPhase_Left (double) 				20.0
#define SCr_C120_AmpHead_CnvPhase_Right (double) 				60.0
#define SCr_C120_AmpHead_CnvSkew_Left (double) 					2.0
#define SCr_C120_AmpHead_CnvSkew_Right (double) 				1.0

//! These Control the overdrive/distortion
//! "Stages" are the number of simulated "tubes" or runs (loops) through
//! 	distortion, listed for each channel.  If you change these, don't forget to
//! 	also update the "Max Stages" value to the highest of the four or else you can
//! 	BREAK STUFF and possibly cause a CRASH in your DAW or host.
//!
//! "T Speed" is part of how fast (in Hz) the "grit" or "grind" of the sound is,
//! 	and is combined with "T Ratio" in part of distorting the sound.  The two
//! 	values multiplied together should not be too high, unless you enjoy "fizzy"-
//! 		sounding guitar amps.
//!
//! "Tube Frequency" A and B affect the tone going in and out of each stage.  Please only
//! 	adjust them if you really know what you are doing.
//!
//! "Tube Frequency Stages" affect how many times the frequency effect is applied to each
//! 	stage.  You can drop it down lower (to as low as 1) to save CPU, and probably wouldn't notice much improvement
//! 	in the smoothness of tone going above 9 or more.
//!
//! "Number (of) Harmonics" controls the number of split harmonic bands (overtones) that get distorted.
//! 	Normally this is set to 5, please only change it if you really know what you are doing.
//! 	If you don't want overtones or want non-harmonic distortion (yawn) you can set it to 1.
//! 	(The first band is the base tone, so it must be set to at least 1, NOT zero.)
#define DIST_TB_MAX_STAGES (int) 								1

#define DIST_TB_STAGES_CLEAN (int) 								1
#define DIST_TB_STAGES_CRUNCH (int) 							1
#define DIST_TB_STAGES_HIGH (int) 								1
#define DIST_TB_STAGES_MEGA (int) 								1

#define Dist_TB_T_Speed (long double) 							200.0
#define Dist_TB_T_Ratio (long double) 							1.0
#define Dist_TB_Gain_Mult_Factor (long double)					1.0
#define Dist_TB_TubeFreqA (double) 								7200.0
#define Dist_TB_TubeFreqB (double)								7200.0
#define Dist_TB_TubeFreqStages (int) 							3
#define Dist_TB_PreAmp_Stages_Max (int)							1
#define Dist_TB_PreAmp_Stages_BoostOff (int)					1
#define Dist_TB_PreAmp_Hardness (double)						1.2750
#define Dist_TB_PowerAmp_Hardness (double)						2.00
#define Dist_TB_PreAmp_Slope (double)							1.200
#define Dist_TB_PowerAmp_Slope (double)							1.400
#define Dist_TB_PreAmp_Frequency (double)						300.0
#define Dist_TB_PowerAmp_Frequency (double)						240.0
#define Dist_TB_PreAmp_Clip_Level (double)						0.9900
#define Dist_TB_PowerAmp_Clip_Level (double)					0.9990
#define Dist_TB_PreAmp_Grit (double)							0.4000
#define Dist_TB_POwerAmp_Grit (double)							0.3000
#define Dist_TB_Distortion_ToneFilter_Low (double)				300.0
#define Dist_TB_Distortion_ToneFilter_High (double)				520.0
#define Dist_TB_Feedback (double)								0.0000

#define Dist_TB_AmpMult_Pre_Low (double)						1.0
#define Dist_TB_AmpMult_Pre_High (double)						64.0
#define Dist_TB_AmpMult_Power_Low (double)						16.0
#define Dist_TB_AmpMult_Power_High (double)						1024.0

//! These are for the output tone stack controls
//! They are frequencies in Hz for the control bands, and determine where
//!		the bands start and end (approx.).
#define SCr_C120_TStack_EqBand_Start_Low (long double)			30.0
#define SCr_C120_TStack_EqBand_Start_Mid (long double)			300.0
#define SCr_C120_TStack_EqBand_Start_High (long double)			1000.0
#define SCr_C120_TStack_EqBand_Start_CntrA (long double)		200.0
#define SCr_C120_TStack_EqBand_Start_CntrB (long double)		1000.0
#define SCr_C120_TStack_EqBand_Start_Pres (long double)			3600.0

#define SCr_C120_TStack_EqBand_End_Low (long double)			300.0
#define SCr_C120_TStack_EqBand_End_Mid (long double)			2000.0
#define SCr_C120_TStack_EqBand_End_High (long double)			7200.0
#define SCr_C120_TStack_EqBand_End_CntrA (long double)			300.0
#define SCr_C120_TStack_EqBand_End_CntrB (long double)			2000.0
#define SCr_C120_TStack_EqBand_End_Pres (long double)			16000.0

//! Base Length of internal convolution impulses for tone stack and built-in cab/mic
//! 	(The acutal impulse length used at run-time can vary, and depends on the host sample rate)
//! Note:  During run-time usage the actual length will automatically be re-calculated when
//!		the host changes sample rates, and then the internal impulses will be re-generated (calculated)
//!		and stored in dynamic memory.  Almost any arbitrary sample rate at or above 44.100 Hz is supported,
//!		including non-standard or unusual rates.  Yes, both impulse length values MUST be the SAME!
//! 		(Except one is an integer, the other floating=point.)
//!
//! The "Sync Rate" values are used for re-calculating the impulses in memory when the sample rate changes.
//! 	Higher values are more accurate but going too big can cause issues with high sample rates.
//!		Please only adjust them if you really know what you are doing.  Yes, the two sync values MUST be the SAME!
//! 		(Except one is an integer, the other floating=point.)
#define SC_TSTACK_D_IMPULSE_MAX_I (long) 						300
#define SC_TSTACK_D_IMPULSE_MAX_F (long double) 				300.0
#define SC_TSTACK_IMPULSE_AMP_MAX_I (long) 						150
#define SC_TSTACK_IMPULSE_AMP_MAX_F (long double) 				150.0
#define SC_TSTACK_IMPULSE_CAB_MAX_I (long) 						320
#define SC_TSTACK_IMPULSE_CAB_MAX_F (long double) 				320.0
#define SC_TSTACK_D_IMPULSE_SyncRate_Integer (long)				64
#define SC_TSTACK_D_IMPULSE_SyncRate_FloatingPnt (long double)	64.0
#define SC_TSTACK_IMPULSE_SyncRate_Integer (long)				32
#define SC_TSTACK_IMPULSE_SyncRate_FloatingPnt (long double)	32.0
#define SC_TSTACK_IMPULSE_SAMPLE_RATE (long double)				44100.0

#define SC_TSTACK_IMPULSE_SyncRate_Power_Factor					6
#define SC_TSTACK_IMPULSE_Tail_PadLength_Factor					1.15

//! ----------------------------------------------------------------------------------------------------------------
class Tube_Bank_F;
class ToneStack_E;
class ToneStack_F;
class Cst_EQ;

#include "sc_parameters.h"
#include "lp_btw.h"
#include "sample_bank_custom.h"
#include "tube_bank_f.h"
#include "tonestack_e.h"
#include "tonestack_f.h"
#include "csts_eq.h"

class SC_GLZ60_Amp_Head
{
//! ----------------------------------------------------------------------------------------------------------------
//! NOTE: "Public" functions of the plug-in are usually the same from (ScorchCrafter) plug-in to plug-in.
//!     If a plug-in doesn't use a specific function or parameter, it will usually return a zero as int (0) or a float/double (0.0).
//! ----------------------------------------------------------------------------------------------------------------
public:
	SC_GLZ60_Amp_Head(double sRt = 44100.0);							//! Create an instance of the plug-in, setting a starting sample rate.  (Default is 44100 kHz.)
	~SC_GLZ60_Amp_Head();

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

	Tube_Bank_F *PluGin[3];
	ToneStack_E *Qstack[3];
	ToneStack_F *Istack[3];

	Cst_EQ *InpFilter[3];
	Cst_EQ *InpFilterB[3];

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
