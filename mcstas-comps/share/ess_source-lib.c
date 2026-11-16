/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright 1997-2013, All rights reserved
*         DTU Physics, Lyngby, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Library: share/ess_source-lib.c
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
* %include "ess_source-lib"
*
*******************************************************************************/

#ifndef ESS_SOURCE_LIB_H
#error McStas : please import this library with %include "ess_source-lib"
#endif

/* Base maths functions used below */
double Mezei_M(double l, double temp)
    {
      double a=949.0/temp;
      return 2*a*a*exp(-a/(l*l))/(l*l*l*l*l);
    }

/* This one is a bit strange - never used in the actual comp it seems? - Should rewrite.... */
double Mezei_F(double t, double tau, int n)
    {
      return (exp(-t/tau)-exp(-n*t/tau))*n/(n-1)/tau;
    }

double ESS_2013_Schoenfeldt_cold_spectrum(double I_SD, double alpha_SD, double lambda_SD, double alpha_l, double lambda_l, double Exponent, double I_1, double alpha_1, double I_2, double alpha_2, double lambda) 
{
  return (I_1 * exp(-alpha_1 * lambda) + I_2 * exp(-alpha_2 * lambda)) * 1 / pow(1 + exp(alpha_l * (lambda - lambda_l)),-Exponent) + I_SD * (1/lambda) * 1/( 1 + exp(alpha_SD * (lambda - lambda_SD)));
}

double ESS_2013_Schoenfeldt_thermal_spectrum(double I_th, double T, double I_SD, double alpha, double lambda_cf, double lambda) 
{
  double k_Th=949;
  return I_th * exp(-k_Th/(T*lambda*lambda))*(2*k_Th*k_Th)/(T*T*pow(lambda,5)) + I_SD * (1/lambda) * 1/(1+exp(alpha*(lambda - lambda_cf)));
}

double ESS_2014_Schoenfeldt_cold_spectrum(double lambda,double height){
if(lambda<=0)return 0;
	return pow((1+exp((-7.84092e+001*exp(-1.00000e+000*height)-8.46887e+000+3)
		*(lambda-(2.48787-0.00729329*height)))),(1.17068e+000*exp(-2.52666e-001*height)-9.29703e-001))
		*((5.17542e+014*exp(-3.91646e-001*height)+7.19417e+013)
		*(exp(-0.77*(lambda))+(-5.03315e-002*exp(-5.79174e-001*height)+5.72765e-002)*exp(-0.323784*(lambda))))
		+(4.12442e+012*exp(-1.25186e-001*height)+8.53849e+011)/((1+exp(2.5*(lambda-2.2)))*lambda);
}

double ESS_2014_Schoenfeldt_thermal_spectrum(double lambda, double height){
	if(lambda<=0)return 0;
	double aOlsqr=949./(325*lambda*lambda);
	return 2*(3.46910e+013*exp(-1.65602e-001*height)+8.08542e+012)
	  *aOlsqr*aOlsqr/lambda*pow(lambda,(3.11752e-001*(1-exp(-3.45363e-001*height))+9.17072e-002))
	  *exp(-aOlsqr)

	  +

	  (4.64873e+012*exp(-1.80747e-001*height)+1.74845e+012)/((1+exp(2.5*(lambda-0.88)))*lambda);
}

double ESS_2015_Schoenfeldt_cold_spectrum(double lambda,double theta){
  if(lambda<=0)return 0;
  double par0=8.44e13/25.;
  double par1=2.5;
  double par2=2.2;
  
  double par3=-13.-.5*(theta-5);
  double par4=2.53;
  double par5=-0.0478073-0.160*exp(-0.45186*(theta-5.)/10.);
  
  /* par6 is tabulated at theta=5,15,...,55 deg: use the nearest */
  int ip = theta >= 50 ? 5 : theta >= 40 ? 4 : theta >= 30 ? 3 : theta >= 20 ? 2 : theta >= 10 ? 1 : 0;
  double par6;
  if(ip==0)par6=5.73745e+015/25.;
  else if(ip==1)par6=5.88284e+015/25.;
  else if(ip==2)par6=6.09573e+015/25.;
  else if(ip==3)par6=6.29116e+015/25.;
  else if(ip==4)par6=6.03436e+015/25.;
  else par6=6.02045e+015/25.;
  double par7=0.788956+0.00854184*(theta-5.)/10.;
  double par8=0.0461868-0.0016464*(theta-5.)/10.;
  double par9=0.325;
  
  double SD_part=par0/((1+exp(par1*(lambda-par2)))*lambda);
  double para_part=pow((1+exp(par3*(lambda-par4))),par5)*(par6*(exp(-par7*(lambda))+par8*exp(-par9*(lambda))));
  return para_part+SD_part;
  
}

double ESS_2015_Schoenfeldt_thermal_spectrum(double lambda, double theta){
    if(lambda<=0)return 0;
    double i=(theta-5.)/10.;
    double par0=4.2906e+013-9.2758e+011*i+8.02603e+011*i*i-1.29523e+011*i*i*i;
    double par2=6.24806e+012-8.84602e+010*i;
    double par3=-0.31107+0.0221138*i;
    double aOlsqr=949./(325*lambda*lambda);
    return par0*2.*aOlsqr*aOlsqr/lambda*pow(lambda,-par3)*exp(-aOlsqr)+par2/((1+exp(2.5*(lambda-0.88)))*lambda);
	  
}


