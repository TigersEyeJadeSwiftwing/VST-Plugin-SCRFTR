#ifndef __PlgEditor_C120__
#define __PlgEditor_C120__

#include "../zlib/zconf.h"
#include "../zlib/zlib.h"
#include "../libpng/pngconf.h"
#include "../libpng/png.h"
// #include <vstgui.h>

#ifdef BUILD_VST

#include "aeffeditor.h"
#include "params_vst_c120.h"
#include "vstgui.h"

#ifndef USE_VER_FOUR_GUI
#include <vstcontrols.h>
#endif

#include <aeffguieditor.h>

#else

#include "params_c120.h"
#include <plugguieditor.h>

#endif	// /BUILD_VST

#include "c120.h"

class PlgEditor_C120 : public AEffGUIEditor, public CControlListener
{
public:
	PlgEditor_C120 (void*);
	virtual ~PlgEditor_C120();

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
	void SetUpAux();

	CBitmap* GUIback[3];
	CBitmap* pGuiKnob;
	CBitmap* pGuiButton;

	CView* gView[3];

	CAnimKnob* kKnob[7];

	COnOffButton* PanelButton[3];

	COnOffButton* kBtn_LoGain;
	COnOffButton* kBtn_HiGain;
	COnOffButton* kBtn_Stereo;
	COnOffButton* kBtn_Boost;
	COnOffButton* kBtn_Bright;

	COnOffButton* SamplingOn[6];
	COnOffButton* SamplingOff[6];

	COnOffButton* kBtn_TStackEven;
	COnOffButton* kBtn_TStackShaped[6];
	COnOffButton* kBtn_CabMic;
	COnOffButton* kBtn_CabMicSelect[6];
	COnOffButton* kDynamicToneOn;

	COnOffButton* kBitDepthOn[3];
	COnOffButton* kBitDepthOff[3];
};

#endif
