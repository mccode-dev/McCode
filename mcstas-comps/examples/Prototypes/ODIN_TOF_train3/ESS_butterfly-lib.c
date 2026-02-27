/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright 1997-2013, All rights reserved
*         DTU Physics, Lyngby, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Library: share/ESS_butterfly-lib.c
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
#error McStas : please import this library with %include "ESS_butterfly-lib"
#endif

#ifdef OPENACC
#define exit(...) noprintf()
#endif

#pragma acc routine seq
double ESS_2015_Schoenfeldt_cold_spectrum(double lambda,double theta){
  if(lambda<=0)return 0;
  double par0=8.44e13/25.;
  double par1=2.5;
  double par2=2.2;
  
  double par3=-13.-.5*(theta-5);
  double par4=2.53;
  double par5=-0.0478073-0.160*exp(-0.45186*(theta-5.)/10.);
  
  double par6;
  if(theta==5)par6=5.73745e+015/25.;
  else if(theta==15)par6=5.88284e+015/25.;
  else if(theta==25)par6=6.09573e+015/25.;
  else if(theta==35)par6=6.29116e+015/25.;
  else if(theta==45)par6=6.03436e+015/25.;
  else if(theta==55)par6=6.02045e+015/25.;
  double par7=0.788956+0.00854184*(theta-5.)/10.;
  double par8=0.0461868-0.0016464*(theta-5.)/10.;
  double par9=0.325;
  
  double SD_part=par0/((1+exp(par1*(lambda-par2)))*lambda);
  double para_part=pow((1+exp(par3*(lambda-par4))),par5)*(par6*(exp(-par7*(lambda))+par8*exp(-par9*(lambda))));
  return para_part+SD_part;
  
}
#pragma acc routine seq
double ESS_2015_Schoenfeldt_thermal_spectrum(double lambda, double theta){
    if(lambda<=0)return 0;
    double i=(theta-5.)/10.;
    double par0=4.2906e+013-9.2758e+011*i+8.02603e+011*i*i-1.29523e+011*i*i*i;
    double par2=6.24806e+012-8.84602e+010*i;
    double par3=-0.31107+0.0221138*i;
    double aOlsqr=949./(325*lambda*lambda);
    return par0*2.*aOlsqr*aOlsqr/lambda*pow(lambda,-par3)*exp(-aOlsqr)+par2/((1+exp(2.5*(lambda-0.88)))*lambda);
	  
}


/* This is ESS_2014_Schoenfeldt_cold_y0 - vertical intensity distribution for the 2014 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2014_Schoenfeldt_cold_y0(double y0,double height){
  
  double one_over_integral_y0_of_height= height/((0.36434*height*height+2.53796*height-0.107774));
  if(y0 < -height/2. || y0 > height/2. )return 0;
  double cosh_ish=(exp(-7e-1/sqrt(height)*(y0-height/2.))+exp(-7e-1/20.*height+7e-1/sqrt(height)*(y0+height/2.)));
  double sinh_ish=(exp(50/sqrt(height)*(y0-height/2.))-1)*(exp(-50/sqrt(height)*(y0+height/2.))-1);
  double tmp=one_over_integral_y0_of_height*cosh_ish*sinh_ish;
  return tmp;
} /* end of ESS_2014_Schoenfeldt_cold_y0 */

/* This is ESS_2014_Schoenfeldt_thermal_y0 - vertical intensity distribution for the 2014 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2014_Schoenfeldt_thermal_y0(double y0,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2014_Schoenfeldt_thermal_y0 */

/* This is ESS_2014_Schoenfeldt_cold_x0 - horizontal intensity distribution for the 2014 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2014_Schoenfeldt_cold_x0(double x0,double height, double width){
  double normalization=1;
  if(x0<-width||x0>width)return 0;
  return normalization*(0.008*x0+1)*(exp(height/2.*(x0-width/2))-1)*(exp(-height/2.*(x0+width/2))-1);
} /* end of ESS_2014_Schoenfeldt_cold_x0 */