/* This is the cold Mezei moderator from 2012 (updated I0 and I2) */
void ESS_Mezei_cold_2012(double *t, double *p, double lambda, double tfocus_width, double tfocus_time, double dt, ess_moderator_struct extras) 
{
  // Spectrum related constants - ESS 2001 Cold moderator
  double T=50, tau=287e-6, tau1=0, tau2=20e-6, chi2=0.9, I0=8.21e11, I2=3.29e11, branch1=1, branch2=0.5, n2=5, n=20;
  
  // Branching
  double branch_tail=tau/ESS_SOURCE_DURATION;
  
  // Other variables
  double tail_flag, tau_l;
  
  // Taken directly from the ESS_moderator.comp:
  tail_flag = (rand01()<branch_tail);   /* Choose tail/bulk */
  if (tail_flag)
    {
      if (rand01() < branch2)
	{
	  if (tau1>0)
	    if (rand01() < branch1)     /* Quick and dirty non-general solution */
	      {  /* FIRST CASE a */
		tau_l = tau;
		*p = 1/(branch1*branch2*branch_tail); /* Correct for switching prob. */
	      }
	    else
	      {  /* FIRST CASE b */
		tau_l = tau1;
		*p = 1/((1-branch1)*branch2*branch_tail); /* Correct for switching prob. */
	      }
	  else
	    {
	      tau_l = tau;
	      *p = 1/(branch2*branch_tail); /* Correct for switching prob. */
	    }
	  *t = -tau_l*log(1e-12+rand01());       /* Sample from long-time tail a */
	  /* Correct for true pulse shape */
	  //	  p *= w_focus;                         /* Correct for target focusing */
	  *p *= tau_l/ESS_SOURCE_DURATION;                         /* Correct for tail part */
	  //p *= I0*w_mult*w_geom*Mezei_M(lambda,T);           /* Calculate true intensity */
	  *p *= I0*Mezei_M(lambda,T);
	}
      else
	{
	  /* SECOND CASE */
	  tau_l = tau2*lambda;
	  *t = -tau_l*log(1e-12+rand01());       /* Sample from long-time tail */
	  *p = n2/(n2-1)*((1-exp(-ESS_SOURCE_DURATION/tau_l))-(1-exp(-n2*ESS_SOURCE_DURATION/tau_l))*exp(-(n2-1)*(*t)/tau_l)/n);
	  /* Correct for true pulse shape */
	  *p /= (1-branch2)*branch_tail;          /* Correct for switching prob. */
	  *p *= tau_l/ESS_SOURCE_DURATION;                         /* Correct for tail part */
	  // p *= w_focus;                         /* Correct for target focusing */
	  //p *= I2*w_mult*w_geom/(1+exp(chi2*lambda-2.2))/lambda;                                         /* Calculate true intensity */
	  *p *= I2/(1+exp(chi2*lambda-2.2))/lambda;                                         /* Calculate true intensity */ 
	}
      *t += ESS_SOURCE_DURATION;                                 /* Add pulse length */
    }
  else /* Tail-flag */
    {
      if (tfocus_width>0) {
	*t = tfocus_time-dt;                    /* Set time to hit time window center */
	*t += randpm1()*tfocus_width/2.0;       /* Add random time within window width */
      } else {
	*t = ESS_SOURCE_DURATION*rand01();                        /* Sample from bulk pulse */
      }
      // FLAG to KILL these on return!

      /* if (t<0) ABSORB;                       /\* Kill neutron if outside pulse duration *\/ */
      /* if (t>ESS_SOURCE_DURATION) ABSORB; */
      if (rand01() < branch2)
	{
	  if (rand01() < branch1)     /* Quick and dirty non-general solution */
	    {  /* FIRST CASE a */
	      tau_l = tau;
	      *p = 1/(branch1*branch2*(1-branch_tail)); /* Correct for switching prob. */
	    }
	  else
	    {  /* FIRST CASE b */
	      tau_l = tau1;
	      *p = 1/((1-branch1)*branch2*(1-branch_tail)); /* Correct for switching prob. */
	    }
	  *p *= 1-n/(n-1)*(exp(-*t/tau_l)-exp(-n*(*t)/tau_l)/n); /* Correct for true pulse shape */
	  //	  p *= w_focus;                         /* Correct for target focusing */
	  if (tfocus_width>0) {
	    *p *= tfocus_width/ESS_SOURCE_DURATION;    	  	  /* Correct for time focusing */
	  }
	  //p *= I0*w_mult*w_geom*M(lambda,T);       /* Calculate true intensity */
	  *p *= I0*Mezei_M(lambda,T);       /* Calculate true intensity */
	}
      else
	{
	  /* SECOND CASE */
	  tau_l = tau2*lambda;
	  *p = 1-n2/(n2-1)*(exp(-*t/tau_l)-exp(-n2*(*t)/tau_l)/n2); /* Correct for true pulse shape */
	  *p /= (1-branch2)*(1-branch_tail);   /* Correct for switching prob. */
	  //p *= w_focus;                         /* Correct for target focusing */
	  if (tfocus_width) {
	    *p *= tfocus_width/ESS_SOURCE_DURATION;    		  /* Correct for time focusing */
	  }
	  //p *= I2*w_mult*w_geom/(1+exp(chi2*lambda-2.2))/lambda;    /* Calculate true intensity */
	  *p *= I2/(1+exp(chi2*lambda-2.2))/lambda;    /* Calculate true intensity */
	}
    }
  
} /* end of ESS_Mezei_cold_2012 */

