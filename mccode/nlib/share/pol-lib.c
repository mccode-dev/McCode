/****************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright 1997-2006, All rights reserved
*         Risoe National Laboratory, Roskilde, Denmark
*         Institut Laue Langevin, Grenoble, France
*
* Library: share/pol-lib.c
*
* %Identification
* Written by: Erik B Knudsen, Astrid Rømer & Peter Christiansen
* Date: Oct 08
* Origin: RISOE
* Release: McStas 1.12
* Version: $Revision: 4466 $
*
* This file is to be imported by polarisation components.
* It handles some shared functions.
* Embedded within instrument in runtime mode.
* Variable names have prefix 'mc_pol_' for 'McStas Polarisation'
* to avoid conflicts
*
* Usage: within SHARE
* %include "pol-lib"
*
****************************************************************************/

#ifndef POL_LIB_H
#include "pol-lib.h"
#endif

#include<sys/stat.h>

%include "read_table-lib"
%include "interpolation-lib"

/*definition of the magnetic stack*/
#ifndef MCMAGNET_STACKSIZE
#define MCMAGNET_STACKSIZE 12
#endif

/*Threshold below which two magnetic fields are considered to be
 * in the same direction.*/
#ifndef mc_pol_angular_accuracy
#define mc_pol_angular_accuracy (1.0*DEG2RAD)
#endif

/*The maximal timestep taken by neutrons in a const field*/
#ifndef mc_pol_initial_timestep
#define mc_pol_initial_timestep 1e-5;
#endif

#ifdef PROP_MAGNET
#undef PROP_MAGNET
#define PROP_MAGNET(dt) \
  do { \
    /* change coordinates from local system to lab system. The magnet stack always refers to the lab system. */ \
    Rotation rotLM; \
    Coords   posLM = POS_A_CURRENT_COMP; \
    rot_transpose(ROT_A_CURRENT_COMP, rotLM); \
    SimpleNumMagnetPrecession(posLM, rotLM, _particle, dt); \
  } while(0)
#endif

enum field_functions{
  tabled=-1,
  none=0,
  constant=1,
  rotating=2,
  majorana=3,
  MSF=4,
  RF=5,
  gradient=6,
};

#pragma acc routine seq
int magnetic_field_dispatcher(int func_id, double x, double y, double z, double t, double *bx,double *by, double *bz, double pars[8]){
  int retval=1;
  switch (func_id){
    case constant: 
      {
        retval=const_magnetic_field(x,y,z,t,bx,by,bz, pars);
        break;
      }
    case majorana:
      {
        retval=majorana_magnetic_field(x,y,z,t,bx,by,bz, pars);
        break;
      }
    case rotating:
      {
        retval=rot_magnetic_field(x,y,z,t,bx,by,bz, pars);
        break;
      }
    case RF:
      {
        /*not implemented yet*/
        break;
      }
    case gradient:
      {
        retval=gradient_magnetic_field(x,y,z,t,bx,by,bz,pars);
        break;
      }
    case none:
      {
        retval=0;*bx=0;*by=0;bz=0;
      }
  }
  return retval;
}


/*traverse the stack and return the magnetic field*/
#pragma acc routine seq
int mcmagnet_get_field(_class_particle *_particle, double x, double y, double z, double t, double *bx,double *by, double *bz, double dummy[8]){
  mcmagnet_field_info *p,**stack;
  Coords in,loc,b,bsum={0,0,0};
  Rotation r;
  /*extract the magnetic field stack experienced by this particle*/
  stack=((mcmagnet_field_info **) _particle->mcMagnet);

  /*PROP_MAGNET takes care of transforming local "PROP" coordinates to lab system*/
  in.x=x;in.y=y;in.z=z;

  int i=0,stat=1;
  p=stack[i];
  *bx=0;*by=0;*bz=0;
  if (p==NULL || p->func_id==0){
    *bx=0;*by=0;*bz=0;
    return 0;
  }
  
  while(p!=NULL){
    /*transform to the coordinate system of the particular magnetic function*/
    loc=coords_sub(rot_apply(*(p->rot),in),*(p->pos));
    stat=magnetic_field_dispatcher((p->func_id),loc.x,loc.y,loc.z,t,&(b.x),&(b.y),&(b.z),p->field_parameters);
    /*check if the field function should be garbage collected*/
    if (!stat){
      /*transform to the lab system and add up. (resusing loc variable - to now contain the field in lab coords)*/
      rot_transpose(*(p->rot),r);
      loc=rot_apply(r,b);
      bsum.x+=loc.x;bsum.y+=loc.y;bsum.z+=loc.z;
      //printf("Bs=(%g %g %g), B=(%g %g %g)\n",bsum.x,bsum.y,bsum.z,loc.x,loc.y,loc.z);
    }
    if (p->stop) break;
    p=stack[++i];
  }
  /*we now have the magnetic field in lab coords in loc., transfer it back to caller*/
  *bx=bsum.x;
  *by=bsum.y;
  *bz=bsum.z;
  return 0;
}

