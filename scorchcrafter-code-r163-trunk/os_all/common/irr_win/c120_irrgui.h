#ifndef __scorch_C120_IRR_GUI__
#define __scorch_C120_IRR_GUI__

#include <../../sdk_vst/public.sdk/source/vst2.x/aeffeditor.h>
#include <../../sdk_vst/public.sdk/source/vst2.x/audioeffectx.h>
#include <../../sdk_vst/pluginterfaces/vst2.x/aeffectx.h>

#include <../../irrlicht/include/irrlicht.h>
#include <windows.h>
#include <iostream>
#include "../../vst_c120/params.h"
#include "../../vst_c120/c120.h"

#ifdef BUILD_WITH_D3D9
#include <d3dx9.h>
#endif

#include "../../embedded/sc_c120_front.h"

// #include "../c120_gui_graphics.h"

#define IRR_GUI_KNOB_ADJ_SPEED (float) 0.02f

using namespace irr;
using namespace core;
using namespace io;
using namespace gui;
using namespace scene;
using namespace video;

class GEventReceiver : public IEventReceiver
{
public:
	virtual bool OnEvent(const SEvent& event)
	{
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

		if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
		{
			if (event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
				LMdown = true;

			if (event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
				LMdown = false;
		}

		return false;
	}

	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}

	GEventReceiver()
	{
		for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;

		LMdown = false;
		RMdown = false;
	}

	virtual bool IsLMouseDown()
	{
		return LMdown;
	}
	virtual bool IsRMouseDown()
	{
		return RMdown;
	}

private:
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
	bool LMdown;
	bool RMdown;
};

class SC_C120_Editor : public AEffEditor
{
public:
	SC_C120_Editor (void* SCeffect);
	virtual ~SC_C120_Editor();

	virtual void setParameter (VstInt32 index, float value);
	virtual bool getRect (ERect** ppRect);
	virtual bool open (void* ptr);
	virtual void close();
	virtual void idle ();
	virtual void draw (ERect* pRect); // (ERect* pRect);

	void setMouseButton(bool dwn);

	// wait (in ms)
	void wait(unsigned long ms);

	// get the current time (in ms)
	unsigned long getTicks ();

	// feedback to appli.
	virtual void doIdleStuff ();

	// get the effect attached to this editor
	AudioEffect* getEffect () { return effect; }

	// get version of this VSTGUI
	VstInt32 getVstGuiVersion () { return long (99); }

	// set/get the knob mode
	virtual bool setKnobMode (VstInt32 val);
	virtual VstInt32 getKnobMode () const { return knobMode; }

	virtual bool onWheel (float distance);

#if VST_2_1_EXTENSIONS
	virtual bool onKeyDown (VstKeyCode& keyCode);
	virtual bool onKeyUp (VstKeyCode& keyCode);

	virtual void beginEdit (VstInt32 index) { ((AudioEffectX*)effect)->beginEdit (index); }
	virtual void endEdit (VstInt32 index)   { ((AudioEffectX*)effect)->endEdit (index); }
#endif

	int guiWide;
	int guiTall;

	void RunIRRGui();
	void InitIRRGui();

	ERect rect;

	IrrlichtDevice* Idevice;
	scene::ISceneManager* smgr;
	video::IVideoDriver* drv;
	gui::IGUIEnvironment* guienv;
	ICursorControl *mCntrl;

	video::SExposedVideoData vData;

	HWND GuiWindow;
	HWND hIrrlichtWindow;

	HINSTANCE hInstance;
	char Win32ClassName[60];

private:
	void RunInput();

	GEventReceiver *evRec;

	bool inIdleStuff;
	VstInt32 knobMode;
	unsigned long long lLastTicks;

	float gParams[kMaxNumParameters];

	bool mButtonDown;

	ITexture *GuiBG[2];
	IGUIImage *BG;
	bool showingFront;

	ITexture *KnobTex[51];
	IGUIImage *Knob[7];

	int knobPos[kMaxNumParameters];
	core::rect<s32> knobCoor[kMaxNumParameters];

	core::position2di lastMousePos;
	bool knobFocused;
	bool knobCanBeFocused;
	int curFocusedKnob;
};

#endif
