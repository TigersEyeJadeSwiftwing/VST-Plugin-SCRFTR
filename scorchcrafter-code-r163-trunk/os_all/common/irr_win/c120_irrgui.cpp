#include "c120_irrgui.h"
#include <cstdio>
// #include "../c120_gui_graphics.h"
#include "../../embedded/sc_c120_front.h"
#include "../../embedded/Cknob.h"

#define kIdleRate    100 // host idle rate in ms
#define kIdleRate2    50
#define kIdleRateMin   4 // minimum time between 2 idles in ms

using namespace irr;
using namespace core;
using namespace io;
using namespace gui;
using namespace scene;
using namespace video;

static bool MBD = false;

static struct tagMSG windowsMessage;

static LRESULT CALLBACK CustomWndProc(HWND hIrrlichtWindow, UINT message,
		WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
		{
			MBD = true;
		}
		break;
	case WM_LBUTTONUP:
		{
			MBD = false;
		}
		break;
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

AEffEditor* createEditor (AudioEffectX* effect)
{
	return new SC_C120_Editor (effect);
}

//!------------------------------------------------------------------------------------
SC_C120_Editor::SC_C120_Editor(void* SCeffect) :
AEffEditor ((AudioEffectX*)SCeffect),
inIdleStuff (false),
guiWide(700),
guiTall(300),
showingFront(true),
mButtonDown(false),
knobFocused(false),
curFocusedKnob(0),
knobCanBeFocused(false)
{
	((AudioEffect*)SCeffect)->setEditor(this);
	systemWindow = 0;
	lLastTicks   = getTicks ();

	OleInitialize (0);

	if (!guiWide) guiWide = 700;
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
	if(!hIrrlichtWindow) return;
	if(!GuiWindow) return;
	if(index >= kMaxNumParameters) return;

	if(!Idevice) return;

	if ((index <= kEQstackPres) && (index >= kGainControl))
	{
		knobPos[index] = int (float (gParams[index] * 50.f));
		knobPos[index] = std::min(50, std::max(0, knobPos[index]));
		if (Knob[index] && KnobTex[knobPos[index]])
			Knob[index]->setImage(KnobTex[knobPos[index]]);
	}

	RunIRRGui();
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

	evRec = new GEventReceiver();
	prm.EventReceiver = evRec;

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

		Idevice = 0;
	}
	DestroyWindow(hIrrlichtWindow);
}
//!------------------------------------------------------------------------------------
void SC_C120_Editor::idle()
{
//	draw();

	if (inIdleStuff)
		return;

	AEffEditor::idle();
}
//!------------------------------------------------------------------------------------
void SC_C120_Editor::draw(ERect* pRect)// (ERect* pRect)
{
	if(!hIrrlichtWindow) return;
	if(!GuiWindow) return;
	if (!Idevice) return;

	drv->beginScene(true, false, video::SColor(255,64,32,128), vData);

	RunInput();
	RunIRRGui();

	smgr->drawAll();
	guienv->drawAll();

//	RunIRRGui();

	drv->endScene();

//	Idevice->yield();
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
//	Idevice->sleep(ms);
	Sleep(ms);
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
	curFocusedKnob = 0;
	mButtonDown = false;

	drv->setTextureCreationFlag(ETCF_ALLOW_NON_POWER_2);

	drv->setTextureCreationFlag(ETCF_ALWAYS_32_BIT);

	io::IReadFile *BG_front = Idevice->getFileSystem()->createMemoryReadFile(sc_panel_front_png, sizeof(sc_panel_front_png), "panel_front", false);
	GuiBG[0] = drv->getTexture(BG_front);
	BG_front->drop();

//	drv->setTextureCreationFlag(ETCF_ALWAYS_32_BIT);
	io::IFileSystem* fs=Idevice->getFileSystem();
	io::IReadFile *Knobfile = fs->createMemoryReadFile(CKnob_png, sizeof(CKnob_png), "pnKnob", false);
	IImage *MainTex = drv->createImageFromFile(Knobfile);
	Knobfile->drop();
	core::stringc Tname=core::stringc("pnKnob");
//	drv->setTextureCreationFlag(ETCF_ALWAYS_16_BIT);
	for (int x = 0; x < 51; x++)
	{
		IImage *SubTex = drv->createImage(MainTex, core::position2d<s32>(0, x * 64), core::dimension2d<u32>(64, 64));
//		c8* imageData = new c8[SubTex->getImageDataSizeInBytes()];
//		io::IWriteFile* memWriteFile = fs->createMemoryWriteFile(imageData,SubTex->getImageDataSizeInBytes(),"tempFile.png");
//		driver->writeImageToFile(SubTex,memWriteFile);
//		io::IReadFile* memReadFile= fs->createMemoryReadFile(imageData,heightMapImage->getImageDataSizeInBytes(),"tempFile2.bmp");
		KnobTex[x] = drv->addTexture(Tname.c_str(), SubTex);
		SubTex->drop();
	}
	MainTex->drop();
//	KnobTex[0] = drv->getTexture(Knobfile);

	BG = guienv->addImage(GuiBG[0], core::position2d<s32>(0,0), true);

	Knob[0] = guienv->addImage(KnobTex[0], core::position2d<s32>(27, 197), true, BG);
	Knob[1] = guienv->addImage(KnobTex[0], core::position2d<s32>(127, 197), true, BG);
	Knob[2] = guienv->addImage(KnobTex[0], core::position2d<s32>(407, 197), true, BG);
	Knob[3] = guienv->addImage(KnobTex[0], core::position2d<s32>(507, 197), true, BG);
	Knob[4] = guienv->addImage(KnobTex[0], core::position2d<s32>(607, 197), true, BG);
	Knob[5] = guienv->addImage(KnobTex[0], core::position2d<s32>(507, 77), true, BG);
	Knob[6] = guienv->addImage(KnobTex[0], core::position2d<s32>(607, 77), true, BG);

	knobCoor[0] = Knob[0]->getRelativePosition();
	knobCoor[1] = Knob[1]->getRelativePosition();
	knobCoor[2] = Knob[2]->getRelativePosition();
	knobCoor[3] = Knob[3]->getRelativePosition();
	knobCoor[4] = Knob[4]->getRelativePosition();
	knobCoor[5] = Knob[5]->getRelativePosition();
	knobCoor[6] = Knob[6]->getRelativePosition();

	drv->setTextureCreationFlag(ETCF_ALWAYS_32_BIT);

//	BG = guienv->addImage(core::rect<s32>(0, 0, guiWide, guiTall));
//	BG->setImage(GuiBG[0]);

	// showingFront = true;

//	io::IReadFile *knobF = Idevice->getFileSystem()->createMemoryReadFile(D_knob3_png, sizeof(D_knob3_png), "cntrl_knob");
//	KnobTex = drv->getTexture(knobF);
//	Knob = guienv->createImageList(KnobTex, core::dimension2d<s32>(100, 100), false);
/*
	knobPos[0] = int(float(((AudioEffectX*)effect)->getParameter(kGainControl) * 100.f));
	knobPos[1] = int(float(((AudioEffectX*)effect)->getParameter(kMasterVol) * 100.f));
	knobPos[2] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackLow) * 100.f));
	knobPos[3] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackMid) * 100.f));
	knobPos[4] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackHigh) * 100.f));
	knobPos[5] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackContour) * 100.f));
	knobPos[6] = int(float(((AudioEffectX*)effect)->getParameter(kEQstackPres) * 100.f));
*/
	RunIRRGui();

	mCntrl = Idevice->getCursorControl();
	lastMousePos = mCntrl->getPosition();
}
void SC_C120_Editor::RunIRRGui()
{
	for (int x = 0; x < kMaxNumParameters; x++)
		gParams[x] = ((AudioEffectX*)effect)->getParameter(x);

	if (showingFront == true)
	{
		BG->setImage(GuiBG[0]);

		knobPos[0] = int(float(gParams[kGainControl] * 50.f));
		knobPos[1] = int(float(gParams[kMasterVol] * 50.f));
		knobPos[2] = int(float(gParams[kEQstackLow] * 50.f));
		knobPos[3] = int(float(gParams[kEQstackMid] * 50.f));
		knobPos[4] = int(float(gParams[kEQstackHigh] * 50.f));
		knobPos[5] = int(float(gParams[kEQstackContour] * 50.f));
		knobPos[6] = int(float(gParams[kEQstackPres] * 50.f));

		knobPos[0] = std::min(50, std::max(0, knobPos[0]));
		knobPos[1] = std::min(50, std::max(0, knobPos[1]));
		knobPos[2] = std::min(50, std::max(0, knobPos[2]));
		knobPos[3] = std::min(50, std::max(0, knobPos[3]));
		knobPos[4] = std::min(50, std::max(0, knobPos[4]));
		knobPos[5] = std::min(50, std::max(0, knobPos[5]));
		knobPos[6] = std::min(50, std::max(0, knobPos[6]));

		Knob[0]->setVisible(true);
		Knob[1]->setVisible(true);
		Knob[2]->setVisible(true);
		Knob[3]->setVisible(true);
		Knob[4]->setVisible(true);
		Knob[5]->setVisible(true);
		Knob[6]->setVisible(true);

		Knob[0]->setImage(KnobTex[knobPos[0]]);
		Knob[1]->setImage(KnobTex[knobPos[1]]);
		Knob[2]->setImage(KnobTex[knobPos[2]]);
		Knob[3]->setImage(KnobTex[knobPos[3]]);
		Knob[4]->setImage(KnobTex[knobPos[4]]);
		Knob[5]->setImage(KnobTex[knobPos[5]]);
		Knob[6]->setImage(KnobTex[knobPos[6]]);
	} else
	{
		BG->setImage(GuiBG[0]);

		Knob[0]->setVisible(false);
		Knob[1]->setVisible(false);
		Knob[2]->setVisible(false);
		Knob[3]->setVisible(false);
		Knob[4]->setVisible(false);
		Knob[5]->setVisible(false);
		Knob[6]->setVisible(false);
	}
}
void SC_C120_Editor::setMouseButton(bool dwn)
{
	if(dwn != mButtonDown)
	{
		if(!dwn)
		{
			knobFocused = false;
		} else
		{
			if (knobCanBeFocused)
			{
				knobFocused = true;
			}
		}
	}

	mButtonDown = dwn;
}
void SC_C120_Editor::RunInput()
{
	if (!Idevice) return;
	if (!mCntrl) return;

	if(GetActiveWindow() == GuiWindow)
	{
		SetFocus(GuiWindow);
		EnableWindow(hIrrlichtWindow, true);
	}

//	if (MBD) setMouseButton(true);
//	if (!MBD) setMouseButton(false);

//	if(MBD) ((AudioEffectX*)effect)->setParameterAutomated(kGainControl, 0.1f);
/*
	MSG msg;
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message== WM_LBUTTONDOWN)
				setMouseButton(true);
			if (msg.message == WM_LBUTTONUP)
				setMouseButton(false);
		}
	}
*/
	const core::position2di newMPos = mCntrl->getPosition();
	if (knobFocused)
	{
		const int mpChange = (newMPos.X - lastMousePos.X) + (lastMousePos.Y - newMPos.Y);
		float nVal = gParams[curFocusedKnob] + float (float (mpChange) * IRR_GUI_KNOB_ADJ_SPEED);
		nVal = fminf(1.0f, fmaxf(0.f, nVal));
		if (showingFront)
		{
			if ((curFocusedKnob <= kEQstackPres) && (curFocusedKnob >= kGainControl))
			{
				((AudioEffectX*)effect)->setParameterAutomated(curFocusedKnob, nVal);
			}
		}
	} else
	{
		knobCanBeFocused = false;
		for (int x = 0; x < 7; x++)
		{
			knobCoor[x] = Knob[x]->getRelativePosition();
			const position2di cntr = knobCoor[x].getCenter();
			knobCoor[x].UpperLeftCorner = position2di(cntr.X - 45, cntr.Y - 45);
			knobCoor[x].LowerRightCorner = position2di(cntr.X + 45, cntr.Y + 45);
			if (knobCoor[x].isPointInside(newMPos))
			{
				knobCanBeFocused = true;
				curFocusedKnob = x;
			}
		}
	}
	lastMousePos = newMPos;

	if (evRec->IsLMouseDown() == true) setMouseButton(true);
	if (evRec->IsLMouseDown() == false) setMouseButton(false);
}

