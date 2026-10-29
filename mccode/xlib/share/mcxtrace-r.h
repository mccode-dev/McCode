/*******************************************************************************
*
* McXtrace, X-ray ray-tracing package
*         Copyright (C) 1997-2009, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Synchrotron SOLEIL, Saint-Aubin, France
*
* Runtime: share/mcxtrace-r.h
*
* %Identification
* Written by: KN
* Date:    Aug 29, 1997
* Release: McXtrace X.Y
* Version: $Revision$
*
* Runtime system header for McXtrace.
*
* In order to use this library as an external library, the following variables
* and macros must be declared (see details in the code)
*
*   struct mcinputtable_struct mcinputtable[];
*   int mcnumipar;
*   char instrument_name[], instrument_source[];
*   int traceenabled, defaultmain;
*   extern MCNUM  mccomp_storein[];
*   extern MCNUM  instrument.counter_AbsorbProp[];
*   extern MCNUM  mcScattered;
*   #define MCCODE_STRING "the McXtrace version"
*
* Usage: Automatically embbeded in the c code.
*
*******************************************************************************/

#ifndef MCXTRACE_R_H
#define MCXTRACE_R_H "$Revision$"

/* Following part is only embedded when not redundant with mcxtrace.h */

#ifndef MCCODE_H

#define CELE     1.602176487e-19   /* [C] Elementary charge CODATA 2006*/
#define MELECTRON 9.10938291e-31    /* [kg] Electron mass CODATA 2006*/
#define M_C      299792458         /* [m/s] speed of light CODATA 2006*/
#define E2K      0.506773091264796 /* Convert E[keV] to k[1/AA] (CELE/(HBAR*M_C)*1e-10)*1e3 */
#define K2E      1.97326972808327  /* Convert k[1/AA] to E[keV] (1e10*M_C*HBAR/CELE)/1e3 */
#define RE       2.8179402894e-5   /*[AA] Thomson scattering length*/
#define ALPHA    7.2973525698e-3   /*[ ] Fine structure constant CODATA 2006*/

#define SCATTER0 do {DEBUG_SCATTER(); SCATTERED++;} while(0)
#define SCATTER SCATTER0

void SCATTER_func(_class_particle *_particle); /* provides function to SCATTER from within libaries */

#define JUMPTOCOMP(comp) mcphoton->_index = INDEX_COMP(comp);

/*magnet stuff is probably redundant*/
#define MAGNET_ON \
  do { \
  } while(0)

#define MAGNET_OFF \
  do { \
  } while(0)

#define ALLOW_BACKPROP \
  do { \
    allow_backprop = 1; \
  } while(0)

#define DISALLOW_BACKPROP \
  do { \
    allow_backprop = 0; \
  } while(0)

/*An interrupt a'la mcMagnet should be inserted below if there's non-zero permeability*/
/*perhaps some kind of PROP_POL*/

#define mcPROP_DL(dl) \
  do { \
    MCNUM mc_k=sqrt( scalar_prod(kx,ky,kz,kx,ky,kz));\
    x = x+ (dl)*kx/mc_k;\
    y = y+ (dl)*ky/mc_k;\
    z = z+ (dl)*kz/mc_k;\
    phi = phi+ 1e10*mc_k*(dl);\
    t = t + (dl)/((double)M_C);\
  }while (0)
/* this had to be taken out to avoid error 700. This may need to be atomic, but probably should be somewhere else.*/
/*    if (isnan(p) || isinf(p)) { instrument->counter_AbsorbProp[INDEX_CURRENT_COMP] =  instrument->counter_AbsorbProp[INDEX_CURRENT_COMP] + 1; ABSORB; }\
*/

/*adapted from PROP_DT(dt)*/
#define PROP_DL(dl) \
  do{ \
    if(dl < 0 && allow_backprop == 0) { RESTORE=1; ABSORB; }; \
    mcPROP_DL(dl); \
    DISALLOW_BACKPROP;\
  } while (0)

/* propagate by time dt [s]: x-rays move c*dt along k, no gravity */
#define PROP_DT(dt) PROP_DL((dt)*M_C)

#define PROP_X0 \
  do { \
    mcPROP_X0; \
    DISALLOW_BACKPROP; \
  }while(0)

#define mcPROP_X0 \
  do { \
    MCNUM mc_dl,mc_k; \
    if(kx == 0) { ABSORB; }; \
    mc_k=sqrt(scalar_prod(kx,ky,kz,kx,ky,kz)); \
    mc_dl= -x * mc_k / kx; \
    if(mc_dl<0 && allow_backprop==0) { ABSORB; };\
    PROP_DL(mc_dl); \
    x = 0; \
  } while(0)

#define PROP_Y0 \
  do { \
    mcPROP_Y0; \
    DISALLOW_BACKPROP; \
  }while(0)

#define mcPROP_Y0 \
  do { \
    MCNUM mc_dl,mc_k; \
    if(ky == 0) { ABSORB; }; \
    mc_k=sqrt(scalar_prod(kx,ky,kz,kx,ky,kz)); \
    mc_dl= -y * mc_k / ky; \
    if(mc_dl<0 && allow_backprop==0) { ABSORB; };\
    PROP_DL(mc_dl); \
    y = 0; \
  } while(0)

#define PROP_Z0 \
  do { \
    mcPROP_Z0; \
    DISALLOW_BACKPROP; \
  }while(0)

#define mcPROP_Z0 \
  do { \
    MCNUM mc_dl,mc_k; \
    if(kz == 0) { ABSORB; }; \
    mc_k=sqrt(scalar_prod(kx,ky,kz,kx,ky,kz)); \
    mc_dl= -z * mc_k / kz; \
    if(mc_dl<0 && allow_backprop==0) { ABSORB; };\
    PROP_DL(mc_dl); \
    z = 0; \
  } while(0)

#ifdef DEBUG

#define DEBUG_STATE() if(!mcdotrace); else \
  printf("STATE: %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g\n", \
      x,y,z,kx,ky,kz,phi,t,Ex,Ey,Ez,p);
#define DEBUG_SCATTER() if(!mcdotrace); else \
  printf("SCATTER: %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g\n", \
      x,y,z,kx,ky,kz,phi,t,Ex,Ey,Ez,p);

#else

#define DEBUG_STATE()
#define DEBUG_SCATTER()

#endif

#endif /* !MCCODE_H */

#endif /* MCXTRACE_R_H */
