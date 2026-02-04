#include "tonestack_k.h"
#include "tstack_params.h"
#include <cstdlib>
#include <algorithm>

#include "../custdata_types.h"

using namespace ScMath;

ToneStack_K::ToneStack_K(const fsx nRate, const SC_ToneStack_Params inpParams) :
	ToneStRate(fhx (nRate)),
	prms(inpParams),
	setShaped(true),
	fpPrec(int (1)),
	dynQenable(true),
	h_DynEnvPos(fhx (0.0)),
	s_DynEnvPos(fsx (0.0)),
	l_DynEnvPos(flx (0.f)),
	curAmp(0),
	bright(false)
{
	s_bValue[0] = fsx (1.0);
	s_bValue[1] = fsx (1.0);
	s_bValue[2] = fsx (1.0);
	s_bValue[3] = fsx (1.0);
	s_bValue[4] = fsx (1.0);
	s_bValue[5] = fsx (1.0);

	for(int x = 0; x < 6; x++)
	{
		l_bValue[x] = flx (s_bValue[x]);
		h_bValue[x] = fhx (s_bValue[x]);

		qBand[x] = new Cst_EQ(true);
		qBand[x]->SetSmpRate(ToneStRate);
	}

	for(int x = 0; x < SC_NUM_TSTACK_J_DEFS_AMP; x++)
	{
		for(int y = 0; y < TnStack_Out[x].nElements; y++)
		{
			AmpHeadEQA[x][y] = new BTW_LP(TnStack_Out[x].Elements[y].low, ToneStRate);
			AmpHeadEQB[x][y] = new BTW_LP(TnStack_Out[x].Elements[y].high, ToneStRate);
//			AmpHeadEQ[x][y] = new Cst_EQ(false);
//			AmpHeadEQ[x][y]->SetSmpRate(ToneStRate);
//			AmpHeadEQ[x][y]->SetBands(TnStack_Out[x].Elements[y].low, TnStack_Out[x].Elements[y].high);
		}
	}
	for(int x = 0; x < 2; x++)
	{
		for(int y = 0; y < TnStack_In[x].nElements; y++)
		{
			AmpInputEQ[x][y] = new Cst_EQ(false);
			AmpInputEQ[x][y]->SetSmpRate(ToneStRate);
			AmpInputEQ[x][y]->SetBands(TnStack_In[x].Elements[y].low, TnStack_In[x].Elements[y].high);
		}
	}

	DynamicQ = new Cst_EQ();
	DynamicQ->SetSmpRate(ToneStRate);
	DynBass = new Cst_EQ();
	DynBass->SetSmpRate(ToneStRate);
	StaticQBand[0] = new Cst_EQ(true);
	StaticQBand[0]->SetSmpRate(ToneStRate);
	StaticQBand[1] = new Cst_EQ(true);
	StaticQBand[1]->SetSmpRate(ToneStRate);

	qBand[0]->SetBands(prms.qb_a_low, prms.qb_b_low);
	qBand[1]->SetBands(prms.qb_a_mid, prms.qb_b_mid);
	qBand[2]->SetBands(prms.qb_a_high, prms.qb_b_high);
	qBand[3]->SetBands(prms.qb_a_cntA, prms.qb_b_cntA);
	qBand[4]->SetBands(prms.qb_a_cntB, prms.qb_b_cntB);
	qBand[5]->SetBands(prms.qb_a_pres, prms.qb_b_pres);

	DynamicQ->SetBands(5.0, prms.DynamicToneTravelFreq);
	DynBass->SetBands(prms.DynamicToneFloorFreq, prms.DynamicTonePassFreq);
	h_DynEnvCurve = (fhx) prms.DynamicToneCurve;
	h_DynEnvOffset = (fhx) prms.DynamicToneOffset;
	h_DynEnvRange = (fhx) prms.DynamicToneRange;
	s_DynEnvCurve = (fsx) prms.DynamicToneCurve;
	s_DynEnvOffset = (fsx) prms.DynamicToneOffset;
	s_DynEnvRange = (fsx) prms.DynamicToneRange;
	l_DynEnvCurve = (flx) prms.DynamicToneCurve;
	l_DynEnvOffset = (flx) prms.DynamicToneOffset;
	l_DynEnvRange = (flx) prms.DynamicToneRange;

	StaticQBand[0]->SetBands(10.0, 22025.0);
	StaticQBand[1]->SetBands(10.0, 22025.0);

	DynamicHard[0] = new Cst_EQ();
	DynamicHard[1] = new Cst_EQ();
	DynamicHard[0]->SetSmpRate(ToneStRate);
	DynamicHard[1]->SetSmpRate(ToneStRate);
	DynamicHard[0]->SetBands(420.0, 625.0);
	DynamicHard[1]->SetBands(1000.0, 2000.0);

	ClearBuffs();
}
ToneStack_K::~ToneStack_K()
{
	for(int x = 0; x < SC_NUM_TSTACK_J_DEFS_AMP; x++)
	{
		for(int y = 0; y < TnStack_Out[x].nElements; y++)
		{
			delete AmpHeadEQA[x][y];
			delete AmpHeadEQB[x][y];
		}
	}
	for(int x = 0; x < 2; x++)
	{
		for(int y = 0; y < TnStack_In[x].nElements; y++)
		{
			delete AmpInputEQ[x][y];
		}
	}

	for (int qb = 0; qb < 6; qb++)
	{
		delete qBand[qb];
	}

	delete DynamicQ;
	delete DynBass;
	delete StaticQBand[0];
	delete StaticQBand[1];

	delete DynamicHard[0];
	delete DynamicHard[1];
}
void ToneStack_K::setNewSampleRate(const fhx nRate)
{
	ToneStRate = fhx (nRate);

	for(int x = 0; x < SC_NUM_TSTACK_J_DEFS_AMP; x++)
	{
		for(int y = 0; y < TnStack_Out[x].nElements; y++)
		{
			AmpHeadEQA[x][y]->setSampleRate((fsx) ToneStRate);
			AmpHeadEQB[x][y]->setSampleRate((fsx) ToneStRate);
		}
	}
	for(int x = 0; x < 2; x++)
	{
		for(int y = 0; y < TnStack_In[x].nElements; y++)
		{
			AmpInputEQ[x][y]->SetSmpRate((fsx) ToneStRate);
		}
	}

	for (int x = 0; x < 6; x++)
	{
		qBand[x]->SetSmpRate((fsx) ToneStRate);
	}

	DynamicQ->SetSmpRate((fsx) ToneStRate);
	DynBass->SetSmpRate((fsx) ToneStRate);
	StaticQBand[0]->SetSmpRate((fsx) ToneStRate);
	StaticQBand[1]->SetSmpRate((fsx) ToneStRate);

	DynamicHard[0]->SetSmpRate((fsx) ToneStRate);
	DynamicHard[1]->SetSmpRate((fsx) ToneStRate);

	ClearBuffs();
}
void ToneStack_K::setBandValue(const int bNum, fsx value)
{
	if(bNum < 0) return;
	if(bNum > 5) return;

	h_bValue[bNum] = fhx (value * h_C_2);
	h_bValue[bNum] *= fhx (h_bValue[bNum]);

	l_bValue[bNum] = flx (h_bValue[bNum]);
	s_bValue[bNum] = fsx (h_bValue[bNum]);
}
void ToneStack_K::setEvenShaped(const bool isShaped)
{
	if(setShaped != isShaped) ClearBuffs();
	setShaped = isShaped;
}
void ToneStack_K::ClearBuffs()
{
	h_DynEnvPos = fhx (0.0);
	s_DynEnvPos = fsx (0.0);
	l_DynEnvPos = flx (0.f);

	for (int x = 0; x < 6; x++)
	{
		qBand[x]->ClearBuffers();
	}

	for(int x = 0; x < SC_NUM_TSTACK_J_DEFS_AMP; x++)
	{
		for(int y = 0; y < TnStack_Out[x].nElements; y++)
		{
			AmpHeadEQA[x][y]->zeroBuffers();
			AmpHeadEQB[x][y]->zeroBuffers();
		}
	}
	for(int x = 0; x < 2; x++)
	{
		for(int y = 0; y < TnStack_In[x].nElements; y++)
		{
			AmpInputEQ[x][y]->ClearBuffers();
		}
	}

	DynamicQ->ClearBuffers();
	StaticQBand[0]->ClearBuffers();
	StaticQBand[1]->ClearBuffers();
	DynBass->ClearBuffers();

	DynamicHard[0]->ClearBuffers();
	DynamicHard[1]->ClearBuffers();
}
void ToneStack_K::SetPrecision(int nLevel)
{
	if(nLevel < 0) nLevel = 0;
	if(nLevel > 2) nLevel = 2;

	fpPrec = nLevel;
}
void ToneStack_K::RunEQ_Controls()
{
	if(fpPrec == 0)
	{
		StaticQBand[0]->l_InputSmpl(l_spl);
		StaticQBand[0]->l_Run();
		for (int x = 0; x < 3; x++)
		{
			qBand[x]->l_InputSmpl(l_spl);
			qBand[x]->l_Run(true, true);
		}
		l_spl = flx (StaticQBand[0]->l_GetOutput() * (l_C_1 + l_C_qrt));
		for (int x = 0; x < 3; x++)
			l_spl += flx (qBand[x]->l_GetOutput() * (l_bValue[x] - l_C_1));

		StaticQBand[1]->l_InputSmpl(l_spl);
		StaticQBand[1]->l_Run();
		for (int x = 3; x < 6; x++)
		{
			qBand[x]->l_InputSmpl(l_spl);
			qBand[x]->l_Run(true, true);
		}
		l_spl = flx (StaticQBand[1]->l_GetOutput() * (l_C_1 + l_C_qrt));
		for (int x = 3; x < 6; x++)
			l_spl += flx (qBand[x]->l_GetOutput() * (l_bValue[x] - l_C_1));

		l_spl *= l_C_half;

		if (dynQenable)
		{
			DynBass->l_InputSmpl(l_spl);
			DynBass->l_Run(true, true);

			l_spl += flx (DynBass->l_GetOutput() * (l_DynEnvOffset - (l_DynEnvPos * l_DynEnvRange)));
		}
	} else
	if(fpPrec == 1)
	{
		StaticQBand[0]->s_InputSmpl(s_spl);
		StaticQBand[0]->s_Run();
		for (int x = 0; x < 3; x++)
		{
			qBand[x]->s_InputSmpl(s_spl);
			qBand[x]->s_Run(true, true);
		}
		s_spl = fsx (StaticQBand[0]->s_GetOutput() * (s_C_1 + s_C_qrt));
		for (int x = 0; x < 3; x++)
			s_spl += fsx (qBand[x]->s_GetOutput() * (s_bValue[x] - s_C_1));

		StaticQBand[1]->s_InputSmpl(s_spl);
		StaticQBand[1]->s_Run();
		for (int x = 3; x < 6; x++)
		{
			qBand[x]->s_InputSmpl(s_spl);
			qBand[x]->s_Run(true, true);
		}
		s_spl = fsx (StaticQBand[1]->s_GetOutput() * (s_C_1 + s_C_qrt));
		for (int x = 3; x < 6; x++)
			s_spl += fsx (qBand[x]->s_GetOutput() * (s_bValue[x] - s_C_1));

		s_spl *= s_C_half;

		if (dynQenable)
		{
			DynBass->s_InputSmpl(s_spl);
			DynBass->s_Run(true, true);

			s_spl += fsx (DynBass->s_GetOutput() * (s_DynEnvOffset - (s_DynEnvPos * s_DynEnvRange)));
		}
	} else
	if(fpPrec == 2)
	{
		StaticQBand[0]->h_InputSmpl(h_spl);
		StaticQBand[0]->h_Run();
		for (int x = 0; x < 3; x++)
		{
			qBand[x]->h_InputSmpl(h_spl);
			qBand[x]->h_Run(true, true);
		}
		h_spl = fhx (StaticQBand[0]->h_GetOutput() * (h_C_1 + h_C_qrt));
		for (int x = 0; x < 3; x++)
			h_spl += fhx (qBand[x]->h_GetOutput() * (h_bValue[x] - h_C_1));

		StaticQBand[1]->h_InputSmpl(h_spl);
		StaticQBand[1]->h_Run();
		for (int x = 3; x < 6; x++)
		{
			qBand[x]->h_InputSmpl(h_spl);
			qBand[x]->h_Run(true, true);
		}
		h_spl = fhx (StaticQBand[1]->h_GetOutput() * (h_C_1 + h_C_qrt));
		for (int x = 3; x < 6; x++)
			h_spl += fhx (qBand[x]->h_GetOutput() * (h_bValue[x] - h_C_1));

		h_spl *= h_C_half;

		if (dynQenable)
		{
			DynBass->h_InputSmpl(h_spl);
			DynBass->h_Run(true, true);

			h_spl += fhx (DynBass->h_GetOutput() * (h_DynEnvOffset - (h_DynEnvPos * h_DynEnvRange)));
		}
	}
}
void ToneStack_K::RunEQ_Input()
{
	const int nFX = TnStack_In[bright].nElements;

	if(fpPrec == 0)
	{
		for (int x = 0; x < nFX; x++)
			AmpInputEQ[bright][x]->l_InputSmpl(l_spl);

		l_spl = l_C_0;

		for (int x = 0; x < nFX; x++)
		{
			AmpInputEQ[bright][x]->l_Run(true, true);
			l_spl += flx (AmpInputEQ[bright][x]->l_GetOutput() * TnStack_In[bright].Elements[x].vol);
		}

		l_spl *= (flx) iVolumesInput[bright];

		if (dynQenable)
		{
			DynamicHard[0]->l_InputSmpl(l_spl);
			DynamicHard[1]->l_InputSmpl(l_spl);
			DynamicHard[0]->l_Run(true, true);
			DynamicHard[1]->l_Run(true, true);
			l_spl += flx ( (l_C_1 - l_Diode_S(l_DynEnvPos, l_C_2)) * DynamicHard[1]->l_GetOutput() * l_DynEnvRange );
			l_spl += flx ( (l_C_1 - l_Diode_S(l_DynEnvPos, l_C_2)) * DynamicHard[1]->l_GetOutput() * l_DynEnvRange );
		}
	} else
	if(fpPrec == 1)
	{
		for (int x = 0; x < nFX; x++)
			AmpInputEQ[bright][x]->s_InputSmpl(s_spl);

		s_spl = s_C_0;

		for (int x = 0; x < nFX; x++)
		{
			AmpInputEQ[bright][x]->s_Run(true, true);
			s_spl += fsx (AmpInputEQ[bright][x]->s_GetOutput() * TnStack_In[bright].Elements[x].vol);
		}

		s_spl *= (fsx) iVolumesInput[bright];

		if (dynQenable)
		{
			DynamicHard[0]->s_InputSmpl(s_spl);
			DynamicHard[1]->s_InputSmpl(s_spl);
			DynamicHard[0]->s_Run(true, true);
			DynamicHard[1]->s_Run(true, true);
			s_spl += fsx ( (s_C_1 - s_Diode_S(s_DynEnvPos, s_C_2)) * DynamicHard[0]->s_GetOutput() * s_DynEnvRange );
			s_spl += fsx ( (s_C_1 - s_Diode_S(s_DynEnvPos, s_C_2)) * DynamicHard[1]->s_GetOutput() * s_DynEnvRange );
		}
	} else
	if(fpPrec == 2)
	{
		for (int x = 0; x < nFX; x++)
			AmpInputEQ[bright][x]->h_InputSmpl(h_spl);

		h_spl = h_C_0;

		for (int x = 0; x < nFX; x++)
		{
			AmpInputEQ[bright][x]->h_Run(true, true);
			h_spl += fhx (AmpInputEQ[bright][x]->h_GetOutput() * TnStack_In[bright].Elements[x].vol);
		}

		h_spl *= (fhx) iVolumesInput[bright];

		if (dynQenable)
		{
			DynamicHard[0]->h_InputSmpl(h_spl);
			DynamicHard[1]->h_InputSmpl(h_spl);
			DynamicHard[0]->h_Run(true, true);
			DynamicHard[1]->h_Run(true, true);
			h_spl += fhx ( (h_C_1 - h_Diode_S(h_DynEnvPos, h_C_2)) * DynamicHard[0]->h_GetOutput() * h_DynEnvRange );
			h_spl += fhx ( (h_C_1 - h_Diode_S(h_DynEnvPos, h_C_2)) * DynamicHard[1]->h_GetOutput() * h_DynEnvRange );
		}
	}
}
void ToneStack_K::RunEQ_Output()
{
	if (!setShaped) return;

	const int nFX = TnStack_Out[curAmp].nElements;

	if(fpPrec == 0)
	{
		for (int x = 0; x < nFX; x++)
			AmpHeadEQA[curAmp][x]->inpSplL(l_spl);

		for (int x = 0; x < nFX; x++)
		{
			AmpHeadEQA[curAmp][x]->RunFilter();
			AmpHeadEQB[curAmp][x]->inpSplL((l_spl - AmpHeadEQA[curAmp][x]->GetOutPutL()) * TnStack_Out[curAmp].Elements[x].vol);
			AmpHeadEQB[curAmp][x]->RunFilter();
		}

		l_spl = l_C_0;

		for (int x = 0; x < nFX; x++)
			l_spl += (flx) AmpHeadEQB[curAmp][x]->GetOutPutL();

		l_spl *= (flx) iVolumesAmp[curAmp];
	} else
	if(fpPrec == 1)
	{
		for (int x = 0; x < nFX; x++)
			AmpHeadEQA[curAmp][x]->inpSplS(s_spl);

		for (int x = 0; x < nFX; x++)
		{
			AmpHeadEQA[curAmp][x]->RunFilter();
			AmpHeadEQB[curAmp][x]->inpSplS((s_spl - AmpHeadEQA[curAmp][x]->GetOutPutS()) * TnStack_Out[curAmp].Elements[x].vol);
			AmpHeadEQB[curAmp][x]->RunFilter();
		}

		s_spl = s_C_0;

		for (int x = 0; x < nFX; x++)
			s_spl += (fsx) AmpHeadEQB[curAmp][x]->GetOutPutS();

		s_spl *= (fsx) iVolumesAmp[curAmp];
	} else
	if(fpPrec == 2)
	{
		for (int x = 0; x < nFX; x++)
			AmpHeadEQA[curAmp][x]->inpSplH(h_spl);

		for (int x = 0; x < nFX; x++)
		{
			AmpHeadEQA[curAmp][x]->RunFilter();
			AmpHeadEQB[curAmp][x]->inpSplH((h_spl - AmpHeadEQA[curAmp][x]->GetOutPutH()) * TnStack_Out[curAmp].Elements[x].vol);
			AmpHeadEQB[curAmp][x]->RunFilter();
		}

		h_spl = h_C_0;

		for (int x = 0; x < nFX; x++)
			h_spl += (fhx) AmpHeadEQB[curAmp][x]->GetOutPutH();
/*
		for (int x = 0; x < nFX; x++)
			AmpHeadEQ[curAmp][x]->h_InputSmpl(h_spl);

		h_spl = h_C_0;

		for (int x = 0; x < nFX; x++)
		{
			AmpHeadEQ[curAmp][x]->h_Run(true, true);
			h_spl += fhx (AmpHeadEQ[curAmp][x]->h_GetOutput() * TnStack_Out[curAmp].Elements[x].vol);
		}
*/
		h_spl *= (fhx) iVolumesAmp[curAmp];
	}
}
void ToneStack_K::l_SetDynamicQ(const flx inp)
{
	DynamicQ->l_InputSmpl(l_Diode_S(l_abs(inp), l_DynEnvCurve));
	DynamicQ->l_Run(true, false);
	l_DynEnvPos = (flx) l_Diode_S(DynamicQ->l_GetOutput(), l_C_3);
}
void ToneStack_K::s_SetDynamicQ(const fsx inp)
{
	DynamicQ->s_InputSmpl(s_Diode_S(s_abs(inp), s_DynEnvCurve));
	DynamicQ->s_Run(true, false);
	s_DynEnvPos = (fsx) s_Diode_S(DynamicQ->s_GetOutput(), s_C_3);
}
void ToneStack_K::h_SetDynamicQ(const fhx inp)
{
	DynamicQ->h_InputSmpl(h_Diode_S(h_abs(inp), h_DynEnvCurve));
	DynamicQ->h_Run(true, false);
	h_DynEnvPos = (fhx) h_Diode_S(DynamicQ->h_GetOutput(), h_C_3);
}