/* This is ESS_2014_Schoenfeldt_thermal_x0 - horizontal intensity distribution for the 2014 Schoenfeldt cold moderator */

double ESS_2014_Schoenfeldt_thermal_x0(double x0,double height, double width){
  // Kept for reference only...
  /* if(x0>-width&&x0<width)return 0; */
  /* if(x0<0)return fmax(0,2.5*(0.0524986*fabs(x0)-1.84817-0.0189762*height+(-1.49712e+002*exp(-4.06814e-001*height))*exp(-4.48657e-001*fabs(x0)))*(exp(7*(x0+width))-1)); */
  /* return fmax(0,2.5*(0.84199+0.00307022*height)*(0.0524986*fabs(x0)-1.84817-0.0189762*height+(-1.49712e+002*exp(-4.06814e-001*height))*exp(-4.48657e-001*fabs(x0)))*(exp(-7*(x0-width))-1)); */  
  if(x0>-23./2.&&x0<23./2.)return 0;
  long double cosh_ish=fmin(0.0524986*fabs(x0)-1.84817-0.0189762*height+(-1.49712e+002*exp(-4.06814e-001*height))*exp(-4.48657e-001*fabs(x0)),0);
  if(x0<0)return (-1.73518e-003*height*height+2.10277e-002*height+7.65692e-001) // intensity
	    *cosh_ish*(exp(7.*(x0+23./2.))-1); // slope 
  return (-1.73518e-003*height*height+2.10277e-002*height+7.65692e-001) // intensity
    *(0.84199+0.00307022*height) // asumetry
    *cosh_ish*(exp(-7.*(x0-23./2.))-1); // slope
} /* end of ESS_2014_Schoenfeldt_thermal_x0 */

/* This is the thermal moderator with 2015 updates, fits from Troels Schoenfeldt */
#pragma acc routine seq
void ESS_2015_Schoenfeldt_thermal(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y)
{
  if ((height_t == 0.03) || (height_t == 0.06)) {
    *p = ESS_2015_Schoenfeldt_thermal_spectrum(lambda, beamportangle);
  } else {
    printf("Sorry! Moderator height must be either %g or %g m\n",0.03,0.06);
    exit(-1);
  }

  /* Troels Schoenfeldt function for timestructure */
  *p *= tmultiplier*ESS_2015_Schoenfeldt_thermal_timedist(*t, lambda, 3 /* cm height */, ESS_SOURCE_DURATION);  
  if (height_c == 0.03) {
    // 3cm case
    *p *= ESS_2015_Schoenfeldt_thermal_y0(100*Y) * ESS_2015_Schoenfeldt_thermal_x0(100*X, beamportangle, Mwidth_t);
  } else {
    // 6cm case
    // Downscale brightness by factor from 
    // "New ESS Moderator Baseline", Ken Andersen, 9/4/2015
    *p *= (6.2e14/9.0e14);
    *p *= ESS_2014_Schoenfeldt_thermal_y0(100*Y, 100*height_c) * ESS_2015_Schoenfeldt_thermal_x0(100*X, beamportangle, Mwidth_t);
  }
} /* end of ESS_2015_Schoenfeldt_thermal */

/* This is the thermal moderator with 2015 updates, fits from Troels Schoenfeldt */
#pragma acc routine seq
void ESS_2015_Schoenfeldt_thermal_no_t(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y)
{
  if ((height_t == 0.03) || (height_t == 0.06)) {
    *p = ESS_2015_Schoenfeldt_thermal_spectrum(lambda, beamportangle);
  } else {
    printf("Sorry! Moderator height must be either %g or %g m\n",0.03,0.06);
    exit(-1);
  }

  if (height_c == 0.03) {
    // 3cm case
    *p *= ESS_2015_Schoenfeldt_thermal_y0(100*Y) * ESS_2015_Schoenfeldt_thermal_x0(100*X, beamportangle, Mwidth_t);
  } else {
    // 6cm case
    // Downscale brightness by factor from 
    // "New ESS Moderator Baseline", Ken Andersen, 9/4/2015
    *p *= (6.2e14/9.0e14);
    *p *= ESS_2014_Schoenfeldt_thermal_y0(100*Y, 100*height_c) * ESS_2015_Schoenfeldt_thermal_x0(100*X, beamportangle, Mwidth_t);
  }
} /* end of ESS_2015_Schoenfeldt_thermal */

