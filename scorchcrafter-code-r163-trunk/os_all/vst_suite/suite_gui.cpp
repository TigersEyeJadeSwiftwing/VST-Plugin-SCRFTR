#include "suite_gui.h"
#include "../common_c/custom_types.h"

#include "../common_c/slot_defs.h"

AEffGUIEditor* createEditor (AudioEffectX* Suite)
{
	return new PlgEditor (Suite);
}
//------------------------------------------------------------------------------------
PlgEditor::PlgEditor (void* ptr)
: AEffGUIEditor (ptr),
nGUIslots(SCr_GUI_Num_Visible_Slots)
{
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = 900;
	rect.bottom = SCr_GUI_Total_GUI_Height;

	for (int x = 0; x < nGUIslots; x++)
	{
		SlotRect[x].setTopLeft(CPoint(0, x*100));
		SlotRect[x].setBottomLeft(CPoint(0, (x+1)*100));
		SlotRect[x].setTopRight(CPoint(500, x*100));
		SlotRect[x].setBottomRight(CPoint(500, (x+1)*100));

		SlotCtrlRect[x].setTopLeft(CPoint(500, x*100));
		SlotCtrlRect[x].setBottomLeft(CPoint(500, (x+1)*100));
		SlotCtrlRect[x].setTopRight(CPoint(600, x*100));
		SlotCtrlRect[x].setBottomRight(CPoint(600, (x+1)*100));
	}

	const int pad = SCr_GUI_Panel_Padding;

	MasterControlRect.setTopLeft(CPoint(600+pad, pad));
	MasterControlRect.setTopRight(CPoint(900-pad, pad));
	MasterControlRect.setBottomLeft(CPoint(600+pad, SCr_GUI_Master_Controls_Height-pad));
	MasterControlRect.setBottomRight(CPoint(900-pad, SCr_GUI_Master_Controls_Height-pad));

	CatMenuRect.setTopLeft(CPoint(600, SCr_GUI_Master_Controls_Height));
	CatMenuRect.setTopRight(CPoint(900, SCr_GUI_Master_Controls_Height));
	CatMenuRect.setBottomLeft(CPoint(600, SCr_GUI_Master_Controls_Height+60));
	CatMenuRect.setBottomRight(CPoint(900, SCr_GUI_Master_Controls_Height+60));

	TextAreaRect.setTopLeft(CPoint(600+pad, SCr_GUI_Master_Controls_Height+60+pad));
	TextAreaRect.setTopRight(CPoint(900-pad, SCr_GUI_Master_Controls_Height+60+pad));
	TextAreaRect.setBottomLeft(CPoint(600+pad, SCr_GUI_Total_GUI_Height-pad));
	TextAreaRect.setBottomRight(CPoint(900-pad, SCr_GUI_Total_GUI_Height-pad));
}
PlgEditor::~PlgEditor()
{
	// delete GUIbackGr;
}
//------------------------------------------------------------------------------------
bool PlgEditor::open (void* ptr)
{
	AEffGUIEditor::open(ptr);

	CRect frameSize (0, 0, rect.right, rect.bottom);
	CFrame *nFrame = new CFrame (frameSize, ptr, this);

	frame = nFrame;
	// nFrame->forget();

	SetUpFront();

//	frame = nFrame;

	setKnobMode(kLinearMode);

//	for (int i = 0; i < kMaxNumParameters; i++)
//		setParameter (i, ((Suite*)effect)->getParameter (i));

	return true;
}
//------------------------------------------------------------------------------------
void PlgEditor::SetUpFront()
{
	if(!frame) return;

	CFrame *newFrame = frame;
	newFrame->removeAll(false);
	CRect frameSize (0, 0, rect.right, rect.bottom);

	GUIback = new CBitmap ("PI_GuiBG");
	GUIPanelBG = new CBitmap ("PI_GuiBGpn");
	GUIslotCtrl = new CBitmap ("PIslotCtrl");
	GUIcatMenu = new CBitmap ("PItxCatMenu");

	gBGView = new CView (frameSize);
	gBGView->setBackground(GUIback);
	newFrame->addView(gBGView);
	gBGView->invalid();

	for (int x = 0; x < nGUIslots; x++)
	{
		gView[x] = new CView (SlotRect[x]);
		gSlotCtrlView[x] = new CView (SlotCtrlRect[x]);
	}
	MasterCtrlView = new CView (MasterControlRect);
	CatMenuView = new CView (CatMenuRect);
	TextView = new CView (TextAreaRect);

	gSlot[0] = new CBitmap ("PImBG_00");
	gSlot[1] = new CBitmap ("PImBG_01");
	gSlot[2] = new CBitmap ("PImBG_02");
	gSlot[3] = new CBitmap ("PImBG_03");
	gSlot[4] = new CBitmap ("PImBG_04");
	gSlot[5] = new CBitmap ("PImBG_05");
	gSlot[6] = new CBitmap ("PImBG_06");
	gSlot[7] = new CBitmap ("PImBG_07");

	for (int x = 0; x < nGUIslots; x++)
	{
		gSlotCtrlView[x]->setBackground(GUIslotCtrl);
		newFrame->addView(gSlotCtrlView[x]);
		gSlotCtrlView[x]->invalid();

		const int y = rand() % SCr_SlotBG_Max_Count;

		gView[x]->setBackground(gSlot[y]);
		newFrame->addView(gView[x]);
		gView[x]->invalid();
	}

	CatMenuView->setBackground(GUIcatMenu);
	newFrame->addView(CatMenuView);
	CatMenuView->invalid();
	MasterCtrlView->setBackground(GUIPanelBG);
	newFrame->addView(MasterCtrlView);
	MasterCtrlView->invalid();
	TextView->setBackground(GUIPanelBG);
	newFrame->addView(TextView);
	TextView->invalid();

/*
	pGuiKnob = new CBitmap ("Plgknob");
	pGuiButton = new CBitmap ("PlgBttnPush");

	PanelButton[0] = new COnOffButton(CRect(570,40,590,60),this,kGuiPanelButtonA,pGuiButton);
	PanelButton[1] = new COnOffButton(CRect(590,40,610,60),this,kGuiPanelButtonB,pGuiButton);
	PanelButton[2] = new COnOffButton(CRect(610,40,630,60),this,kGuiPanelButtonC,pGuiButton);
	PanelButton[0]->setValue(((Suite*)effect)->getParameter(kGuiPanelButtonA));
	PanelButton[1]->setValue(((Suite*)effect)->getParameter(kGuiPanelButtonB));
	PanelButton[2]->setValue(((Suite*)effect)->getParameter(kGuiPanelButtonC));
	newFrame->addView(PanelButton[0]);
	newFrame->addView(PanelButton[1]);
	newFrame->addView(PanelButton[2]);

	kKnob[0] = new CAnimKnob(CRect(27,197,91,261),this,kGainControl,pGuiKnob,CPoint(0,0));
	kKnob[1] = new CAnimKnob(CRect(127,197,191,261),this,kMasterVol,pGuiKnob,CPoint(0,0));
	kKnob[2] = new CAnimKnob(CRect(407,197,471,261),this,kEQstackLow,pGuiKnob,CPoint(0,0));
	kKnob[3] = new CAnimKnob(CRect(507,197,571,261),this,kEQstackMid,pGuiKnob,CPoint(0,0));
	kKnob[4] = new CAnimKnob(CRect(607,197,671,261),this,kEQstackHigh,pGuiKnob,CPoint(0,0));
	kKnob[5] = new CAnimKnob(CRect(507,77,571,141),this,kEQstackContour,pGuiKnob,CPoint(0,0));
	kKnob[6] = new CAnimKnob(CRect(607,77,671,141),this,kEQstackPres,pGuiKnob,CPoint(0,0));

	kKnob[0]->setValue(((Suite*)effect)->getParameter(kGainControl));
	kKnob[1]->setValue(((Suite*)effect)->getParameter(kMasterVol));
	kKnob[2]->setValue(((Suite*)effect)->getParameter(kEQstackLow));
	kKnob[3]->setValue(((Suite*)effect)->getParameter(kEQstackMid));
	kKnob[4]->setValue(((Suite*)effect)->getParameter(kEQstackHigh));
	kKnob[5]->setValue(((Suite*)effect)->getParameter(kEQstackContour));
	kKnob[6]->setValue(((Suite*)effect)->getParameter(kEQstackPres));

	for(int x = 0; x < 7; x++)
	{
		newFrame->addView(kKnob[x]);
	}

	kBtn_LoGain = new COnOffButton(CRect(20,130,40,150),this,kBtnClean,pGuiButton);
	kBtn_HiGain = new COnOffButton(CRect(20,160,40,180),this,kBtnHeavy,pGuiButton);
	kBtn_Boost = new COnOffButton(CRect(20,80,40,100),this,kBtnBoost,pGuiButton);
	kBtn_Stereo = new COnOffButton(CRect(370,80,390,100),this,kMonoStereoControl,pGuiButton);
	kBtn_Bright = new COnOffButton(CRect(370,110,390,130),this,kBtnBright,pGuiButton);

	kBtn_LoGain->setValue(((Suite*)effect)->getParameter(kBtnClean));
	kBtn_HiGain->setValue(((Suite*)effect)->getParameter(kBtnHeavy));
	kBtn_Boost->setValue(((Suite*)effect)->getParameter(kBtnBoost));
	kBtn_Stereo->setValue(((Suite*)effect)->getParameter(kMonoStereoControl));
	kBtn_Bright->setValue(((Suite*)effect)->getParameter(kBtnBright));

	newFrame->addView(kBtn_Boost);
	newFrame->addView(kBtn_HiGain);
	newFrame->addView(kBtn_LoGain);
	newFrame->addView(kBtn_Stereo);
	newFrame->addView(kBtn_Bright);

	GUIback[0]->forget();
	pGuiKnob->forget();
	pGuiButton->forget();
*/
	GUIback->forget();
	GUIPanelBG->forget();
	GUIslotCtrl->forget();
	GUIcatMenu->forget();

	for (int x = 0; x < SCr_SlotBG_Max_Count; x++)
		gSlot[x]->forget();

	frame = newFrame;

	setKnobMode(kLinearMode);
}
//------------------------------------------------------------------------------------
void PlgEditor::close()
{
	//-- on close we need to delete the frame object.
	//-- once again we make sure that the member frame variable is set to zero before we delete it
	//-- so that calls to setParameter won't crash.
	CFrame* oldFrame = frame;
	frame = 0;
	// delete oldFrame;
	oldFrame->forget();
	// delete frame;
	// frame=0;
//	isDisplayingFront = true;
}
//------------------------------------------------------------------------------------
void PlgEditor::valueChanged(CControl* pControl)
{
	//-- valueChanged is called whenever the user changes one of the controls in the User Interface (UI)
	((Suite*)effect)->setParameterAutomated(pControl->getTag(), pControl->getValue());
}
//------------------------------------------------------------------------------------
void PlgEditor::setParameter(VstInt32 index, float value)
{
	//-- setParameter is called when the host automates one of the effects parameter.
	//-- The UI should reflect this state so we set the value of the control to the new value.
	//-- VSTGUI will automaticly redraw changed controls in the next idle (as this call happens to be in the process thread).
	// if(!frame) return;
	if(!frame) return;
/*
	if(index == kGuiPanelButtonA)
	{
		if(frame)
		{
			if (PanelButton[0]) PanelButton[0]->setValue(value);
			if (PanelButton[0]) PanelButton[0]->invalid();
			if (value > 0.50f)
			{
				SetUpFront();
//				((Suite*)effect)->setParameterAutomated(kGuiPanelButtonB, 0.0f);
//				((Suite*)effect)->setParameterAutomated(kGuiPanelButtonC, 0.0f);
			}
		}
	}
	if(index == kGuiPanelButtonB)
	{
		if(frame)
		{
			if (PanelButton[1]) PanelButton[1]->setValue(value);
			if (PanelButton[1]) PanelButton[1]->invalid();
			if (value > 0.50f)
			{
				SetUpBack();
//				((Suite*)effect)->setParameterAutomated(kGuiPanelButtonA, 0.0f);
//				((Suite*)effect)->setParameterAutomated(kGuiPanelButtonC, 0.0f);
			}
		}
	}
	if(index == kGuiPanelButtonC)
	{
		if(frame)
		{
			if (PanelButton[2]) PanelButton[2]->setValue(value);
			if (PanelButton[2]) PanelButton[2]->invalid();
			if (value > 0.50f)
			{
				SetUpAux();
//				((Suite*)effect)->setParameterAutomated(kGuiPanelButtonB, 0.0f);
//				((Suite*)effect)->setParameterAutomated(kGuiPanelButtonA, 0.0f);
			}
		}
	}
//	if(index == kGuiCurrentPanel)
//	{
	//	if (DisplaySide != value)
	//	{
	//		if (value == 0.0f) SetUpFront();
	//		if (value == 0.1f) SetUpBack();
	//		if (value == 0.2f) SetUpAux();
	//		((Suite*)effect)->updateDisplay();
		//	frame->invalid();
	//		DisplaySide = value;
	//	}
//	}
	if (((Suite*)effect)->getParameter(kGuiPanelButtonA) > 0.50f)
	{
		if(index == kGainControl)
		{
			if(kKnob[0])
			{
				kKnob[0]->setValue(value);
				kKnob[0]->invalid();
			}
		}
		if(index == kMasterVol)
		{
			if(kKnob[1])
			{
				kKnob[1]->setValue(value);
				kKnob[1]->invalid();
			}
		}
		if(index == kEQstackLow)
		{
			if(kKnob[2])
			{
				kKnob[2]->setValue(value);
				kKnob[2]->invalid();
			}
		}
		if(index == kEQstackMid)
		{
			if(kKnob[3])
			{
				kKnob[3]->setValue(value);
				kKnob[3]->invalid();
			}
		}
		if(index == kEQstackHigh)
		{
			if(kKnob[4])
			{
				kKnob[4]->setValue(value);
				kKnob[4]->invalid();
			}
		}
		if(index == kEQstackContour)
		{
			if(kKnob[5])
			{
				kKnob[5]->setValue(value);
				kKnob[5]->invalid();
			}
		}
		if(index == kEQstackPres)
		{
			if(kKnob[6])
			{
				kKnob[6]->setValue(value);
				kKnob[6]->invalid();
			}
		}
		if(index == kMonoStereoControl)
		{
			if(kBtn_Stereo)
			{
				kBtn_Stereo->setValue(value);
				kBtn_Stereo->invalid();
			}
		}
		if(index == kBtnBright)
		{
			if(kBtn_Bright)
			{
				kBtn_Bright->setValue(value);
				kBtn_Bright->invalid();
			}
		}
		if(index == kBtnBoost)
		{
			if(kBtn_Boost)
			{
				kBtn_Boost->setValue(value);
				kBtn_Boost->invalid();
			}
		}
		if(index == kBtnClean)
		{
			if(kBtn_LoGain)
			{
				kBtn_LoGain->setValue(value);
				kBtn_LoGain->invalid();
			}
		}
		if(index == kBtnHeavy)
		{
			if(kBtn_HiGain)
			{
				kBtn_HiGain->setValue(value);
				kBtn_HiGain->invalid();
			}
		}
	}
	if (((Suite*)effect)->getParameter(kGuiPanelButtonB) > 0.50f)
	{
		if((index >= kSmpRenderOn1x) && (index <= kSmpRenderOnOFF))
		{
			if(SamplingOn[index-kSmpRenderOn1x])
			{
				SamplingOn[index-kSmpRenderOn1x]->setValue(value);
				SamplingOn[index-kSmpRenderOn1x]->invalid();
			}
		}
		if((index >= kSmpRenderOff1x) && (index <= kSmpRenderOffOFF))
		{
			if(SamplingOff[index-kSmpRenderOff1x])
			{
				SamplingOff[index-kSmpRenderOff1x]->setValue(value);
				SamplingOff[index-kSmpRenderOff1x]->invalid();
			}
		}
		if(index == kToneStFlatEven)
		{
			if(kBtn_TStackEven)
			{
				kBtn_TStackEven->setValue(value);
				kBtn_TStackEven->invalid();
			}
		}
		if((index >= kToneStShapedA) && (index <= kToneStShapedF))
		{
			if(kBtn_TStackShaped[index-kToneStShapedA])
			{
				kBtn_TStackShaped[index-kToneStShapedA]->setValue(value);
				kBtn_TStackShaped[index-kToneStShapedA]->invalid();
			}
		}
		if(index == kInternalCabOff)
		{
			if(kBtn_CabMic)
			{
				kBtn_CabMic->setValue(value);
				kBtn_CabMic->invalid();
			}
		}
		if(index == kDynamicEnabled)
		{
			if(kDynamicToneOn)
			{
				kDynamicToneOn->setValue(value);
				kDynamicToneOn->invalid();
			}
		}
		if((index >= kInternalCabA) && (index <= kInternalCabF))
		{
			if(kBtn_CabMicSelect[index-kInternalCabA])
			{
				kBtn_CabMicSelect[index-kInternalCabA]->setValue(value);
				kBtn_CabMicSelect[index-kInternalCabA]->invalid();
			}
		}

	}
	if (((Suite*)effect)->getParameter(kGuiPanelButtonC) > 0.50f)
	{
		if((index >= kSmpBitOn32) && (index <= kSmpBitOn128))
		{
			if(kBitDepthOn[index-kSmpBitOn32])
			{
				kBitDepthOn[index-kSmpBitOn32]->setValue(value);
				kBitDepthOn[index-kSmpBitOn32]->invalid();
			}
		}
		if((index >= kSmpBitOff32) && (index <= kSmpBitOff128))
		{
			if(kBitDepthOff[index-kSmpBitOff32])
			{
				kBitDepthOff[index-kSmpBitOff32]->setValue(value);
				kBitDepthOff[index-kSmpBitOff32]->invalid();
			}
		}
	}
*/
}
void PlgEditor::idle()
{
	AEffGUIEditor::idle();
}
