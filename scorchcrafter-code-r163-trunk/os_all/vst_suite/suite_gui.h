#ifndef __PlgEditor_Suite__
#define __PlgEditor_Suite__

#include "../zlib/zconf.h"
#include "../zlib/zlib.h"
#include "../libpng/pngconf.h"
#include "../libpng/png.h"
#include <vstgui.h>
#include <aeffeditor.h>
#include "params_vst_suite.h"
#include <vstcontrols.h>
#include <aeffguieditor.h>
#include "suite.h"
#include "../common_c/custom_types.h"

class PlgEditor : public AEffGUIEditor, public CControlListener
{
public:
	PlgEditor (void*);
	virtual ~PlgEditor();

	friend class Suite;

	// from AEffGUIEditor
	bool open (void* ptr);
	void close ();
	void setParameter (VstInt32 index, float value);

	// from CControlListener
	void valueChanged (CControl* pControl);

	void idle();

protected:
	void SetUpFront();

	const int nGUIslots;

	CRect SlotRect[SCr_GUI_Num_Visible_Slots];
	CRect SlotCtrlRect[SCr_GUI_Num_Visible_Slots];
	CRect MasterControlRect;
	CRect CatMenuRect;
	CRect TextAreaRect;

	CBitmap* GUIback;
	CBitmap* GUIPanelBG;
	CBitmap* GUIslotCtrl;
	CBitmap* GUIcatMenu;
	CBitmap* GUIslot_ChanLeft;
	CBitmap* GUIslot_ChanRight;
	CBitmap* GUIslot_PhaseLeft;
	CBitmap* GUIslot_PhaseRight;
	CBitmap* GUIslot_MoveUp;
	CBitmap* GUIslot_MoveDown;
	CBitmap* GUIslot_Clear;
	CBitmap* GUIslot_SlotNumLabel;

	CView* gBGView;
	CView* gView[SCr_GUI_Num_Visible_Slots];
	CView* gSlotCtrlView[SCr_GUI_Num_Visible_Slots];
	CView* gSlotNumLabel[SCr_GUI_Num_Visible_Slots];
	CView* CatMenuView;
	CView* MasterCtrlView;
	CView* TextView;

	CBitmap* gSlot[SCr_SlotBG_Max_Count];
	CBitmap* gKnob[SCr_GUI_Max_BitMaps_Knobs];
	CBitmap* gKnobBG[SCr_GUI_Max_BitMaps_KnobsBG];
	CBitmap* gSliderH[SCr_GUI_Max_BitMaps_SlidersH];
	CBitmap* gButtonPush[SCr_GUI_Max_BitMaps_PushBttn];
	CBitmap* gButtonOnOff[SCr_GUI_Max_BitMaps_OnOffBttn];

	CAnimKnob* kKnob[SCr_GUI_Total_GUISlot_Params];
	COnOffButton* kButton[SCr_GUI_Total_GUISlot_Params];
	CHorizontalSlider* kSliderH[SCr_GUI_Total_GUISlot_Params];
	CButton* kPbutton[SCr_GUI_Total_GUISlot_Params];

	CButton* kSlot_ChanL[SCr_GUI_Num_Visible_Slots];
	CButton* kSlot_ChanR[SCr_GUI_Num_Visible_Slots];
	CButton* kSlot_PhaseL[SCr_GUI_Num_Visible_Slots];
	CButton* kSlot_PhaseR[SCr_GUI_Num_Visible_Slots];
	CButton* kSlot_MoveUp[SCr_GUI_Num_Visible_Slots];
	CButton* kSlot_MoveDown[SCr_GUI_Num_Visible_Slots];
	CButton* kSlot_Clear[SCr_GUI_Num_Visible_Slots];
};

#endif