/* This is the cold Mezei moderator from 2001 (Original I0 and I2) */
void ESS_Mezei_cold(double *t, double *p, double lambda, double tfocus_width, double tfocus_time, double dt, ess_moderator_struct extras) 
{
  // Spectrum related constants - ESS 2001 Cold moderator
  double T=50, tau=287e-6, tau1=0, tau2=20e-6, chi2=0.9, I0=6.9e11, I2=27.6e10, branch1=1, branch2=0.5, n2=5, n=20;
  
  // Branching
  double branch_tail=tau/ESS_SOURCE_DURATION;
  
  // Other variables
  double tail_flag, tau_l;
  
  // Taken directly from the ESS_moderator.comp:
  tail_flag = (rand01()<branch_tail);   /* Choose tail/bulk */
  if (tail_flag)
    {
      if (rand01() < branch2)
	{
	  if (tau1>0)
	    if (rand01() < branch1)     /* Quick and dirty non-general solution */
	      {  /* FIRST CASE a */
		tau_l = tau;
		*p = 1/(branch1*branch2*branch_tail); /* Correct for switching prob. */
	      }
	    else
	      {  /* FIRST CASE b */
		tau_l = tau1;
		*p = 1/((1-branch1)*branch2*branch_tail); /* Correct for switching prob. */
	      }
	  else
	    {
	      /* This is the only active part - tau1 is set to 0! */
	      tau_l = tau;
	      *p = 1/(branch2*branch_tail); /* Correct for switching prob. */
	    }
	  *t = -tau_l*log(1e-12+rand01());       /* Sample from long-time tail a */
	  /* Correct for true pulse shape */
	  //	  p *= w_focus;                         /* Correct for target focusing */
	  *p *= tau_l/ESS_SOURCE_DURATION;                         /* Correct for tail part */
	  //p *= I0*w_mult*w_geom*Mezei_M(lambda,T);           /* Calculate true intensity */
	  *p *= I0*Mezei_M(lambda,T);
	}
      else
	{
	  /* Shine-through from spallation, i.e. not T-dependent */
	  /* SECOND CASE */
	  tau_l = tau2*lambda;
	  *t = -tau_l*log(1e-12+rand01());       /* Sample from long-time tail */
	  *p = n2/(n2-1)*((1-exp(-ESS_SOURCE_DURATION/tau_l))-(1-exp(-n2*ESS_SOURCE_DURATION/tau_l))*exp(-(n2-1)*(*t)/tau_l)/n);
	  /* Correct for true pulse shape */
	  *p /= (1-branch2)*branch_tail;          /* Correct for switching prob. */
	  *p *= tau_l/ESS_SOURCE_DURATION;                         /* Correct for tail part */
	  // p *= w_focus;                         /* Correct for target focusing */
	  //p *= I2*w_mult*w_geom/(1+exp(chi2*lambda-2.2))/lambda;                                         /* Calculate true intensity */
	  *p *= I2/(1+exp(chi2*lambda-2.2))/lambda;                                         /* Calculate true intensity */ 
	}
      *t += ESS_SOURCE_DURATION;                                 /* Add pulse length */
    }
  else /* Tail-flag */
    {
      if (tfocus_width>0) {
	*t = tfocus_time-dt;                    /* Set time to hit time window center */
	*t += randpm1()*tfocus_width/2.0;       /* Add random time within window width */
      } else {
	*t = ESS_SOURCE_DURATION*rand01();                        /* Sample from bulk pulse */
      }
      // FLAG to KILL these on return!

      /* if (t<0) ABSORB;                       /\* Kill neutron if outside pulse duration *\/ */
      /* if (t>ESS_SOURCE_DURATION) ABSORB; */
      if (rand01() < branch2)
	{
	  if (rand01() < branch1)     /* Quick and dirty non-general solution */
	    {  /* FIRST CASE a */
	      tau_l = tau;
	      *p = 1/(branch1*branch2*(1-branch_tail)); /* Correct for switching prob. */
	    }
	  else
	    {  /* FIRST CASE b */
	      tau_l = tau1;
	      *p = 1/((1-branch1)*branch2*(1-branch_tail)); /* Correct for switching prob. */
	    }
	  *p *= 1-n/(n-1)*(exp(-*t/tau_l)-exp(-n*(*t)/tau_l)/n); /* Correct for true pulse shape */
	  //	  p *= w_focus;                         /* Correct for target focusing */
	  if (tfocus_width>0) {
	    *p *= tfocus_width/ESS_SOURCE_DURATION;    	  	  /* Correct for time focusing */
	  }
	  //p *= I0*w_mult*w_geom*M(lambda,T);       /* Calculate true intensity */
	  *p *= I0*Mezei_M(lambda,T);       /* Calculate true intensity */
	}
      else
	{
	  /* SECOND CASE */
	  /* Shine-through from spallation, i.e. not T-dependent */
	  tau_l = tau2*lambda;
	  *p = 1-n2/(n2-1)*(exp(-*t/tau_l)-exp(-n2*(*t)/tau_l)/n2); /* Correct for true pulse shape */
	  *p /= (1-branch2)*(1-branch_tail);   /* Correct for switching prob. */
	  //p *= w_focus;                         /* Correct for target focusing */
	  if (tfocus_width) {
	    *p *= tfocus_width/ESS_SOURCE_DURATION;    		  /* Correct for time focusing */
	  }
	  //p *= I2*w_mult*w_geom/(1+exp(chi2*lambda-2.2))/lambda;    /* Calculate true intensity */
	  *p *= I2/(1+exp(chi2*lambda-2.2))/lambda;    /* Calculate true intensity */
	}
    }
  
} /* end of ESS_Mezei_cold */

