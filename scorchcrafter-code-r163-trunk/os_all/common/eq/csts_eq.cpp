#include "csts_eq.h"

using namespace ScMath;

Cst_EQ::Cst_EQ(bool setSingle) :
	sampL(0.0f),
	sampS(0.0),
	sampH(0.0),
	sRateL(44100.0f),
	sRateS(44100.0),
	sRateH(44100.0),
	runSingle(setSingle),
	freqLow(fsx (100.0)),
	freqHigh(fsx (3000.0))
{
	rateV = new RateFC();
	ClearBuffers();
	SetBands(100.0, 3000.0);
}
void Cst_EQ::SetRunSingle(const bool inp)
{
	runSingle = inp;
}
void Cst_EQ::ClearBuffers()
{
	sampL = 0.f;
	sampS = 0.0;
	sampH = 0.0;

	for (unsigned int x = 0; x < 3; x++)
	{
		qPaL[0][x] = 0.f;
		qPaL[1][x] = 0.f;

		qPaS[0][x] = 0.0;
		qPaS[1][x] = 0.0;

		qPaH[0][x] = 0.0;
		qPaH[1][x] = 0.0;
	}
}
void Cst_EQ::SetSmpRate(const fsx nRate)
{
	sRateL = (flx) nRate;
	sRateS = (fsx) nRate;
	sRateH = (fhx) nRate;

	SetBands(freqLow, freqHigh);

	ClearBuffers();
}
void Cst_EQ::SetBands(fsx lo, fsx hi)
{
	freqLow = fsx (lo);
	freqHigh = fsx (hi);

	if (hi > sRateS) hi = sRateS;
	// if (lo > hi) lo = hi;

	fhx bandFreq;

	bandFreq = (fhx) expl(fhx (-2.0) * fhx (M_PI) * hi / sRateS);
	pValLowH = fhx (fhx (1.0) - bandFreq);
	bValLowH = fhx (-bandFreq);
	pValLowS = fsx (pValLowH);
	bValLowS = fsx (bValLowH);
	pValLowL = flx (pValLowH);
	bValLowL = flx (bValLowH);

	bandFreq = (fhx) expl(fhx (-2.0) * fhx (M_PI) * lo / sRateS);
	pValHiH = fhx (fhx (1.0) - bandFreq);
	bValHiH = fhx (-bandFreq);
	pValHiS = fsx (pValHiH);
	bValHiS = fsx (bValHiH);
	pValHiL = flx (pValHiH);
	bValHiL = flx (bValHiH);
}
void Cst_EQ::l_InputSmpl(const flx inp) {sampL = flx (inp);}
void Cst_EQ::s_InputSmpl(const fsx inp) {sampS = fsx (inp);}
void Cst_EQ::h_InputSmpl(const fhx inp) {sampH = fhx (inp);}
flx Cst_EQ::l_GetOutput() {return sampL;}
fsx Cst_EQ::s_GetOutput() {return sampS;}
fhx Cst_EQ::h_GetOutput() {return sampH;}
void Cst_EQ::RunLPL()
{
	sampL = flx (qPaL[0][0] = flx (pValLowL * sampL - bValLowL * qPaL[0][0] + rateV->zl));
	if (runSingle) return;
	sampL = flx (qPaL[0][1] = flx (pValLowL * sampL - bValLowL * qPaL[0][1] + rateV->zl));
	sampL = flx (qPaL[0][2] = flx (pValLowL * sampL - bValLowL * qPaL[0][2] + rateV->zl));
}
void Cst_EQ::RunHPL()
{
	sampL -= flx (qPaL[1][0] = flx (pValHiL * sampL - bValHiL * qPaL[1][0] + rateV->zl));
	if (runSingle) return;
	sampL -= flx (qPaL[1][1] = flx (pValHiL * sampL - bValHiL * qPaL[1][1] + rateV->zl));
	sampL -= flx (qPaL[1][2] = flx (pValHiL * sampL - bValHiL * qPaL[1][2] + rateV->zl));
}
void Cst_EQ::RunLPS()
{
	sampS = fsx (qPaS[0][0] = fsx ((pValLowS * sampS) - (bValLowS * qPaS[0][0]) + rateV->zs));
	if (runSingle) return;
	sampS = fsx (qPaS[0][1] = fsx ((pValLowS * sampS) - (bValLowS * qPaS[0][1]) + rateV->zs));
	sampS = fsx (qPaS[0][2] = fsx ((pValLowS * sampS) - (bValLowS * qPaS[0][2]) + rateV->zs));
}
void Cst_EQ::RunHPS()
{
	sampS -= fsx (qPaS[1][0] = fsx ((pValHiS * sampS) - (bValHiS * qPaS[1][0]) + rateV->zs));
	if (runSingle) return;
	sampS -= fsx (qPaS[1][1] = fsx ((pValHiS * sampS) - (bValHiS * qPaS[1][1]) + rateV->zs));
	sampS -= fsx (qPaS[1][2] = fsx ((pValHiS * sampS) - (bValHiS * qPaS[1][2]) + rateV->zs));
}
void Cst_EQ::RunLPH()
{
	sampH = fhx (qPaH[0][0] = fhx (pValLowH * sampH - bValLowH * qPaH[0][0] + h_C_deNorm));
	if (runSingle) return;
	sampH = fhx (qPaH[0][1] = fhx (pValLowH * sampH - bValLowH * qPaH[0][1] + h_C_deNorm));
	sampH = fhx (qPaH[0][2] = fhx (pValLowH * sampH - bValLowH * qPaH[0][2] + h_C_deNorm));
}
void Cst_EQ::RunHPH()
{
	sampH -= fhx (qPaH[1][0] = fhx (pValHiH * sampH - bValHiH * qPaH[1][0] + h_C_deNorm));
	if (runSingle) return;
	sampH -= fhx (qPaH[1][1] = fhx (pValHiH * sampH - bValHiH * qPaH[1][1] + h_C_deNorm));
	sampH -= fhx (qPaH[1][2] = fhx (pValHiH * sampH - bValHiH * qPaH[1][2] + h_C_deNorm));
}
void Cst_EQ::RunQL()
{
	RunHPL();
	RunLPL();
}
void Cst_EQ::RunQS()
{
	RunHPS();
	RunLPS();
}
void Cst_EQ::RunQH()
{
	RunHPH();
	RunLPH();
}
void Cst_EQ::l_ForceBufs(const flx inp)
{
	for (int x = 0; x < 3; x++)
	{
		qPaL[0][x] = inp;
		qPaL[1][x] = inp;
	}
}
void Cst_EQ::s_ForceBufs(const fsx inp)
{
	for (int x = 0; x < 3; x++)
	{
		qPaS[0][x] = inp;
		qPaS[1][x] = inp;
	}
}
void Cst_EQ::h_ForceBufs(const fhx inp)
{
	for (int x = 0; x < 3; x++)
	{
		qPaH[0][x] = inp;
		qPaH[1][x] = inp;
	}
}
void Cst_EQ::l_Run(const bool lowP, const bool highP)
{
	if (highP && lowP)
	{
		sampL -= flx (qPaL[1][0] = flx (pValHiL * sampL - bValHiL * qPaL[1][0] + l_C_deNorm));
		sampL = flx (qPaL[0][0] = flx (pValLowL * sampL - bValLowL * qPaL[0][0] + l_C_deNorm));
		if (runSingle) return;
		sampL -= flx (qPaL[1][1] = flx (pValHiL * sampL - bValHiL * qPaL[1][1] + l_C_deNorm));
		sampL = flx (qPaL[0][1] = flx (pValLowL * sampL - bValLowL * qPaL[0][1] + l_C_deNorm));
		sampL -= flx (qPaL[1][2] = flx (pValHiL * sampL - bValHiL * qPaL[1][2] + l_C_deNorm));
		sampL = flx (qPaL[0][2] = flx (pValLowL * sampL - bValLowL * qPaL[0][2] + l_C_deNorm));

		return;
	} else
	if (highP) RunHPL();
	else
	if (lowP) RunLPL();
}
void Cst_EQ::s_Run(const bool lowP, const bool highP)
{
	if (highP && lowP)
	{
		sampS -= fsx (qPaS[1][0] = fsx (pValHiS * sampS - bValHiS * qPaS[1][0] + s_C_deNorm));
		sampS = fsx (qPaS[0][0] = fsx (pValLowS * sampS - bValLowS * qPaS[0][0] + s_C_deNorm));
		if (runSingle) return;
		sampS -= fsx (qPaS[1][1] = fsx (pValHiS * sampS - bValHiS * qPaS[1][1] + s_C_deNorm));
		sampS = fsx (qPaS[0][1] = fsx (pValLowS * sampS - bValLowS * qPaS[0][1] + s_C_deNorm));
		sampS -= fsx (qPaS[1][2] = fsx (pValHiS * sampS - bValHiS * qPaS[1][2] + s_C_deNorm));
		sampS = fsx (qPaS[0][2] = fsx (pValLowS * sampS - bValLowS * qPaS[0][2] + s_C_deNorm));

		return;
	} else
	if (highP) RunHPS();
	else
	if (lowP) RunLPS();
}
void Cst_EQ::h_Run(const bool lowP, const bool highP)
{
	if (highP && lowP)
	{
		sampH -= fhx (qPaH[1][0] = fhx (pValHiH * sampH - bValHiH * qPaH[1][0] + h_C_deNorm));
		sampH = fhx (qPaH[0][0] = fhx (pValLowH * sampH - bValLowH * qPaH[0][0] + h_C_deNorm));
		if (runSingle) return;
		sampH -= fhx (qPaH[1][1] = fhx (pValHiH * sampH - bValHiH * qPaH[1][1] + h_C_deNorm));
		sampH = fhx (qPaH[0][1] = fhx (pValLowH * sampH - bValLowH * qPaH[0][1] + h_C_deNorm));
		sampH -= fhx (qPaH[1][2] = fhx (pValHiH * sampH - bValHiH * qPaH[1][2] + h_C_deNorm));
		sampH = fhx (qPaH[0][2] = fhx (pValLowH * sampH - bValLowH * qPaH[0][2] + h_C_deNorm));

		return;
	} else
	if (highP) RunHPH();
	else
	if (lowP) RunLPH();
}