/* This is the thermal moderator with 2015 updates, fits from Troels Schoenfeldt */
#pragma acc routine seq
void ESS_2015_Schoenfeldt_thermal_only_t(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y)
{
  /* Troels Schoenfeldt function for timestructure */
  *p *= tmultiplier*ESS_2015_Schoenfeldt_thermal_timedist(*t, lambda, 3 /* cm height */, ESS_SOURCE_DURATION);  
}


/* This is the cold moderator with 2015 updates, fits from Troels Schoenfeldt */
/* Parametrization including moderator height for the "pancake" moderator */
#pragma acc routine seq
void ESS_2015_Schoenfeldt_cold(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y)
{
   if ((height_c == 0.03) || (height_c == 0.06)) {
    *p = ESS_2015_Schoenfeldt_cold_spectrum(lambda,beamportangle);
  } else {
    printf("Sorry! Moderator height must be either %g or %g m\n",0.03,0.06);
    exit(-1);
  }

  /* Troels Schoenfeldt function for timestructure */
  *p *= tmultiplier*ESS_2015_Schoenfeldt_cold_timedist(*t, lambda, 3 /* cm height */, ESS_SOURCE_DURATION);
  
  if (height_c == 0.03) {
    // 3cm case
    *p *= ESS_2015_Schoenfeldt_cold_y0(100*Y) * ESS_2015_Schoenfeldt_cold_x0(100*X, beamportangle, Mwidth_c);
  } else {
    // 6cm case
    // Downscale brightness by factor from 
    // "New ESS Moderator Baseline", Ken Andersen, 9/4/2015
    *p *= (10.1e14/16.0e14);
    *p *= ESS_2014_Schoenfeldt_cold_y0(100*Y, 100*height_c) * ESS_2015_Schoenfeldt_cold_x0(100*X, beamportangle, Mwidth_c);
  }
} /* end of ESS_2015_Schoenfeldt_cold */

#pragma acc routine seq
void ESS_2015_Schoenfeldt_cold_no_t(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y)
{
   if ((height_c == 0.03) || (height_c == 0.06)) {
    *p = ESS_2015_Schoenfeldt_cold_spectrum(lambda,beamportangle);
  } else {
    printf("Sorry! Moderator height must be either %g or %g m\n",0.03,0.06);
    exit(-1);
  }
  
  if (height_c == 0.03) {
    // 3cm case
    *p *= ESS_2015_Schoenfeldt_cold_y0(100*Y) * ESS_2015_Schoenfeldt_cold_x0(100*X, beamportangle, Mwidth_c);
  } else {
    // 6cm case
    // Downscale brightness by factor from 
    // "New ESS Moderator Baseline", Ken Andersen, 9/4/2015
    *p *= (10.1e14/16.0e14);
    *p *= ESS_2014_Schoenfeldt_cold_y0(100*Y, 100*height_c) * ESS_2015_Schoenfeldt_cold_x0(100*X, beamportangle, Mwidth_c);
  }
} /* end of ESS_2015_Schoenfeldt_cold */

#pragma acc routine seq
void ESS_2015_Schoenfeldt_cold_only_t(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, double height_t, double Mwidth_t, double height_c, double Mwidth_c, double tmultiplier, double beamportangle, double X, double Y)
{
  /* Troels Schoenfeldt function for timestructure */
  *p *= tmultiplier*ESS_2015_Schoenfeldt_cold_timedist(*t, lambda, 3 /* cm height */, ESS_SOURCE_DURATION);
} /* end of ESS_2015_Schoenfeldt_cold */