#pragma acc routine seq
void *mcmagnet_push(_class_particle *_particle, int func_id, Rotation *magnet_rot, Coords *magnet_pos, int stopbit, double prms[8]){
  /*check if any field has been pushed already*/
  if (_particle->mcMagnet==NULL){
    /*No fields exist in the stack so allocate room for it and point _particle->mcMagnet to it*/
    #ifdef OPENACC
    _particle->mcMagnet=malloc(MCMAGNET_STACKSIZE*sizeof(mcmagnet_field_info *));
    /* Lack of a calloc makes us NULLify manually since we check for NULL further down */ 
    for (int ll=0; ll<MCMAGNET_STACKSIZE; ll++) {
      ((mcmagnet_field_info **) _particle->mcMagnet)[ll]=NULL;
    }
    #else
    _particle->mcMagnet=calloc(MCMAGNET_STACKSIZE,sizeof(mcmagnet_field_info *));
    #endif
    if(!_particle->mcMagnet) {
      fprintf(stderr,"pol-lib: malloc() error pushing field to stack\n");
      exit(-1);
    }
  }
  mcmagnet_field_info **stack=((mcmagnet_field_info **) _particle->mcMagnet);

  /*move the stack one step down start from -2 since we have 0-indexing (i.e. last item is stacksize-1) */
  int i;
  for (i=MCMAGNET_STACKSIZE-2;i>=0;i--){
    stack[i+1]=stack[i];
  }
  /*allocate momery for the new stack item*/
  #ifdef OPENACC
  stack[0]=malloc(sizeof(mcmagnet_field_info));
  #else
  stack[0]=calloc(1, sizeof(mcmagnet_field_info));
  #endif
  if(!stack[0]) {
    fprintf(stderr,"pol-lib: malloc() error allocating field stack[0]\n");
    exit(-1);
  }
  /*drop the new item in*/
  mcmagnet_pack(stack[0],func_id,magnet_rot,magnet_pos,stopbit,prms);
  return NULL;// (void *) stack[0];
}

#pragma acc routine seq
void *mcmagnet_pop(_class_particle *_particle) {
  mcmagnet_field_info **stack=((mcmagnet_field_info **) _particle->mcMagnet);
  /*free memory for upper element*/
#ifdef OPENACC
  free(stack[0]);
#else
  free(stack[0]);
#endif
  /*move the stack one step up*/
  int i;
  for (i=0;i<MCMAGNET_STACKSIZE-2;i++){
    stack[i]=stack[i+1];
  }
  /*if this makes the upper element go NULL also NULL _particle->mcMagnet
    to flag that precession propagation is no longer needed*/
  if(stack[0]==NULL){
    free(_particle->mcMagnet);
    _particle->mcMagnet=NULL;
  }
  return NULL;// (void*) (stack[0]);
}

/*Example magnetic field functions*/
#pragma acc routine seq
int const_magnetic_field(double x, double y, double z, double t,
    double *bx, double *by, double *bz, void *data) {
  if (!data) return 1;
  *bx=((double *)data)[0];
  *by=((double *)data)[1];
  *bz=((double *)data)[2];
  return 0;
}

