#include "module_scrmr.h"

MdTScreamer::MdTScreamer(float insRate) :
	zeroFH(fhx (0.00000000000000001)),
	zeroFS(fsx (0.00000000000001)),
	smplRate(float (insRate)),
	overSampling(int (2)),
	fPUprecisionHigh(true),
	moduleIsOnline(false)
{
	SmpS = new SampleBank_SD(float (smplRate));
	SmpH = new SampleBank_HD(float (smplRate));

	DrStage = new DstTScreamer(float (smplRate));

	iParam[tscream_Gain] = float (0.50f);

	moduleIsOnline = true;
}
MdTScreamer::~MdTScreamer()
{
	delete SmpS;
	delete SmpH;
	delete DrStage;
}
void MdTScreamer::changeSampleRate(const float nRate)
{
	SmpS->setSampleRate(float (nRate));
	SmpH->setSampleRate(float (nRate));
	DrStage->setSmpRate(float (nRate));
}
void MdTScreamer::ClearBuffers()
{
	SmpS->zeroAllSamples();
	SmpH->zeroAllSamples();
	DrStage->clearBuffs();
}
void MdTScreamer::setBypass(const bool bypassEngage)
{
	if((bypassEngage == false) && (moduleIsOnline == false))
	{
		ClearBuffers();
		moduleIsOnline = true;
	}
	if(bypassEngage == true) moduleIsOnline=false;
}
void MdTScreamer::setPrecision(const bool setHigh)
{
	if(fPUprecisionHigh != setHigh) ClearBuffers();
	fPUprecisionHigh = setHigh;
}
void MdTScreamer::setInternalParam(const int prm, float value)
{
	if(prm == tscream_Gain) iParam[tscream_Gain] = float (value);
	if(prm == tscream_Tone)
	{
		iParam[tscream_Tone] = float (value);
		DrStage->setQTone(float (value));
	}
}
void MdTScreamer::setOverSampleRate(const int vLevel)
{
	if(vLevel < 0) return;
//	if(vLevel != overSampling) ClearBuffers();
	overSampling = vLevel;
	if(overSampling > 5) overSampling = 1;
	if(overSampling == 0) overSampling = 1;

	SmpS->setOverSampling(overSampling);
	SmpH->setOverSampling(overSampling);
	DrStage->setOverSamplingRate(overSampling);
}
void MdTScreamer::inputSample(const flx inp)
{
	SmpS->smp[0] = fsx (inp);
	SmpH->smp[0] = fhx (inp);
}
void MdTScreamer::inputSample(const fsx inp)
{
	SmpS->smp[0] = fsx (inp);
	SmpH->smp[0] = fhx (inp);
}
void MdTScreamer::inputSample(const fhx inp)
{
	SmpS->smp[0] = fsx (inp);
	SmpH->smp[0] = fhx (inp);
}
flx MdTScreamer::getOutSampleLD()
{
	if(fPUprecisionHigh == true) return flx (SmpH->smp[0]);
	else return flx (SmpS->smp[0]);
}
fsx MdTScreamer::getOutSampleSD()
{
	if(fPUprecisionHigh == true) return fsx (SmpH->smp[0]);
	else return fsx (SmpS->smp[0]);
}
fhx MdTScreamer::getOutSampleHD()
{
	if(fPUprecisionHigh == true) return fhx (SmpH->smp[0]);
	else return fhx (SmpS->smp[0]);
}
void MdTScreamer::RunInternals(float gain, int numTubes)
{
	if(moduleIsOnline == false) return;

	if(fPUprecisionHigh == true)
	{
		if(fhx (SmpH->smp[0]) == fhx (0.0)) SmpH->smp[0] = fhx (zeroFH);

		SmpH->runUpSampling();

		SmpH->smp[0] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[0]), fhx (gain));
		if(overSampling > 1) SmpH->smp[1] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[1]), fhx (gain));
		if(overSampling > 2)
		{
			SmpH->smp[2] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[2]), fhx (gain));
			SmpH->smp[3] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[3]), fhx (gain));
		}
		if(overSampling > 3)
		{
			SmpH->smp[4] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[4]), fhx (gain));
			SmpH->smp[5] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[5]), fhx (gain));
			SmpH->smp[6] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[6]), fhx (gain));
			SmpH->smp[7] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[7]), fhx (gain));
		}
		if(overSampling > 4)
		{
			SmpH->smp[8] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[8]), fhx (gain));
			SmpH->smp[9] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[9]), fhx (gain));
			SmpH->smp[10] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[10]), fhx (gain));
			SmpH->smp[11] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[11]), fhx (gain));
			SmpH->smp[12] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[12]), fhx (gain));
			SmpH->smp[13] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[13]), fhx (gain));
			SmpH->smp[14] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[14]), fhx (gain));
			SmpH->smp[15] = (fhx) DrStage->applyDistortion(fhx (SmpH->smp[15]), fhx (gain));
		}

		SmpH->runDownSampling();

		if(fhx (SmpH->smp[0]) > fhx (1.0)) SmpH->smp[0] = fhx (1.0);
		if(fhx (SmpH->smp[0]) < fhx (-1.0)) SmpH->smp[0] = fhx (-1.0);
	} else
	{
		if(fsx (SmpS->smp[0]) == fsx (0.0)) SmpS->smp[0] = fsx (zeroFS);

		SmpS->runUpSampling();

		SmpS->smp[0] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[0]), fsx (gain));
		if(overSampling > 1) SmpS->smp[1] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[1]), fsx (gain));
		if(overSampling > 2)
		{
			SmpS->smp[2] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[2]), fsx (gain));
			SmpS->smp[3] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[3]), fsx (gain));
		}
		if(overSampling > 3)
		{
			SmpS->smp[4] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[4]), fsx (gain));
			SmpS->smp[5] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[5]), fsx (gain));
			SmpS->smp[6] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[6]), fsx (gain));
			SmpS->smp[7] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[7]), fsx (gain));
		}
		if(overSampling > 4)
		{
			SmpS->smp[8] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[8]), fsx (gain));
			SmpS->smp[9] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[9]), fsx (gain));
			SmpS->smp[10] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[10]), fsx (gain));
			SmpS->smp[11] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[11]), fsx (gain));
			SmpS->smp[12] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[12]), fsx (gain));
			SmpS->smp[13] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[13]), fsx (gain));
			SmpS->smp[14] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[14]), fsx (gain));
			SmpS->smp[15] = (fsx) DrStage->applyDistortion(fsx (SmpS->smp[15]), fsx (gain));
		}

		SmpS->runDownSampling();

		if(fsx (SmpS->smp[0]) > fsx (1.0)) SmpS->smp[0] = fsx (1.0);
		if(fsx (SmpS->smp[0]) < fsx (-1.0)) SmpS->smp[0] = fsx (-1.0);
	}
}
