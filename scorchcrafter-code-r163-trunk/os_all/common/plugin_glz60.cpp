#include "plugin_glz60.h"
#include <algorithm>

using namespace ScMath;

SC_GLZ60_Amp_Head::SC_GLZ60_Amp_Head(double sRt) :
	fadeFac(fsx (SCr_C120_AmpHead_Fade_Factor)),
	smpRate(fsx (sRt)),
	splInputMono(fsx (0.0)),
	splInputLeft(fsx (0.0)),
	splInputRight(fsx (0.0)),
	splOutputMono(fsx (0.0)),
	splOutputLeft(fsx (0.0)),
	splOutputRight(fsx (0.0)),
	l_deNormSuppress(flx (0.20f)),
	s_deNormSuppress(fsx (0.20)),
	h_deNormSuppress(fhx (0.20)),
	l_dNrmThresh(flx (powf(10.f, -28.f))),
	s_dNrmThresh(fsx (pow(10.0, -32.0))),
	h_dNrmThresh(fhx (powl(10.0, -35.0))),
	l_PI(flx (SC_PI)),
	s_PI(fsx (SC_PI)),
	h_PI(fhx (SC_PI))
{
	FpParam[ScPrmF_Master_Volume] = flx (1.0f);
	FpParam[ScPrmF_Drive_Gain] = flx (0.50f);
	FpParam[ScPrmF_EQ_Low] = flx (0.50f);
	FpParam[ScPrmF_EQ_Mid] = flx (0.50f);
	FpParam[ScPrmF_EQ_High] = flx (0.50f);
	FpParam[ScPrmF_EQ_Contour] = flx (0.50f);
	FpParam[ScPrmF_EQ_Presence] = flx (0.50f);

	IntParam[ScPrmI_Active_And_Ready] = 0;
	IntParam[ScPrmI_Error_Status] = 0;
	IntParam[ScPrmI_Drive_Channel] = 2;
	IntParam[ScPrmI_FP_Precision_Bit_Depth] = SCv_Bit_Depth_64;
	IntParam[ScPrmI_OverSampling_Rate] = SCv_OverSampling_2x;
	IntParam[ScPrmI_Stereo] = 0;
	IntParam[ScPrmI_Phase_Control] = 0;

	fadeMax = long (fsx (fadeFac) * fsx (smpRate / 44100.0));
	fadeCount = fadeMax;

	PluGin[0] = new Tube_Bank_F(fsx (smpRate), fhx (SCr_C120_AmpHead_QSpatialFactor_Mono));
	PluGin[0]->InitTubes();
	PluGin[0]->SetFPprecision(1);
	PluGin[0]->setOverSampling(1);
	PluGin[0]->SetChannel(IntParam[ScPrmI_Drive_Channel]);
	PluGin[0]->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));
	PluGin[1] = new Tube_Bank_F(fsx (smpRate), fhx (SCr_C120_AmpHead_QSpatialFactor_Left));
	PluGin[1]->InitTubes();
	PluGin[1]->SetFPprecision(1);
	PluGin[1]->setOverSampling(1);
	PluGin[1]->SetChannel(IntParam[ScPrmI_Drive_Channel]);
	PluGin[1]->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));
	PluGin[2] = new Tube_Bank_F(fsx (smpRate), fhx (SCr_C120_AmpHead_QSpatialFactor_Right));
	PluGin[2]->InitTubes();
	PluGin[2]->SetFPprecision(1);
	PluGin[2]->setOverSampling(1);
	PluGin[2]->SetChannel(IntParam[ScPrmI_Drive_Channel]);
	PluGin[2]->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));

	Qstack[0] = new ToneStack_E(fsx (smpRate));
	Qstack[1] = new ToneStack_E(fsx (smpRate), SCr_C120_AmpHead_CnvPhase_Left, SCr_C120_AmpHead_CnvSkew_Left);
	Qstack[2] = new ToneStack_E(fsx (smpRate), SCr_C120_AmpHead_CnvPhase_Right, SCr_C120_AmpHead_CnvSkew_Right);
	Istack[0] = new ToneStack_F(fsx (smpRate));
	Istack[1] = new ToneStack_F(fsx (smpRate));
	Istack[2] = new ToneStack_F(fsx (smpRate));

	InpFilter[0] = new Cst_EQ();
	InpFilter[0]->SetSmpRate(smpRate);
	InpFilter[0]->SetBands(SCr_C120_AmpHead_InputQ_Low_Standard, SCr_C120_AmpHead_InputQ_Ceiling);
	InpFilter[1] = new Cst_EQ();
	InpFilter[1]->SetSmpRate(smpRate);
	InpFilter[1]->SetBands(SCr_C120_AmpHead_InputQ_Low_Standard, SCr_C120_AmpHead_InputQ_Ceiling);
	InpFilter[2] = new Cst_EQ();
	InpFilter[2]->SetSmpRate(smpRate);
	InpFilter[2]->SetBands(SCr_C120_AmpHead_InputQ_Low_Standard, SCr_C120_AmpHead_InputQ_Ceiling);

	InpFilterB[0] = new Cst_EQ();
	InpFilterB[0]->SetSmpRate(smpRate);
	InpFilterB[0]->SetBands(SCr_C120_AmpHead_InputQ_Low_Bright, SCr_C120_AmpHead_InputQ_Ceiling);
	InpFilterB[1] = new Cst_EQ();
	InpFilterB[1]->SetSmpRate(smpRate);
	InpFilterB[1]->SetBands(SCr_C120_AmpHead_InputQ_Low_Bright, SCr_C120_AmpHead_InputQ_Ceiling);
	InpFilterB[2] = new Cst_EQ();
	InpFilterB[2]->SetSmpRate(smpRate);
	InpFilterB[2]->SetBands(SCr_C120_AmpHead_InputQ_Low_Bright, SCr_C120_AmpHead_InputQ_Ceiling);

	IntParam[ScPrmI_Active_And_Ready] = 1;
}
SC_GLZ60_Amp_Head::~SC_GLZ60_Amp_Head()
{
	delete PluGin[0];
	delete Qstack[0];
	delete PluGin[1];
	delete Qstack[1];
	delete PluGin[2];
	delete Qstack[2];
	delete Istack[0];
	delete Istack[1];
	delete Istack[2];

	delete InpFilter[0];
	delete InpFilterB[0];
	delete InpFilter[1];
	delete InpFilterB[1];
	delete InpFilter[2];
	delete InpFilterB[2];
}
void SC_GLZ60_Amp_Head::SetNewSampleRate(const double inp)
{
	smpRate = fsx (inp);
	if(fsx (smpRate) < fsx (44100.0)) smpRate = fsx (44100.0);
	if(fsx (smpRate) > fsx (6144000.0)) smpRate = fsx (6144000.0);	// Maximum Sample Rate to avoid absurd, invalid values

	PluGin[0]->setSampleRate(fsx (smpRate));
	Qstack[0]->setNewSampleRate(fsx (smpRate));
	PluGin[1]->setSampleRate(fsx (smpRate));
	Qstack[1]->setNewSampleRate(fsx (smpRate));
	PluGin[2]->setSampleRate(fsx (smpRate));
	Qstack[2]->setNewSampleRate(fsx (smpRate));
	Istack[0]->setNewSampleRate(fsx (smpRate));
	Istack[1]->setNewSampleRate(fsx (smpRate));
	Istack[2]->setNewSampleRate(fsx (smpRate));

	fadeMax = long (fsx (fadeFac) * fsx (smpRate / 44100.0));
	fadeCount = fadeMax;

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);

	InpFilter[0]->SetSmpRate(smpRate);
	InpFilter[0]->SetBands(SCr_C120_AmpHead_InputQ_Low_Standard, SCr_C120_AmpHead_InputQ_Ceiling);
	InpFilterB[0]->SetSmpRate(smpRate);
	InpFilterB[0]->SetBands(SCr_C120_AmpHead_InputQ_Low_Bright, SCr_C120_AmpHead_InputQ_Ceiling);
	InpFilter[1]->SetSmpRate(smpRate);
	InpFilter[1]->SetBands(SCr_C120_AmpHead_InputQ_Low_Standard, SCr_C120_AmpHead_InputQ_Ceiling);
	InpFilterB[1]->SetSmpRate(smpRate);
	InpFilterB[1]->SetBands(SCr_C120_AmpHead_InputQ_Low_Bright, SCr_C120_AmpHead_InputQ_Ceiling);
	InpFilter[2]->SetSmpRate(smpRate);
	InpFilter[2]->SetBands(SCr_C120_AmpHead_InputQ_Low_Standard, SCr_C120_AmpHead_InputQ_Ceiling);
	InpFilterB[2]->SetSmpRate(smpRate);
	InpFilterB[2]->SetBands(SCr_C120_AmpHead_InputQ_Low_Bright, SCr_C120_AmpHead_InputQ_Ceiling);
}
void SC_GLZ60_Amp_Head::ClearAllBuffers()
{
	PluGin[0]->clearBuffs();
	Qstack[0]->ClearBuffs();
	PluGin[1]->clearBuffs();
	Qstack[1]->ClearBuffs();
	PluGin[2]->clearBuffs();
	Qstack[2]->ClearBuffs();
	Istack[0]->ClearBuffs();
	Istack[1]->ClearBuffs();
	Istack[2]->ClearBuffs();

	fadeCount = fadeMax;

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);
}
void SC_GLZ60_Amp_Head::SetFpParam(const int prm, float newValue)
{
	if(prm < 0) return;
	if(prm > ScPrmF_MAX_COUNT) return;
	if(flx (newValue) < flx (0.0f)) return;
	if(flx (newValue) > flx (1.0f)) return;

	if(prm == ScPrmF_Master_Volume) FpParam[prm] = flx (newValue * 2.0f);

	if(prm == ScPrmF_Input_Vol) FpParam[prm] = flx (newValue);

	if(prm == ScPrmF_Drive_Gain)
	{
		FpParam[prm] = flx (newValue);
		PluGin[0]->SetGain(fsx (newValue));
		PluGin[1]->SetGain(fsx (newValue));
		PluGin[2]->SetGain(fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Low)
	{
		FpParam[prm] = flx (newValue);
		Qstack[0]->setBandValue(0, fsx (newValue));
		Qstack[1]->setBandValue(0, fsx (newValue));
		Qstack[2]->setBandValue(0, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Mid)
	{
		FpParam[prm] = flx (newValue);
		Qstack[0]->setBandValue(1, fsx (newValue));
		Qstack[1]->setBandValue(1, fsx (newValue));
		Qstack[2]->setBandValue(1, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_High)
	{
		FpParam[prm] = flx (newValue);
		Qstack[0]->setBandValue(2, fsx (newValue));
		Qstack[1]->setBandValue(2, fsx (newValue));
		Qstack[2]->setBandValue(2, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Contour)
	{
		FpParam[prm] = flx (newValue);
		Qstack[0]->setBandValue(3, fsx (newValue));
		Qstack[1]->setBandValue(3, fsx (newValue));
		Qstack[2]->setBandValue(3, fsx (newValue));
		Qstack[0]->setBandValue(4, fsx (newValue));
		Qstack[1]->setBandValue(4, fsx (newValue));
		Qstack[2]->setBandValue(4, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Presence)
	{
		FpParam[prm] = flx (newValue);
		Qstack[0]->setBandValue(5, fsx (newValue));
		Qstack[1]->setBandValue(5, fsx (newValue));
		Qstack[2]->setBandValue(5, fsx (newValue));
	}
	if(prm == ScPrmF_PreEQ_Low)
	{
		FpParam[prm] = flx (newValue);
		Istack[0]->setBandValue(0, fsx (newValue));
		Istack[1]->setBandValue(0, fsx (newValue));
		Istack[2]->setBandValue(0, fsx (newValue));
	}
	if(prm == ScPrmF_PreEQ_Mid)
	{
		FpParam[prm] = flx (newValue);
		Istack[0]->setBandValue(1, fsx (newValue));
		Istack[1]->setBandValue(1, fsx (newValue));
		Istack[2]->setBandValue(1, fsx (newValue));
	}
	if(prm == ScPrmF_PreEQ_High)
	{
		FpParam[prm] = flx (newValue);
		Istack[0]->setBandValue(2, fsx (newValue));
		Istack[1]->setBandValue(2, fsx (newValue));
		Istack[2]->setBandValue(2, fsx (newValue));
	}
	if(prm == ScPrmF_PreEQ_Contour)
	{
		FpParam[prm] = flx (newValue);
		Istack[0]->setBandValue(3, fsx (newValue));
		Istack[1]->setBandValue(3, fsx (newValue));
		Istack[2]->setBandValue(3, fsx (newValue));
		Istack[0]->setBandValue(4, fsx (newValue));
		Istack[1]->setBandValue(4, fsx (newValue));
		Istack[2]->setBandValue(4, fsx (newValue));
	}
	if(prm == ScPrmF_PreEQ_Presence)
	{
		FpParam[prm] = flx (newValue);
		Istack[0]->setBandValue(5, fsx (newValue));
		Istack[1]->setBandValue(5, fsx (newValue));
		Istack[2]->setBandValue(5, fsx (newValue));
	}
}
void SC_GLZ60_Amp_Head::SetIntParam(const int prm, int newValue)
{
	if(prm < 0) return;
	if(prm > ScPrmI_MAX_COUNT) return;
	if(newValue < 0) return;

	if(prm == ScPrmI_Error_Status)
	{
		IntParam[ScPrmI_Error_Status] = 0;
		ClearAllBuffers();
	}
	if(prm == ScPrmI_Active_And_Ready)
	{
		if(newValue > 1) return;
		IntParam[prm] = newValue;
		ClearAllBuffers();
	}
	if(prm == ScPrmI_Drive_Channel)
	{
		if(newValue > 3) return;
		IntParam[prm] = 0;
		PluGin[0]->SetChannel(0);
		PluGin[1]->SetChannel(0);
		PluGin[2]->SetChannel(0);

		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_FP_Precision_Bit_Depth)
	{
		if(newValue >= SCv_Bit_Depth_MAX_COUNT) return;

		IntParam[prm] = newValue;
		if(newValue == SCv_Bit_Depth_32)
		{
			PluGin[0]->SetFPprecision(0);
			Qstack[0]->SetPrecision(0);
			PluGin[1]->SetFPprecision(0);
			Qstack[1]->SetPrecision(0);
			PluGin[2]->SetFPprecision(0);
			Qstack[2]->SetPrecision(0);
			Istack[0]->SetPrecision(0);
			Istack[1]->SetPrecision(0);
			Istack[2]->SetPrecision(0);
		}
		if(newValue == SCv_Bit_Depth_64)
		{
			PluGin[0]->SetFPprecision(1);
			Qstack[0]->SetPrecision(1);
			PluGin[1]->SetFPprecision(1);
			Qstack[1]->SetPrecision(1);
			PluGin[2]->SetFPprecision(1);
			Qstack[2]->SetPrecision(1);
			Istack[0]->SetPrecision(1);
			Istack[1]->SetPrecision(1);
			Istack[2]->SetPrecision(1);
		}
		if(newValue == SCv_Bit_Depth_128)
		{
			PluGin[0]->SetFPprecision(2);
			Qstack[0]->SetPrecision(2);
			PluGin[1]->SetFPprecision(2);
			Qstack[1]->SetPrecision(2);
			PluGin[2]->SetFPprecision(2);
			Qstack[2]->SetPrecision(2);
			Istack[0]->SetPrecision(2);
			Istack[1]->SetPrecision(2);
			Istack[2]->SetPrecision(2);
		}
		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_OverSampling_Rate)
	{
		if(newValue >= SCv_OverSampling_MAX_COUNT) return;
		IntParam[prm] = newValue;
		int x = 1;
		if(newValue == SCv_OverSampling_1x) x = 1;
		if(newValue == SCv_OverSampling_2x) x = 2;
		if(newValue == SCv_OverSampling_4x) x = 3;
		if(newValue == SCv_OverSampling_8x) x = 4;
		if(newValue == SCv_OverSampling_16x) x = 5;

		PluGin[0]->setOverSampling(x);
		PluGin[1]->setOverSampling(x);
		PluGin[2]->setOverSampling(x);

		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_Stereo)
	{
		if(newValue < 0) return;
		if(newValue > 1) return;
		IntParam[prm] = newValue;
	}
	if(prm == ScPrmI_Bright)
		IntParam[prm] = newValue;
	if(prm == ScPrmI_CabMic_Active)
	{
		if(newValue == 1)
		{
			Qstack[0]->setCabMic(true);
			Qstack[1]->setCabMic(true);
			Qstack[2]->setCabMic(true);
		} else
		{
			Qstack[0]->setCabMic(false);
			Qstack[1]->setCabMic(false);
			Qstack[2]->setCabMic(false);
		}
	}
	if(prm == ScPrmI_CabMic_Num)
	{
		Qstack[0]->SetCabMicNum(newValue);
		Qstack[1]->SetCabMicNum(newValue);
		Qstack[2]->SetCabMicNum(newValue);
	}
	if(prm == ScPrmI_TStack_Shape)
	{
		if(newValue == 1)
		{
			Qstack[0]->setEvenShaped(true);
			Qstack[1]->setEvenShaped(true);
			Qstack[2]->setEvenShaped(true);
		} else
		{
			Qstack[0]->setEvenShaped(false);
			Qstack[1]->setEvenShaped(false);
			Qstack[2]->setEvenShaped(false);
		}
	}
	if(prm == ScPrmI_TStack_ShapeNum)
	{
		Qstack[0]->SetAmpShapeNum(newValue);
		Qstack[1]->SetAmpShapeNum(newValue);
		Qstack[2]->SetAmpShapeNum(newValue);
	}
	if(prm == ScPrmI_Phase_Control)
		IntParam[prm] = newValue;
	if(prm == ScPrmI_TStack_Dynamic)
	{
		if(newValue == 1)
		{
			Qstack[0]->EnableDynamicQ(true);
			Qstack[1]->EnableDynamicQ(true);
			Qstack[2]->EnableDynamicQ(true);
		} else
		{
			Qstack[0]->EnableDynamicQ(false);
			Qstack[1]->EnableDynamicQ(false);
			Qstack[2]->EnableDynamicQ(false);
		}
	}
}
void SC_GLZ60_Amp_Head::InputSample(const double inp)
{
	splInputMono = fsx (inp);
}
void SC_GLZ60_Amp_Head::InputSampleStereo(const double inpL, const double inpR)
{
	splInputMono = fsx (fsx (fsx (inpL) + fsx (inpR)) * fsx (0.50));
	splInputLeft = inpL;
	splInputRight = inpR;
}
double SC_GLZ60_Amp_Head::GetMonoOut()
{
	return fsx (splOutputMono * 2.0);
}
double SC_GLZ60_Amp_Head::GetStereoOutLeft()
{
	return fsx (splOutputLeft * 2.0);
}
double SC_GLZ60_Amp_Head::GetStereoOutRight()
{
	return fsx (splOutputRight * 2.0);
}
float SC_GLZ60_Amp_Head::GetFpParam(const int prm)
{
	if(prm < 0) return float (0.0f);

	if(prm == ScPrmF_Master_Volume) return float (FpParam[prm]);
	if(prm == ScPrmF_Drive_Gain) return float (FpParam[prm]);
	if(prm == ScPrmF_EQ_Low) return float (FpParam[prm]);
	if(prm == ScPrmF_EQ_Mid) return float (FpParam[prm]);
	if(prm == ScPrmF_EQ_High) return float (FpParam[prm]);
	if(prm == ScPrmF_EQ_Contour) return float (FpParam[prm]);

	return float (0.0f);
}
int SC_GLZ60_Amp_Head::GetIntParam(const int prm)
{
	if(prm < 0) return 0;

	if(prm == ScPrmI_Active_And_Ready) return IntParam[prm];
	if(prm == ScPrmI_Error_Status) return IntParam[prm];
	if(prm == ScPrmI_Drive_Channel) return IntParam[prm];
	if(prm == ScPrmI_FP_Precision_Bit_Depth) return IntParam[prm];
	if(prm == ScPrmI_OverSampling_Rate) return IntParam[prm];

	return 0;
}
long long SC_GLZ60_Amp_Head::GetLongParam(const int prm)
{
	return 0;
}
void SC_GLZ60_Amp_Head::RunPlugin()
{
//! ----------------------------------------------------------------------------------------------------------------
//! 32-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_32)
	{
		l_dNrm = flx (l_dNrmThresh * flx (0.1f) * (flx) sinf(l_PI * l_deNormSuppress));
		l_deNormSuppress += flx (0.50f);
		if (l_deNormSuppress > flx (4.0f)) l_deNormSuppress -= flx (2.0f);

		if((IntParam[ScPrmI_Active_And_Ready] == 0) || (IntParam[ScPrmI_OverSampling_Rate] == SCv_OverSampling_OFF))
		{
			fadeCount = fadeMax;
			splOutputMono = fsx (l_dNrm);
			splOutputLeft = fsx (l_dNrm);
			splOutputRight = fsx (l_dNrm);
			return;
		}

		if(IntParam[ScPrmI_Phase_Control] == 0) l_smp[0] = flx (splInputMono);
		else l_smp[0] = flx (-splInputMono);

		l_smp[0] += flx (l_dNrm);
		l_smp[1] = flx (splInputLeft + l_dNrm);
		l_smp[2] = flx (splInputRight + l_dNrm);

		PluGin[0]->SetAmpVt(l_smp[0]);
		PluGin[1]->SetAmpVt(l_smp[1]);
		PluGin[2]->SetAmpVt(l_smp[2]);
		Qstack[0]->SetDynamicQ(l_smp[0]);
		Qstack[1]->SetDynamicQ(l_smp[1]);
		Qstack[2]->SetDynamicQ(l_smp[2]);

		if(IntParam[ScPrmI_Stereo] == 0)
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[0]->SplInL(l_smp[0]);
				InpFilterB[0]->RunQL();
				l_smp[0] = (flx) InpFilterB[0]->GetOutputL();
			} else
			{
				InpFilter[0]->SplInL(l_smp[0]);
				InpFilter[0]->RunQL();
				l_smp[0] = (flx) InpFilter[0]->GetOutputL();
			}
			l_smp[0] *= flx (FpParam[ScPrmF_Input_Vol] * 0.50f);
			Istack[0]->SetInputL(l_smp[0]);
			Istack[0]->RunEQ();
			l_smp[0] = Istack[0]->GetProcessedOutputL();

			PluGin[0]->InputSmp(l_smp[0]);
			PluGin[0]->runUpSampling();
			PluGin[0]->ApplyDistortion();
			PluGin[0]->runDownSampling();

			l_smp[0] = (flx) PluGin[0]->GetOutPL();
			if((l_smp[0] < l_dNrmThresh) && (l_smp[0] > -l_dNrmThresh)) l_smp[0] += l_dNrm;

			Qstack[0]->SetInput(l_smp[0]);
			Qstack[0]->RunConv();
			Qstack[0]->RunEQ();
			l_smp[0] = (flx) Qstack[0]->GetProcessedOutputL();

			l_smp[0] *= flx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (l_smp[0]);
			splOutputLeft = fsx (l_smp[0]);
			splOutputRight = fsx (l_smp[0]);
		} else
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[1]->SplInL(l_smp[1]);
				InpFilterB[2]->SplInL(l_smp[2]);
				InpFilterB[1]->RunQL();
				InpFilterB[2]->RunQL();
				l_smp[1] = (flx) InpFilterB[1]->GetOutputL();
				l_smp[2] = (flx) InpFilterB[2]->GetOutputL();
			} else
			{
				InpFilter[1]->SplInL(l_smp[1]);
				InpFilter[2]->SplInL(l_smp[2]);
				InpFilter[1]->RunQL();
				InpFilter[2]->RunQL();
				l_smp[1] = (flx) InpFilter[1]->GetOutputL();
				l_smp[2] = (flx) InpFilter[2]->GetOutputL();
			}
			l_smp[1] *= flx (FpParam[ScPrmF_Input_Vol] * 0.50f);
			Istack[1]->SetInputL(l_smp[1]);
			Istack[1]->RunEQ();
			l_smp[1] = Istack[1]->GetProcessedOutputL();
			l_smp[2] *= flx (FpParam[ScPrmF_Input_Vol] * 0.50f);
			Istack[2]->SetInputL(l_smp[2]);
			Istack[2]->RunEQ();
			l_smp[2] = Istack[2]->GetProcessedOutputL();

			PluGin[1]->InputSmp(l_smp[1]);
			PluGin[1]->runUpSampling();
			PluGin[1]->ApplyDistortion();
			PluGin[1]->runDownSampling();

			PluGin[2]->InputSmp(l_smp[2]);
			PluGin[2]->runUpSampling();
			PluGin[2]->ApplyDistortion();
			PluGin[2]->runDownSampling();

			l_smp[1] = (flx) PluGin[1]->GetOutPL();
			l_smp[2] = (flx) PluGin[2]->GetOutPL();
			if((l_smp[1] < l_dNrmThresh) && (l_smp[1] > -l_dNrmThresh)) l_smp[1] += l_dNrm;
			if((l_smp[2] < l_dNrmThresh) && (l_smp[2] > -l_dNrmThresh)) l_smp[2] += l_dNrm;

			Qstack[1]->SetInput(l_smp[1]);
			Qstack[1]->RunConv();
			Qstack[1]->RunEQ();
			l_smp[1] = (flx) Qstack[1]->GetProcessedOutputL();

			Qstack[2]->SetInput(l_smp[2]);
			Qstack[2]->RunConv();
			Qstack[2]->RunEQ();
			l_smp[2] = (flx) Qstack[2]->GetProcessedOutputL();

			l_smp[1] *= flx (FpParam[ScPrmF_Master_Volume]);
			l_smp[2] *= flx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (l_smp[1]);
			splOutputLeft = fsx (l_smp[1]);
			splOutputRight = fsx (l_smp[2]);
		}

		if(fadeCount > 0)
		{
			fadeCount--;
			splOutputMono = fsx (0.0);
			splOutputLeft = fsx (0.0);
			splOutputRight = fsx (0.0);
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 64-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_64)
	{
		s_dNrm = fsx (s_dNrmThresh * fsx (0.10) * (fsx) sin(s_PI * s_deNormSuppress));
		s_deNormSuppress += fsx (0.50);
		if (s_deNormSuppress > fsx (4.0)) s_deNormSuppress -= fsx (2.0);

		if((IntParam[ScPrmI_Active_And_Ready] == 0) || (IntParam[ScPrmI_OverSampling_Rate] == SCv_OverSampling_OFF))
		{
			fadeCount = fadeMax;
			splOutputMono = fsx (s_dNrm);
			splOutputLeft = fsx (s_dNrm);
			splOutputRight = fsx (s_dNrm);
			return;
		}

		if(IntParam[ScPrmI_Phase_Control] == 0) s_smp[0] = fsx (splInputMono);
		else s_smp[0] = fsx (-splInputMono);

		s_smp[0] += fsx (s_dNrm);
		s_smp[1] = fsx (splInputLeft + s_dNrm);
		s_smp[2] = fsx (splInputRight + s_dNrm);

		PluGin[0]->SetAmpVt(s_smp[0]);
		PluGin[1]->SetAmpVt(s_smp[1]);
		PluGin[2]->SetAmpVt(s_smp[2]);
		Qstack[0]->SetDynamicQ(s_smp[0]);
		Qstack[1]->SetDynamicQ(s_smp[1]);
		Qstack[2]->SetDynamicQ(s_smp[2]);

		if(IntParam[ScPrmI_Stereo] == 0)
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[0]->SplInS(s_smp[0]);
				InpFilterB[0]->RunQS();
				s_smp[0] = (fsx) InpFilterB[0]->GetOutputS();
			} else
			{
				InpFilter[0]->SplInS(s_smp[0]);
				InpFilter[0]->RunQS();
				s_smp[0] = (fsx) InpFilter[0]->GetOutputS();
			}
			s_smp[0] *= fsx (FpParam[ScPrmF_Input_Vol] * 0.50);
			Istack[0]->SetInputS(s_smp[0]);
			Istack[0]->RunEQ();
			s_smp[0] = Istack[0]->GetProcessedOutputS();

			PluGin[0]->InputSmp(s_smp[0]);
			PluGin[0]->runUpSampling();
			PluGin[0]->ApplyDistortion();
			PluGin[0]->runDownSampling();

			s_smp[0] = (fsx) PluGin[0]->GetOutPS();

			if((s_smp[0] < s_dNrmThresh) && (s_smp[0] > -s_dNrmThresh)) s_smp[0] += s_dNrm;

			Qstack[0]->SetInput(s_smp[0]);
			Qstack[0]->RunConv();
			Qstack[0]->RunEQ();
			s_smp[0] = (fsx) Qstack[0]->GetProcessedOutputS();

			s_smp[0] *= fsx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (s_smp[0]);
			splOutputLeft = fsx (s_smp[0]);
			splOutputRight = fsx (s_smp[0]);
		} else
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[1]->SplInS(s_smp[1]);
				InpFilterB[2]->SplInS(s_smp[2]);
				InpFilterB[1]->RunQS();
				InpFilterB[2]->RunQS();
				s_smp[1] = (fsx) InpFilterB[1]->GetOutputS();
				s_smp[2] = (fsx) InpFilterB[2]->GetOutputS();
			} else
			{
				InpFilter[1]->SplInS(s_smp[1]);
				InpFilter[2]->SplInS(s_smp[2]);
				InpFilter[1]->RunQS();
				InpFilter[2]->RunQS();
				s_smp[1] = (fsx) InpFilter[1]->GetOutputS();
				s_smp[2] = (fsx) InpFilter[2]->GetOutputS();
			}
			s_smp[1] *= fsx (FpParam[ScPrmF_Input_Vol] * 0.50);
			Istack[1]->SetInputS(s_smp[1]);
			Istack[1]->RunEQ();
			s_smp[1] = Istack[1]->GetProcessedOutputS();
			s_smp[2] *= fsx (FpParam[ScPrmF_Input_Vol] * 0.50);
			Istack[2]->SetInputS(s_smp[2]);
			Istack[2]->RunEQ();
			s_smp[2] = Istack[2]->GetProcessedOutputS();

			PluGin[1]->InputSmp(s_smp[1]);
			PluGin[1]->runUpSampling();
			PluGin[1]->ApplyDistortion();
			PluGin[1]->runDownSampling();

			PluGin[2]->InputSmp(s_smp[2]);
			PluGin[2]->runUpSampling();
			PluGin[2]->ApplyDistortion();
			PluGin[2]->runDownSampling();

			s_smp[1] = (fsx) PluGin[1]->GetOutPS();
			s_smp[2] = (fsx) PluGin[2]->GetOutPS();
			if((s_smp[1] < s_dNrmThresh) && (s_smp[1] > -s_dNrmThresh)) s_smp[1] += s_dNrm;
			if((s_smp[2] < s_dNrmThresh) && (s_smp[2] > -s_dNrmThresh)) s_smp[2] += s_dNrm;

			Qstack[1]->SetInput(s_smp[1]);
			Qstack[1]->RunConv();
			Qstack[1]->RunEQ();
			s_smp[1] = (fsx) Qstack[1]->GetProcessedOutputS();

			Qstack[2]->SetInput(s_smp[2]);
			Qstack[2]->RunConv();
			Qstack[2]->RunEQ();
			s_smp[2] = (fsx) Qstack[2]->GetProcessedOutputS();

			s_smp[1] *= fsx (FpParam[ScPrmF_Master_Volume]);
			s_smp[2] *= fsx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (s_smp[1]);
			splOutputLeft = fsx (s_smp[1]);
			splOutputRight = fsx (s_smp[2]);
		}

		if(fadeCount > 0)
		{
			fadeCount--;
			splOutputMono = fsx (0.0);
			splOutputLeft = fsx (0.0);
			splOutputRight = fsx (0.0);
		}
	} else
