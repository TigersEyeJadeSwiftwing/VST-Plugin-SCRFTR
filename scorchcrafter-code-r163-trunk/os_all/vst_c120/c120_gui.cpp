#include "c120_gui.h"

AEffGUIEditor* createEditor (AudioEffectX* ScC120)
{
	return new PlgEditor_C120 (ScC120);
}
//------------------------------------------------------------------------------------
PlgEditor_C120::PlgEditor_C120 (void* ptr)
: AEffGUIEditor (ptr)
{
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = 700;
	rect.bottom = 300;
}
PlgEditor_C120::~PlgEditor_C120()
{
	// delete GUIbackGr;
}
//------------------------------------------------------------------------------------
bool PlgEditor_C120::open (void* ptr)
{
	AEffGUIEditor::open(ptr);

	CRect frameSize (0, 0, rect.right, rect.bottom);
	CFrame *nFrame = new CFrame (frameSize, ptr, this);

	frame = nFrame;
	// nFrame->forget();

//	DisplaySide = ((ScC120*)effect)->getParameter(kGuiCurrentPanel);

	if(((ScC120*)effect)->getParameter(kGuiPanelButtonA) > 0.50f) SetUpFront();

	if(((ScC120*)effect)->getParameter(kGuiPanelButtonB) > 0.50f) SetUpBack();

	if(((ScC120*)effect)->getParameter(kGuiPanelButtonC) > 0.50f) SetUpAux();

//	frame = nFrame;

	setKnobMode(kLinearMode);

//	for (int i = 0; i < kMaxNumParameters; i++)
//		setParameter (i, ((ScC120*)effect)->getParameter (i));

	return true;
}
//------------------------------------------------------------------------------------
void PlgEditor_C120::SetUpFront()
{
	if(!frame) return;

	CFrame *newFrame = frame;
	newFrame->removeAll(false);
	CRect frameSize (0, 0, rect.right, rect.bottom);

	GUIback[0] = new CBitmap ("PlgBGf");

	gView[0] = new CView (frameSize);
	gView[0]->setBackground(GUIback[0]);
	newFrame->addView(gView[0]);
	gView[0]->invalid();

	pGuiKnob = new CBitmap ("Plgknob");
	pGuiButton = new CBitmap ("PlgBttnPush");

	PanelButton[0] = new COnOffButton(CRect(570,40,590,60),this,kGuiPanelButtonA,pGuiButton);
	PanelButton[1] = new COnOffButton(CRect(590,40,610,60),this,kGuiPanelButtonB,pGuiButton);
	PanelButton[2] = new COnOffButton(CRect(610,40,630,60),this,kGuiPanelButtonC,pGuiButton);
	PanelButton[0]->setValue(((ScC120*)effect)->getParameter(kGuiPanelButtonA));
	PanelButton[1]->setValue(((ScC120*)effect)->getParameter(kGuiPanelButtonB));
	PanelButton[2]->setValue(((ScC120*)effect)->getParameter(kGuiPanelButtonC));
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

	kKnob[0]->setValue(((ScC120*)effect)->getParameter(kGainControl));
	kKnob[1]->setValue(((ScC120*)effect)->getParameter(kMasterVol));
	kKnob[2]->setValue(((ScC120*)effect)->getParameter(kEQstackLow));
	kKnob[3]->setValue(((ScC120*)effect)->getParameter(kEQstackMid));
	kKnob[4]->setValue(((ScC120*)effect)->getParameter(kEQstackHigh));
	kKnob[5]->setValue(((ScC120*)effect)->getParameter(kEQstackContour));
	kKnob[6]->setValue(((ScC120*)effect)->getParameter(kEQstackPres));

	for(int x = 0; x < 7; x++)
	{
		newFrame->addView(kKnob[x]);
	}

	kBtn_LoGain = new COnOffButton(CRect(20,130,40,150),this,kBtnClean,pGuiButton);
	kBtn_HiGain = new COnOffButton(CRect(20,160,40,180),this,kBtnHeavy,pGuiButton);
	kBtn_Boost = new COnOffButton(CRect(20,80,40,100),this,kBtnBoost,pGuiButton);
	kBtn_Stereo = new COnOffButton(CRect(370,80,390,100),this,kMonoStereoControl,pGuiButton);
	kBtn_Bright = new COnOffButton(CRect(370,110,390,130),this,kBtnBright,pGuiButton);

	kBtn_LoGain->setValue(((ScC120*)effect)->getParameter(kBtnClean));
	kBtn_HiGain->setValue(((ScC120*)effect)->getParameter(kBtnHeavy));
	kBtn_Boost->setValue(((ScC120*)effect)->getParameter(kBtnBoost));
	kBtn_Stereo->setValue(((ScC120*)effect)->getParameter(kMonoStereoControl));
	kBtn_Bright->setValue(((ScC120*)effect)->getParameter(kBtnBright));

	newFrame->addView(kBtn_Boost);
	newFrame->addView(kBtn_HiGain);
	newFrame->addView(kBtn_LoGain);
	newFrame->addView(kBtn_Stereo);
	newFrame->addView(kBtn_Bright);

	GUIback[0]->forget();
	pGuiKnob->forget();
	pGuiButton->forget();

	frame = newFrame;

	setKnobMode(kLinearMode);
}
//------------------------------------------------------------------------------------
void PlgEditor_C120::SetUpBack()
{
	if(!frame) return;

	CFrame *newFrame = frame;
	newFrame->removeAll(false);
	CRect frameSize (0, 0, rect.right, rect.bottom);

	GUIback[1] = new CBitmap ("PlgBGb");

	gView[1] = new CView (frameSize);
	gView[1]->setBackground(GUIback[1]);
	newFrame->addView(gView[1]);
	gView[1]->invalid();

	pGuiKnob = new CBitmap ("Plgknob");
	pGuiButton = new CBitmap ("PlgBttnPush");

	PanelButton[0] = new COnOffButton(CRect(570,40,590,60),this,kGuiPanelButtonA,pGuiButton);
	PanelButton[1] = new COnOffButton(CRect(590,40,610,60),this,kGuiPanelButtonB,pGuiButton);
	PanelButton[2] = new COnOffButton(CRect(610,40,630,60),this,kGuiPanelButtonC,pGuiButton);
	PanelButton[0]->setValue(((ScC120*)effect)->getParameter(kGuiPanelButtonA));
	PanelButton[1]->setValue(((ScC120*)effect)->getParameter(kGuiPanelButtonB));
	PanelButton[2]->setValue(((ScC120*)effect)->getParameter(kGuiPanelButtonC));
	newFrame->addView(PanelButton[0]);
	newFrame->addView(PanelButton[1]);
	newFrame->addView(PanelButton[2]);

	int x = 0;
	for(x = kSmpRenderOn1x; x <= kSmpRenderOnOFF; x++)
	{
		SamplingOn[x-kSmpRenderOn1x] = new COnOffButton(CRect(20,150+((x-kSmpRenderOn1x) * 20),40,170+((x-kSmpRenderOn1x) * 20)),this,x,pGuiButton);
		SamplingOn[x-kSmpRenderOn1x]->setValue(((ScC120*)effect)->getParameter(x));
		newFrame->addView(SamplingOn[x-kSmpRenderOn1x]);
	}
	for(x = kSmpRenderOff1x; x <= kSmpRenderOffOFF; x++)
	{
		SamplingOff[x-kSmpRenderOff1x] = new COnOffButton(CRect(140,150+((x-kSmpRenderOff1x) * 20),160,170+((x-kSmpRenderOff1x) * 20)),this,x,pGuiButton);
		SamplingOff[x-kSmpRenderOff1x]->setValue(((ScC120*)effect)->getParameter(x));
		newFrame->addView(SamplingOff[x-kSmpRenderOff1x]);
	}

	kBtn_TStackEven = new COnOffButton(CRect(280,120,300,140),this,kToneStFlatEven,pGuiButton);
	kBtn_TStackShaped[0] = new COnOffButton(CRect(280,140,300,160),this,kToneStShapedA,pGuiButton);
	kBtn_TStackShaped[1] = new COnOffButton(CRect(280,160,300,180),this,kToneStShapedB,pGuiButton);
	kBtn_TStackShaped[2] = new COnOffButton(CRect(280,180,300,200),this,kToneStShapedC,pGuiButton);
	kBtn_TStackShaped[3] = new COnOffButton(CRect(280,200,300,220),this,kToneStShapedD,pGuiButton);
	kBtn_TStackShaped[4] = new COnOffButton(CRect(280,220,300,240),this,kToneStShapedE,pGuiButton);
	kBtn_TStackShaped[5] = new COnOffButton(CRect(280,240,300,260),this,kToneStShapedF,pGuiButton);
	kBtn_CabMic = new COnOffButton(CRect(520,140,540,160),this,kInternalCabOff,pGuiButton);
	kBtn_CabMicSelect[0] = new COnOffButton(CRect(520,160,540,180),this,kInternalCabA,pGuiButton);
	kBtn_CabMicSelect[1] = new COnOffButton(CRect(520,180,540,200),this,kInternalCabB,pGuiButton);
	kBtn_CabMicSelect[2] = new COnOffButton(CRect(520,200,540,220),this,kInternalCabC,pGuiButton);
	kBtn_CabMicSelect[3] = new COnOffButton(CRect(520,220,540,240),this,kInternalCabD,pGuiButton);
	kBtn_CabMicSelect[4] = new COnOffButton(CRect(520,240,540,260),this,kInternalCabE,pGuiButton);
	kBtn_CabMicSelect[5] = new COnOffButton(CRect(520,260,540,280),this,kInternalCabF,pGuiButton);
	kDynamicToneOn = new COnOffButton(CRect(340,260,360,280),this,kDynamicEnabled,pGuiButton);

	kBtn_TStackEven->setValue(((ScC120*)effect)->getParameter(kToneStFlatEven));
	kBtn_TStackShaped[0]->setValue(((ScC120*)effect)->getParameter(kToneStShapedA));
	kBtn_TStackShaped[1]->setValue(((ScC120*)effect)->getParameter(kToneStShapedB));
	kBtn_TStackShaped[2]->setValue(((ScC120*)effect)->getParameter(kToneStShapedC));
	kBtn_TStackShaped[3]->setValue(((ScC120*)effect)->getParameter(kToneStShapedD));
	kBtn_TStackShaped[4]->setValue(((ScC120*)effect)->getParameter(kToneStShapedE));
	kBtn_TStackShaped[5]->setValue(((ScC120*)effect)->getParameter(kToneStShapedF));
	kBtn_CabMic->setValue(((ScC120*)effect)->getParameter(kInternalCabOff));
	kBtn_CabMicSelect[0]->setValue(((ScC120*)effect)->getParameter(kInternalCabA));
	kBtn_CabMicSelect[1]->setValue(((ScC120*)effect)->getParameter(kInternalCabB));
	kBtn_CabMicSelect[2]->setValue(((ScC120*)effect)->getParameter(kInternalCabC));
	kBtn_CabMicSelect[3]->setValue(((ScC120*)effect)->getParameter(kInternalCabD));
	kBtn_CabMicSelect[4]->setValue(((ScC120*)effect)->getParameter(kInternalCabE));
	kBtn_CabMicSelect[5]->setValue(((ScC120*)effect)->getParameter(kInternalCabF));
	kDynamicToneOn->setValue(((ScC120*)effect)->getParameter(kDynamicEnabled));

	newFrame->addView(kBtn_TStackEven);
	newFrame->addView(kBtn_TStackShaped[0]);
	newFrame->addView(kBtn_TStackShaped[1]);
	newFrame->addView(kBtn_TStackShaped[2]);
	newFrame->addView(kBtn_TStackShaped[3]);
	newFrame->addView(kBtn_TStackShaped[4]);
	newFrame->addView(kBtn_TStackShaped[5]);
	newFrame->addView(kBtn_CabMic);
	newFrame->addView(kBtn_CabMicSelect[0]);
	newFrame->addView(kBtn_CabMicSelect[1]);
	newFrame->addView(kBtn_CabMicSelect[2]);
	newFrame->addView(kBtn_CabMicSelect[3]);
	newFrame->addView(kBtn_CabMicSelect[4]);
	newFrame->addView(kBtn_CabMicSelect[5]);
	newFrame->addView(kDynamicToneOn);

	GUIback[1]->forget();
	pGuiKnob->forget();
	pGuiButton->forget();

	frame = newFrame;

	setKnobMode(kLinearMode);
}
//------------------------------------------------------------------------------------
void PlgEditor_C120::SetUpAux()
{
	if(!frame) return;

	CFrame *newFrame = frame;
	newFrame->removeAll(false);
	CRect frameSize (0, 0, rect.right, rect.bottom);

	GUIback[2] = new CBitmap ("PlgBGc");

	gView[2] = new CView (frameSize);
	gView[2]->setBackground(GUIback[2]);
	newFrame->addView(gView[2]);
	gView[2]->invalid();

	pGuiKnob = new CBitmap ("Plgknob");
	pGuiButton = new CBitmap ("PlgBttnPush");

	PanelButton[0] = new COnOffButton(CRect(570,40,590,60),this,kGuiPanelButtonA,pGuiButton);
	PanelButton[1] = new COnOffButton(CRect(590,40,610,60),this,kGuiPanelButtonB,pGuiButton);
	PanelButton[2] = new COnOffButton(CRect(610,40,630,60),this,kGuiPanelButtonC,pGuiButton);
	PanelButton[0]->setValue(((ScC120*)effect)->getParameter(kGuiPanelButtonA));
	PanelButton[1]->setValue(((ScC120*)effect)->getParameter(kGuiPanelButtonB));
	PanelButton[2]->setValue(((ScC120*)effect)->getParameter(kGuiPanelButtonC));
	newFrame->addView(PanelButton[0]);
	newFrame->addView(PanelButton[1]);
	newFrame->addView(PanelButton[2]);

	kBitDepthOn[0] = new COnOffButton(CRect(20,150,40,170),this,kSmpBitOn32,pGuiButton);
	kBitDepthOn[1] = new COnOffButton(CRect(20,170,40,190),this,kSmpBitOn64,pGuiButton);
	kBitDepthOn[2] = new COnOffButton(CRect(20,190,40,210),this,kSmpBitOn128,pGuiButton);
	kBitDepthOff[0] = new COnOffButton(CRect(140,150,160,170),this,kSmpBitOff32,pGuiButton);
	kBitDepthOff[1] = new COnOffButton(CRect(140,170,160,190),this,kSmpBitOff64,pGuiButton);
	kBitDepthOff[2] = new COnOffButton(CRect(140,190,160,210),this,kSmpBitOff128,pGuiButton);

	kBitDepthOn[0]->setValue(((ScC120*)effect)->getParameter(kSmpBitOn32));
	kBitDepthOn[1]->setValue(((ScC120*)effect)->getParameter(kSmpBitOn64));
	kBitDepthOn[2]->setValue(((ScC120*)effect)->getParameter(kSmpBitOn128));
	kBitDepthOff[0]->setValue(((ScC120*)effect)->getParameter(kSmpBitOff32));
	kBitDepthOff[1]->setValue(((ScC120*)effect)->getParameter(kSmpBitOff64));
	kBitDepthOff[2]->setValue(((ScC120*)effect)->getParameter(kSmpBitOff128));

	newFrame->addView(kBitDepthOn[0]);
	newFrame->addView(kBitDepthOn[1]);
	newFrame->addView(kBitDepthOn[2]);
	newFrame->addView(kBitDepthOff[0]);
	newFrame->addView(kBitDepthOff[1]);
	newFrame->addView(kBitDepthOff[2]);

	GUIback[2]->forget();
	pGuiKnob->forget();
	pGuiButton->forget();

	frame = newFrame;

	setKnobMode(kLinearMode);
}
//------------------------------------------------------------------------------------
void PlgEditor_C120::close()
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
void PlgEditor_C120::valueChanged(CControl* pControl)
{
	//-- valueChanged is called whenever the user changes one of the controls in the User Interface (UI)
	((ScC120*)effect)->setParameterAutomated(pControl->getTag(), pControl->getValue());
}
//------------------------------------------------------------------------------------
void PlgEditor_C120::setParameter(VstInt32 index, float value)
{
	//-- setParameter is called when the host automates one of the effects parameter.
	//-- The UI should reflect this state so we set the value of the control to the new value.
	//-- VSTGUI will automaticly redraw changed controls in the next idle (as this call happens to be in the process thread).
	// if(!frame) return;
	if(!frame) return;

	if(index == kGuiPanelButtonA)
	{
		if(frame)
		{
			if (PanelButton[0]) PanelButton[0]->setValue(value);
			if (PanelButton[0]) PanelButton[0]->invalid();
			if (value > 0.50f)
			{
				SetUpFront();
//				((ScC120*)effect)->setParameterAutomated(kGuiPanelButtonB, 0.0f);
//				((ScC120*)effect)->setParameterAutomated(kGuiPanelButtonC, 0.0f);
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
//				((ScC120*)effect)->setParameterAutomated(kGuiPanelButtonA, 0.0f);
//				((ScC120*)effect)->setParameterAutomated(kGuiPanelButtonC, 0.0f);
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
//				((ScC120*)effect)->setParameterAutomated(kGuiPanelButtonB, 0.0f);
//				((ScC120*)effect)->setParameterAutomated(kGuiPanelButtonA, 0.0f);
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
	//		((ScC120*)effect)->updateDisplay();
		//	frame->invalid();
	//		DisplaySide = value;
	//	}
//	}
	if (((ScC120*)effect)->getParameter(kGuiPanelButtonA) > 0.50f)
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
	if (((ScC120*)effect)->getParameter(kGuiPanelButtonB) > 0.50f)
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
	if (((ScC120*)effect)->getParameter(kGuiPanelButtonC) > 0.50f)
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
}
void PlgEditor_C120::idle()
{
	AEffGUIEditor::idle();
}
