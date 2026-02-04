#include "c120_gui.h"

AEffGUIEditor* createEditor (AudioEffectX* ScC120)
{
	return new PlgEditor (ScC120);
}
//------------------------------------------------------------------------------------
PlgEditor::PlgEditor (void* ptr)
: AEffGUIEditor (ptr)
{
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = 640;
	rect.bottom = 220;

	isDisplayingFront = true;
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

	if(isDisplayingFront == true) SetUpFront();
	else SetUpBack();

	setKnobMode(kLinearMode);

//	for (int i = 0; i < kMaxNumParameters; i++)
//		setParameter (i, ((ScC120*)effect)->getParameter (i));

	return true;
}
//------------------------------------------------------------------------------------
void PlgEditor::SetUpFront()
{
	if(!frame) return;

	CFrame *newFrame = frame;
	newFrame->removeAll(false);
	GUIbackGrFront = new CBitmap ("PlgBGf");
	CRect frameSize (0, 0, rect.right, rect.bottom);
//	CFrame *newFrame = new CFrame (frameSize, ptr, this);

	gViewFront = new CView (frameSize);
	gViewFront->setBackground(GUIbackGrFront);
	newFrame->addView(gViewFront);

	pGuiKnob = new CBitmap ("Plgknob");
	pGuiButton_Push = new CBitmap ("PlgBttnPush");
	pGuiButton_Flip = new CBitmap ("PlgBttnFlip");
	pGuiButton_Adv = new CBitmap ("PlgBttnAdv");

	AdvButton = new COnOffButton(CRect(550,20,610,120),this,kGuiAdvButton,pGuiButton_Adv);
	newFrame->addView(AdvButton);

	kKnob[0] = new CAnimKnob(CRect(30,170,70,210),this,kGainControl,pGuiKnob,CPoint(0,0));
	kKnob[1] = new CAnimKnob(CRect(100,170,140,210),this,kMasterVol,pGuiKnob,CPoint(0,0));
	kKnob[2] = new CAnimKnob(CRect(250,170,290,210),this,kEQstackLow,pGuiKnob,CPoint(0,0));
	kKnob[3] = new CAnimKnob(CRect(320,170,360,210),this,kEQstackMid,pGuiKnob,CPoint(0,0));
	kKnob[4] = new CAnimKnob(CRect(390,170,430,210),this,kEQstackHigh,pGuiKnob,CPoint(0,0));
	kKnob[5] = new CAnimKnob(CRect(490,170,530,210),this,kEQstackContour,pGuiKnob,CPoint(0,0));
	kKnob[6] = new CAnimKnob(CRect(570,170,610,210),this,kEQstackPres,pGuiKnob,CPoint(0,0));

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

	kBtn_LoGain = new COnOffButton(CRect(370,30,390,50),this,kBtnClean,pGuiButton_Push);
	kBtn_HiGain = new COnOffButton(CRect(370,60,390,80),this,kBtnHeavy,pGuiButton_Push);
	kBtn_Boost = new COnOffButton(CRect(250,30,270,50),this,kBtnBoost,pGuiButton_Push);
	kBtn_Stereo = new COnOffButton(CRect(370,90,390,110),this,kMonoStereoControl,pGuiButton_Push);
	kBtn_Bright = new COnOffButton(CRect(250,60,270,80),this,kBtnBright,pGuiButton_Push);

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

	GUIbackGrFront->forget();
	pGuiKnob->forget();
	pGuiButton_Push->forget();
	pGuiButton_Flip->forget();
	pGuiButton_Adv->forget();

	frame = newFrame;

	setKnobMode(kLinearMode);

//	for (int i = 0; i < kMaxNumParameters; i++)
//		setParameter (i, ((ScC120*)effect)->getParameter (i));
//	newFrame->forget();
}
//------------------------------------------------------------------------------------
void PlgEditor::SetUpBack()
{
	if(!frame) return;
//	CFrame *newFrame(frame);
	CFrame *newFrame = frame;
	newFrame->removeAll(false);
	GUIbackGrBack = new CBitmap ("PlgBGb");
	CRect frameSize (0, 0, rect.right, rect.bottom);
//	CFrame *newFrame = new CFrame (frameSize, ptr, this);

	gViewBack = new CView (frameSize);
	gViewBack->setBackground(GUIbackGrBack);
	newFrame->addView(gViewBack);

	pGuiKnob = new CBitmap ("Plgknob");
	pGuiButton_Push = new CBitmap ("PlgBttnPush");
	pGuiButton_Flip = new CBitmap ("PlgBttnFlip");
	pGuiButton_Adv = new CBitmap ("PlgBttnAdv");

	AdvButton = new COnOffButton(CRect(550,20,610,120),this,kGuiAdvButton,pGuiButton_Adv);
	newFrame->addView(AdvButton);
	AdvButton->setValue(1.0f);

	int x = 0;
	for(x = kSmpRenderOn1x; x <= kSmpRenderOnOFF; x++)
	{
		SamplingOn[x-kSmpRenderOn1x] = new COnOffButton(CRect(20,71+((x-kSmpRenderOn1x) * 23),40,91+((x-kSmpRenderOn1x) * 23)),this,x,pGuiButton_Push);
		SamplingOn[x-kSmpRenderOn1x]->setValue(((ScC120*)effect)->getParameter(x));
		newFrame->addView(SamplingOn[x-kSmpRenderOn1x]);
	}
	for(x = kSmpRenderOff1x; x <= kSmpRenderOffOFF; x++)
	{
		SamplingOff[x-kSmpRenderOff1x] = new COnOffButton(CRect(120,71+((x-kSmpRenderOff1x) * 23),140,91+((x-kSmpRenderOff1x) * 23)),this,x,pGuiButton_Push);
		SamplingOff[x-kSmpRenderOff1x]->setValue(((ScC120*)effect)->getParameter(x));
		newFrame->addView(SamplingOff[x-kSmpRenderOff1x]);
	}

	BtnBitDepthOn[0] = new COnOffButton(CRect(210,140,230,160),this,kSmpBitOn32,pGuiButton_Push);
	BtnBitDepthOn[0]->setValue(((ScC120*)effect)->getParameter(kSmpBitOn32));
	newFrame->addView(BtnBitDepthOn[0]);
	BtnBitDepthOn[1] = new COnOffButton(CRect(210,165,230,185),this,kSmpBitOn64,pGuiButton_Push);
	BtnBitDepthOn[1]->setValue(((ScC120*)effect)->getParameter(kSmpBitOn64));
	newFrame->addView(BtnBitDepthOn[1]);
	BtnBitDepthOn[2] = new COnOffButton(CRect(210,190,230,210),this,kSmpBitOn128,pGuiButton_Push);
	BtnBitDepthOn[2]->setValue(((ScC120*)effect)->getParameter(kSmpBitOn128));
	newFrame->addView(BtnBitDepthOn[2]);

	BtnBitDepthOff[0] = new COnOffButton(CRect(330,140,350,160),this,kSmpBitOff32,pGuiButton_Push);
	BtnBitDepthOff[0]->setValue(((ScC120*)effect)->getParameter(kSmpBitOff32));
	newFrame->addView(BtnBitDepthOff[0]);
	BtnBitDepthOff[1] = new COnOffButton(CRect(330,165,350,185),this,kSmpBitOff64,pGuiButton_Push);
	BtnBitDepthOff[1]->setValue(((ScC120*)effect)->getParameter(kSmpBitOff64));
	newFrame->addView(BtnBitDepthOff[1]);
	BtnBitDepthOff[2] = new COnOffButton(CRect(330,190,350,210),this,kSmpBitOff128,pGuiButton_Push);
	BtnBitDepthOff[2]->setValue(((ScC120*)effect)->getParameter(kSmpBitOff128));
	newFrame->addView(BtnBitDepthOff[2]);

	kBtn_TStackEven = new COnOffButton(CRect(240,20,260,40),this,kToneStFlatEven,pGuiButton_Push);
	kBtn_TStackShaped = new COnOffButton(CRect(240,50,260,70),this,kToneStShaped,pGuiButton_Push);
	kBtn_CabMic = new COnOffButton(CRect(550,160,570,180),this,kInternalCab,pGuiButton_Push);
	kBtn_TStackEven->setValue(((ScC120*)effect)->getParameter(kToneStFlatEven));
	kBtn_TStackShaped->setValue(((ScC120*)effect)->getParameter(kToneStShaped));
	kBtn_CabMic->setValue(((ScC120*)effect)->getParameter(kInternalCab));
	newFrame->addView(kBtn_TStackEven);
	newFrame->addView(kBtn_TStackShaped);
	newFrame->addView(kBtn_CabMic);

	GUIbackGrBack->forget();
	pGuiKnob->forget();
	pGuiButton_Push->forget();
	pGuiButton_Flip->forget();
	pGuiButton_Adv->forget();

	frame = newFrame;

	setKnobMode(kLinearMode);

//	for (int i = 0; i < kMaxNumParameters; i++)
//		setParameter (i, ((ScC120*)effect)->getParameter (i));
//	newFrame->forget();
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
	((ScC120*)effect)->setParameterAutomated(pControl->getTag(), pControl->getValue());
}
//------------------------------------------------------------------------------------
void PlgEditor::setParameter(VstInt32 index, float value)
{
	//-- setParameter is called when the host automates one of the effects parameter.
	//-- The UI should reflect this state so we set the value of the control to the new value.
	//-- VSTGUI will automaticly redraw changed controls in the next idle (as this call happens to be in the process thread).
	// if(!frame) return;
	if(!frame) return;

	if (isDisplayingFront == true)
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
	if(index == kGuiAdvButton)
	{
		if(frame && AdvButton)
		{
			AdvButton->setValue(value);
			if(value == 0.0f) isDisplayingFront = true;
			if(value == 1.0f) isDisplayingFront = false;
			if(isDisplayingFront == true)
			{
				SetUpFront();
			} else
			{
				SetUpBack();
			}
		}
	}
	if (isDisplayingFront == false)
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
		if(index == kSmpBitOn32)
		{
			if(BtnBitDepthOn[0])
			{
				BtnBitDepthOn[0]->setValue(value);
				BtnBitDepthOn[0]->invalid();
			}
		}
		if(index == kSmpBitOn64)
		{
			if(BtnBitDepthOn[1])
			{
				BtnBitDepthOn[1]->setValue(value);
				BtnBitDepthOn[1]->invalid();
			}
		}
		if(index == kSmpBitOn128)
		{
			if(BtnBitDepthOn[2])
			{
				BtnBitDepthOn[2]->setValue(value);
				BtnBitDepthOn[2]->invalid();
			}
		}
		if(index == kSmpBitOff32)
		{
			if(BtnBitDepthOff[0])
			{
				BtnBitDepthOff[0]->setValue(value);
				BtnBitDepthOff[0]->invalid();
			}
		}
		if(index == kSmpBitOff64)
		{
			if(BtnBitDepthOff[1])
			{
				BtnBitDepthOff[1]->setValue(value);
				BtnBitDepthOff[1]->invalid();
			}
		}
		if(index == kSmpBitOff128)
		{
			if(BtnBitDepthOff[2])
			{
				BtnBitDepthOff[2]->setValue(value);
				BtnBitDepthOff[2]->invalid();
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
		if(index == kToneStShaped)
		{
			if(kBtn_TStackShaped)
			{
				kBtn_TStackShaped->setValue(value);
				kBtn_TStackShaped->invalid();
			}
		}
		if(index == kInternalCab)
		{
			if(kBtn_CabMic)
			{
				kBtn_CabMic->setValue(value);
				kBtn_CabMic->invalid();
			}
		}
	}
}
void PlgEditor::idle()
{
	AEffGUIEditor::idle();
}