#pragma acc routine seq
int rot_magnetic_field(double x, double y, double z, double t,
    double *bx, double *by, double *bz, void *data) {
  /* Field of magnitude By that rotates to x in magnetLength m*/
  
  if (!data) return 1;
  double Bmagnitude=((double *)data)[0];//   = mcMagnetData[1];
  double magnetLength=((double *)data)[1];// = mcMagnetData[5];
  *bx =  Bmagnitude * sin(PI/2*z/magnetLength);
  *by =  Bmagnitude * cos(PI/2*z/magnetLength);
  *bz =  0;
  return 0;
}

#pragma acc routine seq
int majorana_magnetic_field(double x, double y, double z, double t,
    double *bx, double *by, double *bz, void *data) {
  /* Large linearly decreasing (from +Bx to -Bx in magnetLength) component along x axis,
   * small constant component along y axis
   */
  if (!data) return 1;
  double Blarge       = ((double *)data)[0];
  double Bsmall       = ((double *)data)[1];
  double magnetLength = ((double *)data)[2];
  *bx =  Blarge -2*Blarge*z/magnetLength;
  *by =  Bsmall;
  *bz =  0;
  return 0;
}


#pragma acc routine seq
int gradient_magnetic_field(double x, double y, double z, double t, double *bx, double *by, double *bz, void *data){
  double *bp = (double *)data;
  double *b1=bp;
  double L=bp[3];
  double *b2=&(bp[4]);
  double alpha=(z+L/2.0)/L;
  *bx=alpha * b1[0] + (1-alpha)*b2[0];
  *by=alpha * b1[1] + (1-alpha)*b2[1];
  *bz=alpha * b1[2] + (1-alpha)*b2[2];
  return 0;
}


/****************************************************************************
* void GetMonoPolFNFM(double Rup, double Rdown, double *FN, double *FM)
*
* ACTION: Calculate FN and FM from reflectivities Rup and Rdown
*
* For a monochromator (nuclear and magnetic scattering), the
* polarisation is done by defining the reflectivity for spin up (Rup)
* and spin down (Rdown) (which can be negative, see now!) and based on
* this the nuclear and magnetic structure factors are calculated:
* FM = sign(Rup)*sqrt(|Rup|) + sign(Rdown)*sqrt(|Rdown|)
* FN = sign(Rup)*sqrt(|Rup|) - sign(Rdown)*sqrt(|Rdown|)
*****************************************************************************/
#pragma acc routine seq
void GetMonoPolFNFM(double mc_pol_Rup, double mc_pol_Rdown,
		    double *mc_pol_FN, double *mc_pol_FM) {
  if (mc_pol_Rup>0)
    mc_pol_Rup   = sqrt(fabs(mc_pol_Rup));
  else
    mc_pol_Rup   = -sqrt(fabs(mc_pol_Rup));

  if (mc_pol_Rdown>0)
    mc_pol_Rdown = sqrt(fabs(mc_pol_Rdown));
  else
    mc_pol_Rdown = -sqrt(fabs(mc_pol_Rdown));

  *mc_pol_FN = 0.5*(mc_pol_Rup + mc_pol_Rdown);
  *mc_pol_FM = 0.5*(mc_pol_Rup - mc_pol_Rdown);
  return;
}

/****************************************************************************
* void GetMonoPolRefProb(double FN, double FM, double sy, double *prob)
*
* ACTION: Calculate reflection probability from sy, FN and FM
*
* For a monochromator with up direction along y the reflection
* probability is given as:
* prob = FN*FN + 2*FN*FM*sy_in + FM*FM
*     (= |Rup| + |Rdown| (for sy_in=0))
* where FN and FM are calculated from Rup and Rdown by GetMonoPolFNFM
*****************************************************************************/
#pragma acc routine seq
void GetMonoPolRefProb(double mc_pol_FN, double mc_pol_FM,
		       double mc_pol_sy, double *mc_pol_prob) {
  *mc_pol_prob = mc_pol_FN*mc_pol_FN + mc_pol_FM*mc_pol_FM
    + 2*mc_pol_FN*mc_pol_FM*mc_pol_sy;
  return;
}

