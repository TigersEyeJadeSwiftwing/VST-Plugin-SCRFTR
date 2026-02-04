#include "tube_b.h"

DstTubeB::DstTubeB(fsx inSRate) :
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
	dsFilterD = new BTWorthLP(fhx (360.0), fsx (sRate * rateFac));
	// dsFilterL = new BTWorthLP_HD(fhx (15.0), float (sRate * rateFac));

	iFiltFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI * (fhx) lwCutFreq / fhx (sRate*rateFac)));
//
	const fhx kFactor = fhx (fhx (44100.0) / fhx (fhx (sRate) * fhx (rateFac)));
	kb = fhx (fhx (0.00750) * fhx (kFactor));
	ka = fhx (fhx (1.0) - fhx (kb));
	lim = fhx (0.40);
	tgt_drve = fhx (1.0);
	drve = fhx (1.0);
	drvo = fhx (1.0);
	kr = fhx (0.0);
	kabs = fhx (0.0);
	trim = fhx ((fhx) powl(fhx (2.0), fhx (fhx (-3.50)) / fhx (6.0)));
	dcFa = fhx (0.0);
	dcFb = fhx (0.0);
	dcFfa = fhx (0.0);
	dcFfb = fhx (0.0);
	fir0 = fhx (0.0);
	lim0 = fhx (0.0);
}
DstTubeB::~DstTubeB()
{
	delete dsFilter;
	delete dsFilterD;
}
void DstTubeB::setSmpRate(fsx nRate)
{
	sRate = fhx (nRate);
	dsFilter->setSampleRate(fsx (sRate * rateFac));
	dsFilterD->setSampleRate(fsx (sRate * rateFac));
	// dsFilterL->setSampleRate(float (sRate * rateFac));
	clearBuffs();
	iFiltFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI * (fhx) lwCutFreq / fhx (sRate*rateFac)));
	leanFac = fhx (fhx (1.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = (fhx) fabsl(fhx (leanFac));

	const fhx kFactor = fhx (fhx (44100.0) / fhx (fhx (sRate) * fhx (rateFac)));
	kb = fhx (fhx (0.00750) * fhx (kFactor));
	ka = fhx (fhx (1.0) - fhx (kb));
}
void DstTubeB::setGain(const fsx nGain)
{
	gGain = fhx (nGain);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);
}
void DstTubeB::clearBuffs()
{
	dsFilter->zeroBuffers();
	dsFilterD->zeroBuffers();
	lastPoint = fhx (0.0);
	distP[0] = fhx (0.0);
	distP[1] = fhx (0.0);
	leanT = fhx (0.0);
	leanB = fhx (0.0);
	lean = fhx (0.0);
	topSide = true;
	iFiltPos = fhx (0.0);
}
void DstTubeB::setOverSamplingRate(int inp)
{
	if(inp == 0) rateFac = fhx (1.0);
	if(inp == 1) rateFac = fhx (1.0);
	if(inp == 2) rateFac = fhx (2.0);
	if(inp == 3) rateFac = fhx (4.0);
	if(inp == 4) rateFac = fhx (8.0);
	if(inp == 5) rateFac = fhx (16.0);
	if(inp == 6) rateFac = fhx (1.0);

	tubePullFac = fhx (fhx (440.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = fhx (fhx (1.0) / fhx (fhx (rateFac) * fhx (sRate)));
	leanFac = (fhx) fabsl(fhx (leanFac));

	dsFilter->setSampleRate(fsx (sRate * rateFac));
	dsFilterD->setSampleRate(float (sRate * rateFac));
	clearBuffs();
	iFiltFreq = fhx ((fhx) 1.0 - (fhx) expl((fhx) -2.0 * (fhx) PI * (fhx) lwCutFreq / fhx (sRate*rateFac)));

	const fhx kFactor = fhx (fhx (44100.0) / fhx (fhx (sRate) * fhx (rateFac)));
	kb = fhx (fhx (0.00750) * fhx (kFactor));
	ka = fhx (fhx (1.0) - fhx (kb));
}
fhx DstTubeB::applyDistortion(fhx inSample, const fhx gn)
{
//	gGain = fhx (gn);
//	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
//	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);

//	gGain = fhx (fhx (1.0) - fhx (fhx (fhx (1.0) - fhx (gGain)) * fhx (fhx (1.0) - fhx (gGain))));

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

	// drySamp = fhx (inSample);

	if(fhx (inSample) == fhx (0.0)) inSample = fhx (zeroF);

	// drySamp = fhx (inSample);
//	inSample = (fhx) dsFilter->runFilter(fhx (inSample));
//	drySamp = (fhx) dsFilterD->runFilter(fhx (drySamp));

	// const fhx dsThresh = fhx (0.250);
/*
	if(fhx (inSample) > fhx (0.0))
	{
		if(topSide == false)
		{
			topSide = true;
//			lean = fhx (0.0);
//			dsWave[0] = fhx (0.0);
//			dsWave[1] = fhx (0.0);
//			dsWave[2] = fhx (0.0);
//			distP[0] = fhx (0.0);
//			distP[1] = fhx (0.0);
		}
//		srPoint = fhx (lastPoint);
		inSample = (fhx) runTube(fhx (inSample));
		// if(fhx (inSample) > fhx (dsThresh)) inSample += fhx (fhx (drySamp) * fhx (0.1250));
	}
	else
	{
		if(topSide == true)
		{
			topSide = false;
//			lean = fhx (0.0);
//			dsWave[0] = fhx (0.0);
//			dsWave[1] = fhx (0.0);
//			dsWave[2] = fhx (0.0);
//			distP[0] = fhx (0.0);
//			distP[1] = fhx (0.0);
		}
//		srPoint = fhx (-lastPoint);
		inSample = fhx (fhx (-1.0) * (fhx) runTube(fhx (-inSample)));
		// if(fhx (inSample) < fhx (-dsThresh)) inSample += fhx (fhx (drySamp) * fhx (0.1250));
	}
*/
	//! inSample = (fhx) dsFilter->runFilter(fhx (inSample));

//	inSample *= fhx ((fhx) fabsl(drySamp) + fhx (1.0));

//	lastPoint = fhx (drySamp);

	// inSample += fhx (drySamp);
	// inSample *= fhx (0.50);

	const fhx hgain = fhx (fhx (gn) * fhx (16.0));
	const fhx ch0 = fhx (inSample);
	const fhx drve_rnd0 = fhx (1.0);
	const fhx drvo_rnd0 = fhx (1.0);
	const fhx h0 = fhx (sinl(ch0 * drve_rnd0) / sinl(ch0) * drve + (tanl(ch0) * drvo_rnd0) * drvo * fhx (0.3));
	dcFa = fhx (fhx (h0) - fhx (dcFfa = fhx (h0*kb+dcFfa*ka)));
	lim0 = (fhx) fminl(fmaxl(dcFa*hgain,fhx (-lim)),lim);
	m01 = fhx (m00);
	m03 = fhx (m02);
	fir0 = fhx (fhx (0.5) * fhx (fhx (m03) + fhx (m02 = (fhx) fhx (0.50) * fhx (m01 + fhx (m00 = (fhx) lim0)))));
	dcFb = fhx (fir0 - fhx (dcFfb = (fhx) fir0*kb+dcFfb*ka));

	inSample = fhx (fhx (fhx (inSample) + fhx (dcFb)) * fhx (trim));

	return fhx (fhx (inSample) * fhx (0.99750));
}
fhx DstTubeB::runTube(fhx iSpl)
{
	const fhx gFac = fhx (fhx (100.0) * fhx (gGain));
	dryPos = fhx (iSpl);
	iSpl = fhx (fhx (fhx (fhx (1.0) + fhx (gFac)) * fhx (iSpl)) / fhx (fhx (1.0) + fhx (fhx (gFac) * fhx (iSpl))));
	iSpl += fhx ((fhx) DirtySine(fhx (fhx (dryPos) * fhx (2.0))));
	iSpl *= fhx (0.50);
/*
	iSpl *= fhx (gGain);
	dsWave[0] += fhx (fhx (leanFac) * fhx (3.0) * fhx (100.0));
	dsWave[1] += fhx (fhx (leanFac) * fhx (3.0) * fhx (200.0));
	dsWave[2] += fhx (fhx (leanFac) * fhx (3.0) * fhx (400.0));
	if(fhx (dsWave[0]) > fhx (3.0)) dsWave[0] = fhx (3.0);
	if(fhx (dsWave[1]) > fhx (3.0)) dsWave[1] = fhx (3.0);
	if(fhx (dsWave[2]) > fhx (3.0)) dsWave[2] = fhx (3.0);

	const fhx mfA = fhx (fhx ((fhx) DirtySine(fhx (dsWave[0])) + fhx (1.0)) * fhx (0.50));
	const fhx mfB = fhx (fhx ((fhx) DirtySine(fhx (dsWave[1])) + fhx (1.0)) * fhx (0.50));
	const fhx mfC = fhx (fhx ((fhx) DirtySine(fhx (dsWave[2])) + fhx (1.0)) * fhx (0.50));

	iSpl += fhx (fhx (iSpl) * fhx (mfA) * fhx (mfB) * fhx (mfC));
	iSpl *= (0.50);
*/
/*
	if(fhx (distP[topSide]) < fhx (iSpl)) distP[topSide] += fhx (fhx (tubePullFac) * fhx (0.01136360));
	if(fhx (distP[topSide]) > fhx (iSpl)) distP[topSide] -= fhx (fhx (tubePullFac) * fhx (0.00227270));
	if(fhx (distP[topSide]) > fhx (1.0)) distP[topSide] = fhx (1.0);
	if(fhx (distP[topSide]) < fhx (0.0)) distP[topSide] = fhx (0.0);
	const bool invSide = (!topSide);
	distP[invSide] -= fhx (fhx (tubePullFac) * fhx (0.00227270));
	if(fhx (distP[invSide]) < fhx (0.0)) distP[invSide] = fhx (0.0);
*/
	// iSpl *= fhx (fhx (1.0) - fhx (fhx (distP[topSide]) * fhx (0.950)));
/*
	const fhx mDrive = fhx (fhx (gGain) * fhx (gGain) * fhx (10.0));
	const fhx smSpl = fhx (fhx (iSpl) * fhx (fhx (2.0) - fhx (iSpl)) * fhx (1.0));
	dryPos = fhx (iSpl);
	iSpl *= fhx (iSpl);
	iSpl += fhx (dryPos);
	iSpl *= fhx (5.0);
	const fhx dCut = fhx (fhx (fhx (fhx (fhx (1.0) + fhx (mDrive)) * fhx (10.0)) / fhx (fhx (1.0) + fhx (fhx (mDrive) * fhx (10.0)))) + fhx (smSpl));
	iSpl = fhx (fhx (fhx (fhx (fhx (1.0) + fhx (mDrive)) * fhx (iSpl)) / fhx (fhx (1.0) + fhx (fhx (mDrive) * fhx (iSpl)))) + fhx (smSpl));
	iSpl /= fhx (dCut);
	iSpl += fhx (dryPos);
	iSpl *= fhx (0.50);
*/
//	lean += fhx (fhx (tubePullFac) * fhx (0.250));
//	if(fhx (lean) > fhx (1.0)) lean = fhx (1.0);
//	const fhx lnPush = fhx (fhx ((fhx) DirtySine(fhx (fhx (lean) * fhx (2.0))) * fhx (0.3750)) + fhx (0.6250));
//	const fhx lnPush = fhx ((fhx) DirtySine(fhx (fhx (lean) * fhx (2.0))));

	// iSpl *= fhx (fhx (1.0) - fhx (fhx (distP[topSide]) * fhx (0.9250)));

//	iSpl *= fhx (0.750);
	// dryPos = fhx (iSpl);
/*
	if(fhx (iSpl) > fhx (0.20))
	{
		iSpl -= fhx (0.20);
		iSpl *= fhx (1.250);
		iSpl = (fhx) DirtySine(fhx (iSpl));
		iSpl *= fhx (0.50);
		iSpl += fhx (0.20);
	}
	iSpl /= fhx (0.70);
	if(fhx (iSpl) > fhx (0.20))
	{
		iSpl -= fhx (0.20);
		iSpl *= fhx (1.250);
		iSpl = (fhx) DirtySine(fhx (iSpl));
		iSpl *= fhx (0.50);
		iSpl += fhx (0.20);
	}
	iSpl /= fhx (0.70);
	if(fhx (iSpl) > fhx (0.20))
	{
		iSpl -= fhx (0.20);
		iSpl *= fhx (1.250);
		iSpl = (fhx) DirtySine(fhx (iSpl));
		iSpl *= fhx (0.50);
		iSpl += fhx (0.20);
	}
	iSpl /= fhx (0.70);
*/
/*
	const fhx mtFac = fhx (0.50);
	const fhx mtFacB = fhx (0.50);

	iSpl += fhx ((fhx) DirtySine(fhx (fhx (dryPos) * fhx (3.0))) * fhx (dryPos) * fhx (mtFac));
	iSpl += fhx ((fhx) DirtySine(fhx (fhx (dryPos) * fhx (5.0))) * fhx (dryPos) * fhx (fhx (mtFac) * fhx (mtFac)));
	iSpl += fhx ((fhx) DirtySine(fhx (fhx (dryPos) * fhx (7.0))) * fhx (dryPos) * fhx (fhx (mtFac) * fhx (mtFac) * fhx (mtFac)));

	iSpl -= fhx ((fhx) DirtySine(fhx (fhx (dryPos) * fhx (2.0))) * fhx (dryPos) * fhx (mtFacB));
	iSpl -= fhx ((fhx) DirtySine(fhx (fhx (dryPos) * fhx (4.0))) * fhx (dryPos) * fhx (fhx (mtFacB) * fhx (mtFacB)));
	iSpl -= fhx ((fhx) DirtySine(fhx (fhx (dryPos) * fhx (6.0))) * fhx (dryPos) * fhx (fhx (mtFacB) * fhx (mtFacB) * fhx (mtFacB)));
	iSpl *= fhx (2.0);
*/
//	iSpl += fhx ((fhx) DirtySine(fhx (dryPos)) * fhx (0.50));

	// iSpl = (fhx) DirtySine(fhx (iSpl));
	// iSpl = (fhx) DirtySine(fhx (iSpl));

//	iSpl *= fhx (fhx (1.0) - fhx (fhx (distP[topSide]) * fhx (0.750)));
//	dryPos *= fhx (fhx (1.0) - fhx (fhx (distP[topSide]) * fhx (0.750)));
/*
	const fhx lnEff = fhx (iSpl);
	const fhx lnDry = fhx (fhx (iSpl) * fhx (fhx (1.0) - fhx (lnEff)));
	iSpl *= fhx (fhx (lnPush) * fhx (lnEff));
	iSpl += fhx (lnDry);
*/
	// iSpl *= fhx (lnPush);

//	const fhx lnPull = fhx (fhx (1.0) - fhx (lnPush));

//	iSpl *= fhx (lnPull);
//	dryPos *= fhx (fhx (1.0) - fhx (lnPull));
//	iSpl += fhx (dryPos);

	// iSpl *= fhx (fhx (1.0) - fhx (fhx (distP[topSide]) * fhx (0.750)));

	// iSpl *= fhx (fhx (1.0) - fhx (fhx (distP[topSide]) * fhx (0.750)));
/*
	const fhx vTrav = fhx (fhx (iSpl) - fhx (srPoint));

	if(fhx (distP[topSide]) < fhx (iSpl)) distP[topSide] += fhx (fhx (tubePullFac) * fhx (6.0));
	if(fhx (distP[topSide]) > fhx (iSpl)) distP[topSide] -= fhx (fhx (tubePullFac) * fhx (3.0));
	if(fhx (distP[topSide]) > fhx (1.0)) distP[topSide] = fhx (1.0);
	if(fhx (distP[topSide]) < fhx (-1.0)) distP[topSide] = fhx (-1.0);

	fhx wPush = fhx (distP[topSide]);
	if(fhx (wPush) >= fhx (0.0)) wPush *= fhx (wPush);
	else wPush *= fhx (-wPush);

	const fhx vEffect = fhx (fhx (1.50) - fhx (fhx (wPush) * fhx (1.0)));

	iSpl *= fhx (vEffect);
*/
/*
	if(fhx (iSpl) > fhx (lean)) lean += fhx (fhx (tubePullFac) * fhx (0.00500));
	else lean -= fhx (fhx (tubePullFac) * fhx (0.00450));
	if(fhx (lean) < fhx (0.0)) lean = fhx (0.0);
	if(fhx (lean) > fhx (1.0)) lean = fhx (1.0);
*/
	// iSpl *= fhx (fhx (2.0) - fhx (iSpl));

//	iSpl *= fhx (gGain);
/*
	iSpl *= fhx (gGain);

	if(fhx (distP[topSide]) < fhx (iSpl)) distP[topSide] += fhx (fhx (tubePullFac) * fhx (0.750));
	if(fhx (distP[topSide]) > fhx (iSpl)) distP[topSide] -= fhx (fhx (tubePullFac) * fhx (1.000));
	if(fhx (distP[topSide]) > fhx (1.0)) distP[topSide] = fhx (1.0);
	if(fhx (distP[topSide]) < fhx (0.0)) distP[topSide] = fhx (0.0);

	const fhx pushDwn = fhx (fhx (2.0) - fhx (fhx (1.0) * fhx (distP[topSide])));

	iSpl *= fhx (pushDwn);
	iSpl *= fhx (0.50);

	if(fhx (iSpl) > fhx (lean)) lean += fhx (fhx (tubePullFac) * fhx (0.00450));
	else lean -= fhx (fhx (tubePullFac) * fhx (0.0500));
	if(fhx (lean) < fhx (0.0)) lean = fhx (0.0);
	if(fhx (lean) > fhx (1.0)) lean = fhx (1.0);

	// iSpl *= fhx (fhx (2.0) - fhx (iSpl));

	dryPos = fhx (iSpl);
	fhx sDwn = fhx (fhx (fhx (fhx (lean) * fhx (1.0250))) - fhx (0.0250));
	if(fhx (sDwn) < fhx (0.0)) sDwn = fhx (fhx (10.0) * fhx (-sDwn));

	const fhx vSpl = fhx (fhx (1.0) - fhx (fhx (iSpl) * fhx (iSpl)));
	const fhx tSpl = fhx (fhx (1.0) - fhx (fhx (vSpl) * fhx (vSpl)));
	iSpl *= fhx (fhx (2.0) - fhx (tSpl));

	iSpl *= fhx (fhx (1.0) - fhx (dryPos));
	dryPos *= fhx (dryPos);
	iSpl += fhx (dryPos);
*/
/*
	if(fhx (distP[topSide]) < fhx (iSpl)) distP[topSide] += fhx (fhx (tubePullFac) * fhx (2.500));
	if(fhx (distP[topSide]) > fhx (iSpl)) distP[topSide] -= fhx (fhx (tubePullFac) * fhx (4.000));
	if(fhx (distP[topSide]) > fhx (1.0)) distP[topSide] = fhx (1.0);
	if(fhx (distP[topSide]) < fhx (0.0)) distP[topSide] = fhx (0.0);

	iSpl *= fhx (gGain);
	dryPos = fhx (iSpl);
	// dryPos = fhx (fhx (iSpl) * fhx (iSpl));
	fhx sDwn = fhx (fhx (fhx (fhx (distP[topSide]) * fhx (1.0250))) - fhx (0.0250));
	if(fhx (sDwn) < fhx (0.0)) sDwn = fhx (fhx (10.0) * fhx (-sDwn));
	// sDwn *= fhx (sDwn);

	const fhx vSpl = fhx (fhx (1.0) - fhx (fhx (iSpl) * fhx (iSpl)));
	const fhx tSpl = fhx (fhx (1.0) - fhx (fhx (vSpl) * fhx (vSpl)));
	iSpl *= fhx (fhx (2.0) - fhx (tSpl));

	iSpl *= fhx (fhx (1.0) - fhx (sDwn));
	dryPos *= fhx (sDwn);
	iSpl += fhx (dryPos);
*/
/*
	const fhx sTravel = fhx (fhx (iSpl) - fhx (srPoint));
	const fhx dTravel = fhx (fabsl(fhx (iSpl) - fhx (distP[topSide])));
	if(fhx (sTravel) > fhx (0.0))
	{
		distP[topSide] += fhx (fhx (dTravel) * fhx (fhx (1.0) - fhx (sTravel)));
	} else
	{
		distP[topSide] -= fhx (fhx (dTravel) * fhx (fhx (1.0) + fhx (sTravel)));
	}

	iSpl *= fhx (fhx (1.0) - fhx (gGain));
	const fhx distPos = fhx (fhx (distP[topSide]) * fhx (gGain));
	iSpl += fhx (distPos);
*/
/*
	lean += fhx (fhx (leanFac) * fhx (200.0));
	if(fhx (lean) > fhx (1.0)) lean = fhx (1.0);
	iSpl *= fhx (fhx (1.0) - fhx ((fhx) DirtySine(fhx (lean)) * fhx (0.50)));
*/
	return fhx (iSpl);
}
fhx DstTubeB::DirtySine(fhx inp)
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