/* This is the thermal Mezei moderator from 2001 - also used in 2012 - TDR */
void ESS_Mezei_thermal(double *t, double *p, double lambda, double tfocus_width, double tfocus_time, double dt, ess_moderator_struct extras)
{
  // Spectrum related constants - ESS 2001 Thermal moderator       
  double T=325, tau=80e-6, tau1=400e-6, tau2=12e-6, chi2=2.5, I0=13.5e11, I2=27.6e10, branch1=0.5, branch2=0.5, n2=5, n=20;

  // Branching
  double branch_tail=tau/ESS_SOURCE_DURATION;
  
  // Other variables
  double tail_flag, tau_l;
  
  // Taken directly from the ESS_moderator.comp:
  tail_flag = (rand01()<branch_tail);   /* Choose tail/bulk */
  if (tail_flag)
    {
      if (rand01() < branch2)
	{
	  if (tau1>0)
	    if (rand01() < branch1)     /* Quick and dirty non-general solution */
	      {  /* FIRST CASE a */
		tau_l = tau;
		*p = 1/(branch1*branch2*branch_tail); /* Correct for switching prob. */
	      }
	    else
	      {  /* FIRST CASE b */
		tau_l = tau1;
		*p = 1/((1-branch1)*branch2*branch_tail); /* Correct for switching prob. */
	      }
	  else
	    {
	      tau_l = tau;
	      *p = 1/(branch2*branch_tail); /* Correct for switching prob. */
	    }
	  *t = -tau_l*log(1e-12+rand01());       /* Sample from long-time tail a */
	  /* Correct for true pulse shape */
	  //	  p *= w_focus;                         /* Correct for target focusing */
	  *p *= tau_l/ESS_SOURCE_DURATION;                         /* Correct for tail part */
	  //p *= I0*w_mult*w_geom*Mezei_M(lambda,T);           /* Calculate true intensity */
	  *p *= I0*Mezei_M(lambda,T);
	}
      else
	{
	  /* SECOND CASE */
	  tau_l = tau2*lambda;
	  *t = -tau_l*log(1e-12+rand01());       /* Sample from long-time tail */
	  *p = n2/(n2-1)*((1-exp(-ESS_SOURCE_DURATION/tau_l))-(1-exp(-n2*ESS_SOURCE_DURATION/tau_l))*exp(-(n2-1)*(*t)/tau_l)/n);
	  /* Correct for true pulse shape */
	  *p /= (1-branch2)*branch_tail;          /* Correct for switching prob. */
	  *p *= tau_l/ESS_SOURCE_DURATION;                         /* Correct for tail part */
	  // p *= w_focus;                         /* Correct for target focusing */
	  //p *= I2*w_mult*w_geom/(1+exp(chi2*lambda-2.2))/lambda;                                         /* Calculate true intensity */
	  *p *= I2/(1+exp(chi2*lambda-2.2))/lambda;                                         /* Calculate true intensity */ 
	}
      *t += ESS_SOURCE_DURATION;                                 /* Add pulse length */
    }
  else /* Tail-flag */
    {
      if (tfocus_width>0) {
	*t = tfocus_time-dt;                    /* Set time to hit time window center */
	*t += randpm1()*tfocus_width/2.0;       /* Add random time within window width */
      } else {
	*t = ESS_SOURCE_DURATION*rand01();                        /* Sample from bulk pulse */
      }
      // FLAG to KILL these on return!

      /* if (t<0) ABSORB;                       /\* Kill neutron if outside pulse duration *\/ */
      /* if (t>ESS_SOURCE_DURATION) ABSORB; */
      if (rand01() < branch2)
	{
	  if (rand01() < branch1)     /* Quick and dirty non-general solution */
	    {  /* FIRST CASE a */
	      tau_l = tau;
	      *p = 1/(branch1*branch2*(1-branch_tail)); /* Correct for switching prob. */
	    }
	  else
	    {  /* FIRST CASE b */
	      tau_l = tau1;
	      *p = 1/((1-branch1)*branch2*(1-branch_tail)); /* Correct for switching prob. */
	    }
	  *p *= 1-n/(n-1)*(exp(-*t/tau_l)-exp(-n*(*t)/tau_l)/n); /* Correct for true pulse shape */
	  //	  p *= w_focus;                         /* Correct for target focusing */
	  if (tfocus_width>0) {
	    *p *= tfocus_width/ESS_SOURCE_DURATION;    	  	  /* Correct for time focusing */
	  }
	  //p *= I0*w_mult*w_geom*M(lambda,T);       /* Calculate true intensity */
	  *p *= I0*Mezei_M(lambda,T);       /* Calculate true intensity */
	}
      else
	{
	  /* SECOND CASE */
	  tau_l = tau2*lambda;
	  *p = 1-n2/(n2-1)*(exp(-*t/tau_l)-exp(-n2*(*t)/tau_l)/n2); /* Correct for true pulse shape */
	  *p /= (1-branch2)*(1-branch_tail);   /* Correct for switching prob. */
	  //p *= w_focus;                         /* Correct for target focusing */
	  if (tfocus_width) {
	    *p *= tfocus_width/ESS_SOURCE_DURATION;    		  /* Correct for time focusing */
	  }
	  //p *= I2*w_mult*w_geom/(1+exp(chi2*lambda-2.2))/lambda;    /* Calculate true intensity */
	  *p *= I2/(1+exp(chi2*lambda-2.2))/lambda;    /* Calculate true intensity */
	}
    }

} /* end of ESS_Mezei_thermal */


/* This is the Mezei moderator with a correction term from Klaus Lieutenant */
void ESS_2012_Lieutenant_cold(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, ess_moderator_struct extras)
{
  ESS_Mezei_cold_2012(t, p,  lambda,  tfocus_w,  tfocus_t, tfocus_dt, extras);
  
  double cor;
  
  /* Correction factors to converts 'predicted' spectrum from cold moderator to the one observed in MCNPX */
  if (lambda<=2.5) cor=log(1.402+0.898*lambda)*(2.0776-4.1093*lambda+4.8836*pow(lambda,2)-2.4715*pow(lambda,3)+0.4521*pow(lambda,4));
  else if (lambda <= 3.5) cor = log(1.402 + 0.898*lambda)*(4.3369 - 1.8367*lambda + 0.2524*pow(lambda,2) );
  else if (lambda  > 3.5) cor = log(1.402 + 0.898*lambda);
  
  *p *=cor;

} /* end of ESS_2012_Lieutenant_cold */


