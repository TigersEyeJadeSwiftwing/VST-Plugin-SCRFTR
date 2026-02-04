#include "tube_d.h"

DstTubeD::DstTubeD(fsx inSRate) :
	PI((fhx) ((fhx) 4.00 * (fhx) atanl((fhx) 1.00))),
	hPI((fhx) ((fhx) PI * (fhx) 0.50)),
	PId(fhx (fhx (PI) * fhx (2.0))),
	zeroF(fhx (0.0000000000000000000000001)),		//! Used to avoid denormal-related issues in parts of the code
	sRate(fhx (inSRate)),
	oSampling(int (1)),
	rateFac(fhx (1.0)),
	leanT(fhx (0.0)),
	leanB(fhx (0.0)),
	lwCutFreq(fhx (20.0)),
	iFiltPos(fhx (0.0)),
	dBuffFac(fhx (1400.0))
{
	if(fhx (sRate) < fhx (44100.0)) sRate = fhx (44100.0);
	if(fhx (sRate) > fhx (1536000.0)) sRate = fhx (1536000.0);

	tubePullFac = fhx (fhx (440.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = fhx (fhx (800.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = (fhx) fabsl(fhx (leanFac));

	dsFilter = new BTWorthLP(fhx (7200.0), fsx (sRate * rateFac));
	// dsFilterL = new BTWorthLP_HD(fhx (15.0), float (sRate * rateFac));

	iFiltFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI * (fhx) lwCutFreq / fhx (sRate*rateFac)));

	dBuffRate = fhx (fhx (sRate) / fhx (dBuffFac));
	dBuffSize = s9x (fhx (fhx (dBuffRate) * fhx (256.0)));
	dBuffRate *= fhx (rateFac);
	dBuff[0] = new fhx[dBuffSize+1];
	dBuff[1] = new fhx[dBuffSize+1];
	dBuffOffSet = s9x (fhx (dBuffRate));

	dBuffPos[0] = s9x (0);
	dBuffPos[1] = s9x (0);
	dBuffDelayed[0] = s9x (-dBuffOffSet);
	dBuffDelayed[1] = s9x (-dBuffOffSet);
}
DstTubeD::~DstTubeD()
{
	delete dsFilter;
	delete [] dBuff[0];
	delete [] dBuff[1];
	// delete dsFilterL;
}
void DstTubeD::setSmpRate(fsx nRate)
{
	sRate = fhx (nRate);
	dsFilter->setSampleRate(fsx (sRate * rateFac));
	// dsFilterL->setSampleRate(float (sRate * rateFac));
	clearBuffs();
	iFiltFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI * (fhx) lwCutFreq / fhx (sRate*rateFac)));

	delete [] dBuff[0];
	delete [] dBuff[1];

	dBuffRate = fhx (fhx (sRate) / fhx (dBuffFac));
	dBuffSize = s9x (fhx (fhx (dBuffRate) * fhx (256.0)));
	dBuffRate *= fhx (rateFac);
	dBuff[0] = new fhx[dBuffSize+1];
	dBuff[1] = new fhx[dBuffSize+1];

	dBuffPos[0] = s9x (0);
	dBuffPos[1] = s9x (0);
	dBuffDelayed[0] = s9x (-dBuffOffSet);
	dBuffDelayed[1] = s9x (-dBuffOffSet);

	dBuffOffSet = s9x (fhx (dBuffRate));
}
void DstTubeD::setGain(const fsx nGain)
{
	gGain = fhx (nGain);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);
}
void DstTubeD::clearBuffs()
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
	dBuffPos[0] = s9x (0);
	dBuffPos[1] = s9x (0);
	dBuffDelayed[0] = s9x (-dBuffOffSet);
	dBuffDelayed[1] = s9x (-dBuffOffSet);
}
void DstTubeD::setOverSamplingRate(int inp)
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

	iFiltFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI * (fhx) lwCutFreq / fhx (sRate*rateFac)));

	dBuffRate = fhx (fhx (sRate) / fhx (dBuffFac));
	dBuffSize = s9x (fhx (fhx (dBuffRate) * fhx (256.0)));
	dBuffRate *= fhx (rateFac);

	dBuffOffSet = s9x (fhx (dBuffRate));

	clearBuffs();
}
fhx DstTubeD::applyDistortion(fhx inSample, const fhx gn)
{
	gGain = fhx (gn);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);

	gGain = fhx (fhx (1.0) - fhx (fhx (fhx (1.0) - fhx (gGain)) * fhx (fhx (1.0) - fhx (gGain))));

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

	//! inSample = (fhx) dsFilter->runFilter(fhx (inSample));

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
			// dBuffDelayed[topSide] = s9x (-dBuffOffSet);
			// dBuffPos[topSide] = s9x (0);
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
			// dBuffDelayed[topSide] = s9x (-dBuffOffSet);
			// dBuffPos[topSide] = s9x (0);
