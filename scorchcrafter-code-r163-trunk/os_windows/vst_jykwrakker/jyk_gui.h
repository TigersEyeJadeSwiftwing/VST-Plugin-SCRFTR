#ifndef __JykEditor__
#define __JykEditor__

#include "../lib_graphics_src/zlib.h"
#include "../lib_graphics_src/zconf.h"
#include "../lib_graphics_src/pngconf.h"
#include "../lib_graphics_src/png.h"
#include <vstgui.h>
#include <aeffguieditor.h>
#include "params.h"
#include <vstcontrols.h>
// #include "resource.h"
#include "jyk.h"

class JykEditor : public AEffGUIEditor, public CControlListener
{
public:
	JykEditor (void*);
	~JykEditor();

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
	CAnimKnob* knob[7];
	CAnimKnob* nums[7];

	COnOffButton* gChanControlClean;
	COnOffButton* gChanControlCrunch;
	COnOffButton* gChanControlHigh;
	COnOffButton* gChanControlMega;

	COnOffButton* sOnline[5];
	COnOffButton* sOffline[5];

	COnOffButton* phaseCon[6];

	COnOffButton* StereoEnable;
};

#endif
