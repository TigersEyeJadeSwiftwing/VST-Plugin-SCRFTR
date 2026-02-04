#ifndef __PlgEditor_Jyk__
#define __PlgEditor_Jyk__

#include "../zlib/zconf.h"
#include "../zlib/zlib.h"
#include "../libpng/pngconf.h"
#include "../libpng/png.h"
#include <vstgui.h>
#include <aeffeditor.h>
#include "params_vst_jyk.h"
#include <vstgui.h>
#include <vstcontrols.h>
#include <aeffguieditor.h>
// #include "plugguieditor.h"
#include "jyk.h"
// #include "../common/sc_parameters.h"
// #include "../common/plugin_c120.h"

class PlgEditor : public AEffGUIEditor, public CControlListener
{
public:
	PlgEditor (void*);
	virtual ~PlgEditor();

	friend class JykWkr;

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
