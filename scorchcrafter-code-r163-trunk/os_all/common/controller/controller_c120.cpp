#include "controller_c120.h"

SC_Controller_C120::SC_Controller_C120(const bool smoothControls) :
controlSmooth(smoothControls),
displayNeedsUpdate(false),
isOnline(true)
{
	for (int x = 0; x < kMaxNumParameters_C120; x++)
	{
		kSmoother[x] = new Cst_EQ(true);
		kSmoother[x]->SetBands(1.0, 5.0);
		kSmoother[x]->SetSmpRate(30.0);

		params_cur[x] = 0.f;
		params_old[x] = 0.f;
		params_set[x] = 0.f;
	}
}
SC_Controller_C120::~SC_Controller_C120()
{
	for (int x = 0; x < kMaxNumParameters_C120; x++)
		delete kSmoother[x];
}
void SC_Controller_C120::SetControlSmoothRate(const fsx inp)
{
	for (int x = 0; x < kMaxNumParameters_C120; x++)
		kSmoother[x]->SetBands(2.0, fsx (inp));

	ClearBuffers();
}
void SC_Controller_C120::SetControlCheckRate(const fsx inp)
{
	for (int x = 0; x < kMaxNumParameters_C120; x++)
		kSmoother[x]->SetSmpRate(fsx (inp));

	ClearBuffers();
}
flx SC_Controller_C120::GetParam(const int index)
{
	if (index < kMaxNumParameters_C120)
		return params_set[index];
	else
		return 0.f;
}
void SC_Controller_C120::SetParam(const int index, const flx val)
{
	if (index >= kMaxNumParameters_C120) return;

	params_set[index] = val;
	if (!controlSmooth) params_cur[index] = val;
}
void SC_Controller_C120::SetParamHard(const int index, const flx val)
{
	if (index >= kMaxNumParameters_C120) return;

	params_set[index] = val;
	params_old[index] = val;
	params_cur[index] = val;
	kSmoother[index]->l_ForceBufs(val);
}
void SC_Controller_C120::UpdatePlugin(SC_C120_Amp_Head **Plg)
{
	if (!*Plg) return;

	bool ampChanUpdate = false;

	for (int x = 0; x < kMaxNumParameters_C120; x++)
	{
		if (params_old[x] == params_cur[x]) continue;

		params_old[x] = params_cur[x];

		if(x == kMasterVol) &*Plg->SetFpParam(ScPrmF_Master_Volume, params_cur[x]);
		if(x == kGainControl) &*Plg->SetFpParam(ScPrmF_Drive_Gain, params_cur[x]);
		if(x == kEQstackLow) &*Plg->SetFpParam(ScPrmF_EQ_Low, params_cur[x]);
		if(x == kEQstackMid) &*Plg->SetFpParam(ScPrmF_EQ_Mid, params_cur[x]);
		if(x == kEQstackHigh) &*Plg->SetFpParam(ScPrmF_EQ_High, params_cur[x]);
		if(x == kEQstackContour) &*Plg->SetFpParam(ScPrmF_EQ_Contour, params_cur[x]);
		if(x == kEQstackPres) &*Plg->SetFpParam(ScPrmF_EQ_Presence, params_cur[x]);

		if(x > kEQstackPres) displayNeedsUpdate = true;

		if (x == kBtnBoost)
			ampChanUpdate = true;

		if (x == kBtnBright)
		{
			if (params_cur[x] >= 0.5f) &*Plg->SetIntParam(ScPrmI_Bright, 1);
			else &*Plg->SetIntParam(ScPrmI_Bright, 0);
		}
		if (x == kInternalCabOff)
		{
			if (params_cur[x] >= 0.5f) &*Plg->SetIntParam(ScPrmI_CabMic_Active, 1);
			else &*Plg->SetIntParam(ScPrmI_CabMic_Active, 0);
		}
		if (x == kToneStFlatEven)
		{
			if (params_cur[x] >= 0.5f) &*Plg->SetIntParam(ScPrmI_TStack_Shape, 1);
			else &*Plg->SetIntParam(ScPrmI_TStack_Shape, 0);
		}
		if (x == kDynamicEnabled)
		{
			if (params_cur[x] >= 0.5f) &*Plg->SetIntParam(ScPrmI_TStack_Dynamic, 1);
			else &*Plg->SetIntParam(ScPrmI_TStack_Dynamic, 0);
		}
		if (x == kMonoStereoControl)
		{
			if (params_cur[x] >= 0.5f) &*Plg->SetIntParam(ScPrmI_Stereo, 1);
			else &*Plg->SetIntParam(ScPrmI_Stereo, 0);
		}

		if (params_cur[x] >= 0.95f)
		{
			if ((x >= kGuiPanelButtonA) && (x <= kGuiPanelButtonC))
				ForceParamRangeExcludingX(kGuiPanelButtonA, kGuiPanelButtonC, x, 0.f);

			if ((x >= kToneStShapedA) && (x <= kToneStShapedF))
			{
				ForceParamRangeExcludingX(kToneStShapedA, kToneStShapedF, x, 0.f);
				&*Plg->SetIntParam(ScPrmI_TStack_ShapeNum, x-kToneStShapedA);
			}

			if ((x >= kInternalCabA) && (x <= kInternalCabF))
			{
				ForceParamRangeExcludingX(kInternalCabA, kInternalCabF, x, 0.f);
				&*Plg->SetIntParam(ScPrmI_CabMic_Num, x-kInternalCabA);
			}

			if ((x >= kSmpRenderOn1x) && (x <= kSmpRenderOnOFF))
			{
				ForceParamRangeExcludingX(kSmpRenderOn1x, kSmpRenderOnOFF, x, 0.f);
				overSmp_On = x;
			}

			if ((x >= kSmpRenderOff1x) && (x <= kSmpRenderOffOFF))
			{
				ForceParamRangeExcludingX(kSmpRenderOff1x, kSmpRenderOffOFF, x, 0.f);
				overSmp_Off = x;
			}

			if ((x >= kSmpBitOn32) && (x <= kSmpBitOn128))
			{
				ForceParamRangeExcludingX(kSmpBitOn32, kSmpBitOn128, x, 0.f);
				bitDepth_On = x;
			}

			if ((x >= kSmpBitOff32) && (x <= kSmpBitOff128))
			{
				ForceParamRangeExcludingX(kSmpBitOff32, kSmpBitOff128, x, 0.f);
				bitDepth_off = x;
			}

			if (x == kBtnClean)
			{
				ForceParam(kBtnHeavy, 0.f);
				ampChanUpdate = true;
			}
			if (x == kBtnHeavy)
			{
				ForceParam(kBtnClean, 0.f);
				ampChanUpdate = true;
			}

			if (x == kTStackBeforePower)
			{
				ForceParam(kTStackAfterPower, 0.f);
				&*Plg->SetIntParam(ScPrmI_TStack_Before_Power, 1);
			}

			if (x == kTStackAfterPower)
			{
				ForceParam(kTStackBeforePower, 0.f);
				&*Plg->SetIntParam(ScPrmI_TStack_Before_Power, 0);
			}
		}
	}

	if (ampChanUpdate)
	{
		int nChan = 0;
		if (params_cur[kBtnHeavy] >= 0.5f) nChan += 2;
		if (params_cur[kBtnBoost] >= 0.5f) nChan += 1;
		&*Plg->SetIntParam(ScPrmI_Drive_Channel, nChan);
	}
}
void SC_Controller_C120::TickControls()
{
	if (!controlSmooth) return;

	for (int x = 0; x < kMaxNumParameters_C120; x++)
	{
		kSmoother[x]->l_InputSmpl(params_set[x]);
		kSmoother[x]->l_Run(true, false);

		const flx conDiff = (flx) ScMath::l_abs(flx (params_cur[x] - params_set[x]));
		if (conDiff > 0.010f)
			params_cur[x] = (flx) kSmoother->l_GetOutput();
		else
			params_cur[x] = params_set[x];
	}
}
void SC_Controller_C120::ResetParams()
{
	for (int x = 0; x < kMaxNumParameters_C120; x++)
		params_set[x] = 0.f;

	params_set[kGainControl]=flx (0.50f);
	params_set[kMasterVol]=flx (0.50f);
	params_set[kEQstackLow]=flx (0.50f);
	params_set[kEQstackMid]=flx (0.50f);
	params_set[kEQstackHigh]=flx (0.50f);
	params_set[kEQstackContour]=flx (0.50f);
	params_set[kEQstackPres]=flx (0.50f);
	params_set[kBtnHeavy] = flx (1.0f);
	params_set[kBtnBoost] = flx (1.0f);

	params_set[kSmpBitOn64] = flx (1.0f);
	params_set[kSmpBitOff128] = flx (1.0f);
	params_set[kSmpRenderOn2x] = flx (1.0f);
	params_set[kSmpRenderOff16x] = flx (1.0f);
	params_set[kToneStShapedA] = flx (1.0f);
	params_set[kToneStFlatEven] = flx (1.0f);
	params_set[kInternalCabA] = flx (1.0f);
	params_set[kDynamicEnabled] = flx (1.0f);

	params_set[kGuiPanelButtonA] = flx (1.0f);

	ClearBuffers();
}
void SC_Controller_C120::ClearBuffers()
{
	for (int x = 0; x < kMaxNumParameters_C120; x++)
	{
		kSmoother[x]->l_ForceBufs(params_set[x]);
		kSmoother[x]->ClearBuffers();

		params_cur[x] = params_set[x];
	}
}
void SC_Controller_C120::ForceParam(const int index, const flx val)
{
	SetParamHard(index, val);
}
void SC_Controller_C120::ForceParamRange(const int indexA, const int indexB, const flx val)
{
	if (indexA > indexB) return;

	for (int x = indexA; x <= indexB; x++)
		SetParamHard(x, val);
}
void SC_Controller_C120::ForceParamRangeExcludingX(const int indexA, const int indexB, const int indexX, const flx val)
{
	if (indexA > indexB) return;

	for (int x = indexA; x <= indexB; x++)
	{
		if (x != indexX)
			SetParamHard(x, val);
	}
}