//! ----------------------------------------------------------------------------------------------------------------
//! 128-bit Processing
//! ----------------------------------------------------------------------------------------------------------------
	if(IntParam[ScPrmI_FP_Precision_Bit_Depth] == SCv_Bit_Depth_128)
	{
		h_dNrm = fhx (h_dNrmThresh * fhx (0.10) * (fhx) sinl(h_PI * h_deNormSuppress));
		h_deNormSuppress += fhx (0.50);
		if (h_deNormSuppress > fhx (4.0)) h_deNormSuppress -= fhx (2.0);

		if((IntParam[ScPrmI_Active_And_Ready] == 0) || (IntParam[ScPrmI_OverSampling_Rate] == SCv_OverSampling_OFF))
		{
			fadeCount = fadeMax;
			splOutputMono = fsx (h_dNrm);
			splOutputLeft = fsx (h_dNrm);
			splOutputRight = fsx (h_dNrm);
			return;
		}

		if(IntParam[ScPrmI_Phase_Control] == 0) h_smp[0] = fhx (splInputMono);
		else h_smp[0] = fhx (-splInputMono);

		h_smp[0] += fhx (h_dNrm);
		h_smp[1] = fhx (splInputLeft + h_dNrm);
		h_smp[2] = fhx (splInputRight + h_dNrm);

		PluGin[0]->SetAmpVt(h_smp[0]);
		PluGin[1]->SetAmpVt(h_smp[1]);
		PluGin[2]->SetAmpVt(h_smp[2]);
		Qstack[0]->SetDynamicQ(h_smp[0]);
		Qstack[1]->SetDynamicQ(h_smp[1]);
		Qstack[2]->SetDynamicQ(h_smp[2]);

		if(IntParam[ScPrmI_Stereo] == 0)
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[0]->SplInH(h_smp[0]);
				InpFilterB[0]->RunQH();
				h_smp[0] = (fhx) InpFilterB[0]->GetOutputH();
			} else
			{
				InpFilter[0]->SplInH(h_smp[0]);
				InpFilter[0]->RunQH();
				h_smp[0] = (fhx) InpFilter[0]->GetOutputH();
			}
			h_smp[0] *= fhx (FpParam[ScPrmF_Input_Vol] * 0.50);
			Istack[0]->SetInputH(h_smp[0]);
			Istack[0]->RunEQ();
			h_smp[0] = Istack[0]->GetProcessedOutputH();

			PluGin[0]->InputSmp(h_smp[0]);
			PluGin[0]->runUpSampling();
			PluGin[0]->ApplyDistortion();
			PluGin[0]->runDownSampling();

			h_smp[0] = (fhx) PluGin[0]->GetOutPH();
			if((h_smp[0] < h_dNrmThresh) && (h_smp[0] > -h_dNrmThresh)) h_smp[0] += h_dNrm;

			Qstack[0]->SetInput(h_smp[0]);
			Qstack[0]->RunConv();
			Qstack[0]->RunEQ();
			h_smp[0] = (fhx) Qstack[0]->GetProcessedOutputH();

			h_smp[0] *= fhx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (h_smp[0]);
			splOutputLeft = fsx (h_smp[0]);
			splOutputRight = fsx (h_smp[0]);
		} else
		{
			if(IntParam[ScPrmI_Bright] == 1)
			{
				InpFilterB[1]->SplInH(h_smp[1]);
				InpFilterB[2]->SplInH(h_smp[2]);
				InpFilterB[1]->RunQH();
				InpFilterB[2]->RunQH();
				h_smp[1] = (fhx) InpFilterB[1]->GetOutputH();
				h_smp[2] = (fhx) InpFilterB[2]->GetOutputH();
			} else
			{
				InpFilter[1]->SplInH(h_smp[1]);
				InpFilter[2]->SplInH(h_smp[2]);
				InpFilter[1]->RunQH();
				InpFilter[2]->RunQH();
				h_smp[1] = (fhx) InpFilter[1]->GetOutputH();
				h_smp[2] = (fhx) InpFilter[2]->GetOutputH();
			}
			h_smp[1] *= fhx (FpParam[ScPrmF_Input_Vol] * 0.50);
			Istack[1]->SetInputH(h_smp[1]);
			Istack[1]->RunEQ();
			h_smp[1] = Istack[1]->GetProcessedOutputH();
			h_smp[2] *= fhx (FpParam[ScPrmF_Input_Vol] * 0.50);
			Istack[2]->SetInputH(h_smp[2]);
			Istack[2]->RunEQ();
			h_smp[2] = Istack[2]->GetProcessedOutputH();

			PluGin[1]->InputSmp(h_smp[1]);
			PluGin[1]->runUpSampling();
			PluGin[1]->ApplyDistortion();
			PluGin[1]->runDownSampling();

			PluGin[2]->InputSmp(h_smp[2]);
			PluGin[2]->runUpSampling();
			PluGin[2]->ApplyDistortion();
			PluGin[2]->runDownSampling();

			h_smp[1] = (fhx) PluGin[1]->GetOutPH();
			h_smp[2] = (fhx) PluGin[2]->GetOutPH();
			if((h_smp[1] < h_dNrmThresh) && (h_smp[1] > -h_dNrmThresh)) h_smp[1] += h_dNrm;
			if((h_smp[2] < h_dNrmThresh) && (h_smp[2] > -h_dNrmThresh)) h_smp[2] += h_dNrm;

			Qstack[1]->SetInput(h_smp[1]);
			Qstack[1]->RunConv();
			Qstack[1]->RunEQ();
			h_smp[1] = (fhx) Qstack[1]->GetProcessedOutputH();

			Qstack[2]->SetInput(h_smp[2]);
			Qstack[2]->RunConv();
			Qstack[2]->RunEQ();
			h_smp[2] = (fhx) Qstack[2]->GetProcessedOutputH();

			h_smp[1] *= fhx (FpParam[ScPrmF_Master_Volume]);
			h_smp[2] *= fhx (FpParam[ScPrmF_Master_Volume]);
			splOutputMono = fsx (h_smp[1]);
			splOutputLeft = fsx (h_smp[1]);
			splOutputRight = fsx (h_smp[2]);
		}

		if(fadeCount > 0)
		{
			fadeCount--;
			splOutputMono = fsx (0.0);
			splOutputLeft = fsx (0.0);
			splOutputRight = fsx (0.0);
		}
	}
}
