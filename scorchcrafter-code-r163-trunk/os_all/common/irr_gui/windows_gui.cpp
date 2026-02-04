#include "windows_gui.h"
#include <cstdio>

#define kIdleRate    100 // host idle rate in ms
#define kIdleRate2    50
#define kIdleRateMin   4 // minimum time between 2 idles in ms

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
	return new PlgEditor (effect);
}
//!------------------------------------------------------------------------------------
PlgEditor::PlgEditor (void* effect) :
	AEffEditor ((AudioEffectX*)effect),
	inIdleStuff (false)
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
PlgEditor::~PlgEditor()
{
	OleUninitialize();
}
#if VST_2_1_EXTENSIONS
bool PlgEditor::onKeyDown (VstKeyCode& keyCode)
{
	return false;
}
//!-----------------------------------------------------------------------------
bool PlgEditor::onKeyUp (VstKeyCode& keyCode)
{
	return false;
}
#endif
//!------------------------------------------------------------------------------------
void PlgEditor::setParameter(VstInt32 index, float value)
{
	return;
//	if(index < kMaxNumParameters) param[index] = value;
}
//!------------------------------------------------------------------------------------
bool PlgEditor::getRect(ERect** ppRect)
{
	*ppRect = &rect;

	return true;
}
//!------------------------------------------------------------------------------------
bool PlgEditor::open (void* ptr)
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

	InitGui();

	ShowWindow(hIrrlichtWindow, SW_SHOW);
	UpdateWindow(hIrrlichtWindow);

	return true;
}
//!------------------------------------------------------------------------------------
void PlgEditor::close()
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
void PlgEditor::idle()
{
	draw();

	if (inIdleStuff)
		return;

	AEffEditor::idle();
}
//!------------------------------------------------------------------------------------
void PlgEditor::draw()// (ERect* pRect)
{

	if (!Idevice) return;

	drv->beginScene(true, true, video::SColor(255,0,0,255), vData);

	RunGui();

	smgr->drawAll();
	guienv->drawAll();
	drv->endScene();

	Idevice->yield();
}
//!------------------------------------------------------------------------------------
bool PlgEditor::setKnobMode (VstInt32 val)
{
	knobMode = val;
	return true;
}
//!-----------------------------------------------------------------------------
bool PlgEditor::onWheel (float distance)
{
	return false;
}
//!-----------------------------------------------------------------------------
void PlgEditor::wait (unsigned long ms)
{
	Idevice->sleep(ms);
}
//!-----------------------------------------------------------------------------
unsigned long PlgEditor::getTicks()
{
	return (unsigned long) GetTickCount();
}
//!------------------------------------------------------------------------------------
void PlgEditor::doIdleStuff()
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
void PlgEditor::RunGui()
{
	return;
}
//!------------------------------------------------------------------------------------
void PlgEditor::InitGui()
{
	return;
}
//!------------------------------------------------------------------------------------
//!------------------------------------------------------------------------------------
