#include "jyk_gui.h"

AEffGUIEditor* createEditor (AudioEffectX* effect)
{
	return new JykEditor (effect);
}
//------------------------------------------------------------------------------------
JykEditor::JykEditor (void* ptr)
: AEffGUIEditor (ptr)
{
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = 800;
	rect.bottom = 200;
}
JykEditor::~JykEditor()
{
	// delete GUIbackGr;
}
//------------------------------------------------------------------------------------
bool JykEditor::open (void* ptr)
{
	AEffGUIEditor::open(ptr);
	GUIbackGr = new CBitmap ("JKBG");
	CRect frameSize (0, 0, 800, 200);
	newFrame = new CFrame (frameSize, ptr, this);

	CView* newView = new CView (frameSize);
	newView->setBackground(GUIbackGr);
	newFrame->addView(newView);

	CBitmap* JGuiKnob = new CBitmap ("JKknob");
	CBitmap* JGuiNums = new CBitmap ("JKnums");
	CBitmap* JGuiSelect = new CBitmap ("JKselBox");
	CBitmap* JGuiSquare = new CBitmap ("JKselSqr");

	knob[0] = new CAnimKnob(CRect(230,110,280,160),this,kEQstackQkick,JGuiKnob,CPoint(0,0));
	knob[1] = new CAnimKnob(CRect(300,110,350,160),this,kEQstackLow,JGuiKnob,CPoint(0,0));
	knob[2] = new CAnimKnob(CRect(370,110,420,160),this,kEQstackMid,JGuiKnob,CPoint(0,0));
	knob[3] = new CAnimKnob(CRect(440,110,490,160),this,kEQstackHigh,JGuiKnob,CPoint(0,0));
	knob[4] = new CAnimKnob(CRect(510,110,560,160),this,kEQstackPres,JGuiKnob,CPoint(0,0));
	knob[5] = new CAnimKnob(CRect(20,110,70,160),this,kGainControl,JGuiKnob,CPoint(0,0));
	knob[6] = new CAnimKnob(CRect(90,110,140,160),this,kMasterVol,JGuiKnob,CPoint(0,0));

	nums[0] = new CAnimKnob(CRect(230,170,280,190),this,kEQstackQkick,JGuiNums,CPoint(0,0));
	nums[1] = new CAnimKnob(CRect(300,170,350,190),this,kEQstackLow,JGuiNums,CPoint(0,0));
	nums[2] = new CAnimKnob(CRect(370,170,420,190),this,kEQstackMid,JGuiNums,CPoint(0,0));
	nums[3] = new CAnimKnob(CRect(440,170,490,190),this,kEQstackHigh,JGuiNums,CPoint(0,0));
	nums[4] = new CAnimKnob(CRect(510,170,560,190),this,kEQstackPres,JGuiNums,CPoint(0,0));
	nums[5] = new CAnimKnob(CRect(20,170,70,190),this,kGainControl,JGuiNums,CPoint(0,0));
	nums[6] = new CAnimKnob(CRect(90,170,140,190),this,kMasterVol,JGuiNums,CPoint(0,0));

	for(int x = 0; x < 7; x++)
	{
		newFrame->addView (knob[x]);
		newFrame->addView (nums[x]);
	}

	gChanControlClean = new COnOffButton(CRect(580,80,620,100),this,kAlgControlA,JGuiSquare);
	gChanControlCrunch = new COnOffButton(CRect(580,100,620,120),this,kAlgControlB,JGuiSquare);
	gChanControlHigh = new COnOffButton(CRect(580,120,620,140),this,kAlgControlC,JGuiSquare);
	gChanControlMega = new COnOffButton(CRect(580,140,620,160),this,kAlgControlD,JGuiSquare);
	newFrame->addView(gChanControlClean);
	newFrame->addView(gChanControlCrunch);
	newFrame->addView(gChanControlHigh);
	newFrame->addView(gChanControlMega);

	sOnline[0] = new COnOffButton(CRect(580,10,620,30),this,kOnline2x,JGuiSelect);
	sOnline[1] = new COnOffButton(CRect(620,10,660,30),this,kOnline4x,JGuiSelect);
	sOnline[2] = new COnOffButton(CRect(660,10,700,30),this,kOnline8x,JGuiSelect);
	sOnline[3] = new COnOffButton(CRect(700,10,740,30),this,kOnline16x,JGuiSelect);
	sOnline[4] = new COnOffButton(CRect(740,10,780,30),this,kOnlineMute,JGuiSelect);

	sOffline[0] = new COnOffButton(CRect(580,30,620,50),this,kOffline2x,JGuiSelect);
	sOffline[1] = new COnOffButton(CRect(620,30,660,50),this,kOffline4x,JGuiSelect);
	sOffline[2] = new COnOffButton(CRect(660,30,700,50),this,kOffline8x,JGuiSelect);
	sOffline[3] = new COnOffButton(CRect(700,30,740,50),this,kOffline16x,JGuiSelect);
	sOffline[4] = new COnOffButton(CRect(740,30,780,50),this,kOfflineMute,JGuiSelect);

	for(int y = 0; y < 5; y++)
	{
		newFrame->addView (sOnline[y]);
		newFrame->addView (sOffline[y]);
	}

	StereoEnable = new COnOffButton(CRect(580,170,620,190),this,kMonoStereoControl,JGuiSquare);
	newFrame->addView (StereoEnable);

	phaseCon[0] = new COnOffButton(CRect(95,55,135,75),this,kPhaseMaster,JGuiSquare);
	phaseCon[1] = new COnOffButton(CRect(235,55,275,75),this,kPhaseFloor,JGuiSquare);
	phaseCon[2] = new COnOffButton(CRect(305,55,345,75),this,kPhaseLow,JGuiSquare);
	phaseCon[3] = new COnOffButton(CRect(375,55,415,75),this,kPhaseMid,JGuiSquare);
	phaseCon[4] = new COnOffButton(CRect(445,55,485,75),this,kPhaseHigh,JGuiSquare);
	phaseCon[5] = new COnOffButton(CRect(515,55,555,75),this,kPhasePres,JGuiSquare);

	newFrame->addView (phaseCon[0]);
	newFrame->addView (phaseCon[1]);
	newFrame->addView (phaseCon[2]);
	newFrame->addView (phaseCon[3]);
	newFrame->addView (phaseCon[4]);
	newFrame->addView (phaseCon[5]);

	setKnobMode(kLinearMode);

	GUIbackGr->forget();
	JGuiKnob->forget();
	JGuiNums->forget();
	JGuiSelect->forget();
	JGuiSquare->forget();

	frame = newFrame;

	for (int i = 0; i < kMaxNumParameters; i++)
		setParameter (i, effect->getParameter (i));

	return true;
}
//------------------------------------------------------------------------------------
void JykEditor::close()
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
void JykEditor::valueChanged(CControl* pControl)
{
	//-- valueChanged is called whenever the user changes one of the controls in the User Interface (UI)
	effect->setParameterAutomated(pControl->getTag(), pControl->getValue());
}
//------------------------------------------------------------------------------------
void JykEditor::setParameter(VstInt32 index, float value)
{
	//-- setParameter is called when the host automates one of the effects parameter.
	//-- The UI should reflect this state so we set the value of the control to the new value.
	//-- VSTGUI will automaticly redraw changed controls in the next idle (as this call happens to be in the process thread).
	// if(!frame) return;
	if (frame && index < kMaxNumParameters)
	{
		if(index == kGainControl)
		{
			if(knob[5])
			{
				knob[5]->setValue(float (value));
				knob[5]->invalid();
			}
			if(nums[5])
			{
				nums[5]->setValue(float (value));
				nums[5]->invalid();
			}
		}
		if(index == kMasterVol)
		{
			if(knob[6])
			{
				knob[6]->setValue(float (value));
				knob[6]->invalid();
			}
			if(nums[6])
			{
				nums[6]->setValue(float (value));
				nums[6]->invalid();
			}
		}
		if(index == kEQstackQkick)
		{
			if(knob[0])
			{
				knob[0]->setValue(float (value));
				knob[0]->invalid();
			}
			if(nums[0])
			{
				nums[0]->setValue(float (value));
				nums[0]->invalid();
			}
		}
		if(index == kEQstackLow)
		{
			if(knob[1])
			{
				knob[1]->setValue(float (value));
				knob[1]->invalid();
			}
			if(nums[1])
			{
				nums[1]->setValue(float (value));
				nums[1]->invalid();
			}
		}
		if(index == kEQstackMid)
		{
			if(knob[2])
			{
				knob[2]->setValue(float (value));
				knob[2]->invalid();
			}
			if(nums[2])
			{
				nums[2]->setValue(float (value));
				nums[2]->invalid();
			}
		}
		if(index == kEQstackHigh)
		{
			if(knob[3])
			{
				knob[3]->setValue(float (value));
				knob[3]->invalid();
			}
			if(nums[3])
			{
				nums[3]->setValue(float (value));
				nums[3]->invalid();
			}
		}
		if(index == kEQstackPres)
		{
			if(knob[4])
			{
				knob[4]->setValue(float (value));
				knob[4]->invalid();
			}
			if(nums[4])
			{
				nums[4]->setValue(float (value));
				nums[4]->invalid();
			}
		}
		if(index == kAlgControlA)
		{
			gChanControlClean->setValue(value);
			gChanControlClean->invalid();
		}
		if(index == kAlgControlB)
		{
			gChanControlCrunch->setValue(value);
			gChanControlCrunch->invalid();
		}
		if(index == kAlgControlC)
		{
			gChanControlHigh->setValue(value);
			gChanControlHigh->invalid();
		}
		if(index == kAlgControlD)
		{
			gChanControlMega->setValue(value);
			gChanControlMega->invalid();
		}
		if(index == kOnline2x)
		{
			sOnline[0]->setValue(value);
			sOnline[0]->invalid();
		}
		if(index == kOnline4x)
		{
			sOnline[1]->setValue(value);
			sOnline[1]->invalid();
		}
		if(index == kOnline8x)
		{
			sOnline[2]->setValue(value);
			sOnline[2]->invalid();
		}
		if(index == kOnline16x)
		{
			sOnline[3]->setValue(value);
			sOnline[3]->invalid();
		}
		if(index == kOnlineMute)
		{
			sOnline[4]->setValue(value);
			sOnline[4]->invalid();
		}
		if(index == kOffline2x)
		{
			sOffline[0]->setValue(value);
			sOffline[0]->invalid();
		}
		if(index == kOffline4x)
		{
			sOffline[1]->setValue(value);
			sOffline[1]->invalid();
		}
		if(index == kOffline8x)
		{
			sOffline[2]->setValue(value);
			sOffline[2]->invalid();
		}
		if(index == kOffline16x)
		{
			sOffline[3]->setValue(value);
			sOffline[3]->invalid();
		}
		if(index == kOfflineMute)
		{
			sOffline[4]->setValue(value);
			sOffline[4]->invalid();
		}
		if(index == kMonoStereoControl)
		{
			StereoEnable->setValue(value);
			StereoEnable->invalid();
		}
		if(index == kPhaseMaster)
		{
			phaseCon[0]->setValue(value);
			phaseCon[0]->invalid();
		}
		if(index == kPhaseFloor)
		{
			phaseCon[1]->setValue(value);
			phaseCon[1]->invalid();
		}
		if(index == kPhaseLow)
		{
			phaseCon[2]->setValue(value);
			phaseCon[2]->invalid();
		}
		if(index == kPhaseMid)
		{
			phaseCon[3]->setValue(value);
			phaseCon[3]->invalid();
		}
		if(index == kPhaseHigh)
		{
			phaseCon[4]->setValue(value);
			phaseCon[4]->invalid();
		}
		if(index == kPhasePres)
		{
			phaseCon[5]->setValue(value);
			phaseCon[5]->invalid();
		}
	}
}
void JykEditor::idle()
{
	AEffGUIEditor::idle();
}
