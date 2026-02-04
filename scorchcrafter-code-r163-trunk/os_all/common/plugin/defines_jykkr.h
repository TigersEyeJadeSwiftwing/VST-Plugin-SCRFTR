#ifndef __ScorchCrafter_Defines_Jykkr__
#define __ScorchCrafter_Defines_Jykkr__

/** ----------------------------------------------------------------------------------------------------------------
	ScorchCrafter Plug-in Defines and Attributes Header
	----------------------------------------------------------------------------------------------------------------
*/

/** These pre-processor define(s) are now kept in this seperate file so that they can be used in both
	C and C++ versions of software. */

/** ----------------------------------------------------------------------------------------------------------------
	These #define(s) can vary, and determine the characteristics of a specific plug-in.
	----------------------------------------------------------------------------------------------------------------
*/

/** Sets tiny bit of silence after clearing buffers or changing sample rate and such, to avoid pops, clicks, and spikes in the audio. */
#ifndef SCr_Global_Fade_Factor
#define SCr_Global_Fade_Factor 32
#endif
#define SCr_JYKR_AmpHead_Fade_Factor (double) (fmax(double 		(160.0), double (SCr_Global_Fade_Factor)))

#if (SC_ENABLE_STEREO_SOUND == 1)
#ifndef SC_JYKR_ENABLE_STEREO
#define SC_JYKR_ENABLE_STEREO
#endif
#endif

/** These control input filter settings.	*/
/** (Frequencies in Hz.)	*/
#define SCr_JYKR_AmpHead_InputQ_Low_Standard (double)			420.0
#define SCr_JYKR_AmpHead_InputQ_Low_Bright (double)				625.0
#define SCr_JYKR_AmpHead_InputQ_Ceiling (double)				6800.0

/** These settings affect how a stereo effect is created in stereo mode.	*/
/** Please only change them if you really know what you are doing!	*/
#define SCr_JYKR_AmpHead_QSpatialFactor_Mono (long double)		15.0
#define SCr_JYKR_AmpHead_QSpatialFactor_Left (long double) 		15.0
#define SCr_JYKR_AmpHead_QSpatialFactor_Right (long double) 	15.0

#define SCr_JYKR_AmpHead_CnvPhase_Rand_Percent	 				1.0000
#define SCr_JYKR_AmpHead_CnvSkew_Rand_Percent 					0.2000

/** These Control the overdrive/distortion.	*/
#define SC_Number_AmpChannels_C120								4

#define Dist_JYKR_TB_TubeFreqA  								{7200.0,	7200.0,		7200.0,		7200.0}
#define Dist_JYKR_TB_TubeFreqB 									{7200.0,	7200.0,		7200.0,		7200.0}

#define Dist_JYKR_TB_TubeFreqStages 							{1,		3,		4,		4}
#define Dist_JYKR_TB_TubeFreqStages_Max							4

#define Dist_JYKR_TB_PreAmp_Hardness 							{12.000,	12.000,		180.000,	180.000}
#define Dist_JYKR_TB_PowerAmp_Hardness 							{12.000,	12.000,		180.000,	180.000}

#define Dist_JYKR_TB_PreAmp_Slope 								{32.0,	32.0,	32.0,	32.0}
#define Dist_JYKR_TB_PowerAmp_Slope 							{32.0,	32.0,	32.0,	32.0}
#define Dist_JYKR_TB_Amp_Slope_Rand_Percent						10.0

#define Dist_JYKR_TB_PreAmp_Frequency 							{30.0,	30.0,	30.0,	30.0}
#define Dist_JYKR_TB_PowerAmp_Frequency 						{30.0,	30.0,	30.0,	30.0}
#define Dist_JYKR_TB_Freq_Rand_Percent 							5.0

#define Dist_JYKR_TB_PreAmp_Grit 								{600.0,		600.0,		2400.0,		2400.0}
#define Dist_JYKR_TB_POwerAmp_Grit 								{600.0,		600.0,		2400.0,		2400.0}
#define Dist_JYKR_TB_Grit_Rand_Percent 							5.0

