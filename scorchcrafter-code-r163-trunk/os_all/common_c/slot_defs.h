#ifndef __scorch_cr_Slot_Defs__
#define __scorch_cr_Slot_Defs__

#include "custom_types.h"

static const SCrSlot_Def_t SlotDef[SCr_slot_Max_Types] = {
	{
		SCr_slot_Empty,
		0.000f,
		SCr_SlotBG_Empty,
		0,
		0,
		{ {0,0} },
		{ {0,0} },
		{ {0,0} },
		{SCr_SlCtrl_NONE},
		{SCr_SlCLabel_Blank},
		{SCr_SlCLabel_Blank}
	},
	{
		SCr_slotComponents_Tube_A,
		0.001f,
		SCr_SlotBG_Gradient_Metal_A,
		3,
		0,
		{ {10,20}, {120,20}, {350,20} },
		{ {10,5}, {120,5}, {350,5} },
		{ {0,0} },
		{SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A},
		{SCr_SlCLabel_Input, SCr_SlCLabel_Out, SCr_SlCLabel_Gain},
		{SCr_SlCLabel_Blank}
	},
	{
		SCr_slotComponents_EQ_Filter,
		0.002f,
		SCr_SlotBG_Gradient_Metal_A,
		6,
		1,
		{ {10,20}, {70,20}, {150,20}, {210,20}, {270,20}, {420,20} },
		{ {10,5}, {70,5}, {150,5}, {210,5}, {270,5}, {420,5} },
		{ {210,75} },
		{SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A},
		{SCr_SlCLabel_Input, SCr_SlCLabel_Out, SCr_SlCLabel_Bass, SCr_SlCLabel_Mid, SCr_SlCLabel_Treble, SCr_SlCLabel_NumPasses},
		{SCr_SlCLabel_EQ}
	},
	{
		SCr_slotComponents_EQ_Tube,
		0.003f,
		SCr_SlotBG_Gradient_Metal_A,
		6,
		1,
		{ {10,20}, {70,20}, {150,20}, {210,20}, {270,20}, {420,20} },
		{ {10,5}, {70,5}, {150,5}, {210,5}, {270,5}, {420,5} },
		{ {210,75} },
		{SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A},
		{SCr_SlCLabel_Input, SCr_SlCLabel_Out, SCr_SlCLabel_Bass, SCr_SlCLabel_Mid, SCr_SlCLabel_Treble, SCr_SlCLabel_NumPasses},
		{SCr_SlCLabel_EQ}
	},
	{
		SCr_slotGuitarHeads_C120,
		0.004f,
		SCr_SlotBG_C120_Red,
		9,
		1,
		{ {5,5}, {60,5}, {165,5}, {220,5}, {275,5}, {385,5}, {440,5}, {5,75}, {5,87} },
		{ {5,60}, {60,60}, {165,60}, {220,60}, {275,60}, {385,60}, {440,60}, {15,75}, {15,87} },
		{ {275,72} },
		{SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_PushButton_A, SCr_SlCtrl_PushButton_A},
		{SCr_SlCLabel_Master, SCr_SlCLabel_Gain, SCr_SlCLabel_Bass, SCr_SlCLabel_Mid, SCr_SlCLabel_Treble, SCr_SlCLabel_Contour, SCr_SlCLabel_Presence, SCr_SlCLabel_HiGain, SCr_SlCLabel_Boost},
		{SCr_SlCLabel_EQ}
	},
	{
		SCr_slotGuitarHeads_Jykwrakker,
		0.005f,
		SCr_SlotBG_C120_Blue,
		9,
		1,
		{ {5,5}, {60,5}, {165,5}, {220,5}, {275,5}, {385,5}, {440,5}, {5,75}, {5,87} },
		{ {5,60}, {60,60}, {165,60}, {220,60}, {275,60}, {385,60}, {440,60}, {15,75}, {15,87} },
		{ {275,72} },
		{SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_Knob_A, SCr_SlCtrl_PushButton_A, SCr_SlCtrl_PushButton_A},
		{SCr_SlCLabel_Master, SCr_SlCLabel_Gain, SCr_SlCLabel_Bass, SCr_SlCLabel_Mid, SCr_SlCLabel_Treble, SCr_SlCLabel_Contour, SCr_SlCLabel_Presence, SCr_SlCLabel_HiGain, SCr_SlCLabel_Boost},
		{SCr_SlCLabel_EQ}
	}
};

#endif
