#include "tonestack_a.h"

ToneStack_A::ToneStack_A(fsx nRate) :
	zeroF(fhx (0.0000000000000000000000000001)),	//! Used to avoid denormal-related issues in parts of the code
	sRate(fhx (nRate)),
	setShaped(false),
	ovSampleRate(int (1)),
	rateFac(fhx (1.0)),
	fpPrec(int (2))
{
	if(fhx (sRate) < fhx (44100.0)) sRate = fhx (44100.0);
	if(fhx (sRate) > fhx (1536000.0)) sRate = fhx (1536000.0);

	bValue[0] = fsx (1.0);
	bValue[1] = fsx (0.50);
	bValue[2] = fsx (0.50);
	bValue[3] = fsx (0.50);
	bValue[4] = fsx (0.50);
	bValue[5] = fsx (0.50);

	SplEQBank = new SampleBank(fsx (sRate));
	SplEQBank->SetFPprecision(fpPrec);

	EqBand_Freq[0][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 30.0 / fhx (sRate*rateFac)));
	EqBand_Freq[1][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 300.0 / fhx (sRate*rateFac)));
	EqBand_Freq[2][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 500.0 / fhx (sRate*rateFac)));
	EqBand_Freq[3][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1000.0 / fhx (sRate*rateFac)));
	EqBand_Freq[4][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 2000.0 / fhx (sRate*rateFac)));

	EqBand_Freq[0][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 300.0 / fhx (sRate*rateFac)));
	EqBand_Freq[1][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 500.0 / fhx (sRate*rateFac)));
	EqBand_Freq[2][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1000.0 / fhx (sRate*rateFac)));
	EqBand_Freq[3][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 2000.0 / fhx (sRate*rateFac)));
	EqBand_Freq[4][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 5000.0 / fhx (sRate*rateFac)));

	for (int x = 0; x < 5; x++)
	{
		EqBand_Pos[x][0] = fhx (0.0);
		EqBand_Pos[x][1] = fhx (0.0);
	}
}
ToneStack_A::~ToneStack_A()
{
	delete SplEQBank;
}
void ToneStack_A::setNewSampleRate(fsx nRate)
{
	sRate = nRate;

	if(fhx (sRate) < fhx (44100.0)) sRate = fhx (44100.0);
	if(fhx (sRate) > fhx (1536000.0)) sRate = fhx (1536000.0);

	EqBand_Freq[0][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 30.0 / fhx (sRate*rateFac)));
	EqBand_Freq[1][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 300.0 / fhx (sRate*rateFac)));
	EqBand_Freq[2][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 500.0 / fhx (sRate*rateFac)));
	EqBand_Freq[3][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1000.0 / fhx (sRate*rateFac)));
	EqBand_Freq[4][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 2000.0 / fhx (sRate*rateFac)));

	EqBand_Freq[0][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 300.0 / fhx (sRate*rateFac)));
	EqBand_Freq[1][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 500.0 / fhx (sRate*rateFac)));
	EqBand_Freq[2][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1000.0 / fhx (sRate*rateFac)));
	EqBand_Freq[3][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 2000.0 / fhx (sRate*rateFac)));
	EqBand_Freq[4][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 5000.0 / fhx (sRate*rateFac)));

	SplEQBank->setSampleRate(fsx (sRate));
	ClearBuffs();
}
void ToneStack_A::setBandValue(const int bNum, const fsx value)
{
	if(bNum < 0) return;
	if(bNum > 5) return;

	bValue[bNum] = fsx (value);
}
void ToneStack_A::setEvenShaped(const bool isShaped)
{
	if(isShaped != setShaped) ClearBuffs();
	setShaped = isShaped;
}
void ToneStack_A::ClearBuffs()
{
	SplEQBank->zeroAllSamples();
	for (int x = 0; x < 5; x++)
	{
		EqBand_Pos[x][0] = fhx (0.0);
		EqBand_Pos[x][1] = fhx (0.0);
	}
}
void ToneStack_A::setOverSampling(int inp)
{
	if(inp < 0) inp = 0;
	if(inp > 6) inp = 6;

	if(inp == 0) rateFac = fhx (1.0);
	if(inp == 1) rateFac = fhx (1.0);
	if(inp == 2) rateFac = fhx (2.0);
	if(inp == 3) rateFac = fhx (4.0);
	if(inp == 4) rateFac = fhx (8.0);
	if(inp == 5) rateFac = fhx (16.0);
	if(inp == 6) rateFac = fhx (1.0);

	EqBand_Freq[0][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 30.0 / fhx (sRate*rateFac)));
	EqBand_Freq[1][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 300.0 / fhx (sRate*rateFac)));
	EqBand_Freq[2][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 500.0 / fhx (sRate*rateFac)));
	EqBand_Freq[3][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1000.0 / fhx (sRate*rateFac)));
	EqBand_Freq[4][0] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 2000.0 / fhx (sRate*rateFac)));

	EqBand_Freq[0][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 300.0 / fhx (sRate*rateFac)));
	EqBand_Freq[1][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 500.0 / fhx (sRate*rateFac)));
	EqBand_Freq[2][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 1000.0 / fhx (sRate*rateFac)));
	EqBand_Freq[3][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 2000.0 / fhx (sRate*rateFac)));
	EqBand_Freq[4][1] = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) M_PI * (fhx) 5000.0 / fhx (sRate*rateFac)));

	ovSampleRate = inp;

	SplEQBank->setOverSampling(inp);

	ClearBuffs();
}
void ToneStack_A::SetInput(const fhx inp)
{
	SplEQBank->smp[0] = fhx ((fhx) fminl((fhx) fmaxl(fhx (inp), fhx (-1.0)), fhx (1.0)));
}
void ToneStack_A::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
	SplEQBank->SetFPprecision(fpPrec);
}
void ToneStack_A::RunInternals()
{
	int effRate = ovSampleRate;
	if(effRate > 5) effRate = 1;

	SplEQBank->runUpSampling();

	RunEQ(0);
	if(effRate > 1) RunEQ(1);
	if(effRate > 2)
	{
		RunEQ(2);
		RunEQ(3);
	}
	if(effRate > 3)
	{
		RunEQ(4);
		RunEQ(5);
		RunEQ(6);
		RunEQ(7);
	}
	if(effRate > 4)
	{
		RunEQ(8);
		RunEQ(9);
		RunEQ(10);
		RunEQ(11);
		RunEQ(12);
		RunEQ(13);
		RunEQ(14);
		RunEQ(15);
	}

	SplEQBank->runDownSampling();
}
void ToneStack_A::RunEQ(const int spNum)
{
	if(fpPrec == 0)
	{
		subSpl[0] = flx (SplEQBank->smp[spNum]);
		subSpl[1] = flx (SplEQBank->smp[spNum]);
		subSpl[2] = flx (SplEQBank->smp[spNum]);
		subSpl[3] = flx (SplEQBank->smp[spNum]);
		subSpl[4] = flx (SplEQBank->smp[spNum]);

		for(int z = 0; z < 5; z++)
		{
			subSpl[z] -= flx (EqBand_Pos[z][0] += flx (flx ((flx) subSpl[z] - (flx) EqBand_Pos[z][0]) * flx (EqBand_Freq[z][0])));
			subSpl[z] = flx (EqBand_Pos[z][1] += flx (flx ((flx) subSpl[z] - (flx) EqBand_Pos[z][1]) * flx (EqBand_Freq[z][1])));

			subSpl[z] *= flx (flx (bValue[z+1]) - flx (0.50f));
		}

		SplEQBank->smp[spNum] += flx (subSpl[0] + subSpl[1] + subSpl[2] + subSpl[3] + subSpl[4]);
		SplEQBank->smp[spNum] *= flx (0.50f);
	}
	if(fpPrec == 1)
	{
		subSpl[0] = fsx (SplEQBank->smp[spNum]);
		subSpl[1] = fsx (SplEQBank->smp[spNum]);
		subSpl[2] = fsx (SplEQBank->smp[spNum]);
		subSpl[3] = fsx (SplEQBank->smp[spNum]);
		subSpl[4] = fsx (SplEQBank->smp[spNum]);

		for(int z = 0; z < 5; z++)
		{
			subSpl[z] -= fsx (EqBand_Pos[z][0] += fsx (fsx ((fsx) subSpl[z] - (fsx) EqBand_Pos[z][0]) * fsx (EqBand_Freq[z][0])));
			subSpl[z] = fsx (EqBand_Pos[z][1] += fsx (fsx ((fsx) subSpl[z] - (fsx) EqBand_Pos[z][1]) * fsx (EqBand_Freq[z][1])));

			subSpl[z] *= fsx (fsx (bValue[z+1]) - fsx (0.50));
		}

		SplEQBank->smp[spNum] += fsx (subSpl[0] + subSpl[1] + subSpl[2] + subSpl[3] + subSpl[4]);
		SplEQBank->smp[spNum] *= fsx (0.50);
	}
	if(fpPrec == 2)
	{
		subSpl[0] = fhx (SplEQBank->smp[spNum]);
		subSpl[1] = fhx (SplEQBank->smp[spNum]);
		subSpl[2] = fhx (SplEQBank->smp[spNum]);
		subSpl[3] = fhx (SplEQBank->smp[spNum]);
		subSpl[4] = fhx (SplEQBank->smp[spNum]);

		for(int z = 0; z < 5; z++)
		{
			subSpl[z] -= fhx (EqBand_Pos[z][0] += fhx (fhx ((fhx) subSpl[z] - (fhx) EqBand_Pos[z][0]) * fhx (EqBand_Freq[z][0])));
			subSpl[z] = fhx (EqBand_Pos[z][1] += fhx (fhx ((fhx) subSpl[z] - (fhx) EqBand_Pos[z][1]) * fhx (EqBand_Freq[z][1])));

			subSpl[z] *= fhx (fhx (bValue[z+1]) - fhx (0.50));
		}

		SplEQBank->smp[spNum] += fhx (subSpl[0] + subSpl[1] + subSpl[2] + subSpl[3] + subSpl[4]);
		SplEQBank->smp[spNum] *= fhx (0.50);
	}
}
