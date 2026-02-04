#ifndef __PlgEditor__
#define __PlgEditor__

#include "../lib_graphics_src/zlib.h"
#include "../lib_graphics_src/zconf.h"
#include "../lib_graphics_src/pngconf.h"
#include "../lib_graphics_src/png.h"
#include <vstgui.h>
#include <aeffguieditor.h>
#include "params.h"
#include "../lib_graphics_src/zlib.h"
#include "../lib_graphics_src/zconf.h"
#include "../lib_graphics_src/pngconf.h"
#include "../lib_graphics_src/png.h"
#include <vstgui.h>
#include <vstcontrols.h>
#include <aeffguieditor.h>
// #include "resource.h"
#include "c120.h"

class PlgEditor : public AEffGUIEditor, public CControlListener
{
public:
	PlgEditor (void*);
	~PlgEditor();

	friend class ScC120;

	// from AEffGUIEditor
	bool open (void* ptr);
	void close ();
	void setParameter (VstInt32 index, float value);

	// from CControlListener
	void valueChanged (CControl* pControl);

	void idle();

protected:
	void SetUpFront();
	void SetUpBack();

	CBitmap* GUIbackGrFront;
	CBitmap* GUIbackGrBack;

	CView* gViewFront;
	CView* gViewBack;

	bool isDisplayingFront;

	CBitmap* pGuiButton_Adv;
	COnOffButton* AdvButton;

	CBitmap* pGuiKnob;
	CAnimKnob* kKnob[7];

	CBitmap* pGuiButton_Push;
	CBitmap* pGuiButton_Flip;

	COnOffButton* kBtn_LoGain;
	COnOffButton* kBtn_HiGain;
	COnOffButton* kBtn_Stereo;
	COnOffButton* kBtn_Boost;
	COnOffButton* kBtn_Bright;

	COnOffButton* SamplingOn[6];
	COnOffButton* SamplingOff[6];
	COnOffButton* BtnBitDepthOn[3];
	COnOffButton* BtnBitDepthOff[3];

	COnOffButton* kBtn_TStackEven;
	COnOffButton* kBtn_TStackShaped;
	COnOffButton* kBtn_CabMic;
};

#endif
