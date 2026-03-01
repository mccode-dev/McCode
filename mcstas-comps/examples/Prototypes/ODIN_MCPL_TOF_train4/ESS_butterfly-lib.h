/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright 1997-2013, All rights reserved
*         DTU Physics, Lyngby, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Library: share/ESS_butterfly-lib.h
*
* %Identification
* Written by: PW
* Date: Nov 7, 2013
* Origin: DTU Physics
* Release: McStas 2.1
* Version: 0.1
*
* This file is to be imported by the ESS_moderator_long component
* It defines a set of brilliance definitions (used via function pointer) for
* easier use of the component.
*
* Usage: within SHARE
* %include "ESS_butterfly-lib"
*
*******************************************************************************/

#ifndef ESS_BUTTERFLY_LIB_H
#define ESS_BUTTERFLY_LIB_H 0.1

#ifndef ESS_SOURCE_DURATION
#define ESS_SOURCE_DURATION 2.857e-3
#endif

#ifndef ESS_SOURCE_FREQUENCY
#define ESS_SOURCE_FREQUENCY 14
#endif

#ifndef ESS_SOURCE_POWER
#define ESS_SOURCE_POWER 5
#endif

/* Struct for extra source parameters - for future geometrical adjustments */
struct ess_struct {
  double X;
  double Y;
  double Z;
  double height_t;
  double height_c;
  double Width_c;
  double Width_t;
  double Mwidth_c;
  double Mwidth_t;
  double tmultiplier;
  double Radius_c;
  double beamportangle;
  int Uniform;
  double extractionangle;
  int Wasleft;
};
typedef struct ess_struct ess_moderator_struct;

typedef void (*functype)(double* t , double* p, double lambda,  double tfocus_w, double tfocus_t, double tfocus_dt, ess_moderator_struct extras);

double ESS_2015_Schoenfeldt_cold_spectrum(double lambda,double theta);
double ESS_2015_Schoenfeldt_thermal_spectrum(double lambda, double theta);

/* List of brilliance definitions */
void ESS_2015_Schoenfeldt_cold(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y);
void ESS_2015_Schoenfeldt_thermal(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y);

void ESS_2015_Schoenfeldt_cold_no_t(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y);
void ESS_2015_Schoenfeldt_thermal_no_t(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y);

void ESS_2015_Schoenfeldt_cold_only_t(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y);
void ESS_2015_Schoenfeldt_thermal_only_t(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y);

/* List of pulse-shape definitions */
double ESS_2015_Schoenfeldt_cold_timedist(double t, double lambda, double height, double pulselength);
double ESS_2015_Schoenfeldt_thermal_timedist(double t, double lambda, double height, double pulselength);

/* List of moderator-geometry-weighting definitions */
double ESS_2014_Schoenfeldt_cold_y0(double y0,double height);
double ESS_2014_Schoenfeldt_cold_x0(double x0,double height, double width);
double ESS_2014_Schoenfeldt_thermal_y0(double y0,double height);
double ESS_2014_Schoenfeldt_thermal_x0(double x0,double height, double width);

double ESS_2015_Schoenfeldt_cold_y0(double y0);
double ESS_2015_Schoenfeldt_cold_x0(double x0, double theta, double width);
double ESS_2015_Schoenfeldt_thermal_y0(double y0);
double ESS_2015_Schoenfeldt_thermal_x0(double x0,double theta, double width);
double ESS_2015_Schoenfeldt_cold_Y(double x0,double height);
double ESS_2015_Schoenfeldt_thermal_Y(double y0,double height);
double ESS_2015_Schoenfeldt_cold_Theta120(double x0,double height);
double ESS_2015_Schoenfeldt_thermal_Theta120(double beamportangle,int isleft);

/* end of ESS_butterfly-lib.h */
#endif

