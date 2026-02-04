#include "../common/plugin_c120.h"

SC_C120_Amp_Head::SC_C120_Amp_Head(double sRt) :
fadeFac(fsx (192.0)),
smpRate(fsx (sRt)),
splInputMono(fsx (0.0)),
splInputLeft(fsx (0.0)),
splInputRight(fsx (0.0)),
splOutputMono(fsx (0.0)),
splOutputLeft(fsx (0.0)),
splOutputRight(fsx (0.0))
{
	FpParam[ScPrmF_Master_Volume] = flx (0.50f);
	FpParam[ScPrmF_Drive_Gain] = flx (0.50f);
	FpParam[ScPrmF_EQ_Low] = flx (0.50f);
	FpParam[ScPrmF_EQ_Mid] = flx (0.50f);
	FpParam[ScPrmF_EQ_High] = flx (0.50f);
	FpParam[ScPrmF_EQ_Contour] = flx (0.00f);
	FpParam[ScPrmF_EQ_Presence] = flx (0.00f);

	IntParam[ScPrmI_Active_And_Ready] = 0;
	IntParam[ScPrmI_Error_Status] = 0;
	IntParam[ScPrmI_Drive_Channel] = 3;
	IntParam[ScPrmI_FP_Precision_Bit_Depth] = SC_Bit_Depth_64;
	IntParam[ScPrmI_OverSampling_Rate] = SC_OverSampling_1x;

	fadeMax = long (fsx (fadeFac) * fsx (smpRate / 44100.0));
	fadeCount = fadeMax;

	PluGin = new Tube_BankA(fsx (smpRate));
	PluGin->InitTubes();
	PluGin->SetFPprecision(1);
	PluGin->setOverSampling(1);
	PluGin->SetChannel(IntParam[ScPrmI_Drive_Channel]);
	PluGin->SetGain(fsx (FpParam[ScPrmF_Drive_Gain]));

	Qstack = new ToneStack_A(fsx (smpRate));

	inputF_Frq = fsx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 430.0 / fhx (smpRate)));
	inputF_Pos = fsx (0.0);
}
SC_C120_Amp_Head::~SC_C120_Amp_Head()
{
	delete PluGin;
	delete Qstack;
}
void SC_C120_Amp_Head::SetNewSampleRate(const double inp)
{
	smpRate = fsx (inp);
	if(fsx (smpRate) < fsx (44100.0)) smpRate = fsx (44100.0);
	if(fsx (smpRate) > fsx (6144000.0)) smpRate = fsx (6144000.0);	// Maximum Sample Rate to avoid absurd, invalid values

	PluGin->setSampleRate(fsx (smpRate));
	Qstack->setNewSampleRate(fsx (smpRate));

	inputF_Frq = fsx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 430.0 / fhx (smpRate)));

	fadeMax = long (fsx (fadeFac) * fsx (smpRate / 44100.0));
	fadeCount = fadeMax;

	inputF_Pos = fsx (0.0);

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);
}
void SC_C120_Amp_Head::ClearAllBuffers()
{
	PluGin->clearBuffs();
	Qstack->ClearBuffs();

	fadeCount = fadeMax;

	splInputMono = fsx (0.0);
	splInputLeft = fsx (0.0);
	splInputRight = fsx (0.0);
	splOutputMono = fsx (0.0);
	splOutputLeft = fsx (0.0);
	splOutputRight = fsx (0.0);
}
void SC_C120_Amp_Head::SetFpParam(const int prm, float newValue)
{
	if(prm < 0) return;
	if(prm > ScPrmF_MAX_COUNT) return;
	if(flx (newValue) < flx (0.0f)) return;
	if(flx (newValue) > flx (1.0f)) return;

	if(prm == ScPrmF_Master_Volume) FpParam[prm] = flx (newValue);
	if(prm == ScPrmF_Drive_Gain)
	{
		FpParam[prm] = flx (newValue);
		PluGin->SetGain(fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Low)
	{
		FpParam[prm] = flx (newValue);
		Qstack->setBandValue(0, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Mid)
	{
		FpParam[prm] = flx (newValue);
		Qstack->setBandValue(1, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_High)
	{
		FpParam[prm] = flx (newValue);
		Qstack->setBandValue(2, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Contour)
	{
		FpParam[prm] = flx (newValue);
		Qstack->setBandValue(3, fsx (newValue));
		Qstack->setBandValue(4, fsx (newValue));
	}
	if(prm == ScPrmF_EQ_Presence)
	{
		FpParam[prm] = flx (newValue);
		Qstack->setBandValue(5, fsx (newValue));
	}
}
void SC_C120_Amp_Head::SetIntParam(const int prm, int newValue)
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
		IntParam[prm] = newValue;
		PluGin->SetChannel(newValue);
		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_FP_Precision_Bit_Depth)
	{
		if(newValue >= SC_Bit_Depth_MAX_COUNT) return;
		IntParam[prm] = newValue;
		if(newValue == SC_Bit_Depth_32)
		{
			PluGin->SetFPprecision(0);
			Qstack->SetPrecision(0);
		}
		if(newValue == SC_Bit_Depth_64)
		{
			PluGin->SetFPprecision(1);
			Qstack->SetPrecision(1);
		}
		if(newValue == SC_Bit_Depth_128)
		{
			PluGin->SetFPprecision(2);
			Qstack->SetPrecision(2);
		}
		fadeCount = fadeMax;
	}
	if(prm == ScPrmI_OverSampling_Rate)
	{
		if(newValue >= SC_OverSampling_MAX_COUNT) return;
		IntParam[prm] = newValue;
		if(newValue == SC_OverSampling_1x) PluGin->setOverSampling(1);
		if(newValue == SC_OverSampling_2x) PluGin->setOverSampling(2);
		if(newValue == SC_OverSampling_4x) PluGin->setOverSampling(3);
		if(newValue == SC_OverSampling_8x) PluGin->setOverSampling(4);
		if(newValue == SC_OverSampling_16x) PluGin->setOverSampling(5);
		fadeCount = fadeMax;
	}
}
void SC_C120_Amp_Head::InputSample(const double inp)
{
	splInputMono = fsx (inp);
}
void SC_C120_Amp_Head::InputSampleStereo(const double inpL, const double inpR)
{
	splInputMono = fsx (fsx (fsx (inpL) + fsx (inpR)) * fsx (0.50));
}
double SC_C120_Amp_Head::GetMonoOut()
{
	return fsx (splOutputMono);
}
double SC_C120_Amp_Head::GetStereoOutLeft()
{
	return fsx (splOutputMono);
}
double SC_C120_Amp_Head::GetStereoOutRight()
{
	return fsx (splOutputMono);
}
float SC_C120_Amp_Head::GetFpParam(const int prm)
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
int SC_C120_Amp_Head::GetIntParam(const int prm)
{
	if(prm < 0) return 0;

	if(prm == ScPrmI_Active_And_Ready) return IntParam[prm];
	if(prm == ScPrmI_Error_Status) return IntParam[prm];
	if(prm == ScPrmI_Drive_Channel) return IntParam[prm];
	if(prm == ScPrmI_FP_Precision_Bit_Depth) return IntParam[prm];
	if(prm == ScPrmI_OverSampling_Rate) return IntParam[prm];

	return 0;
}
long long SC_C120_Amp_Head::GetLongParam(const int prm)
{
	return 0;
}
void SC_C120_Amp_Head::RunPlugin()
{
	if(IntParam[ScPrmI_Active_And_Ready] = 0)
	{
		fadeCount = fadeMax;
		splOutputMono = fsx (0.0);
		return;
	}

	splInputMono -= fsx (inputF_Pos += fsx (fsx ((fsx) splInputMono - (fsx) inputF_Pos) * fsx (inputF_Frq)));

	PluGin->InputSmp(fhx (splInputMono));
	PluGin->runUpSampling();
	PluGin->ApplyDistortion();
	PluGin->runDownSampling();
	splOutputMono = (fsx) PluGin->GetOutP();

	Qstack->SetInput(fsx (splOutputMono));
	Qstack->RunEQ();
	splOutputMono = (fsx) Qstack->GetProcessedOutput();

	splOutputMono *= fsx (FpParam[ScPrmF_Master_Volume]);

	if(fadeCount > 0)
	{
		fadeCount--;
		splOutputMono = fsx (0.0);
	}
}
