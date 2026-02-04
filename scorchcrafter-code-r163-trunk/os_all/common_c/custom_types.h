#ifndef __scorch_cr_custom_types__
#define __scorch_cr_custom_types__

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <float.h>

#ifdef __cplusplus
#include <algorithm>
using namespace std;
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SCr_PI		3.14159265358979323846264338327950288419716939937510L
#define SCr_PI_HALF	(0.50 * 3.14159265358979323846264338327950288419716939937510L)
#define SCr_PI_DOUB	(2.0 * 3.14159265358979323846264338327950288419716939937510L)
#define SCr_DENORM	0.00000000000000000001L

typedef unsigned long long u9x;
typedef signed long long s9x;
typedef unsigned short u7x;
typedef signed short s7x;
typedef unsigned int u8x;
typedef int s8x;

typedef float f32;
typedef double f64;

#define SCmath_Diode_Fixed(x,h)			(x >= 0.0 ? ( x*(h+x) / (1.0+(h*x)+(x*x)) ) : ( x*(h-x) / (1.0+(h*-x)+(x*x)) ))
#define SCmath_Diode_Scaled(x,h,s)		(x >= 0.0 ? ( x*(h+x) / (s+(h*x)+(x*x)) ) : ( x*(h-x) / (s+(h*-x)+(x*x)) ))
#define SCmath_Average(a,b)				( 0.50 * (a + b) )
#define SCmath_Averagex(a,b,x)			( (a*(1.0-x)) + (b*x) )

#define SCr_EQ_MaxNumPasses				5

#define SCr_OverSampling_FilterPasses	2
#define SCr_OverSampling_MaxFactor		16

#define SCr_Number_Slots				24
#define SCr_Params_Per_Slot				12

#ifndef SCr_GUI_Num_Visible_Slots
	#define SCr_GUI_Num_Visible_Slots	6
#endif
#if(SCr_GUI_Num_Visible_Slots < 5)
	#SCr_GUI_Num_Visible_Slots = 5
#endif
#if(SCr_GUI_Num_Visible_Slots > 10)
	#SCr_GUI_Num_Visible_Slots = 10
#endif

#define SCr_GUI_Total_GUISlot_Params	SCr_GUI_Num_Visible_Slots * SCr_Params_Per_Slot

#define SCr_GUI_Total_GUI_Height		(100 * SCr_GUI_Num_Visible_Slots)
#define SCr_GUI_Master_Controls_Height	200
#define SCr_GUI_Panel_Padding			0
#define SCr_GUI_Max_Slot_Extra_Labels	8

#define SCr_GUI_Max_BitMaps_Knobs		2
#define SCr_GUI_Max_BitMaps_KnobsBG		2
#define SCr_GUI_Max_BitMaps_SlidersH	1
#define SCr_GUI_Max_BitMaps_PushBttn	2
#define SCr_GUI_Max_BitMaps_OnOffBttn	2

enum SCr_Slots {
	SCr_slot_Empty,
	SCr_slotComponents_Tube_A,
	SCr_slotComponents_EQ_Filter,
	SCr_slotComponents_EQ_Tube,
	SCr_slotGuitarHeads_C120,
	SCr_slotGuitarHeads_Jykwrakker,
	SCr_slotBassHeads_GLZ60,
	SCr_slotCabMic_A,
	SCr_slot_Max_Types
};
enum SCr_Slot_BackGrounds {
	SCr_SlotBG_Empty,
	SCr_SlotBG_Gradient_Metal_A,
	SCr_SlotBG_Rock_HotStreaks,
	SCr_SlotBG_Metal_DiamondPlate_Red,
	SCr_SlotBG_Metal_DiamondPlate_Yellow,
	SCr_SlotBG_C120_Red,
	SCr_SlotBG_C120_Blue,
	SCr_SlotBG_C120_Gold,
	SCr_SlotBG_Max_Count
};
enum SCr_SlotControlTypes {
	SCr_SlCtrl_NONE,
	SCr_SlCtrl_Knob_A,
	SCr_SlCtrl_PushButton_A,
	SCr_SlCtrl_Slider_A,
	SCr_SlCtrl_Max_Types
};
enum SCr_SlotControlLabels {
	SCr_SlCLabel_Blank,
	SCr_SlCLabel_Volume,
	SCr_SlCLabel_Master,
	SCr_SlCLabel_Input,
	SCr_SlCLabel_Out,
	SCr_SlCLabel_Gain,
	SCr_SlCLabel_Drive,
	SCr_SlCLabel_Tone,
	SCr_SlCLabel_EQ,
	SCr_SlCLabel_Bass,
	SCr_SlCLabel_Mid,
	SCr_SlCLabel_Treble,
	SCr_SlCLabel_Presence,
	SCr_SlCLabel_Contour,
	SCr_SlCLabel_Shape,
	SCr_SlCLabel_Clean,
	SCr_SlCLabel_Crunch,
	SCr_SlCLabel_HiGain,
	SCr_SlCLabel_Dirty,
	SCr_SlCLabel_Distortion,
	SCr_SlCLabel_OverDrive,
	SCr_SlCLabel_Boost,
	SCr_SlCLabel_Phase,
	SCr_SlCLabel_Mix,
	SCr_SlCLabel_Wet,
	SCr_SlCLabel_Dry,
	SCr_SlCLabel_Dynamic,
	SCr_SlCLabel_Delay,
	SCr_SlCLabel_FeedBack,
	SCr_SlCLabel_Decay,
	SCr_SlCLabel_Tail,
	SCr_SlCLabel_Curve,
	SCr_SlCLabel_Slope,
	SCr_SlCLabel_NumPasses,
	SCr_SlCLabel_Loops,
	SCr_SlCLabel_Max_Types
};

typedef struct SCrSlot_Def_s {
	SCr_Slots nameID;
	float fIndex;
	SCr_Slot_BackGrounds BGtype;
	int nControls;
	int nExtraLabels;
	int ctrlPos[SCr_Params_Per_Slot][SCr_Params_Per_Slot];
	int cnLabelPos[SCr_Params_Per_Slot][SCr_Params_Per_Slot];
	int xLabelPos[SCr_GUI_Max_Slot_Extra_Labels][SCr_GUI_Max_Slot_Extra_Labels];
	SCr_SlotControlTypes controls[SCr_Params_Per_Slot];
	SCr_SlotControlLabels conLabels[SCr_Params_Per_Slot];
	SCr_SlotControlLabels extraLabels[SCr_GUI_Max_Slot_Extra_Labels];
} SCrSlot_Def_t;

#endif
