#include "sample_bank.h"

SampleBank::SampleBank(fsx iSRate) :
	zeroF(fhx (0.000000000000000000000010)),
	sRate(fhx (iSRate)),
	osRate(int (1)),
	precisionLevel(int (2))
{
	UpFilter[0] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate));
	UpFilter[1] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate * 2.0));
	UpFilter[2] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate * 4.0));
	UpFilter[3] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate * 8.0));
	UpFilter[4] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate * 16.0));

	DnFilter[0] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate));
	DnFilter[1] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate * 2.0));
	DnFilter[2] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate * 4.0));
	DnFilter[3] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate * 8.0));
	DnFilter[4] = new BTWorthLP(fsx (fsx (sRate) * fsx (0.249750)), fsx (sRate * 16.0));

	zeroAllSamples();
}
SampleBank::~SampleBank()
{
	delete UpFilter[0];
	delete UpFilter[1];
	delete UpFilter[2];
	delete UpFilter[3];
	delete UpFilter[4];

	delete DnFilter[0];
	delete DnFilter[1];
	delete DnFilter[2];
	delete DnFilter[3];
	delete DnFilter[4];
}
void SampleBank::setSampleRate(fsx nSRate)
{
	UpFilter[0]->setSampleRate(fsx (nSRate));
	UpFilter[1]->setSampleRate(fsx (nSRate * 2.0));
	UpFilter[2]->setSampleRate(fsx (nSRate * 4.0));
	UpFilter[3]->setSampleRate(fsx (nSRate * 8.0));
	UpFilter[4]->setSampleRate(fsx (nSRate * 16.0));

	DnFilter[0]->setSampleRate(fsx (nSRate));
	DnFilter[1]->setSampleRate(fsx (nSRate * 2.0));
	DnFilter[2]->setSampleRate(fsx (nSRate * 4.0));
	DnFilter[3]->setSampleRate(fsx (nSRate * 8.0));
	DnFilter[4]->setSampleRate(fsx (nSRate * 16.0));

	zeroAllSamples();
}
void SampleBank::zeroAllSamples()
{
	smp[0] = fhx (0.0);
	smp[1] = fhx (0.0);
	smp[2] = fhx (0.0);
	smp[3] = fhx (0.0);
	smp[4] = fhx (0.0);
	smp[5] = fhx (0.0);
	smp[6] = fhx (0.0);
	smp[7] = fhx (0.0);
	smp[8] = fhx (0.0);
	smp[9] = fhx (0.0);
	smp[10] = fhx (0.0);
	smp[11] = fhx (0.0);
	smp[12] = fhx (0.0);
	smp[13] = fhx (0.0);
	smp[14] = fhx (0.0);
	smp[15] = fhx (0.0);
}
void SampleBank::setOverSampling(int oSLevel)
{
	osRate = int (oSLevel);

	zeroAllSamples();
}
void SampleBank::runUpSampling()
{
	if(precisionLevel == 0)
	{
		if(osRate>=2)
		{
			smp[1]=flx (0.0f);
			smp[0]=(flx) UpFilter[1]->runFilter(flx (smp[0]));
			smp[1]=(flx) UpFilter[1]->runFilter(flx (smp[1]));
			smp[0]*= flx (2.0f);
			smp[1]*= flx (2.0f);
		}
		if(osRate>=3)
		{
			smp[2]=flx (smp[1]);
			smp[1]=flx (0.0f);
			smp[3]=flx (0.0f);
			smp[0]=(flx) UpFilter[2]->runFilter(flx (smp[0]));
			smp[1]=(flx) UpFilter[2]->runFilter(flx (smp[1]));
			smp[2]=(flx) UpFilter[2]->runFilter(flx (smp[2]));
			smp[3]=(flx) UpFilter[2]->runFilter(flx (smp[3]));
			smp[0]*= flx (2.0f);
			smp[1]*= flx (2.0f);
			smp[2]*= flx (2.0f);
			smp[3]*= flx (2.0f);
		}
		if(osRate>=4)
		{
			smp[6]=flx (smp[3]);
			smp[4]=flx (smp[2]);
			smp[2]=flx (smp[1]);
			smp[1]=flx (0.0f);
			smp[3]=flx (0.0f);
			smp[5]=flx (0.0f);
			smp[7]=flx (0.0f);
			smp[0]=(flx) UpFilter[3]->runFilter(flx (smp[0]));
			smp[1]=(flx) UpFilter[3]->runFilter(flx (smp[1]));
			smp[2]=(flx) UpFilter[3]->runFilter(flx (smp[2]));
			smp[3]=(flx) UpFilter[3]->runFilter(flx (smp[3]));
			smp[4]=(flx) UpFilter[3]->runFilter(flx (smp[4]));
			smp[5]=(flx) UpFilter[3]->runFilter(flx (smp[5]));
			smp[6]=(flx) UpFilter[3]->runFilter(flx (smp[6]));
			smp[7]=(flx) UpFilter[3]->runFilter(flx (smp[7]));
			smp[0]*= flx (2.0f);
			smp[1]*= flx (2.0f);
			smp[2]*= flx (2.0f);
			smp[3]*= flx (2.0f);
			smp[4]*= flx (2.0f);
			smp[5]*= flx (2.0f);
			smp[6]*= flx (2.0f);
			smp[7]*= flx (2.0f);
		}
		if(osRate==5)
		{
			smp[14]=flx (smp[7]);
			smp[12]=flx (smp[6]);
			smp[10]=flx (smp[5]);
			smp[8]=flx (smp[4]);
			smp[6]=flx (smp[3]);
			smp[4]=flx (smp[2]);
			smp[2]=flx (smp[1]);
			smp[1]=flx (0.0f);
			smp[3]=flx (0.0f);
			smp[5]=flx (0.0f);
			smp[7]=flx (0.0f);
			smp[9]=flx (0.0f);
			smp[11]=flx (0.0f);
			smp[13]=flx (0.0f);
			smp[15]=flx (0.0f);
			smp[0]=(flx) UpFilter[4]->runFilter(flx (smp[0]));
			smp[1]=(flx) UpFilter[4]->runFilter(flx (smp[1]));
			smp[2]=(flx) UpFilter[4]->runFilter(flx (smp[2]));
			smp[3]=(flx) UpFilter[4]->runFilter(flx (smp[3]));
			smp[4]=(flx) UpFilter[4]->runFilter(flx (smp[4]));
			smp[5]=(flx) UpFilter[4]->runFilter(flx (smp[5]));
			smp[6]=(flx) UpFilter[4]->runFilter(flx (smp[6]));
			smp[7]=(flx) UpFilter[4]->runFilter(flx (smp[7]));
			smp[8]=(flx) UpFilter[4]->runFilter(flx (smp[8]));
			smp[9]=(flx) UpFilter[4]->runFilter(flx (smp[9]));
			smp[10]=(flx) UpFilter[4]->runFilter(flx (smp[10]));
			smp[11]=(flx) UpFilter[4]->runFilter(flx (smp[11]));
			smp[12]=(flx) UpFilter[4]->runFilter(flx (smp[12]));
			smp[13]=(flx) UpFilter[4]->runFilter(flx (smp[13]));
			smp[14]=(flx) UpFilter[4]->runFilter(flx (smp[14]));
			smp[15]=(flx) UpFilter[4]->runFilter(flx (smp[15]));
			smp[0]*= flx (2.0f);
			smp[1]*= flx (2.0f);
			smp[2]*= flx (2.0f);
			smp[3]*= flx (2.0f);
			smp[4]*= flx (2.0f);
			smp[5]*= flx (2.0f);
			smp[6]*= flx (2.0f);
			smp[7]*= flx (2.0f);
			smp[8]*= flx (2.0f);
			smp[9]*= flx (2.0f);
			smp[10]*= flx (2.0f);
			smp[11]*= flx (2.0f);
			smp[12]*= flx (2.0f);
			smp[13]*= flx (2.0f);
			smp[14]*= flx (2.0f);
			smp[15]*= flx (2.0f);
		}
	}
	if(precisionLevel == 1)
	{
		if(osRate>=2)
		{
			smp[1]=fsx (0.0);
			smp[0]=(fsx) UpFilter[1]->runFilter(fsx (smp[0]));
			smp[1]=(fsx) UpFilter[1]->runFilter(fsx (smp[1]));
			smp[0]*= fsx (2.0);
			smp[1]*= fsx (2.0);
		}
		if(osRate>=3)
		{
			smp[2]=fsx (smp[1]);
			smp[1]=fsx (0.0);
			smp[3]=fsx (0.0);
			smp[0]=(fsx) UpFilter[2]->runFilter(fsx (smp[0]));
			smp[1]=(fsx) UpFilter[2]->runFilter(fsx (smp[1]));
			smp[2]=(fsx) UpFilter[2]->runFilter(fsx (smp[2]));
			smp[3]=(fsx) UpFilter[2]->runFilter(fsx (smp[3]));
			smp[0]*= fsx (2.0);
			smp[1]*= fsx (2.0);
			smp[2]*= fsx (2.0);
			smp[3]*= fsx (2.0);
		}
		if(osRate>=4)
		{
			smp[6]=fsx (smp[3]);
			smp[4]=fsx (smp[2]);
			smp[2]=fsx (smp[1]);
			smp[1]=fsx (0.0);
			smp[3]=fsx (0.0);
			smp[5]=fsx (0.0);
			smp[7]=fsx (0.0);
			smp[0]=(fsx) UpFilter[3]->runFilter(fsx (smp[0]));
			smp[1]=(fsx) UpFilter[3]->runFilter(fsx (smp[1]));
			smp[2]=(fsx) UpFilter[3]->runFilter(fsx (smp[2]));
			smp[3]=(fsx) UpFilter[3]->runFilter(fsx (smp[3]));
			smp[4]=(fsx) UpFilter[3]->runFilter(fsx (smp[4]));
			smp[5]=(fsx) UpFilter[3]->runFilter(fsx (smp[5]));
			smp[6]=(fsx) UpFilter[3]->runFilter(fsx (smp[6]));
			smp[7]=(fsx) UpFilter[3]->runFilter(fsx (smp[7]));
			smp[0]*= fsx (2.0);
			smp[1]*= fsx (2.0);
			smp[2]*= fsx (2.0);
			smp[3]*= fsx (2.0);
			smp[4]*= fsx (2.0);
			smp[5]*= fsx (2.0);
			smp[6]*= fsx (2.0);
			smp[7]*= fsx (2.0);
		}
		if(osRate==5)
		{
			smp[14]=fsx (smp[7]);
			smp[12]=fsx (smp[6]);
			smp[10]=fsx (smp[5]);
			smp[8]=fsx (smp[4]);
			smp[6]=fsx (smp[3]);
			smp[4]=fsx (smp[2]);
			smp[2]=fsx (smp[1]);
			smp[1]=fsx (0.0);
			smp[3]=fsx (0.0);
			smp[5]=fsx (0.0);
			smp[7]=fsx (0.0);
			smp[9]=fsx (0.0);
			smp[11]=fsx (0.0);
			smp[13]=fsx (0.0);
			smp[15]=fsx (0.0);
			smp[0]=(fsx) UpFilter[4]->runFilter(fsx (smp[0]));
			smp[1]=(fsx) UpFilter[4]->runFilter(fsx (smp[1]));
			smp[2]=(fsx) UpFilter[4]->runFilter(fsx (smp[2]));
			smp[3]=(fsx) UpFilter[4]->runFilter(fsx (smp[3]));
			smp[4]=(fsx) UpFilter[4]->runFilter(fsx (smp[4]));
			smp[5]=(fsx) UpFilter[4]->runFilter(fsx (smp[5]));
			smp[6]=(fsx) UpFilter[4]->runFilter(fsx (smp[6]));
			smp[7]=(fsx) UpFilter[4]->runFilter(fsx (smp[7]));
			smp[8]=(fsx) UpFilter[4]->runFilter(fsx (smp[8]));
			smp[9]=(fsx) UpFilter[4]->runFilter(fsx (smp[9]));
			smp[10]=(fsx) UpFilter[4]->runFilter(fsx (smp[10]));
			smp[11]=(fsx) UpFilter[4]->runFilter(fsx (smp[11]));
			smp[12]=(fsx) UpFilter[4]->runFilter(fsx (smp[12]));
			smp[13]=(fsx) UpFilter[4]->runFilter(fsx (smp[13]));
			smp[14]=(fsx) UpFilter[4]->runFilter(fsx (smp[14]));
			smp[15]=(fsx) UpFilter[4]->runFilter(fsx (smp[15]));
			smp[0]*= fsx (2.0);
			smp[1]*= fsx (2.0);
			smp[2]*= fsx (2.0);
			smp[3]*= fsx (2.0);
			smp[4]*= fsx (2.0);
			smp[5]*= fsx (2.0);
			smp[6]*= fsx (2.0);
			smp[7]*= fsx (2.0);
			smp[8]*= fsx (2.0);
			smp[9]*= fsx (2.0);
			smp[10]*= fsx (2.0);
			smp[11]*= fsx (2.0);
			smp[12]*= fsx (2.0);
			smp[13]*= fsx (2.0);
			smp[14]*= fsx (2.0);
			smp[15]*= fsx (2.0);
		}
	}
	if(precisionLevel == 2)
	{
		if(osRate>=2)
		{
			smp[1]=fhx (0.0);
			smp[0]=(fhx) UpFilter[1]->runFilter(fhx (smp[0]));
			smp[1]=(fhx) UpFilter[1]->runFilter(fhx (smp[1]));
			smp[0]*= fhx (2.0);
			smp[1]*= fhx (2.0);
		}
		if(osRate>=3)
		{
			smp[2]=fhx (smp[1]);
			smp[1]=fhx (0.0);
			smp[3]=fhx (0.0);
			smp[0]=(fhx) UpFilter[2]->runFilter(fhx (smp[0]));
			smp[1]=(fhx) UpFilter[2]->runFilter(fhx (smp[1]));
			smp[2]=(fhx) UpFilter[2]->runFilter(fhx (smp[2]));
			smp[3]=(fhx) UpFilter[2]->runFilter(fhx (smp[3]));
			smp[0]*= fhx (2.0);
			smp[1]*= fhx (2.0);
			smp[2]*= fhx (2.0);
			smp[3]*= fhx (2.0);
		}
		if(osRate>=4)
		{
			smp[6]=fhx (smp[3]);
			smp[4]=fhx (smp[2]);
			smp[2]=fhx (smp[1]);
			smp[1]=fhx (0.0);
			smp[3]=fhx (0.0);
			smp[5]=fhx (0.0);
			smp[7]=fhx (0.0);
			smp[0]=(fhx) UpFilter[3]->runFilter(fhx (smp[0]));
			smp[1]=(fhx) UpFilter[3]->runFilter(fhx (smp[1]));
			smp[2]=(fhx) UpFilter[3]->runFilter(fhx (smp[2]));
			smp[3]=(fhx) UpFilter[3]->runFilter(fhx (smp[3]));
			smp[4]=(fhx) UpFilter[3]->runFilter(fhx (smp[4]));
			smp[5]=(fhx) UpFilter[3]->runFilter(fhx (smp[5]));
			smp[6]=(fhx) UpFilter[3]->runFilter(fhx (smp[6]));
			smp[7]=(fhx) UpFilter[3]->runFilter(fhx (smp[7]));
			smp[0]*= fhx (2.0);
			smp[1]*= fhx (2.0);
			smp[2]*= fhx (2.0);
			smp[3]*= fhx (2.0);
			smp[4]*= fhx (2.0);
			smp[5]*= fhx (2.0);
			smp[6]*= fhx (2.0);
			smp[7]*= fhx (2.0);
		}
		if(osRate==5)
		{
			smp[14]=fhx (smp[7]);
			smp[12]=fhx (smp[6]);
			smp[10]=fhx (smp[5]);
			smp[8]=fhx (smp[4]);
			smp[6]=fhx (smp[3]);
			smp[4]=fhx (smp[2]);
			smp[2]=fhx (smp[1]);
			smp[1]=fhx (0.0);
			smp[3]=fhx (0.0);
			smp[5]=fhx (0.0);
			smp[7]=fhx (0.0);
			smp[9]=fhx (0.0);
			smp[11]=fhx (0.0);
			smp[13]=fhx (0.0);
			smp[15]=fhx (0.0);
			smp[0]=(fhx) UpFilter[4]->runFilter(fhx (smp[0]));
			smp[1]=(fhx) UpFilter[4]->runFilter(fhx (smp[1]));
			smp[2]=(fhx) UpFilter[4]->runFilter(fhx (smp[2]));
			smp[3]=(fhx) UpFilter[4]->runFilter(fhx (smp[3]));
			smp[4]=(fhx) UpFilter[4]->runFilter(fhx (smp[4]));
			smp[5]=(fhx) UpFilter[4]->runFilter(fhx (smp[5]));
			smp[6]=(fhx) UpFilter[4]->runFilter(fhx (smp[6]));
			smp[7]=(fhx) UpFilter[4]->runFilter(fhx (smp[7]));
			smp[8]=(fhx) UpFilter[4]->runFilter(fhx (smp[8]));
			smp[9]=(fhx) UpFilter[4]->runFilter(fhx (smp[9]));
			smp[10]=(fhx) UpFilter[4]->runFilter(fhx (smp[10]));
			smp[11]=(fhx) UpFilter[4]->runFilter(fhx (smp[11]));
			smp[12]=(fhx) UpFilter[4]->runFilter(fhx (smp[12]));
			smp[13]=(fhx) UpFilter[4]->runFilter(fhx (smp[13]));
			smp[14]=(fhx) UpFilter[4]->runFilter(fhx (smp[14]));
			smp[15]=(fhx) UpFilter[4]->runFilter(fhx (smp[15]));
			smp[0]*= fhx (2.0);
			smp[1]*= fhx (2.0);
			smp[2]*= fhx (2.0);
			smp[3]*= fhx (2.0);
			smp[4]*= fhx (2.0);
			smp[5]*= fhx (2.0);
			smp[6]*= fhx (2.0);
			smp[7]*= fhx (2.0);
			smp[8]*= fhx (2.0);
			smp[9]*= fhx (2.0);
			smp[10]*= fhx (2.0);
			smp[11]*= fhx (2.0);
			smp[12]*= fhx (2.0);
			smp[13]*= fhx (2.0);
			smp[14]*= fhx (2.0);
			smp[15]*= fhx (2.0);
		}
	}
}
void SampleBank::runDownSampling()
{
	if(precisionLevel == 0)
	{
		if(osRate==5)
		{
			smp[0]=(flx) DnFilter[4]->runFilter(flx (smp[0]));
			smp[1]=(flx) DnFilter[4]->runFilter(flx (smp[1]));
			smp[2]=(flx) DnFilter[4]->runFilter(flx (smp[2]));
			smp[3]=(flx) DnFilter[4]->runFilter(flx (smp[3]));
			smp[4]=(flx) DnFilter[4]->runFilter(flx (smp[4]));
			smp[5]=(flx) DnFilter[4]->runFilter(flx (smp[5]));
			smp[6]=(flx) DnFilter[4]->runFilter(flx (smp[6]));
			smp[7]=(flx) DnFilter[4]->runFilter(flx (smp[7]));
			smp[8]=(flx) DnFilter[4]->runFilter(flx (smp[8]));
			smp[9]=(flx) DnFilter[4]->runFilter(flx (smp[9]));
			smp[10]=(flx) DnFilter[4]->runFilter(flx (smp[10]));
			smp[11]=(flx) DnFilter[4]->runFilter(flx (smp[11]));
			smp[12]=(flx) DnFilter[4]->runFilter(flx (smp[12]));
			smp[13]=(flx) DnFilter[4]->runFilter(flx (smp[13]));
			smp[14]=(flx) DnFilter[4]->runFilter(flx (smp[14]));
			smp[15]=(flx) DnFilter[4]->runFilter(flx (smp[15]));
			smp[1]=flx (smp[2]);
			smp[2]=flx (smp[4]);
			smp[3]=flx (smp[6]);
			smp[4]=flx (smp[8]);
			smp[5]=flx (smp[10]);
			smp[6]=flx (smp[12]);
			smp[7]=flx (smp[14]);
		}
		if(osRate>=4)
		{
			smp[0]=(flx) DnFilter[3]->runFilter(flx (smp[0]));
			smp[1]=(flx) DnFilter[3]->runFilter(flx (smp[1]));
			smp[2]=(flx) DnFilter[3]->runFilter(flx (smp[2]));
			smp[3]=(flx) DnFilter[3]->runFilter(flx (smp[3]));
			smp[4]=(flx) DnFilter[3]->runFilter(flx (smp[4]));
			smp[5]=(flx) DnFilter[3]->runFilter(flx (smp[5]));
			smp[6]=(flx) DnFilter[3]->runFilter(flx (smp[6]));
			smp[7]=(flx) DnFilter[3]->runFilter(flx (smp[7]));
			smp[1]=flx (smp[2]);
			smp[2]=flx (smp[4]);
			smp[3]=flx (smp[6]);
		}
		if(osRate>=3)
		{
			smp[0]=(flx) DnFilter[2]->runFilter(flx (smp[0]));
			smp[1]=(flx) DnFilter[2]->runFilter(flx (smp[1]));
			smp[2]=(flx) DnFilter[2]->runFilter(flx (smp[2]));
			smp[3]=(flx) DnFilter[2]->runFilter(flx (smp[3]));
			smp[1]=flx (smp[2]);
		}
		if(osRate>=2)
		{
			smp[0]=(flx) DnFilter[1]->runFilter(flx (smp[0]));
			smp[1]=(flx) DnFilter[1]->runFilter(flx (smp[1]));
		}
		smp[0]=(flx) DnFilter[0]->runFilter(flx (smp[0]));
	}
	if(precisionLevel == 1)
	{
		if(osRate==5)
		{
			smp[0]=(fsx) DnFilter[4]->runFilter(fsx (smp[0]));
			smp[1]=(fsx) DnFilter[4]->runFilter(fsx (smp[1]));
			smp[2]=(fsx) DnFilter[4]->runFilter(fsx (smp[2]));
			smp[3]=(fsx) DnFilter[4]->runFilter(fsx (smp[3]));
			smp[4]=(fsx) DnFilter[4]->runFilter(fsx (smp[4]));
			smp[5]=(fsx) DnFilter[4]->runFilter(fsx (smp[5]));
			smp[6]=(fsx) DnFilter[4]->runFilter(fsx (smp[6]));
			smp[7]=(fsx) DnFilter[4]->runFilter(fsx (smp[7]));
			smp[8]=(fsx) DnFilter[4]->runFilter(fsx (smp[8]));
			smp[9]=(fsx) DnFilter[4]->runFilter(fsx (smp[9]));
			smp[10]=(fsx) DnFilter[4]->runFilter(fsx (smp[10]));
			smp[11]=(fsx) DnFilter[4]->runFilter(fsx (smp[11]));
			smp[12]=(fsx) DnFilter[4]->runFilter(fsx (smp[12]));
			smp[13]=(fsx) DnFilter[4]->runFilter(fsx (smp[13]));
			smp[14]=(fsx) DnFilter[4]->runFilter(fsx (smp[14]));
			smp[15]=(fsx) DnFilter[4]->runFilter(fsx (smp[15]));
			smp[1]=fsx (smp[2]);
			smp[2]=fsx (smp[4]);
			smp[3]=fsx (smp[6]);
			smp[4]=fsx (smp[8]);
			smp[5]=fsx (smp[10]);
			smp[6]=fsx (smp[12]);
			smp[7]=fsx (smp[14]);
		}
		if(osRate>=4)
		{
			smp[0]=(fsx) DnFilter[3]->runFilter(fsx (smp[0]));
			smp[1]=(fsx) DnFilter[3]->runFilter(fsx (smp[1]));
			smp[2]=(fsx) DnFilter[3]->runFilter(fsx (smp[2]));
			smp[3]=(fsx) DnFilter[3]->runFilter(fsx (smp[3]));
			smp[4]=(fsx) DnFilter[3]->runFilter(fsx (smp[4]));
			smp[5]=(fsx) DnFilter[3]->runFilter(fsx (smp[5]));
			smp[6]=(fsx) DnFilter[3]->runFilter(fsx (smp[6]));
			smp[7]=(fsx) DnFilter[3]->runFilter(fsx (smp[7]));
			smp[1]=fsx (smp[2]);
			smp[2]=fsx (smp[4]);
			smp[3]=fsx (smp[6]);
		}
		if(osRate>=3)
		{
			smp[0]=(fsx) DnFilter[2]->runFilter(fsx (smp[0]));
			smp[1]=(fsx) DnFilter[2]->runFilter(fsx (smp[1]));
			smp[2]=(fsx) DnFilter[2]->runFilter(fsx (smp[2]));
			smp[3]=(fsx) DnFilter[2]->runFilter(fsx (smp[3]));
			smp[1]=fsx (smp[2]);
		}
		if(osRate>=2)
		{
			smp[0]=(fsx) DnFilter[1]->runFilter(fsx (smp[0]));
			smp[1]=(fsx) DnFilter[1]->runFilter(fsx (smp[1]));
		}
		smp[0]=(fsx) DnFilter[0]->runFilter(fsx (smp[0]));
	}
	if(precisionLevel == 2)
	{
		if(osRate==5)
		{
			smp[0]=(fhx) DnFilter[4]->runFilter(fhx (smp[0]));
			smp[1]=(fhx) DnFilter[4]->runFilter(fhx (smp[1]));
			smp[2]=(fhx) DnFilter[4]->runFilter(fhx (smp[2]));
			smp[3]=(fhx) DnFilter[4]->runFilter(fhx (smp[3]));
			smp[4]=(fhx) DnFilter[4]->runFilter(fhx (smp[4]));
			smp[5]=(fhx) DnFilter[4]->runFilter(fhx (smp[5]));
			smp[6]=(fhx) DnFilter[4]->runFilter(fhx (smp[6]));
			smp[7]=(fhx) DnFilter[4]->runFilter(fhx (smp[7]));
			smp[8]=(fhx) DnFilter[4]->runFilter(fhx (smp[8]));
			smp[9]=(fhx) DnFilter[4]->runFilter(fhx (smp[9]));
			smp[10]=(fhx) DnFilter[4]->runFilter(fhx (smp[10]));
			smp[11]=(fhx) DnFilter[4]->runFilter(fhx (smp[11]));
			smp[12]=(fhx) DnFilter[4]->runFilter(fhx (smp[12]));
			smp[13]=(fhx) DnFilter[4]->runFilter(fhx (smp[13]));
			smp[14]=(fhx) DnFilter[4]->runFilter(fhx (smp[14]));
			smp[15]=(fhx) DnFilter[4]->runFilter(fhx (smp[15]));
			smp[0]=fhx (fhx (fhx (smp[0]) + fhx (smp[1])) * fhx (0.50));
			smp[2]=fhx (fhx (fhx (smp[2]) + fhx (smp[3])) * fhx (0.50));
			smp[4]=fhx (fhx (fhx (smp[4]) + fhx (smp[5])) * fhx (0.50));
			smp[6]=fhx (fhx (fhx (smp[6]) + fhx (smp[7])) * fhx (0.50));
			smp[8]=fhx (fhx (fhx (smp[8]) + fhx (smp[9])) * fhx (0.50));
			smp[10]=fhx (fhx (fhx (smp[10]) + fhx (smp[11])) * fhx (0.50));
			smp[12]=fhx (fhx (fhx (smp[12]) + fhx (smp[13])) * fhx (0.50));
			smp[14]=fhx (fhx (fhx (smp[14]) + fhx (smp[15])) * fhx (0.50));
			smp[1]=fhx (smp[2]);
			smp[2]=fhx (smp[4]);
			smp[3]=fhx (smp[6]);
			smp[4]=fhx (smp[8]);
			smp[5]=fhx (smp[10]);
			smp[6]=fhx (smp[12]);
			smp[7]=fhx (smp[14]);
		}
		if(osRate>=4)
		{
			smp[0]=(fhx) DnFilter[3]->runFilter(fhx (smp[0]));
			smp[1]=(fhx) DnFilter[3]->runFilter(fhx (smp[1]));
			smp[2]=(fhx) DnFilter[3]->runFilter(fhx (smp[2]));
			smp[3]=(fhx) DnFilter[3]->runFilter(fhx (smp[3]));
			smp[4]=(fhx) DnFilter[3]->runFilter(fhx (smp[4]));
			smp[5]=(fhx) DnFilter[3]->runFilter(fhx (smp[5]));
			smp[6]=(fhx) DnFilter[3]->runFilter(fhx (smp[6]));
			smp[7]=(fhx) DnFilter[3]->runFilter(fhx (smp[7]));
			smp[0]=fhx (fhx (fhx (smp[0]) + fhx (smp[1])) * fhx (0.50));
			smp[2]=fhx (fhx (fhx (smp[2]) + fhx (smp[3])) * fhx (0.50));
			smp[4]=fhx (fhx (fhx (smp[4]) + fhx (smp[5])) * fhx (0.50));
			smp[6]=fhx (fhx (fhx (smp[6]) + fhx (smp[7])) * fhx (0.50));
			smp[1]=fhx (smp[2]);
			smp[2]=fhx (smp[4]);
			smp[3]=fhx (smp[6]);
		}
		if(osRate>=3)
		{
			smp[0]=(fhx) DnFilter[2]->runFilter(fhx (smp[0]));
			smp[1]=(fhx) DnFilter[2]->runFilter(fhx (smp[1]));
			smp[2]=(fhx) DnFilter[2]->runFilter(fhx (smp[2]));
			smp[3]=(fhx) DnFilter[2]->runFilter(fhx (smp[3]));
			smp[0]=fhx (fhx (fhx (smp[0]) + fhx (smp[1])) * fhx (0.50));
			smp[2]=fhx (fhx (fhx (smp[2]) + fhx (smp[3])) * fhx (0.50));
			smp[1]=fhx (smp[2]);
		}
		if(osRate>=2)
		{
			smp[0]=(fhx) DnFilter[1]->runFilter(fhx (smp[0]));
			smp[1]=(fhx) DnFilter[1]->runFilter(fhx (smp[1]));
			smp[0]=fhx (fhx (fhx (smp[0]) + fhx (smp[1])) * fhx (0.50));
		}
		smp[0]=(fhx) DnFilter[0]->runFilter(fhx (smp[0]));
	}
}
void SampleBank::SetFPprecision(const int nLevel)
{
	precisionLevel = nLevel;
	if(precisionLevel < 0) precisionLevel = 0;
	if(precisionLevel > 2) precisionLevel = 2;
}

