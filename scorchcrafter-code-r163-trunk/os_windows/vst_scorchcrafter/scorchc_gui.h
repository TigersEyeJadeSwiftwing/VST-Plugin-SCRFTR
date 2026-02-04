#ifndef __ScorchCEditor__
#define __ScorchCEditor__

#include "../lib_graphics_src/zlib.h"
#include "../lib_graphics_src/zconf.h"
#include "../lib_graphics_src/pngconf.h"
#include "../lib_graphics_src/png.h"
#include <vstgui.h>
#include <aeffguieditor.h>
#include "params.h"
#include <vstcontrols.h>
#include "resource.h"
#include "scr_amp_head.h"

class ScorchCEditor : public AEffGUIEditor, public CControlListener
{
public:
	ScorchCEditor (void*);
	~ScorchCEditor();

	// from AEffGUIEditor
	bool open (void* ptr);
	void close ();
	void setParameter (VstInt32 index, float value);

	// from CControlListener
	void valueChanged (CControl* pControl);

	void idle();

protected:
	CBitmap* GUIbackGr;

	CFrame* newFrame;
	// CControl* controls[kMaxNumParameters];
	CAnimKnob* knob1;
	CAnimKnob* knob2;
	CAnimKnob* knob3;
	CAnimKnob* knob4;
	CAnimKnob* knob5;
	CAnimKnob* knob6;
	CAnimKnob* knob7;
	CAnimKnob* knob8;
	CAnimKnob* knob9;
	CAnimKnob* knob10;
	CAnimKnob* knob11;
	COnOffButton* TStackSel;
	// COnOffButton* GainVCont;
	COnOffButton* gChanControlClean;
	COnOffButton* gChanControlCrunch;
	COnOffButton* gChanControlHigh;
	COnOffButton* gChanControlMega;
	CSpecialDigit* IDisplayGain;
	CSpecialDigit* IDisplayMVol;
	CSpecialDigit* IDisplayEQ0;
	CSpecialDigit* IDisplayEQ1;
	CSpecialDigit* IDisplayEQ2;
	CSpecialDigit* IDisplayEQ3;
	CSpecialDigit* IDisplayEQ4;
	CSpecialDigit* IDisplayFilterX;
	CSpecialDigit* IDisplayFilterI;
	CMovieBitmap* SamplingDispOnline;
	CMovieBitmap* SamplingDispOffline;
};

#endif
