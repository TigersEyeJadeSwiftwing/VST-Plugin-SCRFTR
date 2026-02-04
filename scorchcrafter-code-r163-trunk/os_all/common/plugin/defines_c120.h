#ifndef __ScorchCrafter_Defines_C120__
#define __ScorchCrafter_Defines_C120__

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
#define SCr_Global_Fade_Factor 128
#endif
#define SCr_C120_AmpHead_Fade_Factor (double) (fmax(double 		(8192.0), double (SCr_Global_Fade_Factor)))

#if (SC_ENABLE_STEREO_SOUND == 1)
#ifndef SC_C120_ENABLE_STEREO
#define SC_C120_ENABLE_STEREO
#endif
#endif

/** These settings affect how a stereo effect is created in stereo mode.	*/
/** Please only change them if you really know what you are doing!	*/
#define SCr_C120_AmpHead_QSpatialFactor_Mono					80.0
#define SCr_C120_AmpHead_QSpatialFactor_Left			 		80.0
#define SCr_C120_AmpHead_QSpatialFactor_Right				 	80.0

#define SCr_C120_AmpHead_CnvPhase_Rand_Percent	 				1.0000
#define SCr_C120_AmpHead_CnvSkew_Rand_Percent 					0.2000

#define SCr_C120_AmpHead_RandFactor								0.0750

/** These Control the overdrive/distortion.	*/
#define SC_Number_AmpChannels_C120								4

#define Dist_C120_TB_TubeFreqA  								{7200.0,	7200.0,		7200.0,		7200.0}
#define Dist_C120_TB_TubeFreqB 									{7200.0,	7200.0,		7200.0,		7200.0}
#define Dist_C120_TB_TubeFreqC 									{5.0,		5.0,		5.0,		5.0}
#define Dist_C120_TB_TubeFreqD 									{5.0,		5.0,		5.0,		5.0}
#define Dist_C120_TB_TubeFreq_Rand_Percent						10.0

#define Dist_C120_TB_TubeFreqGritPasses 						{1,			1,			1,			1}

#define Dist_C120_TB_NumberOfStages_Pre							{1,			1,			1,			2}
#define Dist_C120_TB_NumberOfStages_Power						{1,			1,			1,			1}

#define Dist_C120_TB_TubeFreqStages 							{1,			2,			3,			3}
#define Dist_C120_TB_TubeFreqStages_Max							3

#define Dist_C120_TB_PreAmp_Hardness 							{2.0,		4.0,		3.0,		3.0}
#define Dist_C120_TB_PowerAmp_Hardness 							{4.0,		4.0,		3.0,		3.0}
#define Dist_C120_TB_Amp_Hard_Rand_Percent						5.00

#define Dist_C120_TB_PreAmp_Slope 								{4.0,		4.0,		9.0,		9.0}
#define Dist_C120_TB_PowerAmp_Slope 							{4.0,		4.0,		9.0,		9.0}
#define Dist_C120_TB_Amp_Slope_Rand_Percent						7.50

#define Dist_C120_TB_PreAmp_Frequency 							{7200.0,	7200.0,		7200.0,		7200.0}
#define Dist_C120_TB_PowerAmp_Frequency 						{7200.0,	7200.0,		7200.0,		7200.0}
#define Dist_C120_TB_Freq_Rand_Percent 							5.0

#define Dist_C120_TB_PreAmp_Grit 								{12.0,		12.0,		12.0,		12.0}
#define Dist_C120_TB_POwerAmp_Grit 								{12.0,		12.0,		12.0,		12.0}
#define Dist_C120_TB_Grit_Rand_Percent 							7.50

#define Dist_C120_TB_Distortion_ToneFilter_Low 					{7200.0,	7200.0,		7200.0,		7200.0}
#define Dist_C120_TB_Distortion_ToneFilter_High 				{20000.0,	20000.0,	20000.0,	20000.0}
#define Dist_C120_TB_Distortion_ToneFilter_Rand_Percent 		10.0

#define Dist_C120_TB_AmpMult_Pre								{0.10,		0.50,		0.75,		1.05}
#define Dist_C120_TB_AmpMult_Power								{0.50,		0.50,		0.95,		0.95}

#define Dist_C120_TB_RectVolume_Adjust							{0.25,		0.375,		0.925,		0.925}
#define Dist_C120_TB_RectifierPasses 							{0,			0,			0,			0}

#define Dist_C120_TB_OutVolume_Adjust							{2.00,		2.00,		2.00,		2.00}

/** These are for the output tone stack controls	*/
/** They are frequencies in Hz for the control bands, and determine where the bands start and end (approx.).	*/
#define SCr_C120_TStack_EqBand_Start_Low 				20.0
#define SCr_C120_TStack_EqBand_End_Low 					200.0
#define SCr_C120_TStack_EqBand_Start_Mid 				200.0
#define SCr_C120_TStack_EqBand_End_Mid		 			2000.0
#define SCr_C120_TStack_EqBand_Start_High 				2000.0
#define SCr_C120_TStack_EqBand_End_High 				5000.0

#define SCr_C120_TStack_EqBand_Start_CntrA 				200.0
#define SCr_C120_TStack_EqBand_End_CntrA 				300.0
#define SCr_C120_TStack_EqBand_Start_CntrB 				1000.0
#define SCr_C120_TStack_EqBand_End_CntrB 				2000.0
#define SCr_C120_TStack_EqBand_Start_Pres 				3000.0
#define SCr_C120_TStack_EqBand_End_Pres 				7200.0

#define SCr_C120_TStack_DynBass_Freq_A					195.0
#define SCr_C120_TStack_DynBass_Freq_B					40.0
#define SCr_C120_TStack_DynBass_Freq_Travel				10.0
#define SCr_C120_TStack_DynBass_Curve					6.0
#define SCr_C120_TStack_DynBass_Range					5.000
#define SCr_C120_TStack_DynBass_Offset					4.500

#endif