/* This is ESS_2015_Schoenfeldt_cold_y0 - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2015_Schoenfeldt_cold_y0(double y0){
    double par3=30;
    double par4=.35;
    double cosh_ish=exp(-par4*y0)+exp(par4*y0);
    double sinh_ish=pow(1+exp(par3*(y0-3./2.)),-1)*pow(1+exp(-par3*(y0+3./2.)),-1);
    return 1./2.*(double)((double)cosh_ish*(double)sinh_ish);

} /* end of ESS_2015_Schoenfeldt_cold_y0 */

/* This is ESS_2015_Schoenfeldt_thermal_y0 - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2015_Schoenfeldt_thermal_y0(double y0){
    if(y0<-3./2.+0.105){
        return 1.005*exp(-pow((y0+3./2.-0.105)/0.372,2));
    } else if(y0>3./2.-0.105){
        return 1.005*exp(-pow((y0-3./2.+0.105)/0.372,2));
    }
    return 1.005;
} /* end of ESS_2015_Schoenfeldt_thermal_y0 */

/* This is ESS_2015_Schoenfeldt_cold_x0 - horizontal intensity distribution for the 2015 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2015_Schoenfeldt_cold_x0(double x0,double theta, double width){
  // GEOMETRY / SAMPLING SPACE
    double i=(theta-5.)/10.;
    double par0=0.0146115+0.00797729*i-0.00279541*i*i;
    double par1=0.980886;
    if(i==1)par1=0.974217;
    if(i==2)par1=0.981462;
    if(i==3)par1=1.01466;
    if(i==4)par1=1.11707;
    if(i==5)par1=1.16057;
        
    double par2=-4-.75*i;
    if(i==0)par2=-20;
    double par3=-14.9402-0.178369*i+0.0367007*i*i;
    if(i==0)par3*=0.95;
    double par4=-15;
    if(i==3)par4=-3.5;
    if(i==5)par4=-1.9;
    double par5=-7.07979+0.0835695*i-0.0546662*i*i;
    if(i==5)par5*=0.85;
    
    //printf("Angle %g, width is %g\n",theta,width,cos(theta*DEG2RAD)*width);
    //if(i==4) width=width+0.3;
    //if(i==5) width=width-0.7;

    /* Rescaling to achieve a BF1 model */
    double tmp=(par5-par3)/width;
    //printf("Cold x0 in BF1 units: %g,",x0);
    x0=x0*tmp-7.16;
    //printf("x0 in BF2 units: %g, moderator width is %g from %g\n",x0,width,par5-par3);

    /* if (x0<=par5 && x0>=par3) */
    /*   return 1; */
    /* else */
    /*   return 0; */
    

    double line=par0*(x0+12)+par1;
    double CutLeftCutRight=1./((1+exp(par2*(x0-par3)))*(1+exp(-par4*(x0-par5))));

    return line*CutLeftCutRight;
} /* end of ESS_2015_Schoenfeldt_cold_x0 */

