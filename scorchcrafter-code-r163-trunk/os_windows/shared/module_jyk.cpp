#include "module_jyk.h"

MdJykwrakker::MdJykwrakker(fsx insRate) :
	zeroF(fhx (0.00000000000000000000001)),
	smplRate(fsx (insRate)),
	overSampling(int (1)),
	FPUlevel(int (2)),
	moduleIsOnline(false)
{
	Smp = new SampleBank(fsx (smplRate));

	for(int i=0;i<MAX_DIST_STAGES;i++)
	{
		DrStage[0][i] = new DstTubeXa(fsx (smplRate), fhx (DIST_STAGES_CLEAN_RW), fhx (DIST_STAGES_CLEAN_RF));
		DrStage[1][i] = new DstTubeXa(fsx (smplRate), fhx (DIST_STAGES_CRUNCH_RW), fhx (DIST_STAGES_CRUNCH_RF));
		DrStage[2][i] = new DstTubeXa(fsx (smplRate), fhx (DIST_STAGES_HIGH_RW), fhx (DIST_STAGES_HIGH_RF));
		DrStage[3][i] = new DstTubeXa(fsx (smplRate), fhx (DIST_STAGES_MEGA_RW), fhx (DIST_STAGES_MEGA_RF));
	}

	iParam[jykParam_Gain] = float (0.50f);

	inFiltL = new BTWorthLP(fsx (500.0), fsx (smplRate));
	inFiltH = new BTWorthLP(fsx (695.0), fsx (smplRate));
	inFiltCap = new BTWorthLP(fsx (7200.0), fsx (smplRate));

	moduleIsOnline = true;
}
MdJykwrakker::~MdJykwrakker()
{
	delete Smp;
	delete inFiltL;
	delete inFiltH;
	delete inFiltCap;
	for(int i=0;i<MAX_DIST_STAGES;i++)
	{
		delete DrStage[0][i];
		delete DrStage[1][i];
		delete DrStage[2][i];
		delete DrStage[3][i];
	}
}
void MdJykwrakker::changeSampleRate(const fsx nRate)
{
	smplRate = fsx (nRate);
	Smp->setSampleRate(fsx (smplRate));
	inFiltL->setSampleRate(smplRate);
	inFiltH->setSampleRate(smplRate);
	inFiltCap->setSampleRate(smplRate);

	for(int i=0;i<DIST_STAGES_CLEAN;i++)
	{
		DrStage[0][i]->setSmpRate(fsx (smplRate));
	}
	for(int i=0;i<DIST_STAGES_CRUNCH;i++)
	{
		DrStage[1][i]->setSmpRate(fsx (smplRate));
	}
	for(int i=0;i<DIST_STAGES_HIGH;i++)
	{
		DrStage[2][i]->setSmpRate(fsx (smplRate));
	}
	for(int i=0;i<DIST_STAGES_MEGA;i++)
	{
		DrStage[3][i]->setSmpRate(fsx (smplRate));
	}
}
void MdJykwrakker::ClearBuffers()
{
	Smp->zeroAllSamples();
	inFiltH->zeroBuffers();
	inFiltL->zeroBuffers();
	inFiltCap->zeroBuffers();
}
void MdJykwrakker::setBypass(const bool bypassEngage)
{
	if((bypassEngage == false) && (moduleIsOnline == false))
	{
		ClearBuffers();
		moduleIsOnline = true;
	}
	if(bypassEngage == true) moduleIsOnline=false;
}
void MdJykwrakker::setPrecision(const int nLevel)
{
	FPUlevel = nLevel;
	if(FPUlevel < 0) FPUlevel = 0;
	if(FPUlevel > 2) FPUlevel = 2;
	Smp->SetFPprecision(FPUlevel);
}
void MdJykwrakker::setInternalParam(const int prm, float value)
{
	if(prm == jykParam_Gain) iParam[jykParam_Gain] = float (value);
}
void MdJykwrakker::setOverSampleRate(const int vLevel)
{
	if(vLevel < 0) return;
	overSampling = vLevel;
	if(overSampling > 5) overSampling = 5;
	if(overSampling < 0) overSampling = 0;

	Smp->setOverSampling(overSampling);

	for(int i=0;i<MAX_DIST_STAGES;i++)
	{
		DrStage[0][i]->setOverSamplingRate(overSampling);
		DrStage[1][i]->setOverSamplingRate(overSampling);
		DrStage[2][i]->setOverSamplingRate(overSampling);
		DrStage[3][i]->setOverSamplingRate(overSampling);
	}
}
void MdJykwrakker::inputSample(const fhx inp)
{
	Smp->smp[0] = fhx (inp);
}
fhx MdJykwrakker::getOutSample()
{
	return fhx (Smp->smp[0]);
}
void MdJykwrakker::RunInternals(const int dChannel)
{
	if(moduleIsOnline == false) return;

	int numTubes = 0;
	if(dChannel == 0) numTubes = DIST_STAGES_CLEAN;
	if(dChannel == 1) numTubes = DIST_STAGES_CRUNCH;
	if(dChannel == 2) numTubes = DIST_STAGES_HIGH;
	if(dChannel == 3) numTubes = DIST_STAGES_MEGA;
	if(dChannel > 3) return;
	if(dChannel < 0) return;

	if(FPUlevel == 0)
	{
		if(flx (Smp->smp[0]) == flx (0.0)) Smp->smp[0] = flx (zeroF);

		if(dChannel < 3) splB = flx ((flx) inFiltL->runFilter(flx (Smp->smp[0])));
		else splB = flx ((flx) inFiltH->runFilter(flx (Smp->smp[0])));

		Smp->smp[0] -= flx (splB);
		Smp->smp[0] = (flx) inFiltCap->runFilter(flx (Smp->smp[0]));

		Smp->runUpSampling();

		for(int x = 0; x < numTubes; x++)
		{
			DrStage[dChannel][x]->setGain(flx (iParam[jykParam_Gain]));

			Smp->smp[0] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[0]));
			if(overSampling > 1) Smp->smp[1] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[1]));
			if(overSampling > 2)
			{
				Smp->smp[2] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[2]));
				Smp->smp[3] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[3]));
			}
			if(overSampling > 3)
			{
				Smp->smp[4] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[4]));
				Smp->smp[5] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[5]));
				Smp->smp[6] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[6]));
				Smp->smp[7] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[7]));
			}
			if(overSampling > 4)
			{
				Smp->smp[8] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[8]));
				Smp->smp[9] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[9]));
				Smp->smp[10] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[10]));
				Smp->smp[11] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[11]));
				Smp->smp[12] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[12]));
				Smp->smp[13] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[13]));
				Smp->smp[14] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[14]));
				Smp->smp[15] = (flx) DrStage[dChannel][x]->applyDistortion(flx (Smp->smp[15]));
			}
		}

		Smp->runDownSampling();

		Smp->smp[0] *= flx (0.9250f);

		if(flx (Smp->smp[0]) > flx (1.0f)) Smp->smp[0] = flx (1.0f);
		if(fhx (Smp->smp[0]) < fhx (-1.0f)) Smp->smp[0] = fhx (-1.0f);
	}
	if(FPUlevel == 1)
	{
		if(fsx (Smp->smp[0]) == fsx (0.0)) Smp->smp[0] = fsx (zeroF);

		if(dChannel < 3) splB = fsx ((fsx) inFiltL->runFilter(fsx (Smp->smp[0])));
		else splB = fsx ((fsx) inFiltH->runFilter(fsx (Smp->smp[0])));

		Smp->smp[0] -= fsx (splB);
		Smp->smp[0] = (fsx) inFiltCap->runFilter(fsx (Smp->smp[0]));

		Smp->runUpSampling();

		for(int x = 0; x < numTubes; x++)
		{
			DrStage[dChannel][x]->setGain(fsx (iParam[jykParam_Gain]));

			Smp->smp[0] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[0]));
			if(overSampling > 1) Smp->smp[1] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[1]));
			if(overSampling > 2)
			{
				Smp->smp[2] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[2]));
				Smp->smp[3] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[3]));
			}
			if(overSampling > 3)
			{
				Smp->smp[4] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[4]));
				Smp->smp[5] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[5]));
				Smp->smp[6] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[6]));
				Smp->smp[7] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[7]));
			}
			if(overSampling > 4)
			{
				Smp->smp[8] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[8]));
				Smp->smp[9] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[9]));
				Smp->smp[10] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[10]));
				Smp->smp[11] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[11]));
				Smp->smp[12] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[12]));
				Smp->smp[13] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[13]));
				Smp->smp[14] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[14]));
				Smp->smp[15] = (fsx) DrStage[dChannel][x]->applyDistortion(fsx (Smp->smp[15]));
			}
		}

		Smp->runDownSampling();

		Smp->smp[0] *= fsx (0.9250);

		if(fsx (Smp->smp[0]) > fsx (1.0)) Smp->smp[0] = fsx (1.0);
		if(fsx (Smp->smp[0]) < fsx (-1.0)) Smp->smp[0] = fsx (-1.0);
	}
	if(FPUlevel == 2)
	{
		if(fhx (Smp->smp[0]) == fhx (0.0)) Smp->smp[0] = fhx (zeroF);

		if(dChannel < 3) splB = fhx ((fhx) inFiltL->runFilter(fhx (Smp->smp[0])));
		else splB = fhx ((fhx) inFiltH->runFilter(fhx (Smp->smp[0])));

		Smp->smp[0] -= fhx (splB);
		Smp->smp[0] = (fhx) inFiltCap->runFilter(fhx (Smp->smp[0]));

		Smp->runUpSampling();

		for(int x = 0; x < numTubes; x++)
		{
			DrStage[dChannel][x]->setGain(fhx (iParam[jykParam_Gain]));

			Smp->smp[0] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[0]));
			if(overSampling > 1) Smp->smp[1] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[1]));
			if(overSampling > 2)
			{
				Smp->smp[2] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[2]));
				Smp->smp[3] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[3]));
			}
			if(overSampling > 3)
			{
				Smp->smp[4] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[4]));
				Smp->smp[5] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[5]));
				Smp->smp[6] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[6]));
				Smp->smp[7] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[7]));
			}
			if(overSampling > 4)
			{
				Smp->smp[8] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[8]));
				Smp->smp[9] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[9]));
				Smp->smp[10] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[10]));
				Smp->smp[11] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[11]));
				Smp->smp[12] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[12]));
				Smp->smp[13] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[13]));
				Smp->smp[14] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[14]));
				Smp->smp[15] = (fhx) DrStage[dChannel][x]->applyDistortion(fhx (Smp->smp[15]));
			}
		}

		Smp->runDownSampling();

		Smp->smp[0] *= fhx (0.9250);

		if(fhx (Smp->smp[0]) > fhx (1.0)) Smp->smp[0] = fhx (1.0);
		if(fhx (Smp->smp[0]) < fhx (-1.0)) Smp->smp[0] = fhx (-1.0);
	}
}
