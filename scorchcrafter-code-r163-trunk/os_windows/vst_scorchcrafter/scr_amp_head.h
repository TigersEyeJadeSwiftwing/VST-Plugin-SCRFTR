#ifndef __scorchcft_amp_head__
#define __scorchcft_amp_head__

#include "custdata_types.h"
#include "../lib_graphics_src/zlib.h"
#include "../lib_graphics_src/png.h"
#include <audioeffectx.h>
#include "scorchc_gui.h"
#include "params.h"
#include "resource.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>

#ifndef MAX_STAGES
#define MAX_STAGES 4
#endif
#ifndef DIST_STAGES_CLEAN
#define DIST_STAGES_CLEAN 1
#endif
#ifndef DIST_STAGES_CRUNCH
#define DIST_STAGES_CRUNCH 2
#endif
#ifndef DIST_STAGES_HIGH
#define DIST_STAGES_HIGH 3
#endif
#ifndef DIST_STAGES_MEGA
#define DIST_STAGES_MEGA 4
#endif

#define COMPONENT_TABLE_SIZE 512
// #define CTSR 511
//!-------------------------------------------------------------------------------------------------------
enum LOWPASS_LIST
{
	LPF_Bottom_In,
	LPF_Filter_I,
	LPF_Filter_Ib,
	LPF_Filter_Xlow,
	LPF_Filter_Xhigh,
	LPF_Top_Ceil,
	LPF_MAX_COUNT
};
enum EQ_LIST
{
	EQ_ToneStack_Low,
	EQ_ToneStack_High,
	EQ_ToneStack_CeilA,
	EQ_ToneStack_BottQ,
	EQ_ToneStack_Pres,
	EQ_ToneStack_CeilB,
	EQ_MAX_COUNT
};

class ScorchCraft : public AudioEffectX
{
public:
	ScorchCraft (audioMasterCallback audioMaster);
	~ScorchCraft ();

//! Processing
	virtual void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
	virtual void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames);

//! Program
	virtual void setProgramName (char* name);
	virtual void getProgramName (char* name);

//! Parameters
	virtual void setParameter (VstInt32 index, float value);
	virtual float getParameter (VstInt32 index);
	virtual void getParameterLabel (VstInt32 index, char* label);
	virtual void getParameterDisplay (VstInt32 index, char* text);
	virtual void getParameterName (VstInt32 index, char* text);

	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual VstInt32 getVendorVersion ();

	virtual void setSampleRate (float smplRate);
	virtual void open();
	virtual void close();
	virtual void suspend();
	virtual void resume();

private:
//! Functions
	void ZeroSamples();
	f9x RunFX(const f9x inpSpl);
	void CalcSampDepth();
	void ZeroPassBuffers();
	void SetupLowPassFs();
	void LoadLPassCutOffs();
	f9x RunLowPass(const LOWPASS_LIST LowPassID, int smpLev, f9x inputS);
	f9x RunEQLPass(const EQ_LIST EqID, f9x inputS);
	void RunUpSampling();
	f9x RunUpSampleFilter(const int sLevel, f9x inputS);
	void RunDownSampling();
	f9x RunDownSampleFilter(const int sLevel, f9x inputS);
	void RunInputFilterSystem();
	void RunInputFilter(const int sLev, const int sampNum);
	void RunDistortion(const int sLev);
	void DistortTypeHigh(const int sLevI, const int sNum);
	void DistortTypeCrunch(const int sLevI, const int sNum);
	void DistortTypeClean(const int sLevI, const int sNum);
	void DistortTypeMega(const int sLevI, const int sNum);
	f9x RunTubeA(const int x, const int y, f9x smpl);
	f9x RunTubeB(const int x, const int y, f9x smpl);
	void RunOutputEqFilter();
	void CheckChannelSelect(const int nChan);
	void LoadTubeTables();

//! Basic Stuff
	char programName[kVstMaxProgNameLen + 1];

	float inputFloat;
	double inputDouble;
	float outputFloat;
	double outputDouble;

	float sRate;
	float params[kMaxNumParameters];
	f9x sample[16];
	bool isOnline;
	int SampleDepthCurrent;
	int SampleDepthOld;
	int SampleDepthOnline;
	int SampleDepthOffline;
	const f9x PI;
	const f9x hPI;
	const f9x zeroFudge;

//!	Channel Controls
	bool cleanGainChannelActive;
	bool crunchGainChannelActive;
	bool highGainChannelActive;
	bool megaGainChannelActive;
	int oldChanSelect;

//! Low-Pass stuff
	f9x cutOff[LPF_MAX_COUNT][5];
	f9x c[LPF_MAX_COUNT][5];
	f9x damp[LPF_MAX_COUNT][5], proc0[LPF_MAX_COUNT][5], proc1[LPF_MAX_COUNT][5], procXX[LPF_MAX_COUNT][5];
	f9x procA[LPF_MAX_COUNT][5], procB[LPF_MAX_COUNT][5], procC[LPF_MAX_COUNT][5];

//! EQ Stuff
	f9x EQcutOff[EQ_MAX_COUNT];
	f9x EQc[EQ_MAX_COUNT];
	f9x EQdamp[EQ_MAX_COUNT], EQproc0[EQ_MAX_COUNT], EQproc1[EQ_MAX_COUNT], EQprocXX[EQ_MAX_COUNT];
	f9x EQprocA[EQ_MAX_COUNT], EQprocB[EQ_MAX_COUNT], EQprocC[EQ_MAX_COUNT];

//! Up and Down Sampling Filter Stuff
	f9x suT[4][5];
	f9x suCoef[4][5];
	f9x suHist[4][5];

	f9x sdT[4][5];
	f9x sdCoef[4][5];
	f9x sdHist[4][5];

	f9x sCutoff[5];
	f9x wp[5];
	f9x scGain[5];

//! Tone Stack Stuff
	int tStackTypeSelection, oldTStackSel;

//! Special
	f9x rateFac;
	int fadeCount;
	int fadeMax;
	const f9x fadeFac;
	f9x dstRead;
	f9x qFade;

//! Distorion
	f9x dsFilter[MAX_STAGES];
	f9x lastPoint[MAX_STAGES][2];
	f9x peakP[MAX_STAGES][2];
	f9x distP[MAX_STAGES][2];

	f9x lastSpl;
	f9x vtcTravel;
	bool upTravel;

	f9x *driveTubeLGup;
	f9x *driveTubeLGdn;
	f9x *driveTubeHGup;
	f9x *driveTubeHGdn;
	const f9x tbFacF;
	const u7x tbFacI;
};

#endif