/* This is the cold moderator with 2013 updates, fits from Troels Schoenfeldt */
/* Parametrization including moderator height for the "pancake" moderator */
void ESS_2013_Schoenfeldt_cold(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, ess_moderator_struct extras)
{
    /* From the forthcoming Schoenfeldt et al.
       S_cold(\lambda) = (I_1*exp(-\alpha_1*lambda)  + I_2*exp(-\alpha_2*\lambda)) * 1/(1+exp(\alpha_l * (\lambda-lambda_l)))^(1/\gamma)
       + I_SD * (1/lambda) * 1/(1+exp(\alpha_SD*(\lambda-\lambda_SD)))
    */

  /* As function of moderator height, parameters for the brilliance expression */
  double height[7]    = {0.10, 0.05, 0.03, 0.015, 0.01, 0.005, 0.001};
  double I_SD[7]      = {4.75401e+011, 7.0319e+011,  8.36605e+011, 9.41035e+011, 9.54305e+011, 9.83515e+011, 9.54108e+011};
  double alpha_SD[7]  = {0.9, 0.9, 0.9, 0.9, 0.9, 0.9, 0.9};
  double lambda_SD[7] = {2.44444, 2.44444, 2.44444, 2.44444, 2.44444, 2.44444, 2.44444};
  double alpha_l[7]   = {-11.9056, -13.8444, -17.8359, -19.6643, -23.0058, -21.6241, -18.82};
  double lambda_l[7]  = {2.53562, 2.53527, 2.53956, 2.53243, 2.53375, 2.5364, 2.51714};
  double Exponent[7]  = {-0.259162, -0.215819, -0.160541, -0.140769, -0.119278, -0.124298, -0.144056};
  double I_1[7]       = {1.22098e+013, 2.57992e+013, 4.43235e+013, 8.86873e+013, 1.26172e+014, 2.02098e+014, 3.32623e+014};
  double alpha_1[7]   = {0.653579, 0.720244, 0.772538, 0.871765, 0.927905, 1.01579, 1.11621};
  double I_2[7]       = {2.97518e+011, 1.11421e+012, 1.8961e+012,  4.00852e+012, 5.05278e+012, 6.98605e+012, 7.89424e+012};
  double alpha_2[7]   = {0.261097, 0.307898, 0.317865, 0.346354, 0.354282, 0.371298, 0.38382};

  double S_a, S_b;
  double dS;
  int j, idxa, idxb;
  if ((extras.height_c <= height[0]) && (extras.height_c >= height[6])) {
    for (j=0; j<7; j++) {
      if (extras.height_c == height[j]) {
	S_a = ESS_2013_Schoenfeldt_cold_spectrum(I_SD[j], alpha_SD[j], lambda_SD[j], alpha_l[j], lambda_l[j], Exponent[j], I_1[j], alpha_1[j], I_2[j], alpha_2[j], lambda);
	  *p = S_a;
	  break;
	  
	} else if (extras.height_c < height[j] && extras.height_c > height[j+1]) {
	  dS = (height[j]-extras.height_c)/(height[j]-height[j+1]);
	  /* Linear interpolation between the two closest heights */
	  S_a = ESS_2013_Schoenfeldt_cold_spectrum(I_SD[j], alpha_SD[j], lambda_SD[j], alpha_l[j], lambda_l[j], Exponent[j], I_1[j], alpha_1[j], I_2[j], alpha_2[j], lambda);
	  S_b = ESS_2013_Schoenfeldt_cold_spectrum(I_SD[j+1], alpha_SD[j+1], lambda_SD[j+1], alpha_l[j+1], lambda_l[j+1], Exponent[j+1], I_1[j+1], alpha_1[j+1], I_2[j+1], alpha_2[j+1], lambda);
	  *p = (1-dS)*S_a + dS*S_b;
	  break;
      }
    }
  } else {
    printf("Sorry! Moderator height must be between %g and %g m\n",height[6],height[0]);
    exit(-1);
  }

  /* Next is time structure... */
  *t=0;
  *t = extras.tmultiplier*ESS_SOURCE_DURATION*(rand01());
  /* Troels Schoenfeldt function for timestructure */
  *p *= extras.tmultiplier*ESS_2014_Schoenfeldt_cold_timedist(*t, lambda, 100*extras.height_c, ESS_SOURCE_DURATION);
} /* end of ESS_2013_Schoenfeldt_cold */

/* This is the cold moderator with 2014 updates, fits from Troels Schoenfeldt */
/* Parametrization including moderator height for the "pancake" moderator */
void ESS_2014_Schoenfeldt_cold(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, ess_moderator_struct extras)
{
   if ((extras.height_c <= 0.12) && (extras.height_c >= 0.01)) {
    *p = ESS_2014_Schoenfeldt_cold_spectrum(lambda,100*extras.height_c);
  } else {
    printf("Sorry! Moderator height must be between %g and %g m\n",0.12,0.01);
    exit(-1);
  }

  /* Next is time structure... */
  *t=0;
  double pt=0;
  
  // Potentially apply russian roulette technique 
  //while (rand01()>pt) {
  *t = extras.tmultiplier*ESS_SOURCE_DURATION*(rand01());
  /* Troels Schoenfeldt function for timestructure */
  pt = extras.tmultiplier*ESS_2014_Schoenfeldt_cold_timedist(*t, lambda, 100*extras.height_c, ESS_SOURCE_DURATION);
  //}
  
  *p *= pt;
  if (extras.Uniform==0) *p *= ESS_2014_Schoenfeldt_cold_y0(100*extras.Y, 100*extras.height_c) * ESS_2014_Schoenfeldt_cold_x0(-100*extras.X, 100*extras.height_c, 100*extras.Width_c);
} /* end of ESS_2014_Schoenfeldt_cold */

/* This is ESS_2014_Schoenfeldt_cold_y0 - vertical intensity distribution for the 2014 Schoenfeldt cold moderator */
double ESS_2014_Schoenfeldt_cold_y0(double y0,double height){
  
  double one_over_integral_y0_of_height= height/((0.36434*height*height+2.53796*height-0.107774));
  if(y0 < -height/2. || y0 > height/2. )return 0;
  double cosh_ish=(exp(-7e-1/sqrt(height)*(y0-height/2.))+exp(-7e-1/20.*height+7e-1/sqrt(height)*(y0+height/2.)));
  double sinh_ish=(exp(50/sqrt(height)*(y0-height/2.))-1)*(exp(-50/sqrt(height)*(y0+height/2.))-1);
  double tmp=one_over_integral_y0_of_height*cosh_ish*sinh_ish;
  return tmp;
} /* end of ESS_2014_Schoenfeldt_cold_y0 */

