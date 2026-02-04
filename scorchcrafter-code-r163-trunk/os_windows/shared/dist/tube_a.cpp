#include "tube_a.h"

DstTubeA::DstTubeA(float inSRate) :
	PI((fhx) ((fhx) 4.00 * (fhx) atanl((fhx) 1.00))),
	hPI((fhx) ((fhx) PI * (fhx) 0.50)),
	PId(fhx (fhx (PI) * fhx (2.0))),
	zeroF(fhx (0.00000000000000001)),	//! Used to avoid denormal-related issues in parts of the code
	sRate(fhx (inSRate)),
	oSampling(int (1)),
	rateFac(fhx (1.0)),
	leanT(fhx (0.0)),
	leanB(fhx (0.0))
{
	if(fhx (sRate) < fhx (44100.0)) sRate = fhx (44100.0);
	if(fhx (sRate) > fhx (1536000.0)) sRate = fhx (1536000.0);

	tubePullFac = fhx (fhx (441.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = fhx (fhx (800.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = (fhx) fabsl(fhx (leanFac));

	dsFilter = new BTWorthLP_HD(fhx (7200.0), float (sRate * rateFac));
}
DstTubeA::~DstTubeA()
{
	delete dsFilter;
}
void DstTubeA::setSmpRate(float nRate)
{
	sRate = fhx (nRate);
	dsFilter->setSampleRate(float (sRate * rateFac));
	clearBuffs();
}
void DstTubeA::setGain(const float nGain)
{
	gGain = fhx (nGain);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);
}
void DstTubeA::clearBuffs()
{
	dsFilter->zeroBuffers();
	lastPoint = fhx (0.0);
	distP = fhx (0.0);
	leanT = fhx (0.0);
	leanB = fhx (0.0);
	lean = fhx (0.0);
}
void DstTubeA::setOverSamplingRate(int inp)
{
	if(inp == 0) rateFac = fhx (1.0);
	if(inp == 1) rateFac = fhx (1.0);
	if(inp == 2) rateFac = fhx (2.0);
	if(inp == 3) rateFac = fhx (4.0);
	if(inp == 4) rateFac = fhx (8.0);
	if(inp == 5) rateFac = fhx (16.0);
	if(inp == 6) rateFac = fhx (1.0);

	tubePullFac = fhx (fhx (441.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = fhx (fhx (800.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = (fhx) fabsl(fhx (leanFac));

	dsFilter->setSampleRate(float (sRate * rateFac));
	clearBuffs();
}
fhx DstTubeA::applyDistortion(fhx inSample, fhx (gn))
{
	gGain = fhx (gn);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);

	if(fhx (inSample) > fhx (1.0)) inSample = fhx (1.0);
	if(fhx (inSample) < fhx (-1.0)) inSample = fhx (-1.0);

	dryPos = fhx (inSample);
	inSample = (fhx) dsFilter->runFilter(fhx (inSample));

	dryPos -= fhx (inSample);

	if(fhx (inSample) == fhx (0.0)) inSample = fhx (zeroF);
	if(fhx (inSample) > fhx (0.0))
	{
		inSample = (fhx) runTube(fhx (inSample));
	}
	else
	{
		inSample = fhx (fhx (-1.0) * (fhx) runTube(fhx (-inSample)));
	}

	inSample += fhx (fhx (dryPos) * fhx (gGain));

	return fhx (fhx (inSample) * fhx (0.9250));
}
fhx DstTubeA::runTube(fhx iSpl)
{
	if(fhx (iSpl) < fhx (0.0)) iSpl = fhx (-iSpl);
	if(fhx (iSpl) > fhx (1.0)) iSpl = fhx (1.0);

	// const fhx mSpl = fhx (fhx (1.0) - fhx (fhx (fhx (1.0) - fhx (iSpl)) * fhx (fhx (1.0) - fhx (iSpl))));

	if(fhx (iSpl * iSpl) > fhx (distP)) distP += fhx (fhx (tubePullFac) * fhx (1.00));	//1.00
	else distP -= fhx (fhx (tubePullFac) * fhx (0.010));	//0.10

	if(fhx (distP) > fhx (0.50)) distP = fhx (0.50);
	if(fhx (distP) < fhx (0.0)) distP = fhx (0.0);

//	dEffect = fhx (fhx (fhx (1.0) - fhx (fhx (distP) * fhx (0.4250))) * fhx (fhx (gGain) * fhx (0.9250)));
//	dEffect = fhx (fhx (fhx (1.0) - fhx (fhx (distP) * fhx (0.90))) * fhx (fhx (gGain) * fhx (0.9250)));
//	if(fhx (dEffect) > fhx (0.9750)) dEffect = fhx (0.9750);

//	iSpl *= fhx (fhx (1.0) - fhx (fhx (distP) * fhx (0.90)));

	iSpl *= fhx (fhx (0.99990) * fhx (gGain));
//	dryPos = fhx (iSpl);

	const fhx invSpl = fhx (fhx (1.0) - fhx (iSpl));
	const fhx qSpl = fhx (fhx (invSpl) * fhx (invSpl) * fhx (invSpl) * fhx (invSpl));

	//! const fhx dgDist = fhx (fhx (1.0) - fhx (dEffect));
	//! const fhx dgDist = fhx (fhx (1.0) - fhx (fhx (gGain) * fhx (0.92250)));
	//! const fhx dgDist = fhx (fhx (1.0) - fhx (fhx (gGain) * fhx (lean)));
	fhx dgDist = fhx (fhx (fhx (1.0) - fhx (fhx (gGain) * fhx (0.90))) / fhx (qSpl));
	dgDist *= fhx (fhx (0.50) + fhx (distP));

	dsSpl = fhx (fhx (iSpl) * fhx (fhx (fhx (1.0) + fhx (dgDist)) / fhx (fhx (iSpl) + fhx (dgDist))));

	// iSpl *= fhx (distP);
//	dsSpl *= fhx (fhx (1.0) - fhx (fhx (distP) * fhx (gGain)));
//	iSpl += fhx (dsSpl);
//	iSpl *= fhx (0.50);

//	iSpl = fhx (dsSpl);

//	if(fhx (iSpl) > fhx (1.0)) iSpl = fhx (1.0);

	return fhx (dsSpl);
}