//! -------------------------------------------------------------------------------------------------------

SampleBank_SD::SampleBank_SD(float iSRate) :
	zeroF(fsx (0.00000000000001)),
	sRate(fsx (iSRate)),
	osRate(int (1))
{
	UpFilter[0] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate));
	UpFilter[1] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 2.0f));
	UpFilter[2] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 4.0f));
	UpFilter[3] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 8.0f));
	UpFilter[4] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 16.0f));

	DnFilter[0] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate));
	DnFilter[1] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 2.0f));
	DnFilter[2] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 4.0f));
	DnFilter[3] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 8.0f));
	DnFilter[4] = new BTWorthLP_SD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 16.0f));

	zeroAllSamples();
}
SampleBank_SD::~SampleBank_SD()
{
	delete UpFilter[0];
	delete UpFilter[1];
	delete UpFilter[2];
	delete UpFilter[3];
	delete UpFilter[4];

	delete DnFilter[0];
	delete DnFilter[1];
	delete DnFilter[2];
	delete DnFilter[3];
	delete DnFilter[4];
}
void SampleBank_SD::setSampleRate(float nSRate)
{
	UpFilter[0]->setSampleRate(float (nSRate));
	UpFilter[1]->setSampleRate(float (nSRate * 2.0f));
	UpFilter[2]->setSampleRate(float (nSRate * 4.0f));
	UpFilter[3]->setSampleRate(float (nSRate * 8.0f));
	UpFilter[4]->setSampleRate(float (nSRate * 16.0f));

	DnFilter[0]->setSampleRate(float (nSRate));
	DnFilter[1]->setSampleRate(float (nSRate * 2.0f));
	DnFilter[2]->setSampleRate(float (nSRate * 4.0f));
	DnFilter[3]->setSampleRate(float (nSRate * 8.0f));
	DnFilter[4]->setSampleRate(float (nSRate * 16.0f));

	zeroAllSamples();
}
void SampleBank_SD::zeroAllSamples()
{
	smp[0] = fsx (0.0);
	smp[1] = fsx (0.0);
	smp[2] = fsx (0.0);
	smp[3] = fsx (0.0);
	smp[4] = fsx (0.0);
	smp[5] = fsx (0.0);
	smp[6] = fsx (0.0);
	smp[7] = fsx (0.0);
	smp[8] = fsx (0.0);
	smp[9] = fsx (0.0);
	smp[10] = fsx (0.0);
	smp[11] = fsx (0.0);
	smp[12] = fsx (0.0);
	smp[13] = fsx (0.0);
	smp[14] = fsx (0.0);
	smp[15] = fsx (0.0);
}
void SampleBank_SD::setOverSampling(int oSLevel)
{
	osRate = int (oSLevel);

	zeroAllSamples();
}
void SampleBank_SD::runUpSampling()
{
	if(osRate>=2)
	{
		smp[1]=fsx (0.0);
		smp[0]=(fsx) UpFilter[1]->runFilter(fsx (smp[0]));
		smp[1]=(fsx) UpFilter[1]->runFilter(fsx (smp[1]));
		smp[0]*= fsx (2.0);
		smp[1]*= fsx (2.0);
	}
	if(osRate>=3)
	{
		smp[2]=fsx (smp[1]);
		smp[1]=fsx (0.0);
		smp[3]=fsx (0.0);
		smp[0]=(fsx) UpFilter[2]->runFilter(fsx (smp[0]));
		smp[1]=(fsx) UpFilter[2]->runFilter(fsx (smp[1]));
		smp[2]=(fsx) UpFilter[2]->runFilter(fsx (smp[2]));
		smp[3]=(fsx) UpFilter[2]->runFilter(fsx (smp[3]));
		smp[0]*= fsx (2.0);
		smp[1]*= fsx (2.0);
		smp[2]*= fsx (2.0);
		smp[3]*= fsx (2.0);
	}
	if(osRate>=4)
	{
		smp[6]=fsx (smp[3]);
		smp[4]=fsx (smp[2]);
		smp[2]=fsx (smp[1]);
		smp[1]=fsx (0.0);
		smp[3]=fsx (0.0);
		smp[5]=fsx (0.0);
		smp[7]=fsx (0.0);
		smp[0]=(fsx) UpFilter[3]->runFilter(fsx (smp[0]));
		smp[1]=(fsx) UpFilter[3]->runFilter(fsx (smp[1]));
		smp[2]=(fsx) UpFilter[3]->runFilter(fsx (smp[2]));
		smp[3]=(fsx) UpFilter[3]->runFilter(fsx (smp[3]));
		smp[4]=(fsx) UpFilter[3]->runFilter(fsx (smp[4]));
		smp[5]=(fsx) UpFilter[3]->runFilter(fsx (smp[5]));
		smp[6]=(fsx) UpFilter[3]->runFilter(fsx (smp[6]));
		smp[7]=(fsx) UpFilter[3]->runFilter(fsx (smp[7]));
		smp[0]*= fsx (2.0);
		smp[1]*= fsx (2.0);
		smp[2]*= fsx (2.0);
		smp[3]*= fsx (2.0);
		smp[4]*= fsx (2.0);
		smp[5]*= fsx (2.0);
		smp[6]*= fsx (2.0);
		smp[7]*= fsx (2.0);
	}
	if(osRate==5)
	{
		smp[14]=fsx (smp[7]);
		smp[12]=fsx (smp[6]);
		smp[10]=fsx (smp[5]);
		smp[8]=fsx (smp[4]);
		smp[6]=fsx (smp[3]);
		smp[4]=fsx (smp[2]);
		smp[2]=fsx (smp[1]);
		smp[1]=fsx (0.0);
		smp[3]=fsx (0.0);
		smp[5]=fsx (0.0);
		smp[7]=fsx (0.0);
		smp[9]=fsx (0.0);
		smp[11]=fsx (0.0);
		smp[13]=fsx (0.0);
		smp[15]=fsx (0.0);
		smp[0]=(fsx) UpFilter[4]->runFilter(fsx (smp[0]));
		smp[1]=(fsx) UpFilter[4]->runFilter(fsx (smp[1]));
		smp[2]=(fsx) UpFilter[4]->runFilter(fsx (smp[2]));
		smp[3]=(fsx) UpFilter[4]->runFilter(fsx (smp[3]));
		smp[4]=(fsx) UpFilter[4]->runFilter(fsx (smp[4]));
		smp[5]=(fsx) UpFilter[4]->runFilter(fsx (smp[5]));
		smp[6]=(fsx) UpFilter[4]->runFilter(fsx (smp[6]));
		smp[7]=(fsx) UpFilter[4]->runFilter(fsx (smp[7]));
		smp[8]=(fsx) UpFilter[4]->runFilter(fsx (smp[8]));
		smp[9]=(fsx) UpFilter[4]->runFilter(fsx (smp[9]));
		smp[10]=(fsx) UpFilter[4]->runFilter(fsx (smp[10]));
		smp[11]=(fsx) UpFilter[4]->runFilter(fsx (smp[11]));
		smp[12]=(fsx) UpFilter[4]->runFilter(fsx (smp[12]));
		smp[13]=(fsx) UpFilter[4]->runFilter(fsx (smp[13]));
		smp[14]=(fsx) UpFilter[4]->runFilter(fsx (smp[14]));
		smp[15]=(fsx) UpFilter[4]->runFilter(fsx (smp[15]));
		smp[0]*= fsx (2.0);
		smp[1]*= fsx (2.0);
		smp[2]*= fsx (2.0);
		smp[3]*= fsx (2.0);
		smp[4]*= fsx (2.0);
		smp[5]*= fsx (2.0);
		smp[6]*= fsx (2.0);
		smp[7]*= fsx (2.0);
		smp[8]*= fsx (2.0);
		smp[9]*= fsx (2.0);
		smp[10]*= fsx (2.0);
		smp[11]*= fsx (2.0);
		smp[12]*= fsx (2.0);
		smp[13]*= fsx (2.0);
		smp[14]*= fsx (2.0);
		smp[15]*= fsx (2.0);
	}
}
void SampleBank_SD::runDownSampling()
{
	if(osRate==5)
	{
		smp[0]=(fsx) DnFilter[4]->runFilter(fsx (smp[0]));
		smp[1]=(fsx) DnFilter[4]->runFilter(fsx (smp[1]));
		smp[2]=(fsx) DnFilter[4]->runFilter(fsx (smp[2]));
		smp[3]=(fsx) DnFilter[4]->runFilter(fsx (smp[3]));
		smp[4]=(fsx) DnFilter[4]->runFilter(fsx (smp[4]));
		smp[5]=(fsx) DnFilter[4]->runFilter(fsx (smp[5]));
		smp[6]=(fsx) DnFilter[4]->runFilter(fsx (smp[6]));
		smp[7]=(fsx) DnFilter[4]->runFilter(fsx (smp[7]));
		smp[8]=(fsx) DnFilter[4]->runFilter(fsx (smp[8]));
		smp[9]=(fsx) DnFilter[4]->runFilter(fsx (smp[9]));
		smp[10]=(fsx) DnFilter[4]->runFilter(fsx (smp[10]));
		smp[11]=(fsx) DnFilter[4]->runFilter(fsx (smp[11]));
		smp[12]=(fsx) DnFilter[4]->runFilter(fsx (smp[12]));
		smp[13]=(fsx) DnFilter[4]->runFilter(fsx (smp[13]));
		smp[14]=(fsx) DnFilter[4]->runFilter(fsx (smp[14]));
		smp[15]=(fsx) DnFilter[4]->runFilter(fsx (smp[15]));
		smp[1]=fsx (smp[2]);
		smp[2]=fsx (smp[4]);
		smp[3]=fsx (smp[6]);
		smp[4]=fsx (smp[8]);
		smp[5]=fsx (smp[10]);
		smp[6]=fsx (smp[12]);
		smp[7]=fsx (smp[14]);
	}
	if(osRate>=4)
	{
		smp[0]=(fsx) DnFilter[3]->runFilter(fsx (smp[0]));
		smp[1]=(fsx) DnFilter[3]->runFilter(fsx (smp[1]));
		smp[2]=(fsx) DnFilter[3]->runFilter(fsx (smp[2]));
		smp[3]=(fsx) DnFilter[3]->runFilter(fsx (smp[3]));
		smp[4]=(fsx) DnFilter[3]->runFilter(fsx (smp[4]));
		smp[5]=(fsx) DnFilter[3]->runFilter(fsx (smp[5]));
		smp[6]=(fsx) DnFilter[3]->runFilter(fsx (smp[6]));
		smp[7]=(fsx) DnFilter[3]->runFilter(fsx (smp[7]));
		smp[1]=fsx (smp[2]);
		smp[2]=fsx (smp[4]);
		smp[3]=fsx (smp[6]);
	}
	if(osRate>=3)
	{
		smp[0]=(fsx) DnFilter[2]->runFilter(fsx (smp[0]));
		smp[1]=(fsx) DnFilter[2]->runFilter(fsx (smp[1]));
		smp[2]=(fsx) DnFilter[2]->runFilter(fsx (smp[2]));
		smp[3]=(fsx) DnFilter[2]->runFilter(fsx (smp[3]));
		smp[1]=fsx (smp[2]);
	}
	if(osRate>=2)
	{
		smp[0]=(fsx) DnFilter[1]->runFilter(fsx (smp[0]));
		smp[1]=(fsx) DnFilter[1]->runFilter(fsx (smp[1]));
	}
	smp[0]=(fsx) DnFilter[0]->runFilter(fsx (smp[0]));
}

