#include "tube_c.h"

DstTubeC::DstTubeC(float inSRate) :
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

	const fhx tubeWDatUp[51] =
	{
	0.0158350368954091424,  //! index = 0
	0.0722038942426011776,  //! index = 1
	0.1282561147040311808,  //! index = 2
	0.1837292823978846976,  //! index = 3
	0.2383680563942953984,  //! index = 4
	0.2919273076485129216,  //! index = 5
	0.3442572122254406656,  //! index = 6
	0.3951363675211974656,  //! index = 7
	0.4443603552964174336,  //! index = 8
	0.4918963601769307136,  //! index = 9
	0.5374922639271961600,  //! index = 10
	0.5810693202548159488,  //! index = 11
	0.6224925736976146432,  //! index = 12
	0.6616504345515991040,  //! index = 13
	0.6985181075950607360,  //! index = 14
	0.7330830420982372352,  //! index = 15
	0.7651751868094675968,  //! index = 16
	0.7950457225440830464,  //! index = 17
	0.8224559262894717952,  //! index = 18
	0.8475676370395760640,  //! index = 19
	0.8704384971187477504,  //! index = 20
	0.8909747462853322752,  //! index = 21
	0.9093972619419898880,  //! index = 22
	0.9257268287617751040,  //! index = 23
	0.9401042153722877952,  //! index = 24
	0.9525537807535996928,  //! index = 25
	0.9632807401001791488,  //! index = 26
	0.9723311010855223296,  //! index = 27
	0.9798220506803462144,  //! index = 28
	0.9859389788449454080,  //! index = 29
	0.9907479987875958784,  //! index = 30
	0.9943924997621831680,  //! index = 31
	0.9970174157575845888,  //! index = 32
	0.9987348809001113600,  //! index = 33
	0.9996226717694308352,  //! index = 34
	0.9999312786093772800,  //! index = 35
	0.9999955756973217792,  //! index = 36
	1.0000000000000000000,  //! index = 37
	0.9999990202230659072,  //! index = 38
	0.9999823028284360704,  //! index = 39
	0.9999172980023572480,  //! index = 40
	0.9997683988207638528,  //! index = 41
	0.9995040288494227456,  //! index = 42
	0.9991035766839646208,  //! index = 43
	0.9985433601684183040,  //! index = 44
	0.9978306971479136256,  //! index = 45
	0.9969358397714780160,  //! index = 46
	0.9958632969796765696,  //! index = 47
	0.9946214278810857472,  //! index = 48
	0.9931765232790011904,  //! index = 49
	0.9915492234944368640   //! index = 50
	};

	const fhx tubeWDatDn[51] =
	{
	0.0558770229921031168,  //! index = 0
	0.1119272160549443456,  //! index = 1
	0.1675370281883193856,  //! index = 2
	0.2224478220534523136,  //! index = 3
	0.2764091256239861248,  //! index = 4
	0.3292643121774610432,  //! index = 5
	0.3808640111122849280,  //! index = 6
	0.4309874724305785344,  //! index = 7
	0.4793536944597784576,  //! index = 8
	0.5258588370747809792,  //! index = 9
	0.5701952310038209536,  //! index = 10
	0.6124451559290174464,  //! index = 11
	0.6522842534161434624,  //! index = 12
	0.6896375653051956224,  //! index = 13
	0.7242753610621925376,  //! index = 14
	0.7560880275989731328,  //! index = 15
	0.7847451603273974784,  //! index = 16
	0.8101263368762491904,  //! index = 17
	0.8318461243555050496,  //! index = 18
	0.8494196376805168128,  //! index = 19
	0.8624012931197076480,  //! index = 20
	0.8718133033157694464,  //! index = 21
	0.8795854992526516224,  //! index = 22
	0.8865233146205645824,  //! index = 23
	0.8929130831909541888,  //! index = 24
	0.8989746557225029632,  //! index = 25
	0.9047197395653889024,  //! index = 26
	0.9102686073048240128,  //! index = 27
	0.9155178827983869952,  //! index = 28
	0.9206157279763795968,  //! index = 29
	0.9255612996348422144,  //! index = 30
	0.9302896067656265728,  //! index = 31
	0.9348681987768780800,  //! index = 32
	0.9392963388180017152,  //! index = 33
	0.9435153527489640448,  //! index = 34
	0.9475866829359611904,  //! index = 35
	0.9514558503860645888,  //! index = 36
	0.9552320116416546816,  //! index = 37
	0.9589083145592868864,  //! index = 38
	0.9624302966375899136,  //! index = 39
	0.9658427482557270016,  //! index = 40
	0.9690954456244852736,  //! index = 41
	0.9722083373966667776,  //! index = 42
	0.9751969715768936448,  //! index = 43
	0.9779994061102522368,  //! index = 44
	0.9806870126454536192,  //! index = 45
	0.9832011874684200960,  //! index = 46
	0.9855414859544768512,  //! index = 47
	0.9877211663226793984,  //! index = 48
	0.9897238651183343616,  //! index = 49
	0.9915492234944368640   //! index = 50
	};

	int x = 0;
	cnvFac = fhx (0.0);
	for(; x < 51; x++)
	{
		tBuff[x+1] = fhx (tubeWDatUp[x]);
		cnvFac += fhx (tBuff[x+1]);
	}
	for(x=0; x < 51; x++)
	{
		tBuff[x+52] = fhx (tubeWDatDn[50-x]);
		cnvFac += fhx (tBuff[x+52]);
	}
	tBuff[103] = fhx (0.0);
	cnvFac = fhx (fhx (1.0) / fhx (cnvFac));
}
DstTubeC::~DstTubeC()
{
	delete dsFilter;
}
void DstTubeC::setSmpRate(float nRate)
{
	sRate = fhx (nRate);
	dsFilter->setSampleRate(float (sRate * rateFac));
	clearBuffs();
}
void DstTubeC::setGain(const float nGain)
{
	gGain = fhx (nGain);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);
}
void DstTubeC::clearBuffs()
{
	dsFilter->zeroBuffers();
	lastPoint = fhx (0.0);
	distP = fhx (0.0);
	leanT = fhx (0.0);
	leanB = fhx (0.0);
	lean = fhx (0.0);
	topSide = true;

	int x = 0;
	for(; x < 128; x++)
	{
		sBuff[x] = fhx (0.0);
	}
	sBuffIndex = 0;
	tBuffIndex = 0;
}
void DstTubeC::setOverSamplingRate(int inp)
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
fhx DstTubeC::applyDistortion(fhx inSample, fhx (gn))
{
	gGain = fhx (gn);
	if(fhx (gGain) < fhx (zeroF)) gGain = fhx (zeroF);
	if(fhx (gGain) > fhx (1.0)) gGain = fhx (1.0);

	if(fhx (inSample) > fhx (1.0)) inSample = fhx (1.0);
	if(fhx (inSample) < fhx (-1.0)) inSample = fhx (-1.0);

	sBuff[sBuffIndex] = fhx (inSample);
	sBuffIndex++;
	if(sBuffIndex == 128) sBuffIndex = 0;

	dryPos = fhx (inSample);
	//! inSample = (fhx) dsFilter->runFilter(fhx (inSample));

	if(fhx (inSample) == fhx (0.0)) inSample = fhx (zeroF);

	inSample = (fhx) runTube(fhx (inSample));

	dryPos *= fhx (fhx (1.0) - fhx (gGain));
	inSample *= fhx (gGain);
	inSample += fhx (dryPos);

	return fhx (fhx (inSample) * fhx (0.9750));
}
fhx DstTubeC::runTube(fhx iSpl)
{
	if(fhx (iSpl) < fhx (0.0)) iSpl = fhx (-iSpl);
	if(fhx (iSpl) > fhx (1.0)) iSpl = fhx (1.0);

	fhx sAdd = fhx (0.0);
	int y = sBuffIndex;
	for(int x = 0; x < 104; x++)
	{
		sAdd += fhx (fhx (tBuff[x]) * fhx (sBuff[y]));
		y--;
		if(y < 0) y = 127;
	}
	sAdd *= fhx (cnvFac);

	return fhx (sAdd);
}
