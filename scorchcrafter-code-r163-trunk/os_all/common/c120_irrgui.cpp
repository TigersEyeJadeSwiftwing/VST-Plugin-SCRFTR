#include "c120_irrgui.h"
#include <cstdio>

#define kIdleRate    100 // host idle rate in ms
#define kIdleRate2    50
#define kIdleRateMin   4 // minimum time between 2 idles in ms

using namespace irr;
using namespace core;
using namespace io;
using namespace gui;
using namespace scene;
using namespace video;

static struct tagMSG windowsMessage;

static LRESULT CALLBACK CustomWndProc(HWND hIrrlichtWindow, UINT message,
		WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			HWND hwndCtl = (HWND)lParam;
			int code = HIWORD(wParam);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hIrrlichtWindow, message, wParam, lParam);
}

using namespace irr;

AEffEditor* createEditor (AudioEffectX* effect)
{
	return new SC_C120_Editor (effect);
}
//!------------------------------------------------------------------------------------

SC_C120_Editor::SC_C120_Editor(void* effect) :
AEffEditor ((AudioEffectX*)effect),
inIdleStuff (false),
guiWide(700),
guiTall(300),
showingFront(true)
{
	((AudioEffect*)effect)->setEditor(this);

	OleInitialize (0);

	if (!guiWide) guiWide = 600;
	if (!guiTall) guiTall = 300;

	rect.left = 0;
	rect.top = 0;
	rect.right = guiWide;
	rect.bottom = guiTall;
}
//!------------------------------------------------------------------------------------
SC_C120_Editor::~SC_C120_Editor()
{
	OleUninitialize();
}
#if VST_2_1_EXTENSIONS
bool SC_C120_Editor::onKeyDown (VstKeyCode& keyCode)
{
	return false;
}
//!-----------------------------------------------------------------------------
bool SC_C120_Editor::onKeyUp (VstKeyCode& keyCode)
{
	return false;
}
#endif
//!------------------------------------------------------------------------------------
void SC_C120_Editor::setParameter(VstInt32 index, float value)
{
	return;
//	if(index < kMaxNumParameters) param[index] = value;
}
//!------------------------------------------------------------------------------------
bool SC_C120_Editor::getRect(ERect** ppRect)
{
	*ppRect = &rect;

	return true;
}
//!------------------------------------------------------------------------------------
bool SC_C120_Editor::open (void* ptr)
{
	AEffEditor::open (ptr);

	hInstance = (HINSTANCE)GetModuleHandle(0);

	GuiWindow = reinterpret_cast<HWND>(&systemWindow);

	int xyz = 60;
	GetClassName(reinterpret_cast<HWND>(systemWindow), Win32ClassName, xyz);

	WNDCLASSEX wc;
	wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hIcon = NULL;
    wc.hInstance = hInstance;
    wc.lpfnWndProc = (WNDPROC)CustomWndProc;
    wc.lpszClassName = TEXT("IrrWd");
    wc.lpszMenuName = 0;
    wc.style = CS_HREDRAW | CS_VREDRAW;

    RegisterClassEx(&wc);

	hIrrlichtWindow = CreateWindow(Win32ClassName, "",
			WS_CHILD | WS_VISIBLE, // | BS_OWNERDRAW,
			rect.left, rect.top, rect.right, rect.bottom, reinterpret_cast<HWND>(systemWindow), NULL, hInstance, NULL);

	SIrrlichtCreationParameters prm;
	prm.DriverType = video::EDT_SOFTWARE;
#ifdef BUILD_WITH_D3D9
	prm.DriverType = video::EDT_DIRECT3D9;
#endif
#ifdef BUILD_WITH_OPENGL
	prm.DriverType = video::EDT_OPENGL;
#endif
#ifdef BUILD_WITH_SOFTWARE
	prm.DriverType = video::EDT_SOFTWARE;
#endif
	prm.WindowId = reinterpret_cast<void*>(hIrrlichtWindow);

	Idevice = irr::createDeviceEx(prm);
	smgr = Idevice->getSceneManager();
	drv = Idevice->getVideoDriver();
	guienv = Idevice->getGUIEnvironment();

	if (prm.DriverType == video::EDT_OPENGL)
	{
		HDC HDc=GetDC(hIrrlichtWindow);
		PIXELFORMATDESCRIPTOR pfd={0};
		pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
		int pf = GetPixelFormat(HDc);
		DescribePixelFormat(HDc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		pfd.dwFlags |= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.cDepthBits=16;
		pf = ChoosePixelFormat(HDc, &pfd);
		SetPixelFormat(HDc, pf, &pfd);
		vData.OpenGLWin32.HDc = HDc;
		vData.OpenGLWin32.HRc=wglCreateContext(HDc);
		wglShareLists((HGLRC)drv->getExposedVideoData().OpenGLWin32.HRc, (HGLRC)vData.OpenGLWin32.HRc);
	}

	InitIRRGui();

	ShowWindow(hIrrlichtWindow, SW_SHOW);
	UpdateWindow(hIrrlichtWindow);

	return true;
}
//!------------------------------------------------------------------------------------
void SC_C120_Editor::close()
{
	if (Idevice)
	{
		smgr->clear();
		guienv->clear();

		Idevice->closeDevice();
		Idevice->drop();
	}
	DestroyWindow(hIrrlichtWindow);
}
//!------------------------------------------------------------------------------------
void SC_C120_Editor::idle()
{
	draw();

	if (inIdleStuff)
		return;

	AEffEditor::idle();
}
//!------------------------------------------------------------------------------------
void SC_C120_Editor::draw()// (ERect* pRect)
{

	if (!Idevice) return;

	drv->beginScene(true, true, video::SColor(255,64,32,128), vData);

	RunIRRGui();

	smgr->drawAll();
//	guienv->drawAll();

	RunIRRGui();

	drv->endScene();

	Idevice->yield();
}
//!------------------------------------------------------------------------------------
bool SC_C120_Editor::setKnobMode (VstInt32 val)
{
	knobMode = val;
	return true;
}
//!-----------------------------------------------------------------------------
bool SC_C120_Editor::onWheel (float distance)
{
	return false;
}
//!-----------------------------------------------------------------------------
void SC_C120_Editor::wait (unsigned long ms)
{
	Idevice->sleep(ms);
}
//!-----------------------------------------------------------------------------
unsigned long SC_C120_Editor::getTicks()
{
	return (unsigned long) GetTickCount();
}
//!------------------------------------------------------------------------------------
void SC_C120_Editor::doIdleStuff()
{
	// get the current time
	unsigned long currentTicks = getTicks ();

	if (currentTicks < lLastTicks)
	{
		wait (kIdleRateMin);
		currentTicks += kIdleRateMin;
		if (currentTicks < lLastTicks - kIdleRate2)
			return;
	}

	if (PeekMessage (&windowsMessage, NULL, WM_PAINT, WM_PAINT, PM_REMOVE))
		DispatchMessage (&windowsMessage);

	// save the next time
 	lLastTicks = currentTicks + kIdleRate;

	inIdleStuff = true;

	if (effect)
		effect->masterIdle ();

	inIdleStuff = false;
}
//!------------------------------------------------------------------------------------
void SC_C120_Editor::InitIRRGui()
{
	io::IReadFile *BG_front = Idevice->getFileSystem()->createMemoryReadFile(sc_panel_front_png, sizeof(sc_panel_front_png), "panel_front");
	GuiBG[0] = drv->getTexture(BG_front);

	BG = guienv->addImage(core::rect<s32>(0, 0, guiWide, guiTall));
	BG->setImage(GuiBG[0]);

	showingFront = true;

	io::IReadFile *knobF = Idevice->getFileSystem()->createMemoryReadFile(D_knob3_png, sizeof(D_knob3_png), "cntrl_knob");
	KnobTex = drv->getTexture(knobF);
	Knob = guienv->createImageList(KnobTex, core::dimension2d<s32>(100, 100), true);

	knobPos[0] = int(float(((AudioEffectX*)effect)->getParameter(kGainControl) * 100.f));
	knobPos[1] = int(float(((AudioEffectX*)effect)->getParameter(kMasterVol) * 100.f));
	knobPos[2] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackLow) * 100.f));
	knobPos[3] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackMid) * 100.f));
	knobPos[4] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackHigh) * 100.f));
	knobPos[5] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackContour) * 100.f));
	knobPos[6] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackPres) * 100.f));

	RunIRRGui();
}
void SC_C120_Editor::RunIRRGui()
{
	if (showingFront == true)
	{
		knobPos[0] = int(float(((AudioEffectX*)effect)->getParameter(kGainControl) * 100.f));
		knobPos[1] = int(float(((AudioEffectX*)effect)->getParameter(kMasterVol) * 100.f));
		knobPos[2] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackLow) * 100.f));
		knobPos[3] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackMid) * 100.f));
		knobPos[4] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackHigh) * 100.f));
		knobPos[5] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackContour) * 100.f));
		knobPos[6] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackPres) * 100.f));

		BG->draw();

		Knob->draw(knobPos[0], core::position2d<s32>(15,185));
		Knob->draw(knobPos[1], core::position2d<s32>(115,185));
		Knob->draw(knobPos[2], core::position2d<s32>(395,185));
		Knob->draw(knobPos[3], core::position2d<s32>(495,185));
		Knob->draw(knobPos[4], core::position2d<s32>(595,185));
		Knob->draw(knobPos[5], core::position2d<s32>(495,65));
		Knob->draw(knobPos[6], core::position2d<s32>(595,65));
	}
}