//			distP[0] = fhx (0.0);
//			distP[1] = fhx (0.0);
		}
		srPoint = fhx (-lastPoint);
		inSample = fhx (fhx (-1.0) * (fhx) runTube(fhx (-inSample)));
	}

	lastPoint = fhx (drySamp);

	dBuffPos[0] += s9x (1);
	dBuffPos[1] += s9x (1);
	dBuffDelayed[0] += s9x (1);
	dBuffDelayed[1] += s9x (1);

//	if(s9x (dBuffPos[0]) >= s9x (dBuffSize)) dBuffPos[0] = s9x (0);
//	if(s9x (dBuffPos[1]) >= s9x (dBuffSize)) dBuffPos[1] = s9x (0);
//	if(s9x (dBuffDelayed[0]) >= s9x (dBuffSize)) dBuffDelayed[0] = s9x (0);
//	if(s9x (dBuffDelayed[1]) >= s9x (dBuffSize)) dBuffDelayed[1] = s9x (0);

	// inSample += fhx (drySamp);
	// inSample *= fhx (0.50);

	return fhx (fhx (inSample) * fhx (0.999750));
}
fhx DstTubeD::runTube(fhx iSpl)
{
	const bool invSide =! topSide;
	if(s9x (dBuffPos[topSide]) >= s9x (dBuffOffSet)) dBuffPos[topSide] = s9x (0);
	if(s9x (dBuffDelayed[topSide]) >= s9x (dBuffOffSet)) dBuffDelayed[topSide] = s9x (0);
	if(s9x (dBuffPos[invSide]) >= s9x (dBuffOffSet)) dBuffPos[invSide] = s9x (0);
	if(s9x (dBuffDelayed[invSide]) >= s9x (dBuffOffSet)) dBuffDelayed[invSide] = s9x (0);

	fhx tPull = fhx (0.0);
	if(s9x (dBuffDelayed[topSide]) >= s9x (0)) tPull = fhx (dBuff[topSide][dBuffDelayed[topSide]]);
	dBuff[topSide][dBuffPos[topSide]] = fhx (iSpl);
	dBuff[invSide][dBuffPos[topSide]] = fhx (-iSpl);

	if(fhx (tPull) > fhx (1.0)) tPull = fhx (1.0);
	tPull *= fhx (gGain);
	tPull = fhx (fhx (1.0) - fhx (tPull));

	iSpl *= fhx (tPull);

	iSpl = fhx ((fhx) DirtySine(fhx (iSpl)));
/*
	if(fhx (distP[topSide]) < fhx (iSpl)) distP[topSide] += fhx (fhx (tubePullFac) * fhx (0.01136360));
	if(fhx (distP[topSide]) > fhx (iSpl)) distP[topSide] -= fhx (fhx (tubePullFac) * fhx (0.00227270));
	if(fhx (distP[topSide]) > fhx (1.0)) distP[topSide] = fhx (1.0);
	if(fhx (distP[topSide]) < fhx (0.0)) distP[topSide] = fhx (0.0);
	const bool invSide = (!topSide);
	distP[invSide] -= fhx (fhx (tubePullFac) * fhx (0.00227270));
	if(fhx (distP[invSide]) < fhx (0.0)) distP[invSide] = fhx (0.0);

	lean += fhx (fhx (tubePullFac) * fhx (0.250));
	if(fhx (lean) > fhx (1.0)) lean = fhx (1.0);
	const fhx lnPush = fhx (fhx ((fhx) DirtySine(fhx (fhx (lean) * fhx (3.0))) * fhx (0.450)) + fhx (0.550));

	dryPos = fhx (iSpl);
	iSpl = (fhx) DirtySine(fhx (iSpl));
	iSpl = (fhx) DirtySine(fhx (iSpl));
	iSpl = (fhx) DirtySine(fhx (iSpl));

	const fhx lnEff = fhx (fhx (iSpl) * fhx (iSpl));
	const fhx lnDry = fhx (fhx (iSpl) * fhx (fhx (1.0) - fhx (lnEff)));
	iSpl *= fhx (fhx (lnPush) * fhx (lnEff));
	iSpl += fhx (lnDry);

	iSpl *= fhx (gGain);
	dryPos *= fhx (fhx (1.0) - fhx (gGain));
	iSpl += fhx (dryPos);

	iSpl *= fhx (fhx (1.0) - fhx (fhx (distP[topSide]) * fhx (0.750)));
*/
	return fhx (iSpl);
}
fhx DstTubeD::DirtySine(fhx inp)
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
