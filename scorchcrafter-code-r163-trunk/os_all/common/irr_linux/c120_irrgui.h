#ifndef __scorch_C120_IRR_GUI__
#define __scorch_C120_IRR_GUI__

#include <aeffeditor.h>

#include <../../irrlicht/include/irrlicht.h>
#include <windows.h>
#include <iostream>
#include "../../vst_c120/params.h"

#ifdef BUILD_WITH_D3D9
#include <d3dx9.h>
#endif

#include "c120_gui_graphics.h"

using namespace irr;
using namespace core;
using namespace io;
using namespace gui;
using namespace scene;
using namespace video;

class SC_C120_Editor : public AEffEditor
{
public:
	SC_C120_Editor (void* effect);
	virtual ~SC_C120_Editor();

	virtual void setParameter (VstInt32 index, float value);
	virtual bool getRect (ERect** ppRect);
	virtual bool open (void* ptr);
	virtual void close();
	virtual void idle ();
	virtual void draw (); // (ERect* pRect);

	// wait (in ms)
	void wait(unsigned long ms);

	// get the current time (in ms)
	unsigned long getTicks ();

	// feedback to appli.
	virtual void doIdleStuff ();

	// get the effect attached to this editor
	AudioEffect* getEffect () { return effect; }

	// get version of this VSTGUI
	VstInt32 getVstGuiVersion () { return long (999); }

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
//	float param[kMaxNumParameters];

	IrrlichtDevice* Idevice;
	scene::ISceneManager* smgr;
	video::IVideoDriver* drv;
	gui::IGUIEnvironment* guienv;

	video::SExposedVideoData vData;

protected:
	HWND GuiWindow;
	HWND hIrrlichtWindow;

	HINSTANCE hInstance;
	char Win32ClassName[60];

private:
	bool inIdleStuff;
	VstInt32 knobMode;
	unsigned long long lLastTicks;

	ITexture *GuiBG[2];
	IGUIImage *BG;
	bool showingFront;

	ITexture *KnobTex;
	IGUIImageList *Knob;

	int knobPos[7];
};

#endif
