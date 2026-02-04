#ifndef __PlgEditor_Glz_GLZ__
#define __PlgEditor_Glz_GLZ__

#include "../zlib/zconf.h"
#include "../zlib/zlib.h"
#include "../libpng/pngconf.h"
#include "../libpng/png.h"
#include <vstgui.h>
#include <aeffeditor.h>
#include "params_glz60.h"
#include <vstgui.h>
#include <vstcontrols.h>
#include <aeffguieditor.h>
#include "glz60.h"

class PlgEditor_Glz : public AEffGUIEditor, public CControlListener
{
public:
	PlgEditor_Glz (void*);
	virtual ~PlgEditor_Glz();

	friend class ScGLZ60;

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
	CBitmap* pGuiKnobSm;
	CBitmap* pGuiButton;

	CView* gView[3];

	CAnimKnob* kKnob[3];
	CAnimKnob* kKnobSm[10];

	COnOffButton* PanelButton[3];

	COnOffButton* kBtn_Stereo;
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
