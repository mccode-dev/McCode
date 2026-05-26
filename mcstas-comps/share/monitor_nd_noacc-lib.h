/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright 1997-2002, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Library: share/monitor_nd_noacc-lib.h
*
* %Identification
* Written by: EF
* Date: Aug 28, 2002
* Origin: ILL
* Modified by: TW, Nov 2020: introduced user doubles
* Release: McStas 1.6
* Version: $Revision$
*
* This file is to be imported by the monitor_nd related components
* It handles some shared functions.
*
* Usage: within SHARE
* %include "monitor_nd_noacc-lib"
*
*******************************************************************************/

#ifndef MONITOR_ND_NOACCLIB_H
#define MONITOR_ND_NOACCLIB_H "Yes"
#define MONnD_COORD_NMAX  30  /* max number of variables to record */

#ifdef OPENACC
#define OPENACC_BAK
#undef OPENACC
#undef printf
#undef sprintf
#undef exit
#endif

  typedef struct Monitornd_noaccDefines
  {
    int COORD_NONE  ;
    int COORD_X     ;
    int COORD_Y     ;
    int COORD_Z     ;
    int COORD_RADIUS; 
    int COORD_VX    ;
    int COORD_VY    ;
    int COORD_VZ    ;
    int COORD_V     ;
    int COORD_T     ;
    int COORD_P     ;
    int COORD_SX    ;
    int COORD_SY    ;
    int COORD_SZ    ;
    int COORD_KX    ;
    int COORD_KY    ;
    int COORD_KZ    ;
    int COORD_K     ;
    int COORD_ENERGY;
    int COORD_LAMBDA;
    int COORD_KXY   ;
    int COORD_KYZ   ;
    int COORD_KXZ   ;
    int COORD_VXY   ;
    int COORD_VYZ   ;
    int COORD_VXZ   ;
    int COORD_HDIV  ;
    int COORD_VDIV  ;
    int COORD_ANGLE ;
    int COORD_NCOUNT;
    int COORD_THETA ;
    int COORD_PHI   ;
    int COORD_USER0 ;
    int COORD_USER1 ;
    int COORD_USER2 ;
    int COORD_USER3 ;
    int COORD_USER4 ;
    int COORD_USER5 ;
    int COORD_USER6 ;
    int COORD_USER7 ;
    int COORD_USER8 ;
    int COORD_USER9 ;
    int COORD_USERDOUBLE0 ;
    int COORD_USERDOUBLE1 ;
    int COORD_USERDOUBLE2 ;
    int COORD_USERDOUBLE3 ;
    int COORD_USERDOUBLE4 ;
    int COORD_USERDOUBLE5 ;
    int COORD_USERDOUBLE6 ;
    int COORD_USERDOUBLE7 ;
    int COORD_USERDOUBLE8 ;
    int COORD_USERDOUBLE9 ;
    int COORD_USERDOUBLE10 ;
    int COORD_USERDOUBLE11 ;
    int COORD_USERDOUBLE12 ;
    int COORD_USERDOUBLE13 ;
    int COORD_USERDOUBLE14 ;
    int COORD_USERDOUBLE15 ;
    int COORD_XY    ;
    int COORD_XZ    ;
    int COORD_YZ    ;
    int COORD_PIXELID;

    /* token modifiers */
    int COORD_VAR   ; /* next token should be a variable or normal option */
    int COORD_MIN   ; /* next token is a min value */
    int COORD_MAX   ; /* next token is a max value */
    int COORD_DIM   ; /* next token is a bin value */
    int COORD_FIL   ; /* next token is a filename */
    int COORD_EVNT  ; /* next token is a buffer size value */
    int COORD_3HE   ; /* next token is a 3He pressure value */
    int COORD_LOG   ; /* next variable will be in log scale */
    int COORD_ABS   ; /* next variable will be in abs scale */
    int COORD_SIGNAL; /* next variable will be the signal var */
    int COORD_AUTO  ; /* set auto limits */

    char TOKEN_DEL[32]; /* token separators */

    char SHAPE_SQUARE; /* shape of the monitor */
    char SHAPE_DISK  ;
    char SHAPE_SPHERE;
    char SHAPE_CYLIND;
    char SHAPE_BANANA; /* cylinder without top/bottom, on restricted angular area */
    char SHAPE_BOX   ;
    char SHAPE_PREVIOUS;
    char SHAPE_OFF;

  } Monitornd_noaccDefines_type;

  typedef struct Monitornd_noaccVariables
  {
    double area;
    double Sphere_Radius     ;
    double Cylinder_Height   ;
    char   Flag_With_Borders ;   /* 2 means xy borders too */
    char   Flag_List         ;   /* 1 store 1 buffer, 2 is list all, 3 list all+append */
    char   Flag_nexusbins    ;   /* Only relevant in NeXus mode. -1 fully disable, 0 store BINS arrays if list mode, 1 store always */
    char   Flag_Multiple     ;   /* 1 when n1D, 0 for 2D */
    char   Flag_Verbose      ;
    int    Flag_Shape        ;
    char   Flag_Auto_Limits  ;   /* get limits from first Buffer */
    char   Flag_Absorb       ;   /* monitor is also a slit */
    char   Flag_per_cm2      ;   /* flux is per cm2 */
    char   Flag_log          ;   /* log10 of the flux */
    char   Flag_parallel     ;   /* set neutron state back after detection (parallel components) */
    char   Flag_Binary_List  ;
    char   Flag_capture      ;   /* lambda monitor with lambda/lambda(2200m/s = 1.7985 Angs) weightening */
    int    Flag_signal       ;   /* 0:monitor p, else monitor a mean value */
    int    Flag_mantid       ;   /* 0:normal monitor, else do mantid-event specifics */
    int    Flag_OFF          ;   /* Flag to indicate external geometry from OFF file */
    long long OFF_polyidx;   /* When intersection is done externally by off_intersect, this gives the 
				    polygon number, i.e. pixel index */
    unsigned long Coord_Number      ;   /* total number of variables to monitor, plus intensity (0) */
    unsigned long Coord_NumberNoPixel;  /* same but without counting PixelID */
    unsigned long Buffer_Block      ;   /* Buffer size for list or auto limits */
    long long Neutron_Counter   ;   /* event counter, simulation total counts is mcget_ncount() */
    unsigned long Buffer_Counter    ;   /* index in Buffer size (for realloc) */
    unsigned long Buffer_Size       ;
    int    Coord_Type[MONnD_COORD_NMAX];      /* type of variable */
    char   Coord_Label[MONnD_COORD_NMAX][30]; /* label of variable */
    char   Coord_Var[MONnD_COORD_NMAX][30];   /* short id of variable */
    long   Coord_Bin[MONnD_COORD_NMAX];       /* bins of variable array */
    long   Coord_BinProd[MONnD_COORD_NMAX];   /* product of bins of variable array */
    double Coord_Min[MONnD_COORD_NMAX];
    double Coord_Max[MONnD_COORD_NMAX];
    char   Monitor_Label[MONnD_COORD_NMAX*30];/* Label for monitor */
    char   Mon_File[128];                     /* output file name */

    /* these don't seem to be used anymore as they are superseded by _particle
    double cx, cy, cz;
    double cvx, cvy, cvz;
    double ckx, cky, ckz;
    double csx, csy, csz;
    double cEx, cEy, cEz;
    double cs1, cs2, ct, cphi, cp; */

    double He3_pressure;
    char   Flag_UsePreMonitor    ;   /* use a previously stored neutron parameter set */
    char   UserName0[128];
    char   UserName1[128];
    char   UserName2[128];
    char   UserName3[128];
    char   UserName4[128];
    char   UserName5[128];
    char   UserName6[128];
    char   UserName7[128];
    char   UserName8[128];
    char   UserName9[128];
    char   UserVariable0[128];
    char   UserVariable1[128];
    char   UserVariable2[128];
    char   UserVariable3[128];
    char   UserVariable4[128];
    char   UserVariable5[128];
    char   UserVariable6[128];
    char   UserVariable7[128];
    char   UserVariable8[128];
    char   UserVariable9[128];
    double UserDoubles[16];
    char   option[CHAR_BUF_LENGTH];

    long long int Nsum;
    double psum, p2sum;
    double **Mon2D_N;
    double **Mon2D_p;
    double **Mon2D_p2;
    double *Mon2D_Buffer;
    unsigned long PixelID;

    double mxmin,mxmax,mymin,mymax,mzmin,mzmax;
    double mean_dx, mean_dy, min_x, min_y, max_x, max_y, mean_p;

    char   compcurname[128];
    Coords compcurpos;
    Rotation compcurrot;
    int compcurindex;
  } Monitornd_noaccVariables_type;

/* monitor_nd_noacc-lib function prototypes */
/* ========================================================================= */

void Monitor_nd_noaccInit(Monitornd_noaccDefines_type *, Monitornd_noaccVariables_type *, MCNUM, MCNUM, MCNUM, MCNUM, MCNUM, MCNUM, MCNUM, MCNUM, MCNUM, int, int);
int Monitor_nd_noaccTrace(Monitornd_noaccDefines_type *, Monitornd_noaccVariables_type *, _class_particle* _particle);
MCDETECTOR Monitor_nd_noaccSave(Monitornd_noaccDefines_type *, Monitornd_noaccVariables_type *);
void Monitor_nd_noaccFinally(Monitornd_noaccDefines_type *, Monitornd_noaccVariables_type *);
void Monitor_nd_noaccMcDisplay(Monitornd_noaccDefines_type *, Monitornd_noaccVariables_type *);

#ifdef OPENACC_BAK
#define OPENACC
#define fprintf(stderr,...) printf(__VA_ARGS__)
#define sprintf(string,...) printf(__VA_ARGS__)
#define printf(...) noprintf()
#define exit(...) noprintf()
#endif

#endif


/* end of monitor_nd_noacc-lib.h */
