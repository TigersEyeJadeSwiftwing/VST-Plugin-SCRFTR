#include "sample_bank_custom_b.h"

using namespace ScMath;

SampleBank_Cst_B::SampleBank_Cst_B(fsx iSRate, const int firSize) :
	sRateH(fhx (iSRate)),
	sRateS(fsx (iSRate)),
	sRateL(flx (iSRate)),
	cRateH(fhx (iSRate)),
	cRateS(fsx (iSRate)),
	cRateL(flx (iSRate)),
	osRate(int (2)),
	precisionLevel(int (1))
{
	rfV = new RateFC();

	UpFilter[0] = new FIR_LP_EQ(sRateS, sRateS*s_C_half, firSize);
	UpFilter[1] = new FIR_LP_EQ(sRateS*2.0, sRateS*s_C_half, firSize);
	UpFilter[2] = new FIR_LP_EQ(sRateS*4.0, sRateS*s_C_half, firSize);
	UpFilter[3] = new FIR_LP_EQ(sRateS*8.0, sRateS*s_C_half, firSize);
	UpFilter[4] = new FIR_LP_EQ(sRateS*16.0, sRateS*s_C_half, firSize);

	DnFilter[0] = new FIR_LP_EQ(sRateS, sRateS*s_C_half, firSize);
	DnFilter[1] = new FIR_LP_EQ(sRateS*2.0, sRateS*s_C_half, firSize);
	DnFilter[2] = new FIR_LP_EQ(sRateS*4.0, sRateS*s_C_half, firSize);
	DnFilter[3] = new FIR_LP_EQ(sRateS*8.0, sRateS*s_C_half, firSize);
	DnFilter[4] = new FIR_LP_EQ(sRateS*16.0, sRateS*s_C_half, firSize);

	zeroAllSamples();
}
SampleBank_Cst_B::~SampleBank_Cst_B()
{
	for (int x = 0; x < 5; x++)
	{
		delete UpFilter[x];
		delete DnFilter[x];
	}

	delete rfV;
}
void SampleBank_Cst_B::InputSmp(const fsx inp)
{
	if(precisionLevel == 0) smpl[0] = flx (inp);
	if(precisionLevel == 1) smps[0] = fsx (inp);
	if(precisionLevel == 2) smph[0] = fhx (inp);
}
void SampleBank_Cst_B::setSampleRate(fsx nSRate)
{
	sRateL = flx (nSRate);
	sRateS = fsx (nSRate);
	sRateH = fhx (nSRate);

	cRateL = flx (rfV->vl[osRate] * flx (sRateL));
	cRateS = fsx (rfV->vs[osRate] * fsx (sRateS));
	cRateH = fhx (rfV->vh[osRate] * fhx (sRateH));

	UpFilter[0]->ChangeRates(sRateS, sRateS*s_C_half);
	UpFilter[1]->ChangeRates(sRateS*2.0, sRateS*s_C_half);
	UpFilter[2]->ChangeRates(sRateS*4.0, sRateS*s_C_half);
	UpFilter[3]->ChangeRates(sRateS*8.0, sRateS*s_C_half);
	UpFilter[4]->ChangeRates(sRateS*16.0, sRateS*s_C_half);

	DnFilter[0]->ChangeRates(sRateS, sRateS*s_C_half);
	DnFilter[1]->ChangeRates(sRateS*2.0, sRateS*s_C_half);
	DnFilter[2]->ChangeRates(sRateS*4.0, sRateS*s_C_half);
	DnFilter[3]->ChangeRates(sRateS*8.0, sRateS*s_C_half);
	DnFilter[4]->ChangeRates(sRateS*16.0, sRateS*s_C_half);

	zeroAllSamples();
}
void SampleBank_Cst_B::zeroAllSamples()
{
	for (int x = 0; x < 16; x++)
	{
		smpl[x] = l_C_0;
		smps[x] = s_C_0;
		smph[x] = h_C_0;
	}
}
void SampleBank_Cst_B::setOverSampling(const int oSLevel)
{
	if(oSLevel < 0) return;
	if(oSLevel > 6) return;

	osRate = int (oSLevel);
	cRateL = flx (rfV->vl[osRate] * flx (sRateL));
	cRateS = fsx (rfV->vs[osRate] * fsx (sRateS));
	cRateH = fhx (rfV->vh[osRate] * fhx (sRateH));

	zeroAllSamples();
}
void SampleBank_Cst_B::runUpSampling()
{
	if(osRate > 5) return;
	if(osRate < 1) return;

	const int filt_select = osRate-1;

	if(precisionLevel == 0)
	{
		for (unsigned int x = 1; x < rfV->vi[osRate]; x++)
			smpl[x] = l_C_0;
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			UpFilter[filt_select]->l_Input(smpl[y]);
			UpFilter[filt_select]->l_RunQ();
			smpl[y] = (flx) UpFilter[filt_select]->l_GetOutput();

			smpl[y] *= flx (rfV->vl[osRate]);
		}
	} else
	if(precisionLevel == 1)
	{
		for (unsigned int x = 1; x < rfV->vi[osRate]; x++)
			smps[x] = s_C_0;
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			UpFilter[filt_select]->s_Input(smps[y]);
			UpFilter[filt_select]->s_RunQ();
			smps[y] = (fsx) UpFilter[filt_select]->s_GetOutput();

			smps[y] *= fsx (rfV->vs[osRate]);
		}
	} else
	if(precisionLevel == 2)
	{
		for (unsigned int x = 1; x < rfV->vi[osRate]; x++)
			smph[x] = h_C_0;
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			UpFilter[filt_select]->h_Input(smph[y]);
			UpFilter[filt_select]->h_RunQ();
			smph[y] = (fhx) UpFilter[filt_select]->h_GetOutput();

			smph[y] *= fhx (rfV->vh[osRate]);
		}
	}
}
void SampleBank_Cst_B::runDownSampling()
{
	if(osRate > 5) return;
	if(osRate < 1) return;

	const int filt_select = osRate-1;

	if(precisionLevel == 0)
	{
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			DnFilter[filt_select]->l_Input(smpl[y]);
			DnFilter[filt_select]->l_RunQ();
			smpl[y] = (flx) DnFilter[filt_select]->l_GetOutput();

			// smpl[y] *= flx (rfV->vl[osRate]);
		}
	} else
	if(precisionLevel == 1)
	{
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			DnFilter[filt_select]->s_Input(smps[y]);
			DnFilter[filt_select]->s_RunQ();
			smps[y] = (fsx) DnFilter[filt_select]->s_GetOutput();

			// smps[y] *= fsx (rfV->vs[osRate]);
		}
	} else
	if(precisionLevel == 2)
	{
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			DnFilter[filt_select]->h_Input(smph[y]);
			DnFilter[filt_select]->h_RunQ();
			smph[y] = (fhx) DnFilter[filt_select]->h_GetOutput();

			// smph[y] *= fhx (rfV->vh[osRate]);
		}
	}
}
void SampleBank_Cst_B::SetFPprecision(const int nLevel)
{
	precisionLevel = nLevel;
	if(precisionLevel < 0) precisionLevel = 0;
	if(precisionLevel > 2) precisionLevel = 2;
}
fsx SampleBank_Cst_B::GetOutP()
{
	if(precisionLevel == 0) return fsx (smpl[0]);
	if(precisionLevel == 1) return fsx (smps[0]);
	if(precisionLevel == 2) return fsx (smph[0]);
}
flx SampleBank_Cst_B::GetOutPL()
{
	return flx (smpl[0]);
}
fsx SampleBank_Cst_B::GetOutPS()
{
	return fsx (smps[0]);
}
fhx SampleBank_Cst_B::GetOutPH()
{
	return fhx (smph[0]);
}
