#include "scorchc_gui.h"

AEffGUIEditor* createEditor (AudioEffectX* effect)
{
	return new ScorchCEditor (effect);
}
//------------------------------------------------------------------------------------
ScorchCEditor::ScorchCEditor (void* ptr)
: AEffGUIEditor (ptr)
{
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = 817;
	rect.bottom = 500;
}
ScorchCEditor::~ScorchCEditor()
{
	// delete GUIbackGr;
}
//------------------------------------------------------------------------------------
bool ScorchCEditor::open (void* ptr)
{
	AEffGUIEditor::open(ptr);
	GUIbackGr = new CBitmap ("NGBG");
	CRect frameSize (0, 0, 817, 500);
	newFrame = new CFrame (frameSize, ptr, this);

	CView* newView = new CView (frameSize);
	newView->setBackground(GUIbackGr);
	newFrame->addView(newView);

	CBitmap* SCKnob = new CBitmap ("NGKnob");
	CBitmap* SCKnobS = new CBitmap ("NGKnobS");
	CBitmap* SCLED = new CBitmap ("NGLED");
	// CBitmap* SCLEDr = new CBitmap ("NGLEDr");
	CBitmap* SCchan = new CBitmap ("NGchan");
	CBitmap* SCnums = new CBitmap ("F12nums");
	CBitmap* dSmpDsp = new CBitmap ("SmpDisp");

	knob1 = new CAnimKnob(CRect(546,235,596,285),this,kSamplingOnline,SCKnobS,CPoint(0,0));
	newFrame->addView (knob1);
	knob2 = new CAnimKnob(CRect(643,235,693,285),this,kSamplingRender,SCKnobS,CPoint(0,0));
	newFrame->addView (knob2);
	knob3 = new CAnimKnob(CRect(676,310,726,360),this,kMasterVol,SCKnob,CPoint(0,0));
	newFrame->addView (knob3);
	knob4 = new CAnimKnob(CRect(81,310,131,360),this,kGainControl,SCKnob,CPoint(0,0));
	newFrame->addView (knob4);
	knob5 = new CAnimKnob(CRect(335,310,385,360),this,kEQstackLow,SCKnob,CPoint(0,0));
	newFrame->addView (knob5);
	knob6 = new CAnimKnob(CRect(421,310,471,360),this,kEQstackMid,SCKnob,CPoint(0,0));
	newFrame->addView (knob6);
	knob7 = new CAnimKnob(CRect(506,310,556,360),this,kEQstackHigh,SCKnob,CPoint(0,0));
	newFrame->addView (knob7);
	knob8 = new CAnimKnob(CRect(590,310,640,360),this,kEQstackPres,SCKnob,CPoint(0,0));
	newFrame->addView (knob8);
	knob9 = new CAnimKnob(CRect(250,310,300,360),this,kEQstackQkick,SCKnob,CPoint(0,0));
	newFrame->addView (knob9);
	knob10 = new CAnimKnob(CRect(258,235,308,285),this,kFilterI,SCKnob,CPoint(0,0));
	newFrame->addView (knob10);
	knob11 = new CAnimKnob(CRect(190,235,240,285),this,kFilterX,SCKnob,CPoint(0,0));
	newFrame->addView (knob11);
	// GainVCont=new COnOffButton(CRect(170,320,206,356),this,kAlgControl,SCLEDr);
	// newFrame->addView(GainVCont);
	TStackSel = new COnOffButton(CRect(76,247,176,277),this,kToneStackType,SCLED);
	newFrame->addView (TStackSel);
	gChanControlClean = new COnOffButton(CRect(137,309,157,319),this,kAlgControlA,SCchan);
	gChanControlCrunch = new COnOffButton(CRect(137,327,157,337),this,kAlgControlB,SCchan);
	gChanControlHigh = new COnOffButton(CRect(137,345,157,355),this,kAlgControlC,SCchan);
	gChanControlMega = new COnOffButton(CRect(137,363,157,373),this,kAlgControlD,SCchan);
	newFrame->addView(gChanControlClean);
	newFrame->addView(gChanControlCrunch);
	newFrame->addView(gChanControlHigh);
	newFrame->addView(gChanControlMega);

	float counterD;
	counterD=float (effect->getParameter(kGainControl) * 1000.f);
	IDisplayGain = new CSpecialDigit(CRect(164,451,212,463),this,kGainControl,long (counterD),4,NULL,NULL,12,12,SCnums);
	newFrame->addView(IDisplayGain);
	counterD=float (effect->getParameter(kMasterVol) * 1000.f);
	IDisplayMVol = new CSpecialDigit(CRect(723,451,771,463),this,kMasterVol,long (counterD),4,NULL,NULL,12,12,SCnums);
	newFrame->addView(IDisplayMVol);
	counterD=float (effect->getParameter(kEQstackQkick) * 1000.f);
	IDisplayEQ0 = new CSpecialDigit(CRect(324,451,372,463),this,kEQstackQkick,long (counterD),4,NULL,NULL,12,12,SCnums);
	newFrame->addView(IDisplayEQ0);
	counterD=float (effect->getParameter(kEQstackLow) * 1000.f);
	IDisplayEQ1 = new CSpecialDigit(CRect(404,451,452,463),this,kEQstackLow,long (counterD),4,NULL,NULL,12,12,SCnums);
	newFrame->addView(IDisplayEQ1);
	counterD=float (effect->getParameter(kEQstackMid) * 1000.f);
	IDisplayEQ2 = new CSpecialDigit(CRect(483,451,531,463),this,kEQstackMid,long (counterD),4,NULL,NULL,12,12,SCnums);
	newFrame->addView(IDisplayEQ2);
	counterD=float (effect->getParameter(kEQstackHigh) * 1000.f);
	IDisplayEQ3 = new CSpecialDigit(CRect(563,451,611,463),this,kEQstackHigh,long (counterD),4,NULL,NULL,12,12,SCnums);
	newFrame->addView(IDisplayEQ3);
	counterD=float (effect->getParameter(kEQstackPres) * 1000.f);
	IDisplayEQ4 = new CSpecialDigit(CRect(644,451,692,463),this,kEQstackPres,long (counterD),4,NULL,NULL,12,12,SCnums);
	newFrame->addView(IDisplayEQ4);
	counterD=float (effect->getParameter(kFilterX) * 1000.f);
	IDisplayFilterX = new CSpecialDigit(CRect(45,451,93,463),this,kFilterX,long (counterD),4,NULL,NULL,12,12,SCnums);
	newFrame->addView(IDisplayFilterX);
	counterD=float (effect->getParameter(kFilterI) * 1000.f);
	IDisplayFilterI = new CSpecialDigit(CRect(104,451,152,463),this,kFilterI,long (counterD),4,NULL,NULL,12,12,SCnums);
	newFrame->addView(IDisplayFilterI);
	SamplingDispOnline = new CMovieBitmap(CRect(563,435,611,447),this,kSamplingOnline,7,12,dSmpDsp,CPoint(0,0));
	newFrame->addView(SamplingDispOnline);
	SamplingDispOffline = new CMovieBitmap(CRect(723,435,771,447),this,kSamplingRender,7,12,dSmpDsp,CPoint(0,0));
	newFrame->addView(SamplingDispOffline);

	setKnobMode(kLinearMode);

	GUIbackGr->forget();
	SCKnob->forget();
	SCKnobS->forget();
	SCnums->forget();
	dSmpDsp->forget();
	SCLED->forget();
	SCchan->forget();

	frame=newFrame;

	for (int i = 0; i < kMaxNumParameters; i++)
		setParameter (i, effect->getParameter (i));

	return true;
}
//------------------------------------------------------------------------------------
void ScorchCEditor::close()
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
}
//------------------------------------------------------------------------------------
void ScorchCEditor::valueChanged(CControl* pControl)
{
	//-- valueChanged is called whenever the user changes one of the controls in the User Interface (UI)
	effect->setParameterAutomated(pControl->getTag(), pControl->getValue());
}
//------------------------------------------------------------------------------------
void ScorchCEditor::setParameter(VstInt32 index, float value)
{
	//-- setParameter is called when the host automates one of the effects parameter.
	//-- The UI should reflect this state so we set the value of the control to the new value.
	//-- VSTGUI will automaticly redraw changed controls in the next idle (as this call happens to be in the process thread).
	// if(!frame) return;
	if (frame && index < kMaxNumParameters)
	{

	float counterD;
	int xx;
	switch(index)
    {
    case kSamplingOnline:
        counterD = float (effect->getParameter(index));
        if(knob1) knob1->setValue(counterD);
        xx= int ((counterD * 6.f) + 1.f);
		xx--;
		counterD=float (xx);
		if(SamplingDispOnline) SamplingDispOnline->setValue(counterD/6.f);
		if(SamplingDispOnline) SamplingDispOnline->invalid();
        break;
	case kSamplingRender:
		counterD = float (effect->getParameter(index));
        if(knob2) knob2->setValue(counterD);
        xx= int ((counterD * 6.f) + 1.f);
		xx--;
		counterD=float (xx);
		if(SamplingDispOffline) SamplingDispOffline->setValue(counterD/6.f);
		if(SamplingDispOffline) SamplingDispOffline->invalid();
        break;
	case kMasterVol:
        if(knob3) knob3->setValue(effect->getParameter(index));
		counterD=(effect->getParameter(index) * 1000.f);
		if(IDisplayMVol) IDisplayMVol->setValue(counterD);
		if(IDisplayMVol) IDisplayMVol->invalid();
        break;
	case kGainControl:
        if(knob4) knob4->setValue(effect->getParameter(index));
		counterD=(effect->getParameter(index) * 1000.f);
		if(IDisplayGain) IDisplayGain->setValue(counterD);
		if(IDisplayGain) IDisplayGain->invalid();
        break;
	case kEQstackLow:
        if(knob5) knob5->setValue(effect->getParameter(index));
		counterD=(effect->getParameter(index) * 1000.f);
		if(IDisplayEQ1) IDisplayEQ1->setValue(counterD);
		if(IDisplayEQ1) IDisplayEQ1->invalid();
        break;
	case kEQstackMid:
        if(knob6) knob6->setValue(effect->getParameter(index));
		counterD=(effect->getParameter(index) * 1000.f);
		if(IDisplayEQ2) IDisplayEQ2->setValue(counterD);
		if(IDisplayEQ2) IDisplayEQ2->invalid();
        break;
	case kEQstackHigh:
        if(knob7) knob7->setValue(effect->getParameter(index));
		counterD=(effect->getParameter(index) * 1000.f);
		if(IDisplayEQ3) IDisplayEQ3->setValue(counterD);
		if(IDisplayEQ3) IDisplayEQ3->invalid();
        break;
	case kEQstackPres:
        if(knob8) knob8->setValue(effect->getParameter(index));
		counterD=(effect->getParameter(index) * 1000.f);
		if(IDisplayEQ4) IDisplayEQ4->setValue(counterD);
		if(IDisplayEQ4) IDisplayEQ4->invalid();
        break;
	case kEQstackQkick:
        if(knob9) knob9->setValue(effect->getParameter(index));
		counterD=(effect->getParameter(index) * 1000.f);
		if(IDisplayEQ0) IDisplayEQ0->setValue(counterD);
		if(IDisplayEQ0) IDisplayEQ0->invalid();
        break;
	case kFilterI:
        if(knob10) knob10->setValue(effect->getParameter(index));
		counterD=(effect->getParameter(index) * 1000.f);
		if(IDisplayFilterI) IDisplayFilterI->setValue(counterD);
		if(IDisplayFilterI) IDisplayFilterI->invalid();
        break;
	case kFilterX:
        if(knob11) knob11->setValue(effect->getParameter(index));
		counterD=(effect->getParameter(index) * 1000.f);
		if(IDisplayFilterX) IDisplayFilterX->setValue(counterD);
		if(IDisplayFilterX) IDisplayFilterX->invalid();
        break;
	case kAlgControlA:
		if(gChanControlClean)
		{
			gChanControlClean->setValue(effect->getParameter(index));
			gChanControlClean->invalid();
		}
		// effect->setParameterAutomated(kAlgControlB, float (0.f));
		// effect->setParameterAutomated(kAlgControlC, float (0.f));
		// effect->setParameterAutomated(kAlgControlD, float (0.f));
        break;
	case kAlgControlB:
		if(gChanControlCrunch)
		{
			gChanControlCrunch->setValue(effect->getParameter(index));
			gChanControlCrunch->invalid();
		}
		// effect->setParameterAutomated(kAlgControlA, float (0.f));
		// effect->setParameterAutomated(kAlgControlC, float (0.f));
		// effect->setParameterAutomated(kAlgControlD, float (0.f));
        break;
	case kAlgControlC:
		if(gChanControlHigh)
		{
			gChanControlHigh->setValue(effect->getParameter(index));
			gChanControlHigh->invalid();
		}
		// effect->setParameterAutomated(kAlgControlA, float (0.f));
		// effect->setParameterAutomated(kAlgControlB, float (0.f));
		// effect->setParameterAutomated(kAlgControlD, float (0.f));
        break;
	case kAlgControlD:
		if(gChanControlMega)
		{
			gChanControlMega->setValue(effect->getParameter(index));
			gChanControlMega->invalid();
		}
		// effect->setParameterAutomated(kAlgControlA, float (0.f));
		// effect->setParameterAutomated(kAlgControlB, float (0.f));
		// effect->setParameterAutomated(kAlgControlC, float (0.f));
        break;
	case kToneStackType:
		if(TStackSel) TStackSel->setValue(effect->getParameter(index));
		break;
	}
	}
}
void ScorchCEditor::idle()
{
	AEffGUIEditor::idle();
}