/* This is ESS_2014_Schoenfeldt_thermal_y0 - vertical intensity distribution for the 2014 Schoenfeldt cold moderator */
double ESS_2014_Schoenfeldt_thermal_y0(double y0,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2014_Schoenfeldt_thermal_y0 */

/* This is ESS_2014_Schoenfeldt_cold_x0 - horizontal intensity distribution for the 2014 Schoenfeldt cold moderator */
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

/* This is ESS_2014_Schoenfeldt_cold_Y - vertical intensity distribution for the 2014 Schoenfeldt cold moderator */
double ESS_2014_Schoenfeldt_cold_Y(double Y,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2014_Schoenfeldt_cold_Y */

/* This is ESS_2014_Schoenfeldt_thermal_Y - vertical intensity distribution for the 2014 Schoenfeldt cold moderator */
double ESS_2014_Schoenfeldt_thermal_Y(double Y,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2014_Schoenfeldt_thermal_Y */

/* This is ESS_2014_Schoenfeldt_cold_Theta120 - vertical intensity distribution for the 2014 Schoenfeldt cold moderator */
double ESS_2014_Schoenfeldt_cold_Theta120(double Theta120,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2014_Schoenfeldt_cold_Theta120 */

/* This is ESS_2014_Schoenfeldt_thermal_Theta120 - vertical intensity distribution for the 2014 Schoenfeldt cold moderator */
double ESS_2014_Schoenfeldt_thermal_Theta120(double beamportangle,int isleft){
  if(!isleft)return cos((beamportangle-30)*DEG2RAD)/cos(30*DEG2RAD);
  return cos((90-beamportangle)*DEG2RAD)/cos(30*DEG2RAD);
/* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2014_Schoenfeldt_thermal_Theta120 */

/* This is ESS_2014_Schoenfeldt_cold_timedist time-distribution of the 2014 Schoenfeldt cold moderator */ 
double ESS_2014_Schoenfeldt_cold_timedist(double time,double lambda,double height, double pulselength){
        if(time<0)return 0;
        double tau=3.00094e-004*(4.15681e-003*lambda*lambda+2.96212e-001*exp(-1.78408e-001*height)+7.77496e-001)*exp(-6.63537e+001*pow(fmax(1e-13,lambda+.9),-8.64455e+000));
        if(time<pulselength)return ((1-exp(-time/tau)));
        return ((1-exp(-pulselength/tau))*exp(-(time-pulselength)/tau));

} /* end of ESS_2014_Schoenfeldt_cold_timedist */


/* This is ESS_2014_Schoenfeldt_thermal_timedist time-distribution of the 2014 Schoenfeldt cold moderator */    
double ESS_2014_Schoenfeldt_thermal_timedist(double time,double lambda,double height, double pulselength){
        if(time<0)return 0;
        double tau=3.00000e-004*(1.23048e-002*lambda*lambda+1.75628e-001*exp(-1.82452e-001*height)+9.27770e-001)*exp(-3.91090e+001*pow(fmax(1e-13,lambda+9.87990e-001),-7.65675e+000));
        if(time<pulselength)return ((1-exp(-time/tau)));
        return ((1-exp(-pulselength/tau))*exp(-(time-pulselength)/tau));
} /* end of ESS_2014_Schoenfeldt_thermal_timedist */

/* HERE IT IS */


/* This is the thermal moderator with 2013 updates, fits from Troels Schoenfeldt */
void ESS_2013_Schoenfeldt_thermal(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, ess_moderator_struct extras)
{
  /*  From the forthcoming Schoenfeldt et al.
     S_Th(\lambda) = I_Th * exp(k_Th/(T*\lambda^2))*(2*k_Th^2)/(T^2*\lambda^5) + I_SD * \lambda^-1 * 1/(1+exp(\alpha*(\lambda - \lambda_cf))
   */
  
  /* As function of moderator height, parameters for the brilliance expression */
  double height[7]    = {0.10, 0.05, 0.03, 0.015, 0.01, 0.005, 0.001};
  double I_th[7]      = {2.97527e+012, 4.35192e+012, 5.18047e+012, 6.0305e+012,  6.20079e+012, 6.44927e+012, 6.55127e+012};
  double T[7]         = {303.764, 306.099, 307.497, 311.292, 310.525, 310.822, 317.56};
  double I_SD[7]      = {5.38083e+011, 7.3059e+011,  8.94408e+011, 9.89515e+011, 1.02135e+012, 1.07415e+012, 1.12157e+012};
  double alpha[7]     = {2.5, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5};
  double lambda_cf[7] = {0.88, 0.88, 0.88, 0.88, 0.88, 0.88, 0.88};

  double S_a, S_b;
  double dS;
  int j, idxa, idxb;
  if ((extras.height_t <= height[0]) && (extras.height_t >= height[6])) {
    for (j=0; j<6; j++) {
      if (extras.height_c == height[j]) {
	S_a = ESS_2013_Schoenfeldt_thermal_spectrum(I_th[j], T[j], I_SD[j], alpha[j], lambda_cf[j], lambda);
	*p = S_a;
	break;
      } else if (extras.height_t <= height[j] && extras.height_t > height[j+1]) {
	dS = (height[j]-extras.height_t)/(height[j]-height[j+1]);
	/* Linear interpolation between the two closest heights */
	S_a = ESS_2013_Schoenfeldt_thermal_spectrum(I_th[j], T[j], I_SD[j], alpha[j], lambda_cf[j], lambda);
	S_b = ESS_2013_Schoenfeldt_thermal_spectrum(I_th[j+1], T[j+1], I_SD[j+1], alpha[j+1], lambda_cf[j+1], lambda);;
	*p = (1-dS)*S_a + dS*S_b;
	break;
      }
    }
  } else {
    printf("Sorry! Moderator height must be between %g and %g cm\n",height[6],height[0]);
    exit(-1);
  }
  /* Next is time structure... */
  *t=0;
  *t = extras.tmultiplier*ESS_SOURCE_DURATION*(rand01());
  /* Troels Schoenfeldt function for timestructure */
  *p *= extras.tmultiplier*ESS_2014_Schoenfeldt_thermal_timedist(*t, lambda, 100*extras.height_t, ESS_SOURCE_DURATION);
  /* No corrections for "geometrical anisotropy" at this point */
  
} /* end of ESS_2013_Schoenfeldt_thermal */

/* This is the thermal moderator with 2014 updates, fits from Troels Schoenfeldt */
void ESS_2014_Schoenfeldt_thermal(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, ess_moderator_struct extras)
{
  if ((extras.height_t <= 0.12) && (extras.height_t >= 0.01)) {
    *p = ESS_2014_Schoenfeldt_thermal_spectrum(lambda, 100*extras.height_t);
  } else {
    printf("Sorry! Moderator height must be between %g and %g cm\n",0.12,0.01);
    exit(-1);
  }
 
  /* Next is time structure... */
  *t=0;
  *t = extras.tmultiplier*ESS_SOURCE_DURATION*(rand01());
  /* Troels Schoenfeldt function for timestructure */
  *p *= extras.tmultiplier*ESS_2014_Schoenfeldt_thermal_timedist(*t, lambda, 100*extras.height_t, ESS_SOURCE_DURATION);   /* Using Width_c is NOT a typo - dependent on cold moderator geometry - WTF?*/
  *p *= ESS_2014_Schoenfeldt_thermal_y0(100*extras.X, 100*extras.height_t) * ESS_2014_Schoenfeldt_thermal_x0(100*extras.X, 100*extras.height_t, 100*extras.Width_t);
  *p *= ESS_2014_Schoenfeldt_thermal_Theta120(extras.beamportangle,extras.Wasleft);
  //printf("%g\n",ESS_2014_Schoenfeldt_thermal_Theta120(extras.beamportangle,extras.Wasleft));
} /* end of ESS_2014_Schoenfeldt_thermal */

/* 2015 start */

/* This is the thermal moderator with 2015 updates, fits from Troels Schoenfeldt */
void ESS_2015_Schoenfeldt_thermal(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, ess_moderator_struct extras)
{
  if ((extras.height_t == 0.03) || (extras.height_t == 0.06)) {
    *p = ESS_2015_Schoenfeldt_thermal_spectrum(lambda, extras.beamportangle);
  } else {
    printf("Sorry! Moderator height must be either %g or %g m\n",0.03,0.06);
    exit(-1);
  }

 
  /* Next is time structure... */
  *t=0;
  *t = extras.tmultiplier*ESS_SOURCE_DURATION*(rand01());
  /* Troels Schoenfeldt function for timestructure */
  *p *= extras.tmultiplier*ESS_2015_Schoenfeldt_thermal_timedist(*t, lambda, 100*extras.height_c, ESS_SOURCE_DURATION);  
  if (extras.height_c == 0.03) {
    // 3cm case
    *p *= ESS_2015_Schoenfeldt_thermal_y0(100*extras.Y) * ESS_2015_Schoenfeldt_thermal_x0(100*extras.X, extras.beamportangle);
  } else {
    // 6cm case
    // Downscale brightness by factor from 
    // "New ESS Moderator Baseline", Ken Andersen, 9/4/2015
    *p *= (6.2e14/9.0e14);
    *p *= ESS_2014_Schoenfeldt_thermal_y0(100*extras.Y, 100*extras.height_c) * ESS_2015_Schoenfeldt_thermal_x0(100*extras.X, extras.beamportangle);
  }
} /* end of ESS_2015_Schoenfeldt_thermal */


/* This is the cold moderator with 2015 updates, fits from Troels Schoenfeldt */
/* Parametrization including moderator height for the "pancake" moderator */
void ESS_2015_Schoenfeldt_cold(double *t, double *p, double lambda, double tfocus_w, double tfocus_t, double tfocus_dt, ess_moderator_struct extras)
{
   if ((extras.height_c == 0.03) || (extras.height_c == 0.06)) {
    *p = ESS_2015_Schoenfeldt_cold_spectrum(lambda,extras.beamportangle);
  } else {
    printf("Sorry! Moderator height must be either %g or %g m\n",0.03,0.06);
    exit(-1);
  }

  /* Next is time structure... */
  *t=0;
  double pt=0;
  
  // Potentially apply russian roulette technique 
  //while (rand01()>pt) {
  *t = extras.tmultiplier*ESS_SOURCE_DURATION*(rand01());
  /* Troels Schoenfeldt function for timestructure */
  pt = extras.tmultiplier*ESS_2015_Schoenfeldt_cold_timedist(*t, lambda, 100*extras.height_c, ESS_SOURCE_DURATION);
  //}
  
  *p *= pt;
  if (extras.Uniform==0) {
    if (extras.height_c == 0.03) {
      // 3cm case
      *p *= ESS_2015_Schoenfeldt_cold_y0(100*extras.Y) * ESS_2015_Schoenfeldt_cold_x0(100*extras.X, extras.beamportangle);
    } else {
      // 6cm case
      // Downscale brightness by factor from 
      // "New ESS Moderator Baseline", Ken Andersen, 9/4/2015
      *p *= (10.1e14/16.0e14);
      *p *= ESS_2014_Schoenfeldt_cold_y0(100*extras.Y, 100*extras.height_c) * ESS_2015_Schoenfeldt_cold_x0(100*extras.X, extras.beamportangle);
    }
  }
} /* end of ESS_2015_Schoenfeldt_cold */

/* This is ESS_2015_Schoenfeldt_cold_y0 - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
double ESS_2015_Schoenfeldt_cold_y0(double y0){
    double par3=30;
    double par4=.35;
    long double cosh_ish=exp(-par4*y0)+exp(par4*y0);
    long double sinh_ish=pow(1+exp(par3*(y0-3./2.)),-1)*pow(1+exp(-par3*(y0+3./2.)),-1);
    return 1./2.*(double)((long double)cosh_ish*(long double)sinh_ish);

} /* end of ESS_2015_Schoenfeldt_cold_y0 */

/* This is ESS_2015_Schoenfeldt_thermal_y0 - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
double ESS_2015_Schoenfeldt_thermal_y0(double y0){
    if(y0<-3./2.+0.105){
        return 1.005*exp(-pow((y0+3./2.-0.105)/0.372,2));
    } else if(y0>3./2.-0.105){
        return 1.005*exp(-pow((y0-3./2.+0.105)/0.372,2));
    }
    return 1.005;
} /* end of ESS_2015_Schoenfeldt_thermal_y0 */

/* This is ESS_2015_Schoenfeldt_cold_x0 - horizontal intensity distribution for the 2015 Schoenfeldt cold moderator */
double ESS_2015_Schoenfeldt_cold_x0(double x0,double theta){
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
    if(i==0)par3=-14.27;
    double par4=-15;
    if(i==3)par4=-3.5;
    if(i==5)par4=-1.9;
    double par5=-7.07979+0.0835695*i-0.0546662*i*i;
    if(i==4)par5=-8.1;

    double line=par0*(x0+12)+par1;
    double CutLeftCutRight=1./((1+exp(par2*(x0-par3)))*(1+exp(-par4*(x0-par5))));

    return line*CutLeftCutRight;
} /* end of ESS_2015_Schoenfeldt_cold_x0 */

/* This is ESS_2015_Schoenfeldt_thermal_x0 - horizontal intensity distribution for the 2015 Schoenfeldt cold moderator */
double ESS_2015_Schoenfeldt_thermal_x0(double x0,double theta){
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

    double soften1=1./(1+exp(8.*(x0-par0)));
    double soften2=1./(1+exp(8.*(x0-par1)));
    double CutLeftCutRight=1./((1+exp(par6*(x0-par7)))*(1+exp(-par8*(x0-par9))));
    double line1=par4*(x0-par0)+par2;
    double line2=(par2-par3)/(par0-par1)*(x0-par0)+par2;
    double line3=par5*(x0-par1)+par3;
    double add45degbumb=1.2*exp(-(x0+7.55)*(x0+7.55)/.35/.35);


    return CutLeftCutRight*(
        (line1+add45degbumb)*soften1
        +line2*soften2*(1-soften1)
        +line3*(1-soften2)
        );
} /* end of ESS_2015_Schoenfeldt_thermal_x0 */

/* This is ESS_2015_Schoenfeldt_cold_Y - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
double ESS_2015_Schoenfeldt_cold_Y(double Y,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2015_Schoenfeldt_cold_Y */

/* This is ESS_2015_Schoenfeldt_thermal_Y - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
double ESS_2015_Schoenfeldt_thermal_Y(double Y,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2015_Schoenfeldt_thermal_Y */

/* This is ESS_2015_Schoenfeldt_cold_Theta120 - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
double ESS_2015_Schoenfeldt_cold_Theta120(double Theta120,double height){
  /* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2015_Schoenfeldt_cold_Theta120 */

/* This is ESS_2015_Schoenfeldt_thermal_Theta120 - vertical intensity distribution for the 2015 Schoenfeldt cold moderator */
double ESS_2015_Schoenfeldt_thermal_Theta120(double beamportangle,int isleft){
  if(!isleft)return cos((beamportangle-30)*DEG2RAD)/cos(30*DEG2RAD);
  return cos((90-beamportangle)*DEG2RAD)/cos(30*DEG2RAD);
/* Placeholder - we assume that this distribution is flat for now */
  return 1;
} /* end of ESS_2015_Schoenfeldt_thermal_Theta120 */


/* This is ESS_2015_Schoenfeldt_cold_timedist time-distribution of the 2014 Schoenfeldt cold moderator */ 
double ESS_2015_Schoenfeldt_cold_timedist(double time,double lambda,double height, double pulselength){
        if(time<0)return 0;
        double tau=3.00094e-004*(4.15681e-003*lambda*lambda+2.96212e-001*exp(-1.78408e-001*height)+7.77496e-001)*exp(-6.63537e+001*pow(fmax(1e-13,lambda+.9),-8.64455e+000));
        if(time<pulselength)return ((1-exp(-time/tau)));
        return ((1-exp(-pulselength/tau))*exp(-(time-pulselength)/tau));

} /* end of ESS_2015_Schoenfeldt_cold_timedist */

/* This is ESS_2015_Schoenfeldt_thermal_timedist time-distribution of the 2015 Schoenfeldt cold moderator */    
double ESS_2015_Schoenfeldt_thermal_timedist(double time,double lambda,double height, double pulselength){
        if(time<0)return 0;
        double tau=3.00000e-004*(1.23048e-002*lambda*lambda+1.75628e-001*exp(-1.82452e-001*height)+9.27770e-001)*exp(-3.91090e+001*pow(fmax(1e-13,lambda+9.87990e-001),-7.65675e+000));
        if(time<pulselength)return ((1-exp(-time/tau)));
        return ((1-exp(-pulselength/tau))*exp(-(time-pulselength)/tau));
} /* end of ESS_2015_Schoenfeldt_thermal_timedist */

/*2015 end */

double TSC2015_z0_BF3cm(const double x0){
    if(x0<-7.16)
        return (8.27-5.1)/(-7.16+14.2)*(x0+14.2)+5.1;
    return 8.27;
}

/* Display of geometry - flat and TDR-like */
void ESS_mcdisplay_flat(double geometry)
{
}/* end of ESS_mcdisplay_flat */

void ESS_mcdisplay_TDRlike(double geometry)
{
}/* end of ESS_mcdisplay_TDRlike */


/* end of ess_source-lib.c */
