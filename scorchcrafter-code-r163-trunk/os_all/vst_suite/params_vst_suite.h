#ifndef __SCr_Suite_Parameters__
#define __SCr_Suite_Parameters__

#include "../common_c/custom_types.h"

enum kScorchCparameters_Suite
{
	kSuite_MasterVol = 0,
	kSuite_InputBoost = 1,
	kSuite_Sampling_OnLine = 2,
	kSuite_Sampling_OffLine = 3,
	kSuite_GUI_Catagory_Select_start = 4,
	kSuite_GUI_Catagory_Select_End = 12,
	kSuite_GUI_Item_Select_Start = 13,
	kSuite_GUI_Item_Select_End = 21,
	kSuite_GUI_ScrollSlots_Up = 22,
	kSuite_GUI_ScrollSlots_Down = 23,
	kSuite_GUISlot_Controls_Start = 24,
	kSuite_GUISlot_Controls_End = (kSuite_GUISlot_Controls_Start + (7 * SCr_GUI_Num_Visible_Slots)),
	kSuite_GUISlot_Defs_Start = (kSuite_GUISlot_Controls_End + 1),
	kSuite_GUISlot_Defs_End = (kSuite_GUISlot_Defs_Start + SCr_GUI_Num_Visible_Slots),
	kSuite_GUISlots_Start = (kSuite_GUISlot_Defs_End + 1),
	kSuite_GUISlots_End = (kSuite_GUISlots_Start + (SCr_Params_Per_Slot * SCr_GUI_Num_Visible_Slots)),
	kSuite_Slots_Defs_Start = (kSuite_GUISlots_End + 1),
	kSuite_Slots_Defs_End = (kSuite_Slots_Defs_Start + SCr_Number_Slots),
	kSuite_Slots_Start = (kSuite_Slots_Defs_End + 1),
	kSuite_Slots_End = (kSuite_Slots_Start + (SCr_Params_Per_Slot * SCr_Number_Slots)),
	kSuite_MaxNumParameters = (kSuite_Slots_End + 1)
};

#endif
