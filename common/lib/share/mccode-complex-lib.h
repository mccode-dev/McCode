/*****************************************************************************
*
* McStas, neutron ray-tracing package and McXtrace X-ray ray-tracing package
*         Copyright 1997-2025, All rights reserved
*         DTU Physics, Kongens Lyngby, Denmark
*
* Library: share/mccode-complex-lib.h
*
* %Identification
* Written by: Peter Willendrup
* Date: November, 2025
* Origin: DTU
*
* This file is to be imported by components requiring support for complex numbers.
* Includes solutions for MSVC on Windows which does not implement e.g. c99 complex.
*
* Usage: within SHARE
* %include "mccode-complex-lib"
*
****************************************************************************/

#ifndef MCCODE_COMPLEX_LIB_H
#define MCCODE_COMPLEX_LIB_H

// All supported compilers (gcc, clang, MSVC, nvc) provide <complex.h>; MSVC
// lacks the C99 complex operators, hence the function wrappers below.
#include <complex.h>
// NOT MSVC
#ifndef _MSC_EXTENSIONS
// Note: leave two spaces between double and cdouble because double->float
// in generate._convert_type is not clever enough.
typedef complex double  cdouble;
inline cdouble cplx(const double x, const double y) { return x + y*I; }
inline cdouble cneg(const cdouble a) { return -a; }
inline cdouble radd(const double a, const cdouble b) { return a+b; }
inline cdouble rmul(const double a, const cdouble b) { return a*b; }
inline cdouble cadd(const cdouble a, const cdouble b) { return a+b; }
inline cdouble csub(const cdouble a, const cdouble b) { return a-b; }
inline cdouble cmul(const cdouble a, const cdouble b) { return a*b; }
inline cdouble cdiv(const cdouble a, const cdouble b) { return a/b; }
#else
// MSVC
typedef _Dcomplex  cdouble;
inline cdouble cplx(const double x, const double y) { cdouble z = _Cbuild(x,y); return z; }
inline cdouble cneg(const cdouble a) { return cplx( -creal(a) , -cimag(a) ); }
inline cdouble radd(const double a, const cdouble b) { return cplx(a+creal(b),cimag(b)); }
inline cdouble rmul(const double a, const cdouble b) { return cplx(a*creal(b),a*cimag(b)); }
inline cdouble cadd(const cdouble a, const cdouble b) { return cplx(creal(a)+creal(b),cimag(a)+cimag(b)); }
inline cdouble csub(const cdouble a, const cdouble b) { return cplx(creal(a)-creal(b),cimag(a)-cimag(b)); }
inline cdouble cmul(const cdouble a, const cdouble b) { return cplx(creal(a)*creal(b) - cimag(a)*cimag(b), cimag(a)*creal(b) + creal(a)*cimag(b)); }
inline cdouble cdiv(const cdouble a, const cdouble b) { return cplx((creal(a)*creal(b)+cimag(a)*cimag(b))/(creal(b)*creal(b)+cimag(b)*cimag(b)),(cimag(a)*creal(b)-creal(a)*cimag(b))/(creal(b)*creal(b)+cimag(b)*cimag(b))); }
#endif

#endif
/* end of mccode-complex-lib.h */
