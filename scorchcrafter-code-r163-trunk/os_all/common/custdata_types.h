#pragma once
#ifndef __scorch_cdata_types__
#define __scorch_cdata_types__

#include "cfloat"
#include <cmath>
#include <stdint.h>

//! Determine floating-point precision for running audio
//! dependending on compiler DEFINE flags.

#ifdef PRECISION_LOW
#define SCPREC 1
#endif
#ifdef PRECISION_NORMAL
#define SCPREC 2
#endif
#ifdef PRECISION_MAX
#define SCPREC 3
#endif

//! If unspecified, assume NORMAL precision.
#ifndef SCPREC
#define SCPREC 2
#endif

#if (SCPREC==1)
#define __FLT_EVAL_METHOD 0
typedef float f9x;
#endif
#if (SCPREC==2)
#define __FLT_EVAL_METHOD 2
typedef double f9x;
#endif
#if (SCPREC==3)
#define __FLT_EVAL_METHOD 2
typedef long double f9x;
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SC_PI 3.14159265358979323846264338327950288419716939937510

#ifndef sinf
#define sinf(x) ((float) sin(x))
#endif
#ifndef sinl
#define sinl(x) ((long double) sin(x))
#endif
#ifndef cosf
#define cosf(x) ((float) cos(x))
#endif
#ifndef cosl
#define cosl(x) ((long double) cos(x))
#endif
#ifndef expf
#define expf(x) ((float) exp(x))
#endif
#ifndef expl
#define expl(x) ((long double) exp(x))
#endif
#ifndef powf
#define powf(x,y) ((float) pow(x,y))
#endif
#ifndef powl
#define powl(x,y) ((long double) pow(x,y))
#endif

typedef unsigned long long u9x;
typedef signed long long s9x;
typedef unsigned short u7x;
typedef signed short s7x;
typedef unsigned int u8x;
typedef int s8x;

//!	The "L" in the names refers to Low precision (32-bit), the "S" means Standard precision (64-bit),
//!		and the "H" is for High precision, which should usually be 128-bit.
//!	This naming system is used all over ScorchCrafter source code, in function names, variable names, etc.
//!		(So please get used to it.)

typedef float flx;
typedef double fsx;
typedef long double fhx;

namespace ScMath
{
//!	These are "helper" constants, used to make some coding tasks easier.
//!	The "C" after the precision designation indicates a Constant, fixed value.
	static const flx l_C_0 = flx (0.0f);
	static const flx l_C_1 = flx (1.0f);
	static const flx l_C_2 = flx (2.0f);
	static const flx l_C_3 = flx (3.0f);
	static const flx l_C_4 = flx (4.0f);
	static const flx l_C_5 = flx (5.0f);
	static const flx l_C_10 = flx (10.0f);
	static const flx l_C_100 = flx (100.0f);
	static const flx l_C_half = flx (0.50f);
	static const flx l_C_qrt = flx (0.250f);
	static const flx l_C_tenth = flx (0.10f);
	static const flx l_C_tenthQrt = flx (0.0250f);
	static const flx l_C_hundredth = flx (0.010f);
	static const flx l_C_pi = flx (SC_PI);
	static const flx l_C_pi_half = flx (SC_PI * flx (0.50f));
	static const flx l_C_pi_doub = flx (SC_PI * flx (2.0f));
	static const flx l_C_deNorm = flx ( powl(fhx (10.0), fhx (-28.0)) );
	static const flx l_CA_Standard[11] = {
		0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f};
	static const flx l_CA_Exponential[11] = {
		0.f, 1.f, 3.f, 6.f, 10.f, 15.f, 21.f, 28.f, 36.f, 45.f, 55.f};

