#include "tonestack_f.h"
#include <cmath>

const fhx ToneStack_F::qBandRange_Low[2] = {SCr_C120_TStack_EqBand_Start_Low, SCr_C120_TStack_EqBand_End_Low};
const fhx ToneStack_F::qBandRange_Mid[2] = {SCr_C120_TStack_EqBand_Start_Mid, SCr_C120_TStack_EqBand_End_Mid};
const fhx ToneStack_F::qBandRange_High[2] = {SCr_C120_TStack_EqBand_Start_High, SCr_C120_TStack_EqBand_End_High};
const fhx ToneStack_F::qBandRange_ContourL[2] = {SCr_C120_TStack_EqBand_Start_CntrA, SCr_C120_TStack_EqBand_End_CntrA};
const fhx ToneStack_F::qBandRange_ContourH[2] = {SCr_C120_TStack_EqBand_Start_CntrB, SCr_C120_TStack_EqBand_End_CntrB};
const fhx ToneStack_F::qBandRange_Presence[2] = {SCr_C120_TStack_EqBand_Start_Pres, SCr_C120_TStack_EqBand_End_Pres};

ToneStack_F::ToneStack_F(fsx nRate) :
	sRate(fhx (nRate)),
	fpPrec(int (1))
{
	s_bValue[0] = fsx (0.50);
	s_bValue[1] = fsx (0.50);
	s_bValue[2] = fsx (0.50);
	s_bValue[3] = fsx (0.50);
	s_bValue[4] = fsx (0.50);
	s_bValue[5] = fsx (0.50);

	for(int x = 0; x < 6; x++)
	{
		l_bValue[x] = flx (s_bValue[x]);
		h_bValue[x] = fhx (s_bValue[x]);

		qBand[x] = new Cst_EQ(true);
		qBand[x]->SetSmpRate(sRate);
	}

	qBand[0]->SetBands(qBandRange_Low[0], qBandRange_Low[1]);
	qBand[1]->SetBands(qBandRange_Mid[0], qBandRange_Mid[1]);
	qBand[2]->SetBands(qBandRange_High[0], qBandRange_High[1]);
	qBand[3]->SetBands(qBandRange_ContourL[0], qBandRange_ContourL[1]);
	qBand[4]->SetBands(qBandRange_ContourH[0], qBandRange_ContourH[1]);
	qBand[5]->SetBands(qBandRange_Presence[0], qBandRange_Presence[1]);

	ClearBuffs();
}
ToneStack_F::~ToneStack_F()
{
	for (int qb = 0; qb < 6; qb++)
	{
		delete qBand[qb];
	}
}
void ToneStack_F::setNewSampleRate(const fsx nRate)
{
	sRate = fhx (nRate);
	for (int x = 0; x < 6; x++)
	{
		qBand[x]->SetSmpRate(sRate);
	}
	ClearBuffs();
}
void ToneStack_F::setBandValue(const int bNum, fsx value)
{
	if(bNum < 0) return;
	if(bNum > 5) return;

	l_bValue[bNum] = flx (value);
	s_bValue[bNum] = fsx (value);
	h_bValue[bNum] = fhx (value);
}
void ToneStack_F::ClearBuffs()
{
	for (int x = 0; x < 6; x++)
	{
		qBand[x]->ClearBuffers();
	}
}
void ToneStack_F::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
}
void ToneStack_F::RunEQ()
{
	if(fpPrec == 0)
	{
		for (unsigned int x = 0; x < 6; x++)
		{
			qBand[x]->SplInL(l_spl);
			qBand[x]->RunQL();
		}
		for (unsigned int x = 0; x < 6; x++)
			l_spl += flx (qBand[x]->GetOutputL() * flx ((l_bValue[x] * l_bValue[x]) - flx (0.25f)) * flx (4.0f));
	} else
	if(fpPrec == 1)
	{
		for (unsigned int x = 0; x < 6; x++)
		{
			qBand[x]->SplInS(s_spl);
			qBand[x]->RunQS();
		}
		for (unsigned int x = 0; x < 6; x++)
			s_spl += fsx (qBand[x]->GetOutputS() * fsx ((s_bValue[x] * s_bValue[x]) - fsx (0.25)) * fsx (4.0));
	} else
	if(fpPrec == 2)
	{
		for (unsigned int x = 0; x < 6; x++)
		{
			qBand[x]->SplInH(h_spl);
			qBand[x]->RunQH();
		}
		for (unsigned int x = 0; x < 6; x++)
			h_spl += fhx (qBand[x]->GetOutputH() * fhx ((h_bValue[x] * h_bValue[x]) - fhx (0.25)) * fhx (4.0));
	}
}