/****************************************************************************
* void SetMonoPolRefOut(double FN, double FM, double refProb,
*		     double* sx, double* sy, double* sz) {
*
* ACTION: Set the outgoing polarisation vector of the reflected neutrons
* given FN, FM and the reflection probability.
*
* For a monochromator with up direction along y the outgoing polarisation
* is given as:
*	sx = (FN*FN - FM*FM)*sx_in/R0;
*	sy = ((FN*FN - FM*FM)*sy_in + 2*FN*FM + FM*FM*sy_in)/R0;
*	sz = (FN*FN - FM*FM)*sz_in/R0;
* where sx_in, sy_in, and sz_in is the incoming polarisation, and
* FN and FM are calculated from Rup and Rdown by GetMonoPolFNFM
*****************************************************************************/
#pragma acc routine seq
void SetMonoPolRefOut(double mc_pol_FN, double mc_pol_FM,
		      double mc_pol_refProb, double* mc_pol_sx,
		      double* mc_pol_sy, double* mc_pol_sz) {
  *mc_pol_sx = (mc_pol_FN*mc_pol_FN - mc_pol_FM*mc_pol_FM)*(*mc_pol_sx)
    /mc_pol_refProb;
  *mc_pol_sy = ((mc_pol_FN*mc_pol_FN - mc_pol_FM*mc_pol_FM)*(*mc_pol_sy)
		+ 2*mc_pol_FN*mc_pol_FM + 2*mc_pol_FM*mc_pol_FM*(*mc_pol_sy))
    /mc_pol_refProb;
  *mc_pol_sz = (mc_pol_FN*mc_pol_FN - mc_pol_FM*mc_pol_FM)*(*mc_pol_sz)
    /mc_pol_refProb;
  return;
}

/****************************************************************************
* void SetMonoPolTransOut(double FN, double FM, double refProb,
*			  double* sx, double* sy, double* sz) {
*
* ACTION: Set the outgoing polarisation vector of the transmitted neutrons
* given FN, FM and the REFLECTION probability.
*
* We use that the polarization is conserved so:
* s_in = refProb*s_ref+(1-refProb)*s_trans, and then
* s_trans = (s_in-refProb*s_ref)/(1-refProb)
* where refProb is calculated using the routine GetMonoPolRefProb
* and s_ref is calculated by SetMonoPolRefOut
*****************************************************************************/
#pragma acc routine seq
void SetMonoPolTransOut(double mc_pol_FN, double mc_pol_FM,
			double mc_pol_refProb, double* mc_pol_sx,
			double* mc_pol_sy, double* mc_pol_sz) {
  double mc_pol_sx_ref = *mc_pol_sx, mc_pol_sy_ref = *mc_pol_sy;
  double mc_pol_sz_ref = *mc_pol_sz;

  // By passing 1 as probability we get mc_pol_refProb*s_out_ref
  SetMonoPolRefOut(mc_pol_FN, mc_pol_FM, 1,
		   &mc_pol_sx_ref, &mc_pol_sy_ref, &mc_pol_sz_ref);
  *mc_pol_sx = (*mc_pol_sx - mc_pol_sx_ref)/(1 - mc_pol_refProb);
  *mc_pol_sy = (*mc_pol_sy - mc_pol_sy_ref)/(1 - mc_pol_refProb);
  *mc_pol_sz = (*mc_pol_sz - mc_pol_sz_ref)/(1 - mc_pol_refProb);
  return;
}