/* This is ESS_2015_Schoenfeldt_thermal_x0 - horizontal intensity distribution for the 2015 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2015_Schoenfeldt_thermal_x0(double x0,double theta, double width){
    double i=(theta-5.)/10.;
    double par0=-5.54775+0.492804*i;
    double par1=-0.265929-0.711477*i;
    if(theta==55)par1=-2.55;

    double par2=0.821885+0.00914832*i;
    double par3=1.31108-0.00698647*i;
    if(theta==55)par3=1.23;
    double par4=-.035;
    double par5=-0.0817358+0.00807125*i;
        
    double par6=-8;
    double par7=-7.15;
    if(theta==45)par7=-8.2;
    if(theta==55)par7=-7.7;

    double par8=-8;
    double par9=7.15;
    if(theta==45)par9=7.5;
    if(theta==55)par9=8.2;

    /* Rescaling to achieve a BF1 model */
    double tmp=(par9-par7)/width;
    //printf("Thermal x0 in BF1 units: %g,",x0);
    x0=x0*tmp-7.16;
    //printf(" x0 in BF2 units: %g, moderator width is %g from %g\n",x0,width,par9-par7);
    
    /* if (x0<=par9 && x0>=par7) */
    /*   return 1; */
    /* else */
    /*   return 0; */
    
    double soften1=1./(1+exp(8.*(x0-par0)));
    double soften2=1./(1+exp(8.*(x0-par1)));
    double CutLeftCutRight=1./((1+exp(par6*(x0-par7)))*(1+exp(-par8*(x0-par9))));
    double line1=par4*(x0-par0)+par2;
    double line2=(par2-par3)/(par0-par1)*(x0-par0)+par2;
    double line3=par5*(x0-par1)+par3;
    double add45degbumb=1.2*exp(-(x0+7.55)*(x0+7.55)/.35/.35);


    return CutLeftCutRight*(
        (line1)*soften1
        +line2*soften2*(1-soften1)
        +line3*(1-soften2)
        );
} /* end of ESS_2015_Schoenfeldt_thermal_x0 */

/* This is ESS_2015_Schoenfeldt_cold_Y - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2015_Schoenfeldt_cold_Y(double Y,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2015_Schoenfeldt_cold_Y */

/* This is ESS_2015_Schoenfeldt_thermal_Y - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2015_Schoenfeldt_thermal_Y(double Y,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2015_Schoenfeldt_thermal_Y */

/* This is ESS_2015_Schoenfeldt_cold_Theta120 - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2015_Schoenfeldt_cold_Theta120(double Theta120,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2015_Schoenfeldt_cold_Theta120 */

/* This is ESS_2015_Schoenfeldt_thermal_Theta120 - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
#pragma acc routine seq
double ESS_2015_Schoenfeldt_thermal_Theta120(double beamportangle,int isleft){
  if(!isleft)return cos((beamportangle-30)*DEG2RAD)/cos(30*DEG2RAD);
  return cos((90-beamportangle)*DEG2RAD)/cos(30*DEG2RAD);
/* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2015_Schoenfeldt_thermal_Theta120 */


/* This is ESS_2015_Schoenfeldt_cold_timedist time-distribution of the 2014 Schoenfeldt cold moderator */ 
#pragma acc routine seq
double ESS_2015_Schoenfeldt_cold_timedist(double time,double lambda,double height, double pulselength){
        if(time<0)return 0;
        double tau=3.00094e-004*(4.15681e-003*lambda*lambda+2.96212e-001*exp(-1.78408e-001*height)+7.77496e-001)*exp(-6.63537e+001*pow(fmax(1e-13,lambda+.9),-8.64455e+000));
        if(time<pulselength)return ((1-exp(-time/tau)));
        return ((1-exp(-pulselength/tau))*exp(-(time-pulselength)/tau));

} /* end of ESS_2015_Schoenfeldt_cold_timedist */

/* This is ESS_2015_Schoenfeldt_thermal_timedist time-distribution of the 2015 Schoenfeldt cold moderator */    
#pragma acc routine seq
double ESS_2015_Schoenfeldt_thermal_timedist(double time,double lambda,double height, double pulselength){
        if(time<0)return 0;
        double tau=3.00000e-004*(1.23048e-002*lambda*lambda+1.75628e-001*exp(-1.82452e-001*height)+9.27770e-001)*exp(-3.91090e+001*pow(fmax(1e-13,lambda+9.87990e-001),-7.65675e+000));
        if(time<pulselength)return ((1-exp(-time/tau)));
        return ((1-exp(-pulselength/tau))*exp(-(time-pulselength)/tau));
} /* end of ESS_2015_Schoenfeldt_thermal_timedist */

/* end of ESS_butterfly-lib.c */
#ifdef OPENACC
#undef exit
#endif