#define Dist_JYKR_TB_Distortion_ToneFilter_Low 					{15.0,		15.0,		35.0,		35.0}
#define Dist_JYKR_TB_Distortion_ToneFilter_High 				{525.0,		525.0,		695.0,		695.0}
#define Dist_JYKR_TB_Distortion_ToneFilter_Rand_Percent 		5.0

#define Dist_JYKR_TB_AmpMult_Pre								{400.0,		2400.0,		3500.0,		35000.0}
#define Dist_JYKR_TB_AmpMult_Power								{2400.0,	2400.0,		35000.0,	35000.0}

#define Dist_JYKR_TB_OutVolume_Adjust							{5.00,		2.00,		1.00,		1.00}

/** These are for the output tone stack controls	*/
/** They are frequencies in Hz for the control bands, and determine where the bands start and end (approx.).	*/
#define SCr_JYKR_TStack_EqBand_Start_Low 				20.0
#define SCr_JYKR_TStack_EqBand_Start_Mid 				300.0
#define SCr_JYKR_TStack_EqBand_Start_High 				1000.0
#define SCr_JYKR_TStack_EqBand_Start_CntrA 				200.0
#define SCr_JYKR_TStack_EqBand_Start_CntrB 				1000.0
#define SCr_JYKR_TStack_EqBand_Start_Pres 				3000.0

#define SCr_JYKR_TStack_EqBand_End_Low 					200.0
#define SCr_JYKR_TStack_EqBand_End_Mid		 			2000.0
#define SCr_JYKR_TStack_EqBand_End_High 				7000.0
#define SCr_JYKR_TStack_EqBand_End_CntrA 				300.0
#define SCr_JYKR_TStack_EqBand_End_CntrB 				2000.0
#define SCr_JYKR_TStack_EqBand_End_Pres 				7200.0

/** Base Length of internal convolution impulses for tone stack and built-in cab/mic
		(The acutal impulse length used at run-time can vary, and depends on the host sample rate)
	 Note:  During run-time usage the actual length will automatically be re-calculated when
			the host changes sample rates, and then the internal impulses will be re-generated (calculated)
			and stored in dynamic memory.  Almost any arbitrary sample rate at or above 44.100 Hz is supported,
			including non-standard or unusual rates.	*/
/** The "Sync Rate" value(s) are used for re-calculating the impulses in memory when the sample rate changes.
		Higher values are more accurate but going too big can cause issues with high sample rates.
		This is currently implemented in an integer power-of-two value, so that a value of
		1=2, 2=4, 3=8, 4=16, 5=32, 6=64, 7=128, 8=256, 9=512, 10=1024, etc...
	Default is "9" for a calculated Sync Rate of 256x the base rate of the coded impulse values (usually 44100.0 Hz),
		which means the original coded impulse is expanded to 512 times its original size, then downsampled via
		decimation to the host sample rate.
	The "(Zero) Pad(dding) Length Factor" value determines how many extra zeros are in the impulse before the whole
		thing is smoothed with a filter, so the impulse doesn't end abruptly.  It is multiplied by the "lengths"
		values and then the host sample rate divided by the base coded impulse rate to determine the resulting
		size of the impulse(s) in memory.  Example: With a coded base rate of 44,100 Hz, a host sample rate of
		96,000 Hz, a length of 400 samples, and a padding factor of 1.20, the resulting impulse in memory that
		is used for convolution would be approx. ( (400 * 1.20) * (96000 / 44100) ) = ~1045 samples, give or
		take a little.
*/
#define SC_JYKR_TSTACK_IMPULSE_SAMPLE_RATE						44100.0

#define SC_JYKR_TSTACK_IMPULSE_Lengths_Amp						360
#define SC_JYKR_TSTACK_IMPULSE_Lengths_Cab						640
#define SC_JYKR_TSTACK_IMPULSE_SyncRate_Power_Factor			9
#define SC_JYKR_TSTACK_IMPULSE_Tail_PadLength_Factor			1.100

#endif
