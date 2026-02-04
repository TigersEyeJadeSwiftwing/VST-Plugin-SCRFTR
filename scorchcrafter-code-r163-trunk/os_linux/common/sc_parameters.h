#ifndef __ScorchCrafter_Global_PlugIn_Params__
#define __ScorchCrafter_Global_PlugIn_Params__

//! ------------------------------------------------------------------
//! Global Parameter List
//! ------------------------------------------------------------------
//! All ScorchCrafter plug-ins use some, but usually not all of
//! the parameters from these lists.  Which ones are used depends
//! on what specific plugin(s) is/are included.
//!
//! The first list is for parameters set with floating-point
//! (32-bit "float") values, which are valid from 0.0f to 1.0f.
//!
//! The second list is for parameters set usually by integer values,
//! with a minimum and maximum value that can differ depending on what
//! the parameter is and what the plug-in accepts.
//!
//! The second list is sometimes also used for boolean values, with
//! 0 == false and 1 == true.
//!
//! Both lists can also be used for getting values returned by functions
//! of a plugin.  (For example, you may notice the "Error_Status" entry
//! in the second list.  Usually a value of 0 (zero) for this means
//! that no error is detected and all is well.)
//! ------------------------------------------------------------------

enum SC_Parameters_FP
{
	ScPrmF_Master_Volume,
	ScPrmF_Mix_Wet,
	ScPrmF_Mix_Dry,
	ScPrmF_Effect_Amount,
	ScPrmF_Drive_Gain,
	ScPrmF_EQ_Low,
	ScPrmF_EQ_Mid,
	ScPrmF_EQ_High,
	ScPrmF_EQ_Contour,
	ScPrmF_EQ_Floor,
	ScPrmF_EQ_Presence,
	ScPrmF_MAX_COUNT
};
enum SC_Parameters_INT
{
	ScPrmI_Active_And_Ready,
	ScPrmI_Error_Status,
	ScPrmI_Drive_Channel,
	ScPrmI_Bright,
	ScPrmI_OverSampling_Rate,
	ScPrmI_Phase_Control,
	ScPrmI_FP_Precision_Bit_Depth,
	ScPrmI_MAX_COUNT
};
enum SC_FP_Bit_Depth_Rates
{
	SC_Bit_Depth_32,
	SC_Bit_Depth_64,
	SC_Bit_Depth_128,
	SC_Bit_Depth_MAX_COUNT
};
enum SC_OverSampling_Rates
{
	SC_OverSampling_1x,
	SC_OverSampling_2x,
	SC_OverSampling_4x,
	SC_OverSampling_8x,
	SC_OverSampling_16x,
	SC_OverSampling_MAX_COUNT
};

#endif