//! -------------------------------------------------------------------------------------------------------

SampleBank_HD::SampleBank_HD(float iSRate) :
	zeroF(fhx (0.00000000000000001)),
	sRate(fhx (iSRate)),
	osRate(int (1))
{
	UpFilter[0] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate));
	UpFilter[1] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 2.0f));
	UpFilter[2] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 4.0f));
	UpFilter[3] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 8.0f));
	UpFilter[4] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 16.0f));

	DnFilter[0] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate));
	DnFilter[1] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 2.0f));
	DnFilter[2] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 4.0f));
	DnFilter[3] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 8.0f));
	DnFilter[4] = new BTWorthLP_HD(fsx (fsx (sRate) * fsx (0.250)), float (sRate * 16.0f));

	zeroAllSamples();
}
SampleBank_HD::~SampleBank_HD()
{
	delete UpFilter[0];
	delete UpFilter[1];
	delete UpFilter[2];
	delete UpFilter[3];
	delete UpFilter[4];

	delete DnFilter[0];
	delete DnFilter[1];
	delete DnFilter[2];
	delete DnFilter[3];
	delete DnFilter[4];
}
void SampleBank_HD::setSampleRate(float nSRate)
{
	UpFilter[0]->setSampleRate(float (nSRate));
	UpFilter[1]->setSampleRate(float (nSRate * 2.0f));
	UpFilter[2]->setSampleRate(float (nSRate * 4.0f));
	UpFilter[3]->setSampleRate(float (nSRate * 8.0f));
	UpFilter[4]->setSampleRate(float (nSRate * 16.0f));

	DnFilter[0]->setSampleRate(float (nSRate));
	DnFilter[1]->setSampleRate(float (nSRate * 2.0f));
	DnFilter[2]->setSampleRate(float (nSRate * 4.0f));
	DnFilter[3]->setSampleRate(float (nSRate * 8.0f));
	DnFilter[4]->setSampleRate(float (nSRate * 16.0f));

	zeroAllSamples();
}
void SampleBank_HD::zeroAllSamples()
{
	smp[0] = fhx (0.0);
	smp[1] = fhx (0.0);
	smp[2] = fhx (0.0);
	smp[3] = fhx (0.0);
	smp[4] = fhx (0.0);
	smp[5] = fhx (0.0);
	smp[6] = fhx (0.0);
	smp[7] = fhx (0.0);
	smp[8] = fhx (0.0);
	smp[9] = fhx (0.0);
	smp[10] = fhx (0.0);
	smp[11] = fhx (0.0);
	smp[12] = fhx (0.0);
	smp[13] = fhx (0.0);
	smp[14] = fhx (0.0);
	smp[15] = fhx (0.0);
}
void SampleBank_HD::setOverSampling(int oSLevel)
{
	osRate = int (oSLevel);

	zeroAllSamples();
}
void SampleBank_HD::runUpSampling()
{
	if(osRate>=2)
	{
		smp[1]=fhx (0.0);
		smp[0]=(fhx) UpFilter[1]->runFilter(fhx (smp[0]));
		smp[1]=(fhx) UpFilter[1]->runFilter(fhx (smp[1]));
		smp[0]*= fhx (2.0);
		smp[1]*= fhx (2.0);
	}
	if(osRate>=3)
	{
		smp[2]=fhx (smp[1]);
		smp[1]=fhx (0.0);
		smp[3]=fhx (0.0);
		smp[0]=(fhx) UpFilter[2]->runFilter(fhx (smp[0]));
		smp[1]=(fhx) UpFilter[2]->runFilter(fhx (smp[1]));
		smp[2]=(fhx) UpFilter[2]->runFilter(fhx (smp[2]));
		smp[3]=(fhx) UpFilter[2]->runFilter(fhx (smp[3]));
		smp[0]*= fhx (2.0);
		smp[1]*= fhx (2.0);
		smp[2]*= fhx (2.0);
		smp[3]*= fhx (2.0);
	}
	if(osRate>=4)
	{
		smp[6]=fhx (smp[3]);
		smp[4]=fhx (smp[2]);
		smp[2]=fhx (smp[1]);
		smp[1]=fhx (0.0);
		smp[3]=fhx (0.0);
		smp[5]=fhx (0.0);
		smp[7]=fhx (0.0);
		smp[0]=(fhx) UpFilter[3]->runFilter(fhx (smp[0]));
		smp[1]=(fhx) UpFilter[3]->runFilter(fhx (smp[1]));
		smp[2]=(fhx) UpFilter[3]->runFilter(fhx (smp[2]));
		smp[3]=(fhx) UpFilter[3]->runFilter(fhx (smp[3]));
		smp[4]=(fhx) UpFilter[3]->runFilter(fhx (smp[4]));
		smp[5]=(fhx) UpFilter[3]->runFilter(fhx (smp[5]));
		smp[6]=(fhx) UpFilter[3]->runFilter(fhx (smp[6]));
		smp[7]=(fhx) UpFilter[3]->runFilter(fhx (smp[7]));
		smp[0]*= fhx (2.0);
		smp[1]*= fhx (2.0);
		smp[2]*= fhx (2.0);
		smp[3]*= fhx (2.0);
		smp[4]*= fhx (2.0);
		smp[5]*= fhx (2.0);
		smp[6]*= fhx (2.0);
		smp[7]*= fhx (2.0);
	}
	if(osRate==5)
	{
		smp[14]=fhx (smp[7]);
		smp[12]=fhx (smp[6]);
		smp[10]=fhx (smp[5]);
		smp[8]=fhx (smp[4]);
		smp[6]=fhx (smp[3]);
		smp[4]=fhx (smp[2]);
		smp[2]=fhx (smp[1]);
		smp[1]=fhx (0.0);
		smp[3]=fhx (0.0);
		smp[5]=fhx (0.0);
		smp[7]=fhx (0.0);
		smp[9]=fhx (0.0);
		smp[11]=fhx (0.0);
		smp[13]=fhx (0.0);
		smp[15]=fhx (0.0);
		smp[0]=(fhx) UpFilter[4]->runFilter(fhx (smp[0]));
		smp[1]=(fhx) UpFilter[4]->runFilter(fhx (smp[1]));
		smp[2]=(fhx) UpFilter[4]->runFilter(fhx (smp[2]));
		smp[3]=(fhx) UpFilter[4]->runFilter(fhx (smp[3]));
		smp[4]=(fhx) UpFilter[4]->runFilter(fhx (smp[4]));
		smp[5]=(fhx) UpFilter[4]->runFilter(fhx (smp[5]));
		smp[6]=(fhx) UpFilter[4]->runFilter(fhx (smp[6]));
		smp[7]=(fhx) UpFilter[4]->runFilter(fhx (smp[7]));
		smp[8]=(fhx) UpFilter[4]->runFilter(fhx (smp[8]));
		smp[9]=(fhx) UpFilter[4]->runFilter(fhx (smp[9]));
		smp[10]=(fhx) UpFilter[4]->runFilter(fhx (smp[10]));
		smp[11]=(fhx) UpFilter[4]->runFilter(fhx (smp[11]));
		smp[12]=(fhx) UpFilter[4]->runFilter(fhx (smp[12]));
		smp[13]=(fhx) UpFilter[4]->runFilter(fhx (smp[13]));
		smp[14]=(fhx) UpFilter[4]->runFilter(fhx (smp[14]));
		smp[15]=(fhx) UpFilter[4]->runFilter(fhx (smp[15]));
		smp[0]*= fhx (2.0);
		smp[1]*= fhx (2.0);
		smp[2]*= fhx (2.0);
		smp[3]*= fhx (2.0);
		smp[4]*= fhx (2.0);
		smp[5]*= fhx (2.0);
		smp[6]*= fhx (2.0);
		smp[7]*= fhx (2.0);
		smp[8]*= fhx (2.0);
		smp[9]*= fhx (2.0);
		smp[10]*= fhx (2.0);
		smp[11]*= fhx (2.0);
		smp[12]*= fhx (2.0);
		smp[13]*= fhx (2.0);
		smp[14]*= fhx (2.0);
		smp[15]*= fhx (2.0);
	}
}
void SampleBank_HD::runDownSampling()
{
	if(osRate==5)
	{
		smp[0]=(fhx) DnFilter[4]->runFilter(fhx (smp[0]));
		smp[1]=(fhx) DnFilter[4]->runFilter(fhx (smp[1]));
		smp[2]=(fhx) DnFilter[4]->runFilter(fhx (smp[2]));
		smp[3]=(fhx) DnFilter[4]->runFilter(fhx (smp[3]));
		smp[4]=(fhx) DnFilter[4]->runFilter(fhx (smp[4]));
		smp[5]=(fhx) DnFilter[4]->runFilter(fhx (smp[5]));
		smp[6]=(fhx) DnFilter[4]->runFilter(fhx (smp[6]));
		smp[7]=(fhx) DnFilter[4]->runFilter(fhx (smp[7]));
		smp[8]=(fhx) DnFilter[4]->runFilter(fhx (smp[8]));
		smp[9]=(fhx) DnFilter[4]->runFilter(fhx (smp[9]));
		smp[10]=(fhx) DnFilter[4]->runFilter(fhx (smp[10]));
		smp[11]=(fhx) DnFilter[4]->runFilter(fhx (smp[11]));
		smp[12]=(fhx) DnFilter[4]->runFilter(fhx (smp[12]));
		smp[13]=(fhx) DnFilter[4]->runFilter(fhx (smp[13]));
		smp[14]=(fhx) DnFilter[4]->runFilter(fhx (smp[14]));
		smp[15]=(fhx) DnFilter[4]->runFilter(fhx (smp[15]));
		smp[1]=fhx (smp[2]);
		smp[2]=fhx (smp[4]);
		smp[3]=fhx (smp[6]);
		smp[4]=fhx (smp[8]);
		smp[5]=fhx (smp[10]);
		smp[6]=fhx (smp[12]);
		smp[7]=fhx (smp[14]);
	}
	if(osRate>=4)
	{
		smp[0]=(fhx) DnFilter[3]->runFilter(fhx (smp[0]));
		smp[1]=(fhx) DnFilter[3]->runFilter(fhx (smp[1]));
		smp[2]=(fhx) DnFilter[3]->runFilter(fhx (smp[2]));
		smp[3]=(fhx) DnFilter[3]->runFilter(fhx (smp[3]));
		smp[4]=(fhx) DnFilter[3]->runFilter(fhx (smp[4]));
		smp[5]=(fhx) DnFilter[3]->runFilter(fhx (smp[5]));
		smp[6]=(fhx) DnFilter[3]->runFilter(fhx (smp[6]));
		smp[7]=(fhx) DnFilter[3]->runFilter(fhx (smp[7]));
		smp[1]=fhx (smp[2]);
		smp[2]=fhx (smp[4]);
		smp[3]=fhx (smp[6]);
	}
	if(osRate>=3)
	{
		smp[0]=(fhx) DnFilter[2]->runFilter(fhx (smp[0]));
		smp[1]=(fhx) DnFilter[2]->runFilter(fhx (smp[1]));
		smp[2]=(fhx) DnFilter[2]->runFilter(fhx (smp[2]));
		smp[3]=(fhx) DnFilter[2]->runFilter(fhx (smp[3]));
		smp[1]=fhx (smp[2]);
	}
	if(osRate>=2)
	{
		smp[0]=(fhx) DnFilter[1]->runFilter(fhx (smp[0]));
		smp[1]=(fhx) DnFilter[1]->runFilter(fhx (smp[1]));
	}
	smp[0]=(fhx) DnFilter[0]->runFilter(fhx (smp[0]));
}