	static const fsx s_C_0 = fsx (0.0);
	static const fsx s_C_1 = fsx (1.0);
	static const fsx s_C_2 = fsx (2.0);
	static const fsx s_C_3 = fsx (3.0);
	static const fsx s_C_4 = fsx (4.0);
	static const fsx s_C_5 = fsx (5.0);
	static const fsx s_C_10 = fsx (10.0);
	static const fsx s_C_100 = fsx (100.0);
	static const fsx s_C_half = fsx (0.50);
	static const fsx s_C_qrt = fsx (0.250);
	static const fsx s_C_tenth = fsx (0.10);
	static const fsx s_C_tenthQrt = fsx (0.0250);
	static const fsx s_C_hundredth = fsx (0.010);
	static const fsx s_C_pi = fsx (SC_PI);
	static const fsx s_C_pi_half = fsx (SC_PI * fsx (0.50));
	static const fsx s_C_pi_doub = fsx (SC_PI * fsx (2.0));
	static const fsx s_C_deNorm = fsx ( powl(fhx (10.0), fhx (-30.0)) );
	static const fsx s_CA_Standard[11] = {
		0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
	static const fsx s_CA_Exponential[11] = {
		0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0, 45.0, 55.0};

	static const fhx h_C_0 = fhx (0.0);
	static const fhx h_C_1 = fhx (1.0);
	static const fhx h_C_2 = fhx (2.0);
	static const fhx h_C_3 = fhx (3.0);
	static const fhx h_C_4 = fhx (4.0);
	static const fhx h_C_5 = fhx (5.0);
	static const fhx h_C_10 = fhx (10.0);
	static const fhx h_C_100 = fhx (100.0);
	static const fhx h_C_half = fhx (0.50);
	static const fhx h_C_qrt = fhx (0.250);
	static const fhx h_C_tenth = fhx (0.10);
	static const fhx h_C_tenthQrt = fhx (0.0250);
	static const fhx h_C_hundredth = fhx (0.010);
	static const fhx h_C_pi = fhx (SC_PI);
	static const fhx h_C_pi_half = fhx (SC_PI * fhx (0.50));
	static const fhx h_C_pi_doub = fhx (SC_PI * fhx (2.0));
	static const fhx h_C_deNorm = fhx ( powl(fhx (10.0), fhx (-30.0)) );
	static const fhx h_CA_Standard[11] = {
		0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
	static const fhx h_CA_Exponential[11] = {
		0.0, 1.0, 3.0, 6.0, 10.0, 15.0, 21.0, 28.0, 36.0, 45.0, 55.0};

//!	Custom, often-used math functions (mostly floating-point)

//!	Return absolute value
	inline
	flx l_abs(const flx inp) { return flx (inp >= l_C_0 ? inp : -inp); };
	inline
	fsx s_abs(const fsx inp) { return fsx (inp >= s_C_0 ? inp : -inp); };
	inline
	fhx h_abs(const fhx inp) { return fhx (inp >= h_C_0 ? inp : -inp); };

//!	Return "Inverted" value (within 0.0 and 1.0)
	inline
	flx l_inv(const flx inp) { return flx (l_C_1 - inp); };
	inline
	fsx s_inv(const fsx inp) { return fsx (s_C_1 - inp); };
	inline
	fhx h_inv(const fhx inp) { return fhx (h_C_1 - inp); };

//!	Return (basic, simple) average of two values
	inline
	flx l_avg(const flx a, const flx b) { return flx (l_C_half * (a + b)); };
	inline
	fsx s_avg(const fsx a, const fsx b) { return fsx (s_C_half * (a + b)); };
	inline
	fhx h_avg(const fhx a, const fhx b) { return fhx (h_C_half * (a + b)); };

//!	Return skewed average of two values, with "biasTowoardB" a value from 0.0 to 1.0.  High bias values (near 1.0) result in
//!		results closer to the "b" input value, while low bias values (near 0.0) result in values closer to the "a" input value.
	inline
	flx l_avgx(const flx a, const flx b, const flx biasTowardB) {
		return flx ((a * (l_C_1-biasTowardB)) + (b*biasTowardB)); };
	inline
	fsx s_avgx(const fsx a, const fsx b, const fsx biasTowardB) {
		return fsx ((a * (s_C_1-biasTowardB)) + (b*biasTowardB)); };
	inline
	fhx h_avgx(const fhx a, const fhx b, const fhx biasTowardB) {
		return fhx ((a * (h_C_1-biasTowardB)) + (b*biasTowardB)); };

//!	Basic min() and max() functions
	inline
	flx l_min(const flx val, const flx clipVal) {
		return flx ((val > clipVal) ? clipVal : val); };
	inline
	fsx s_min(const fsx val, const fsx clipVal) {
		return fsx ((val > clipVal) ? clipVal : val); };
	inline
	fhx h_min(const fhx val, const fhx clipVal) {
		return fhx ((val > clipVal) ? clipVal : val); };
	inline
	flx l_max(const flx val, const flx clipVal) {
		return flx ((val < clipVal) ? clipVal : val); };
	inline
	fsx s_max(const fsx val, const fsx clipVal) {
		return fsx ((val < clipVal) ? clipVal : val); };
	inline
	fhx h_max(const fhx val, const fhx clipVal) {
		return fhx ((val < clipVal) ? clipVal : val); };

//!	These take input values from -1.0f to +1.0f and warp them outward away from 0.0f in
//!		a shape that somewhat resembles a sine wave.  The results don't typically work as
//!		well as a real sin() function though, with the only advantage being quick and a lot
//!		less costly in CPU usage.
	inline
	flx l_UglySine(const flx inp) {
		return flx ( inp >= l_C_0 ? (inp * (l_C_2 - inp)) : (inp * (l_C_2 + inp)) ); };
	inline
	fsx s_UglySine(const fsx inp) {
		return fsx ( inp >= s_C_0 ? (inp * (s_C_2 - inp)) : (inp * (s_C_2 + inp)) ); };
	inline
	fhx h_UglySine(const fhx inp) {
		return fhx ( inp >= h_C_0 ? (inp * (h_C_2 - inp)) : (inp * (h_C_2 + inp)) ); };

//!	These "Diode" functions simulate running a signal through a diode-like device, and can
//!		be used to alter a floating-point value to do a number of things, including distortion,
//!		or even just to keep an unknown value from exceeding 1.0, but using a curve and not
//!		a hard-clipping barrier.
//!	I've seen this algorithm in more than once place on the 'net, but couldn't find any
//!		verifiable information on who orginally came up with it.
//!	Functions with "U" in the name are for -unsigned- computations, and as such are meant for
//! 	positive or zero values only.  Functions with "S" are "signed" and can return values
//!		that are usually from -1.0 to +1.0, depending on the input and parameters.
	inline
	flx l_Diode_U(const flx inp, const flx hard) {
		return flx ( inp * (hard+inp) / (l_C_1 + (hard*inp) + (inp*inp)) ); };
	inline
	fsx s_Diode_U(const fsx inp, const fsx hard) {
		return fsx ( inp * (hard+inp) / (s_C_1 + (hard*inp) + (inp*inp)) ); };
	inline
	fhx h_Diode_U(const fhx inp, const fhx hard) {
		return fhx ( inp * (hard+inp) / (h_C_1 + (hard*inp) + (inp*inp)) ); };

	inline
	flx l_Diode_S(const flx inp, const flx hard) {
		return flx ((inp >= l_C_0) ? ( inp * (hard+inp) / (l_C_1 + (hard*inp) + (inp*inp)) )
			: ( inp * (hard-inp) / (l_C_1 + (hard*-inp) + (inp*inp)) )); };
	inline
	fsx s_Diode_S(const fsx inp, const fsx hard) {
		return fsx ((inp >= s_C_0) ? ( inp * (hard+inp) / (s_C_1 + (hard*inp) + (inp*inp)) )
			: ( inp * (hard-inp) / (s_C_1 + (hard*-inp) + (inp*inp)) )); };
	inline
	fhx h_Diode_S(const fhx inp, const fhx hard) {
		return fhx ((inp >= h_C_0) ? ( inp * (hard+inp) / (h_C_1 + (hard*inp) + (inp*inp)) )
			: ( inp * (hard-inp) / (h_C_1 + (hard*-inp) + (inp*inp)) )); };

	inline
	flx l_DiodeScaled_U(const flx inp, const flx hard, const flx scale) {
		return flx ( inp * (hard+inp) / (scale + (hard*inp) + (inp*inp)) ); };
	inline
	fsx s_DiodeScaled_U(const fsx inp, const fsx hard, const fsx scale) {
		return fsx ( inp * (hard+inp) / (scale + (hard*inp) + (inp*inp)) ); };
	inline
	fhx h_DiodeScaled_U(const fhx inp, const fhx hard, const fhx scale) {
		return fhx ( inp * (hard+inp) / (scale + (hard*inp) + (inp*inp)) ); };

	inline
	flx l_DiodeScaled_S(const flx inp, const flx hard, const flx scale) {
		return flx ((inp >= l_C_0) ? ( inp * (hard+inp) / (scale + (hard*inp) + (inp*inp)) )
			: ( inp * (hard-inp) / (scale + (hard*-inp) + (inp*inp)) )); };
	inline
	fsx s_DiodeScaled_S(const fsx inp, const fsx hard, const fsx scale) {
		return fsx ((inp >= s_C_0) ? ( inp * (hard+inp) / (scale + (hard*inp) + (inp*inp)) )
			: ( inp * (hard-inp) / (scale + (hard*-inp) + (inp*inp)) )); };
	inline
	fhx h_DiodeScaled_S(const fhx inp, const fhx hard, const fhx scale) {
		return fhx ((inp >= h_C_0) ? ( inp * (hard+inp) / (scale + (hard*inp) + (inp*inp)) )
			: ( inp * (hard-inp) / (scale + (hard*-inp) + (inp*inp)) )); };

	inline
	flx l_FauxTriode(const flx inp, const flx hard) {
		return flx ((inp >= l_C_0) ?
			( inp * l_C_half * (hard+inp) / (l_C_hundredth + (hard*inp)) )
			: ( inp * l_C_half * (hard-inp) / (l_C_hundredth + (hard*-inp)) )
			); };
	inline
	fsx s_FauxTriode(const fsx inp, const fsx hard) {
		return fsx ((inp >= s_C_0) ?
			( inp * s_C_half * (hard+inp) / (s_C_hundredth + (hard*inp)) )
			: ( inp * s_C_half * (hard-inp) / (s_C_hundredth + (hard*-inp)) )
			); };
	inline
	fhx h_FauxTriode(const fhx inp, const fhx hard) {
		return fhx ((inp >= h_C_0) ?
			( inp * h_C_half * (hard+inp) / (h_C_hundredth + (hard*inp)) )
			: ( inp * h_C_half * (hard-inp) / (h_C_hundredth + (hard*-inp)) )
			); };
	inline
	flx l_FauxTriodeScaled(const flx inp, const flx hard, const flx scale) {
		return flx ((inp >= l_C_0) ?
			( inp * l_C_half * (hard+inp) / (scale + (hard*inp)) )
			: ( inp * l_C_half * (hard-inp) / (scale + (hard*-inp)) )
			); };
	inline
	fsx s_FauxTriodeScaled(const fsx inp, const fsx hard, const fsx scale) {
		return fsx ((inp >= s_C_0) ?
			( inp * s_C_half * (hard+inp) / (scale + (hard*inp)) )
			: ( inp * s_C_half * (hard-inp) / (scale + (hard*-inp)) )
			); };
	inline
	fhx h_FauxTriodeScaled(const fhx inp, const fhx hard, const fhx scale) {
		return fhx ((inp >= h_C_0) ?
			( inp * h_C_qrt * (hard+(inp*inp*h_C_2)) / (scale + (hard*inp)) )
			: ( inp * h_C_qrt * (hard+(inp*inp*h_C_2)) / (scale + (hard*-inp)) )
			); };

	inline
	flx l_Triode(const flx inp, const flx hard) {
		return flx ((inp >= l_C_0) ?
				( inp * (hard+(inp+inp)) / ( (l_C_10*(l_C_tenth+inp)) + ((hard+inp)*inp) + (inp*inp) ) )
			:	( inp * (hard-(inp+inp)) / ( (l_C_10*(l_C_tenth-inp)) + ((hard-inp)*-inp) + (inp*inp) ) )
			); };
	inline
	fsx s_Triode(const fsx inp, const fsx hard) {
		return fsx ((inp >= s_C_0) ?
				( inp * (hard+(inp+inp)) / ( (s_C_10*(s_C_tenth+inp)) + ((hard+inp)*inp) + (inp*inp) ) )
			:	( inp * (hard-(inp+inp)) / ( (s_C_10*(s_C_tenth-inp)) + ((hard-inp)*-inp) + (inp*inp) ) )
			); };
	inline
	fhx h_Triode(const fhx inp, const fhx hard) {
		return fhx ((inp >= h_C_0) ?
				( inp * (hard+(inp+inp)) / ( (h_C_10*(h_C_tenth+inp)) + ((hard+inp)*inp) + (inp*inp) ) )
			:	( inp * (hard-(inp+inp)) / ( (h_C_10*(h_C_tenth-inp)) + ((hard-inp)*-inp) + (inp*inp) ) )
			); };

	inline
	flx l_TriodeX(const flx inp, const flx hard, const flx old) {
		return flx ((inp >= l_C_0) ?
				( inp * (hard+inp) / ( (l_C_100*(l_C_1+old)) + (hard*inp) + (inp*inp) ) )
			:	( inp * (hard-inp) / ( (l_C_100*(l_C_1-old)) + (hard*-inp) + (inp*inp) ) )
			); };
	inline
	fsx s_TriodeX(const fsx inp, const fsx hard, const fsx old) {
		return fsx ((inp >= s_C_0) ?
				( inp * (hard+inp) / ( (s_C_100*(s_C_1+old)) + (hard*inp) + (inp*inp) ) )
			:	( inp * (hard-inp) / ( (s_C_100*(s_C_1-old)) + (hard*-inp) + (inp*inp) ) )
			); };
	inline
	fhx h_TriodeX(const fhx inp, const fhx hard, const fhx old) {
		return fhx ((inp >= h_C_0) ?
				( inp * (hard+inp) / ( (h_C_100*(h_C_1+old)) + (hard*inp) + (inp*inp) ) )
			:	( inp * (hard-inp) / ( (h_C_100*(h_C_1-old)) + (hard*-inp) + (inp*inp) ) )
			); };

	inline
	flx l_TriodeY(const flx inp, const flx hard, const flx old) {
		return flx ((inp >= l_C_0) ?
				( inp * l_C_half * l_C_10 * (hard+(inp+inp)) / ( (l_C_10*(l_C_1-old)) + (hard*inp) + (inp*inp) ) )
			:	( inp * l_C_half * l_C_10 * (hard-(inp+inp)) / ( (l_C_10*(l_C_1-old)) + (hard*-inp) + (inp*inp) ) )
			); };
	inline
	fsx s_TriodeY(const fsx inp, const fsx hard, const fsx old) {
		return fsx ((inp >= s_C_0) ?
				( inp * s_C_half * s_C_10 * (hard+(inp+inp)) / ( (s_C_10*(s_C_1-old)) + (hard*inp) + (inp*inp) ) )
			:	( inp * s_C_half * s_C_10 * (hard-(inp+inp)) / ( (s_C_10*(s_C_1-old)) + (hard*-inp) + (inp*inp) ) )
			); };
	inline
	fhx h_TriodeY(const fhx inp, const fhx hard, const fhx old) {
		return fhx ((inp >= h_C_0) ?
				( inp * h_C_half * h_C_10 * (hard+(inp+inp)) / ( (h_C_10*(h_C_1-old)) + (hard*inp) + (inp*inp) ) )
			:	( inp * h_C_half * h_C_10 * (hard-(inp+inp)) / ( (h_C_10*(h_C_1-old)) + (hard*-inp) + (inp*inp) ) )
			); };

	inline
	flx l_TriodeZ(const flx inp, const flx hard, const flx xf, const flx slp, const flx pSmp) {
		return flx ((inp >= l_C_0) ?
				( inp * (hard+(inp*inp)) / ( (xf*(slp+pSmp)) + (hard*inp) + (inp*inp*inp) ) )
			:	( inp * (hard+(inp*inp)) / ( (xf*(slp+pSmp)) + (hard*-inp) + (inp*inp*-inp) ) )
			); };
	inline
	fsx s_TriodeZ(const fsx inp, const fsx hard, const fsx xf, const fsx slp, const fsx pSmp) {
		return fsx ((inp >= s_C_0) ?
				( inp * (hard+(inp*inp)) / ( (xf*(slp+pSmp)) + (hard*inp) + (inp*inp*inp) ) )
			:	( inp * (hard+(inp*inp)) / ( (xf*(slp+pSmp)) + (hard*-inp) + (inp*inp*-inp) ) )
			); };
	inline
	fhx h_TriodeZ(const fhx inp, const fhx hard, const fhx xf, const fhx slp, const fhx pSmp) {
		return fhx ((inp >= h_C_0) ?
				( inp * (hard+(inp*inp)) / ( (xf*(slp+pSmp)) + (hard*inp) + (inp*inp*inp) ) )
			:	( inp * (hard+(inp*inp)) / ( (xf*(slp+pSmp)) + (hard*-inp) + (inp*inp*-inp) ) )
			); };

	inline
	flx l_Triode_XX(const flx inp, const flx hard, const flx xf, const flx slp) {
		const flx t_hFac_a = flx ( (inp >= l_C_0) ? (inp) : (-inp) );
		const flx t_hFac_b = flx ( (t_hFac_a >= l_C_1) ? (l_C_1) : (t_hFac_a * (l_C_2-t_hFac_a)) );
		const flx x_slope = flx (slp + (t_hFac_b*hard));
		return flx ((inp >= l_C_0) ?
				( inp * (hard+(inp*inp)) / ( (xf*x_slope) + (hard*inp) + (inp*inp*inp) ) )
			:	( inp * (hard+(inp*inp)) / ( (xf*x_slope) + (hard*-inp) + (inp*inp*-inp) ) )
			); };
	inline
	fsx s_Triode_XX(const fsx inp, const fsx hard, const fsx xf, const fsx slp) {
		const fsx t_hFac_a = fsx ( (inp >= s_C_0) ? (inp) : (-inp) );
		const fsx t_hFac_b = fsx ( (t_hFac_a >= s_C_1) ? (s_C_1) : (t_hFac_a * (s_C_2-t_hFac_a)) );
		const fsx x_slope = fsx (slp + (t_hFac_b*hard));
		return fsx ((inp >= s_C_0) ?
				( inp * (hard+(inp*inp)) / ( (xf*x_slope) + (hard*inp) + (inp*inp*inp) ) )
			:	( inp * (hard+(inp*inp)) / ( (xf*x_slope) + (hard*-inp) + (inp*inp*-inp) ) )
			); };
	inline
	fhx h_Triode_XX(const fhx inp, const fhx hard, const fhx xf, const fhx slp) {
		const fhx t_hFac_a = fhx ( (inp >= h_C_0) ? (inp) : (-inp) );
		const fhx t_hFac_b = fhx ( (t_hFac_a >= h_C_1) ? (h_C_1) : (t_hFac_a * (h_C_2-t_hFac_a)) );
		const fhx x_slope = fhx (slp + (t_hFac_b*hard));
		return fhx ((inp >= h_C_0) ?
				( inp * (hard+(inp*inp)) / ( (xf*x_slope) + (hard*inp) + (inp*inp*inp) ) )
			:	( inp * (hard+(inp*inp)) / ( (xf*x_slope) + (hard*-inp) + (inp*inp*-inp) ) )
			); };

	inline
	fhx h_Triode_XXX(const fhx inp, const fhx hard, const fhx xf, const fhx slp) {
		return fhx ((inp >= h_C_0) ?
				( inp * (hard+(inp*inp)) / ( (xf*(slp+inp)) + (hard* inp) + (inp*inp* inp) ) )
			:	( inp * (hard+(inp*inp)) / ( (xf*(slp-inp)) + (hard*-inp) + (inp*inp*-inp) ) )
			); };

	inline
	fhx h_Triode_Layered(const fhx inp, const fhx gain, const fhx modulation, const int layers) {
		const int lay = ( (layers < 1) ? (1) : (layers) );
		const fhx mult = fhx ( (lay==1) ? (h_C_1) : (lay==2) ? (h_C_2) : (lay==3) ? (h_C_3) : (lay==4) ? (h_C_4) : (h_C_5) );
		const fhx wv = fhx ( (modulation >= h_C_0) ?
							(modulation * (h_C_1+modulation) / (h_C_1 + (modulation*modulation) + modulation)) :
							(modulation * (h_C_1-modulation) / (h_C_1 + (modulation*modulation) - modulation)) );
		const fhx gn = fhx ( (h_C_2 + wv) / gain );
		const fhx layA = fhx ( (inp >= h_C_0) ? (inp) : (-inp) );
		const fhx layB = fhx ( (lay > 1) ? (gn + layA) : (gn) );
		const fhx layC = fhx ( (lay > 2) ? ((h_C_2*gn) + layA) : (h_C_0) );
		const fhx layD = fhx ( (lay > 3) ? ((h_C_3*gn) + layA) : (h_C_0) );
		const fhx layE = fhx ( (lay > 4) ? ((h_C_4*gn) + layA) : (h_C_0) );
		const fhx t_calc = fhx ( mult / (layA + layB + layC + layD + layE) );
		return fhx ( inp * t_calc );
	};

	inline
	flx l_Triode_Layered_Optimized(flx inp, const flx gain, const flx modulation, const int layers, const flx modSlope) {
		const int lay = ( (layers < 2) ? (2) : (layers) );
		const flx mult = l_CA_Standard[( (lay > 10) ? (10) : (lay) )];
		const flx wv = flx ( (inp >= l_C_0) ?
							( inp * (modulation+inp) / (l_C_1 + (inp*inp) + (modulation* inp)) ) :
							( inp * (modulation-inp) / (l_C_1 + (inp*inp) + (modulation*-inp)) ) ) ;
		const flx gx = flx ( l_C_half * ((inp >= l_C_0) ? (inp) : (-inp)) );
		const flx gy = flx ( gx * (gx+l_C_1) / (l_C_1 + gx + (gx*gx)) );
		inp *= flx (l_C_half - gy);
		const flx mSlope = flx ( (modSlope < l_C_1) ? (l_C_1) : (modSlope) );
		const flx gn = flx ( l_C_4 * (mSlope + wv) / gain );
		const flx w0 = flx ( (inp >= l_C_0) ? (inp) : (-inp) );
		const flx w1 = flx ( gn * l_CA_Exponential[( (lay > 10) ? (9) : (lay-1) )] );
		const flx w2 = flx ( w0 * mult );
		return flx ( inp * mult / (w1+w2) );
	};
	inline
	fsx s_Triode_Layered_Optimized(fsx inp, const fsx gain, const fsx modulation, const int layers, const fsx modSlope) {
		const int lay = ( (layers < 2) ? (2) : (layers) );
		const fsx mult = s_CA_Standard[( (lay > 10) ? (10) : (lay) )];
		const fsx wv = fsx ( (inp >= s_C_0) ?
							( inp * (modulation+inp) / (s_C_1 + (inp*inp) + (modulation* inp)) ) :
							( inp * (modulation-inp) / (s_C_1 + (inp*inp) + (modulation*-inp)) ) ) ;
		const fsx gx = fsx ( s_C_half * ((inp >= s_C_0) ? (inp) : (-inp)) );
		const fsx gy = fsx ( gx * (gx+s_C_1) / (s_C_1 + gx + (gx*gx)) );
		inp *= fsx (s_C_half - gy);
		const fsx mSlope = fsx ( (modSlope < s_C_1) ? (s_C_1) : (modSlope) );
		const fsx gn = fsx ( s_C_4 * (mSlope + wv) / gain );
		const fsx w0 = fsx ( (inp >= s_C_0) ? (inp) : (-inp) );
		const fsx w1 = fsx ( gn * s_CA_Exponential[( (lay > 10) ? (9) : (lay-1) )] );
		const fsx w2 = fsx ( w0 * mult );
		return fsx ( inp * mult / (w1+w2) );
	};
	inline
	fhx h_Triode_Layered_Optimized(fhx inp, const fhx gain, const fhx modulation, const int layers, const fhx modSlope) {
		const int lay = ( (layers < 2) ? (2) : (layers) );
		const fhx mult = h_CA_Standard[( (lay > 10) ? (10) : (lay) )];
		const fhx wv = fhx ( (inp >= h_C_0) ?
							( inp * (modulation+inp) / (h_C_1 + (inp*inp) + (modulation* inp)) ) :
							( inp * (modulation-inp) / (h_C_1 + (inp*inp) + (modulation*-inp)) ) ) ;
		const fhx gx = fhx ( h_C_half * ((inp >= h_C_0) ? (inp) : (-inp)) );
		const fhx gy = fhx ( gx * (gx+h_C_1) / (h_C_1 + gx + (gx*gx)) );
		inp *= fhx (h_C_half - gy);
		const fhx mSlope = fhx ( (modSlope < h_C_1) ? (h_C_1) : (modSlope) );
		const fhx gn = fhx ( h_C_4 * (mSlope + wv) / gain );
		const fhx w0 = fhx ( (inp >= h_C_0) ? (inp) : (-inp) );
		const fhx w1 = fhx ( gn * h_CA_Exponential[( (lay > 10) ? (9) : (lay-1) )] );
		const fhx w2 = fhx ( w0 * mult );
		return fhx ( inp * mult / (w1+w2) );
	};

	inline
	flx l_Triode_Simple(flx inp, const flx gain, const flx modulation, const flx slope) {
		const flx md = flx (modulation + l_C_1);
		inp *= flx (md);
		const flx gn = flx (l_C_2 / gain);
		const flx sa = flx ( (inp >= l_C_0) ? inp : -inp );
		const flx out = flx ( inp * (slope + (sa*gain)) / (gn + (sa*slope*gain) + sa) );
		return flx ( out / md );
	};
	inline
	fsx s_Triode_Simple(fsx inp, const fsx gain, const fsx modulation, const fsx slope) {
		const fsx md = fsx (modulation + s_C_1);
		inp *= fsx (md);
		const fsx gn = fsx (s_C_2 / gain);
		const fsx sa = fsx ( (inp >= s_C_0) ? inp : -inp );
		const fsx out = fsx ( inp * (slope + (sa*gain)) / (gn + (sa*slope*gain) + sa) );
		return fsx ( out / md );
	};
	inline
	fhx h_Triode_Simple(fhx inp, const fhx gain, const fhx modulation, const fhx slope) {
		const fhx md = fhx (modulation + h_C_1);
		inp *= fhx (md);
		const fhx gn = fhx (h_C_2 / gain);
		const fhx sa = fhx ( (inp >= h_C_0) ? inp : -inp );
		const fhx out = fhx ( inp * (slope + (sa*gain)) / (gn + (sa*slope*gain) + sa) );
		return fhx ( out / md );
	};

	inline
	flx l_Rectify_Simple(const flx inp, const flx mFactor) {
		const flx mf = flx (mFactor + l_C_1);
		const flx mult = flx ( mf * l_C_3 );
		const flx x = flx ( (inp >= l_C_0) ?
							( inp * (l_C_1+inp) / ((l_C_1/mf) + (inp*inp) + inp) ) :
							( inp * (l_C_1-inp) / ((l_C_1/mf) + (inp*inp) - inp) ) );
		return flx ( mult * (-inp + (x*mf)) );
	};
	inline
	fsx s_Rectify_Simple(const fsx inp, const fsx mFactor) {
		const fsx mf = fsx (mFactor + s_C_1);
		const fsx mult = fsx ( mf * s_C_3 );
		const fsx x = fsx ( (inp >= s_C_0) ?
							( inp * (s_C_1+inp) / ((s_C_1/mf) + (inp*inp) + inp) ) :
							( inp * (s_C_1-inp) / ((s_C_1/mf) + (inp*inp) - inp) ) );
		return fsx ( mult * (-inp + (x*mf)) );
	};
	inline
	fhx h_Rectify_Simple(const fhx inp, const fhx mFactor) {
		const fhx mf = fhx (mFactor + h_C_1);
		const fhx mult = fhx ( mf * h_C_3 );
		const fhx x = fhx ( (inp >= h_C_0) ?
							( inp * (h_C_1+inp) / ((h_C_1/mf) + (inp*inp) + inp) ) :
							( inp * (h_C_1-inp) / ((h_C_1/mf) + (inp*inp) - inp) ) );
		return fhx ( mult * (-inp + (x*mf)) );
	};

	inline
	fhx FIRwindow_Bartlett(const int weight, const int size) {
		const fhx w = fhx (weight);
		const fhx s = fhx (size);
		return fhx ( h_C_1 - ( (h_C_2*(w-(s*h_C_half))) / s ) );
	};
	inline
	fhx FIRwindow_Hanning(const int weight, const int size) {
		const fhx w = fhx (weight);
		const fhx s = fhx (size);
		return fhx ( h_C_half - ( h_C_half*cosl( (h_C_pi_doub*w) / s ) ) );
	};
	inline
	fhx FIRwindow_Hamming(const int weight, const int size) {
		const fhx w = fhx (weight);
		const fhx s = fhx (size);
		return fhx ( (fhx) 0.54 - ( (fhx) 0.46 * cosl( (h_C_pi_doub*w) / s ) ) );
	};
	inline
	fhx FIRwindow_Blackman(const int weight, const int size) {
		const fhx w = fhx (weight);
		const fhx s = fhx (size);
		return fhx ( ((fhx) 0.42) - (h_C_half * cosl((h_C_pi_doub*w)/s)) + ((fhx) 0.08 * cosl((h_C_pi*h_C_4*w)/s)) );
	};

//!	These are used to compute a near-zero value for handling denormals.
	inline
	flx l_calc_denorm() { return flx (powf(10.0f, -28.0f)); };
	inline
	fsx s_calc_denorm() { return fsx (pow(10.0, -30.0)); };
	inline
	fhx h_calc_denorm() { return fhx (powl((fhx) 10.0, (fhx) -32.0)); };
}

class RateFC
{
public:
	RateFC();
	static const int vi[7];
	static const flx vl[7];
	static const fsx vs[7];
	static const fhx vh[7];
	const flx zl;
	const fsx zs;
	const fhx zh;
	const flx l_dnrm;
	const fsx s_dnrm;
	const fhx h_dnrm;
};

#endif
