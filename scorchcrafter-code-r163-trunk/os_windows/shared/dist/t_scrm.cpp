#include "t_scrm.h"

DstTScreamer::DstTScreamer(float inSRate) :
	PI((fhx) ((fhx) 4.00 * (fhx) atanl((fhx) 1.00))),
	hPI((fhx) ((fhx) PI * (fhx) 0.50)),
	PId(fhx (fhx (PI) * fhx (2.0))),
	zeroF(fhx (0.00000000000000001)),	//! Used to avoid denormal-related issues in parts of the code
	sRate(fhx (inSRate)),
	oSampling(int (2)),
	rateFac(fhx (2.0)),
	toneSet(fhx (1.0))
{
	if(fhx (sRate) < fhx (44100.0)) sRate = fhx (44100.0);
	if(fhx (sRate) > fhx (1536000.0)) sRate = fhx (1536000.0);

	ScFilter = new BTWorthLP_HD(f9x (720.484), float (sRate));
}
DstTScreamer::~DstTScreamer()
{
	delete ScFilter;
}
void DstTScreamer::setSmpRate(float nRate)
{
	sRate = fhx (nRate);
	ScFilter->setSampleRate(float (nRate * rateFac));
	clearBuffs();
}
void DstTScreamer::setGain(const float nGain)
{
	gGain = fhx (nGain);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);
}
void DstTScreamer::clearBuffs()
{
	filterPos = fhx (0.0);
	ScFilter->zeroBuffers();
}
void DstTScreamer::setOverSamplingRate(int inp)
{
	if(inp == 0) rateFac = fhx (1.0);
	if(inp == 1) rateFac = fhx (1.0);
	if(inp == 2) rateFac = fhx (2.0);
	if(inp == 3) rateFac = fhx (4.0);
	if(inp == 4) rateFac = fhx (8.0);
	if(inp == 5) rateFac = fhx (16.0);
	if(inp == 6) rateFac = fhx (1.0);

	ScFilter->setSampleRate(float (sRate * rateFac));

	clearBuffs();
}
fhx DstTScreamer::applyDistortion(fhx inSample, fhx (gn))
{
	gGain = fhx (fhx (1.0) - fhx (fhx (gn) * fhx (0.9750)));
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (0.9750)) gGain = fhx (0.9750);

	if(fhx (inSample) > fhx (1.0)) inSample = fhx (1.0);
	if(fhx (inSample) < fhx (-1.0)) inSample = fhx (-1.0);

	if(fhx (inSample) == fhx (0.0)) inSample = fhx (zeroF);

	inSample -= (fhx) ScFilter->runFilter(fhx (inSample));

	fhx dryPos = fhx (inSample);

	if(fhx (inSample) > fhx (0.0)) inSample = (fhx) runTube(fhx (inSample));
	else inSample = fhx (fhx (-1.0) * (fhx) runTube(fhx (-inSample)));

	inSample += fhx (dryPos);
	inSample *= fhx (0.50);

	const fhx filterFac = fhx (fhx (toneSet) * fhx (22000.0) * fhx (fhx (fhx (1.0) - (fhx) fabsl(inSample)) * fhx (fhx (1.0) - (fhx) fabsl(inSample))));

	const f9x filterFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI *
							(fhx) filterFac / (fhx) ((fhx) sRate * (f9x) rateFac)));

	inSample = fhx (filterPos += fhx (fhx ((fhx) inSample - (fhx) filterPos) * fhx (filterFreq)));

	return fhx (inSample);
}
fhx DstTScreamer::runTube(fhx iSpl)
{
	dsSpl = fhx (fhx (iSpl) * fhx (fhx (fhx (1.0) + fhx (gGain)) / fhx (fhx (iSpl) + fhx (gGain))));

	return fhx (dsSpl);
}
void DstTScreamer::setQTone(float qSet)
{
	toneSet = fhx (fhx (0.01250) + fhx (fhx (qSet) * fhx (0.9750)));
}
