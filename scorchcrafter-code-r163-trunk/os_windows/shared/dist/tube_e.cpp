#include "tube_e.h"

DstTubeE::DstTubeE(fsx inSRate) :
	PI((fhx) ((fhx) 4.00 * (fhx) atanl((fhx) 1.00))),
	hPI((fhx) ((fhx) PI * (fhx) 0.50)),
	PId(fhx (fhx (PI) * fhx (2.0))),
	zeroF(fhx (0.00000000000000000000001)),		//! Used to avoid denormal-related issues in parts of the code
	sRate(fhx (inSRate)),
	oSampling(int (1)),
	rateFac(fhx (1.0)),
	leanT(fhx (0.0)),
	leanB(fhx (0.0)),
	lwCutFreq(fhx (20.0)),
	iFiltPos(fhx (0.0))
{
	if(fhx (sRate) < fhx (44100.0)) sRate = fhx (44100.0);
	if(fhx (sRate) > fhx (1536000.0)) sRate = fhx (1536000.0);

	tubePullFac = fhx (fhx (440.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = fhx (fhx (1.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = (fhx) fabsl(fhx (leanFac));

	dsFilter = new BTWorthLP(fhx (7200.0), fsx (sRate * rateFac));
	// dsFilterL = new BTWorthLP_HD(fhx (15.0), float (sRate * rateFac));

	iFiltFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI * (fhx) lwCutFreq / fhx (sRate*rateFac)));
}
DstTubeE::~DstTubeE()
{
	delete dsFilter;
	// delete dsFilterL;
}
void DstTubeE::setSmpRate(fsx nRate)
{
	sRate = fhx (nRate);
	dsFilter->setSampleRate(fsx (sRate * rateFac));
	// dsFilterL->setSampleRate(float (sRate * rateFac));
	clearBuffs();
	iFiltFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI * (fhx) lwCutFreq / fhx (sRate*rateFac)));
}
void DstTubeE::setGain(const fsx nGain)
{
	gGain = fhx (nGain);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);
}
void DstTubeE::clearBuffs()
{
	dsFilter->zeroBuffers();
	// dsFilterL->zeroBuffers();
	lastPoint = fhx (0.0);
	distP[0] = fhx (0.0);
	distP[1] = fhx (0.0);
	leanT = fhx (0.0);
	leanB = fhx (0.0);
	lean = fhx (0.0);
	topSide = true;
	iFiltPos = fhx (0.0);
	leanFrq = fhx (0.0);
	tbAtt = fhx (0.0);
}
void DstTubeE::setOverSamplingRate(int inp)
{
	if(inp == 0) rateFac = fhx (1.0);
	if(inp == 1) rateFac = fhx (1.0);
	if(inp == 2) rateFac = fhx (2.0);
	if(inp == 3) rateFac = fhx (4.0);
	if(inp == 4) rateFac = fhx (8.0);
	if(inp == 5) rateFac = fhx (16.0);
	if(inp == 6) rateFac = fhx (1.0);

	tubePullFac = fhx (fhx (440.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = fhx (fhx (800.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = (fhx) fabsl(fhx (leanFac));

	dsFilter->setSampleRate(fsx (sRate * rateFac));
	// dsFilterL->setSampleRate(float (sRate * rateFac));
	clearBuffs();
	iFiltFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI * (fhx) lwCutFreq / fhx (sRate*rateFac)));
}
fhx DstTubeE::applyDistortion(fhx inSample, const fhx gn)
{
	gGain = fhx (gn);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);

//!	gGain = fhx (fhx (1.0) - fhx (fhx (fhx (1.0) - fhx (gGain)) * fhx (fhx (1.0) - fhx (gGain))));

	// inSample *= fhx (0.98750);
//	inSample *= fhx (gGain);

	if(fhx (inSample) > fhx (1.0)) inSample = fhx (1.0);
	if(fhx (inSample) < fhx (-1.0)) inSample = fhx (-1.0);

	// const fhx inpAmpl = (fhx) fabsl(fhx (inSample));
//	const fhx freqMod = fhx (fhx (44100.0) / fhx (sRate * rateFac));
	// const fhx newFreq = fhx (fhx (fhx (21000.0) * fhx (1.0 - inpAmpl)) + fhx (1000.0));
	// dsFilter->changeFreq(fhx (newFreq));
	// drySamp = fhx (fhx (inSample) * fhx (fhx (1.0) - fhx (inpAmpl)));
//	drySamp = fhx (fhx (inSample) * fhx (0.150));
	//! const fhx sInCut = fhx (iFiltPos += fhx (fhx ((fhx) inSample - (fhx) iFiltPos) * fhx (iFiltFreq)));
	//! inSample -= fhx (fhx (sInCut) * fhx (fhx (1.0) - (fhx) fabsl(fhx (inSample))));

	// inSample = (fhx) dsFilter->runFilter(fhx (inSample));

	//! drySamp = fhx (inSample);
	// inSample *= fhx (0.850);
	//! inSample += fhx (drySamp);
	// inSample *= fhx (inpAmpl);
	// inSample += fhx (drySamp);
//	inSample -= (fhx) dsFilterL->runFilter(fhx (inSample));
	// inSample *= fhx (2.0);

	drySamp = fhx (inSample);

	if(fhx (inSample) == fhx (0.0)) inSample = fhx (zeroF);
	if(fhx (inSample) > fhx (0.0))
	{
		if(topSide == false)
		{
			topSide = true;
			lean = fhx (0.0);
			leanFrq = fhx (0.0);
			tbAtt = fhx (0.0);
//			distP[0] = fhx (0.0);
//			distP[1] = fhx (0.0);
		}
		srPoint = fhx (lastPoint);
		inSample = (fhx) runTube(fhx (inSample));
	}
	else
	{
		if(topSide == true)
		{
			topSide = false;
			lean = fhx (0.0);
			leanFrq = fhx (0.0);
			tbAtt = fhx (0.0);
//			distP[0] = fhx (0.0);
//			distP[1] = fhx (0.0);
		}
		srPoint = fhx (-lastPoint);
		inSample = fhx (fhx (-1.0) * (fhx) runTube(fhx (-inSample)));
	}

	lastPoint = fhx (drySamp);

	// inSample += fhx (drySamp);
	// inSample *= fhx (0.50);

	return fhx (fhx (inSample) * fhx (0.999750));
}
fhx DstTubeE::runTube(fhx iSpl)
{
	iSpl *= fhx (gGain);

	const fhx thresh = fhx (0.1250);

	if(fhx(iSpl) < fhx (thresh))
	{
		lean += fhx (leanFac);
		tbAtt = fhx (0.0);
	}
	else
	{
		if(fhx (leanFrq) == fhx (0.0))
		{
			fhx lnMod = fhx (lean);
			if(fhx (lnMod) < fhx (zeroF)) lnMod = fhx (zeroF);
			lnMod = fhx (fhx (1.0) / fhx (lnMod));
			leanFrq = fhx (fhx (fhx (sRate) * fhx (rateFac)) / fhx (lnMod));
			// leanFrq = fhx (fhx (fhx (sRate) * fhx (rateFac)) - fhx (leanFrq));
			if(fhx (leanFrq) < fhx (0.0)) leanFrq = fhx (-1.0);
		}
	}
	if(fhx (lean) > fhx (1.0)) lean = fhx (1.0);

	if(fhx (leanFrq) > fhx (zeroF))
	{
		tbAtt += fhx ((fhx (leanFac) * fhx (fhx (sRate*rateFac) / fhx (leanFrq))));
		iSpl += fhx (fhx ((fhx) sinl(fhx (fhx (tbAtt) * fhx (0.0750) * fhx (PI))) + fhx (0.0)) * fhx (fhx (iSpl) * fhx (0.50)));
	}

	iSpl *= (0.666660);

	return fhx (iSpl);
}
fhx DstTubeE::DirtySine(fhx inp)
{
	while(fhx (inp) > fhx (4.0)) inp -= fhx (4.0);
	if(fhx (inp) < fhx (0.0)) return fhx (0.0);

	if(fhx (inp) <= fhx (1.0))
	{
		inp *= fhx (0.50);
		inp += fhx (0.50);
		inp *= fhx (fhx (2.0) - fhx (inp));
		inp -= fhx (0.750);
		inp *= fhx (4.0);
		return fhx (inp);
	}
	if(fhx (inp) <= fhx (2.0))
	{
		inp = fhx (fhx (2.0) - fhx (inp));
		inp *= fhx (0.50);
		inp += fhx (0.50);
		inp *= fhx (fhx (2.0) - fhx (inp));
		inp -= fhx (0.750);
		inp *= fhx (4.0);
		return fhx (inp);
	}
	if(fhx (inp) <= fhx (3.0))
	{
		inp -= fhx (2.0);
		inp *= fhx (0.50);
		inp += fhx (0.50);
		inp *= fhx (fhx (2.0) - fhx (inp));
		inp -= fhx (0.750);
		inp *= fhx (4.0);
		return fhx (-inp);
	}
	if(fhx (inp) <= fhx (4.0))
	{
		inp = fhx (fhx (4.0) - fhx (inp));
		inp *= fhx (0.50);
		inp += fhx (0.50);
		inp *= fhx (fhx (2.0) - fhx (inp));
		inp -= fhx (0.750);
		inp *= fhx (4.0);
		return fhx (-inp);
	}

	return fhx (0.0);
}