/****************************************************************************
* void SimpleNumMagnetPrecession(double x, double y, double z, double t,
*			         double vx, double vy, double vz,
*			         double* sx, double* sy, double* sz, double dt)
*
*****************************************************************************/
#pragma acc routine seq
void SimpleNumMagnetPrecession(Coords posMagnet, Rotation rotMagnet, _class_particle *precess_particle, double dt) {

  double Bx, By, Bz, mc_pol_phiz;
  double BxStart, ByStart, BzStart, Bstart;
  double BxTemp, ByTemp, BzTemp, Btemp;
  double mc_pol_timeStep, mc_pol_sp;
  const double mc_pol_spThreshold  = cos(mc_pol_angular_accuracy);
  _class_particle ploc=*precess_particle;
  _class_particle *pp = &ploc;
  Rotation mc_pol_rotBack;
  
  /* change coordinates from current local system to lab system */
  mccoordschange(posMagnet, rotMagnet, pp);
  mcmagnet_get_field(pp, pp->x, pp->y, pp->z, pp->t,&BxTemp, &ByTemp, &BzTemp,NULL);

  do {

    Bx = 0; By = 0; Bz = 0; mc_pol_phiz = 0;
    BxStart = BxTemp; ByStart = ByTemp; BzStart = BzTemp;
    Bstart = sqrt(BxStart*BxStart + ByStart*ByStart + BzStart*BzStart);
    
    mc_pol_timeStep = mc_pol_initial_timestep;

    /*check if we need to take multiple steps of maximum size mc_pol_timeStep*/
    if(dt<mc_pol_timeStep){
      mc_pol_timeStep = dt;
    }
    double xp,yp,zp; 
    do {
      xp = pp->x+ pp->vx*mc_pol_timeStep;
      yp = pp->y+ pp->vy*mc_pol_timeStep;
      zp = pp->z+ pp->vz*mc_pol_timeStep;

      mcmagnet_get_field(pp,xp,yp,zp, pp->t+mc_pol_timeStep, &BxTemp, &ByTemp, &BzTemp, NULL);
      /* not so elegant, but this is how we make sure that the steps decrease
       when the WHILE condition is not met*/
      mc_pol_timeStep *= 0.5;

      Btemp = sqrt(BxTemp*BxTemp + ByTemp*ByTemp + BzTemp*BzTemp);

      mc_pol_sp = scalar_prod(BxStart, ByStart, BzStart, BxTemp, ByTemp, BzTemp);
      mc_pol_sp /= Bstart*Btemp;

    } while (mc_pol_sp<mc_pol_spThreshold && mc_pol_timeStep>FLT_EPSILON);

    mc_pol_timeStep*=2;

    // update coordinate values
    pp->x = xp;
    pp->y = yp;
    pp->z = zp;
    pp->t += mc_pol_timeStep;
    dt -= mc_pol_timeStep;

    /*precess around mean magnetic field*/
    Bx = 0.5 * (BxStart + BxTemp);
    By = 0.5 * (ByStart + ByTemp);
    Bz = 0.5 * (BzStart + BzTemp);
    mc_pol_phiz = fmod(sqrt(Bx*Bx+ By*By+ Bz*Bz) * mc_pol_timeStep*mc_pol_omegaL, 2*PI);

    /* Do the neutron spin precession for the small timestep*/
    if(!(Bx==0 && By==0 && Bz==0)) {

      double sx_in = pp->sx;
      double sy_in = pp->sy;
      double sz_in = pp->sz;

      rotate(pp->sx, pp->sy, pp->sz, sx_in,sy_in,sz_in, mc_pol_phiz, Bx, By, Bz);
    }

  } while (dt>0);

  /* change back spin coordinates from lab system to local system*/
  rot_transpose(rotMagnet, mc_pol_rotBack);
  /*have to do this "manually" since mccordschange does not commute/reverse*/
  pp->x-=posMagnet.x; pp->y-=posMagnet.y; pp->z-=posMagnet.z;
  mccoordschange_polarisation(mc_pol_rotBack, &(pp->vx), &(pp->vy), &(pp->vz));
  mccoordschange_polarisation(mc_pol_rotBack, &(pp->sx), &(pp->sy), &(pp->sz));
  /*copy back the spin polarization coordinates to the caller*/
  precess_particle->sx=pp->sx;
  precess_particle->sy=pp->sy;
  precess_particle->sz=pp->sz;
}

/****************************************************************************
* double GetConstantField(double length, double lambda, double angle)
*
* Return the magnetic field in Tesla required to flip a neutron with
* wavelength lambda(1/velocity), angle degrees, over the specified
* length(=time*velocity).
*
*****************************************************************************/
#pragma acc routine seq
double GetConstantField(double mc_pol_length, double mc_pol_lambda,
			double mc_pol_angle)
{
  const double mc_pol_velocity = K2V*2*PI/mc_pol_lambda;
  const double mc_pol_time = mc_pol_length/mc_pol_velocity;

  // B*omegaL*time = angle
  return mc_pol_angle*DEG2RAD/mc_pol_omegaL/mc_pol_time; // T
}

/* end of regular pol-lib.c */
