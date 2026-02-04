#include "sample_bank_custom.h"

SampleBank_Cst::SampleBank_Cst(fsx iSRate) :
	sRateH(fhx (iSRate)),
	sRateS(fsx (iSRate)),
	sRateL(flx (iSRate)),
	cRateH(fhx (iSRate)),
	cRateS(fsx (iSRate)),
	cRateL(flx (iSRate)),
	osRate(int (2)),
	precisionLevel(int (1)),
	l_zero((flx) ScMath::l_calc_denorm()),
	s_zero((fsx) ScMath::s_calc_denorm()),
	h_zero((fhx) ScMath::h_calc_denorm())
{
	rfV = new RateFC();

	UpFilter = new BTW_LP(fsx (fhx (sRateH * (fhx) 0.250)), fsx (fhx (sRateH * (fhx) 2.0)));
	DnFilter = new BTW_LP(fsx (fhx (sRateH * (fhx) 0.250)), fsx (fhx (sRateH * (fhx) 2.0)));

	zeroAllSamples();
}
SampleBank_Cst::~SampleBank_Cst()
{
	delete UpFilter;
	delete DnFilter;

	delete rfV;
}
void SampleBank_Cst::InputSmp(const fsx inp)
{
	if(precisionLevel == 0) smpl[0] = flx (inp);
	if(precisionLevel == 1) smps[0] = fsx (inp);
	if(precisionLevel == 2) smph[0] = fhx (inp);
}
void SampleBank_Cst::setSampleRate(fsx nSRate)
{
	sRateL = flx (nSRate);
	sRateS = fsx (nSRate);
	sRateH = fhx (nSRate);

	cRateL = flx (rfV->vl[osRate] * flx (sRateL));
	cRateS = fsx (rfV->vs[osRate] * fsx (sRateS));
	cRateH = fhx (rfV->vh[osRate] * fhx (sRateH));

	UpFilter->setSampleRate(fsx (cRateS));
	DnFilter->setSampleRate(fsx (cRateS));

	UpFilter->changeFreq(sRateS * 0.250);
	DnFilter->changeFreq(sRateS * 0.250);

	zeroAllSamples();
}
void SampleBank_Cst::zeroAllSamples()
{
	for (int x = 0; x < 16; x++)
	{
		smpl[x] = flx (l_zero);
		smps[x] = fsx (s_zero);
		smph[x] = fhx (h_zero);
	}
}
void SampleBank_Cst::setOverSampling(const int oSLevel)
{
	if(oSLevel < 0) return;
	if(oSLevel > 6) return;

	osRate = int (oSLevel);
	cRateL = flx (rfV->vl[osRate] * flx (sRateL));
	cRateS = fsx (rfV->vs[osRate] * fsx (sRateS));
	cRateH = fhx (rfV->vh[osRate] * fhx (sRateH));

	UpFilter->setSampleRate(fsx (cRateS));
	DnFilter->setSampleRate(fsx (cRateS));

	UpFilter->changeFreq(fsx (fhx (sRateH * (fhx) 0.250)));
	DnFilter->changeFreq(fsx (fhx (sRateH * (fhx) 0.250)));

	zeroAllSamples();
}
void SampleBank_Cst::runUpSampling()
{
	if(osRate > 5) return;
	if(osRate < 2) return;

	if(precisionLevel == 0)
	{
		for (unsigned int x = 1; x < rfV->vi[osRate]; x++)
			smpl[x] = (flx) l_zero;
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			UpFilter->inpSplL((flx) smpl[y]);
			UpFilter->RunFilter();
			smpl[y] = (flx) UpFilter->GetOutPutL();

			smpl[y] *= flx (rfV->vl[osRate]);
		}
	} else
	if(precisionLevel == 1)
	{
		for (unsigned int x = 1; x < rfV->vi[osRate]; x++)
			smps[x] = (fsx) s_zero;
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			UpFilter->inpSplS((fsx) smps[y]);
			UpFilter->RunFilter();
			smps[y] = (fsx) UpFilter->GetOutPutS();

			smps[y] *= fsx (rfV->vs[osRate]);
		}
	} else
	if(precisionLevel == 2)
	{
		for (unsigned int x = 1; x < rfV->vi[osRate]; x++)
			smph[x] = (fhx) h_zero;
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			UpFilter->inpSplH((fhx) smph[y]);
			UpFilter->RunFilter();
			smph[y] = (fhx) UpFilter->GetOutPutH();

			smph[y] *= fhx (rfV->vh[osRate]);
		}
	}
}
void SampleBank_Cst::runDownSampling()
{
	if(osRate > 5) return;
	if(osRate < 2) return;

	if(precisionLevel == 0)
	{
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			DnFilter->inpSplL((flx) smpl[y]);
			DnFilter->RunFilter();
			smpl[y] = (flx) DnFilter->GetOutPutL();
		}
	} else
	if(precisionLevel == 1)
	{
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			DnFilter->inpSplS((fsx) smps[y]);
			DnFilter->RunFilter();
			smps[y] = (fsx) DnFilter->GetOutPutS();
		}
	} else
	if(precisionLevel == 2)
	{
		for (unsigned int y = 0; y < rfV->vi[osRate]; y++)
		{
			DnFilter->inpSplH((fhx) smph[y]);
			DnFilter->RunFilter();
			smph[y] = (fhx) DnFilter->GetOutPutH();
		}
	}
}
void SampleBank_Cst::SetFPprecision(const int nLevel)
{
	precisionLevel = nLevel;
	if(precisionLevel < 0) precisionLevel = 0;
	if(precisionLevel > 2) precisionLevel = 2;
}
fsx SampleBank_Cst::GetOutP()
{
	if(precisionLevel == 0) return fsx (smpl[0]);
	if(precisionLevel == 1) return fsx (smps[0]);
	if(precisionLevel == 2) return fsx (smph[0]);
}
flx SampleBank_Cst::GetOutPL()
{
	return flx (smpl[0]);
}
fsx SampleBank_Cst::GetOutPS()
{
	return fsx (smps[0]);
}
fhx SampleBank_Cst::GetOutPH()
{
	return fhx (smph[0]);
}
