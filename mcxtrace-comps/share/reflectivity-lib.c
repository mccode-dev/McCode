%include "mccode-complex-lib"
#include <stdarg.h>
#include <math.h>

#ifdef REFLIBNAME
#undef REFLIBNAME
#endif
#define REFLIBNAME "reflectivity-lib"

/*Single entry point init function. This will determine the underlying init function base on the type paremeter.
  If the typoe is UNDEFINED, will try to read a datafile and guess from its structure.
  Else will try to unpack parameters from the array of doubles.

  Another option is to call the specific init functions directly. E.g reflec_Init_parratt etc.*/

int reflec_Init(t_Reflec *R, enum reflec_Type typ, char *file, void *pars){
  if (R==NULL){
    R=calloc(1,sizeof(t_Reflec));
  }
  int status;
  R->type=typ;
  switch(typ){
    case COATING_UNDEFINED:
      {
        /*no type is given: Assume that it is stated as a #param=<TYPE>
          item in the datafile header*/
        reflec_Init_File(R,file);
        break;
      }
    case CONSTANT:
      {
          if (pars){
	    reflec_Init_const(R,((double *)pars)[0]);
          }else{
              reflec_Init_const(R,0);
	  }
	  break;
      }
    case PARRATT:
      {
          /*cast the first address to be a scalar integer. The latter ones are pointers to double arrays.*/
          int N=(unsigned int)((double **) pars)[0];
          if(pars){
              reflec_Init_parratt(R, N,((double **) pars)[1], ((double **) pars)[2], ((double **) pars)[3]);        
          }else{
              fprintf(stderr,"Warning: %s: No parameters specified to Parratt reflectivity algortihm. Setting R=0.\n",
                      REFLIBNAME);
              R->type=CONSTANT;
              R->prms.rconst.R=0;
          }
          break;
      }
    case KINEMATIC:
      {
          if(pars){
	    reflec_Init_kinematic(R, (int) ((double *)pars)[0],((double *)pars)[1],((double *)pars)[2],((double *)pars)[3]);
          }else{
            reflec_Init_kinematic(R, (int) 0, 0.0, 0.0, 0.0);
          }
          break;
      }
    case BARE:
      {
          stracpy(R->prms.rb.matrl,file,255);
          reflec_Init_File(R,R->prms.rb.matrl);
          break;
      }
    case COATING:
      {
          stracpy(R->prms.rc.matrl,file,255);
          reflec_Init_File(R,R->prms.rc.matrl);
          break;
      }
    case Q_PARAMETRIC:
      {
          stracpy(R->prms.rqpm.fname,file,255);
          reflec_Init_File(R,R->prms.rqpm.fname);
          break;
      }
    case ETH_PARAMETRIC:
      {
          stracpy(R->prms.rethpm.fname,file,255);
          reflec_Init_File(R,R->prms.rethpm.fname);
          break;
      }
    default:
      {
        fprintf(stderr,"Error: %s: Undetermined reflectivity parameterization type. Setting R=0.\n",
                REFLIBNAME);
	free(R);
	R=NULL;
	return -1;
      }
  }
  return 0;
}

int reflec_Init_parratt(t_Reflec *R, int N, double *d, double *delta, double *beta){
    R->type=PARRATT;
    R->prms.rp.N=N;
    R->prms.rp.d=d;
    R->prms.rp.delta=delta;
    R->prms.rp.beta=beta;
    return 0;
}

int reflec_Init_kinematic(t_Reflec *R, int N, double Gamma, double Lambda, double rhoAB){
    R->type=KINEMATIC;
    R->prms.rk.N=N;
    R->prms.rk.Gamma=Gamma;
    R->prms.rk.Lambda=Lambda;
    R->prms.rk.rho_AB=rhoAB;
    return 0;
}
 int reflec_Init_const(t_Reflec *R, double R0){
   R->type=CONSTANT;
   R->prms.rconst.R=R0;
   return 0;
 }

/* Initialize a container object for various types of reflectivity parametrization using
 * an input file as source.*/

int reflec_Init_File(t_Reflec *R, char *filename){
    if (R==NULL){
      R=calloc(1,sizeof(t_Reflec));
    }
    /*a pointer representation has to be used here - otherwise the memory may be garbage collected
      upon return from this function.*/
    t_Table *table=malloc(sizeof(t_Table));

    /*if the filename is neither empty, blank, nor "NULL" read it, else return a constant opaque surface (R=0)*/
    int status;
    if(!(filename && strlen(filename) && strcmp(filename,"NULL") && (status = Table_Read(table, filename, 1)!=-1) ) ) {
      fprintf(stderr,"Warning: %s: No reflectivity file given. Surface is opaque.\n",
              REFLIBNAME);
      R->type=CONSTANT;
      R->prms.rconst.R=0;
      return 0;
    }

    /*if the type has not already been set try to extract it from file header*/
    if (R->type==UNDETERMINED || R->type==COATING_UNDEFINED){
      R->type=get_table_reflec_type(table);
    }

    switch(R->type){
      case CONSTANT:
        {
          R->prms.rconst.R=Table_Index(*table, 0, 0);
          break;
        }

      case BARE:
        {
          char** header_parsed=Table_ParseHeader(table->header,"material", "d", NULL);
          if(!header_parsed[0] || !header_parsed[1] ){
            fprintf(stderr,"Error: %s: Could not parse file \"%s\".\n",
                    REFLIBNAME, filename);
            exit(-1);
          }
          stracpy(R->prms.rb.matrl,header_parsed[0],255);
          R->prms.rb.d=strtod(header_parsed[1], NULL);
          break;
        }

      case COATING:
        {
          char **header_parsed=Table_ParseHeader(table->header, "material", "Z", "A[r]", "rho", "d", NULL);
          if(!(header_parsed[1] && header_parsed[2] && header_parsed[3])){
            fprintf(stderr,"Error: %s: Could not parse file \"%s\".\n",
                    REFLIBNAME,filename);
            exit(-1);
          } else {
            stracpy(R->prms.rc.matrl,header_parsed[0],255);
            R->prms.rc.T = table;
            R->prms.rc.d = strtod(header_parsed[4], NULL);
            R->prms.rc.rho=strtod(header_parsed[3],NULL);
            R->prms.rc.Z=strtod(header_parsed[1],NULL);
            R->prms.rc.At=strtod(header_parsed[2],NULL);
          }
          break;
        }

      case Q_PARAMETRIC:
        {
          stracpy(R->prms.rqpm.fname,filename,255);
          R->prms.rqpm.T=table;
          R->prms.rqpm.qmin=Table_Index(*table,0,0);
          R->prms.rqpm.qmax=Table_Index(*table,table->rows,0);
          break;
        }

      case PARRATT:
        {
          char **header_parsed = Table_ParseHeader(table->header, "#N=", "#d=", "#delta=", "#beta=", NULL);
          if (! (header_parsed[0] && header_parsed[1] && header_parsed[2] && header_parsed[3])){
            fprintf(stderr,"Error: %s: Could not parse file \"%s\".\n",
                    REFLIBNAME,filename);
            exit(-1);
          }
          unsigned long N=strtol(header_parsed[0], NULL, 10);
          R->prms.rp.N = N;
          R->prms.rp.d = calloc(N,sizeof(double));
          *(R->prms.rp.d) = strtod(header_parsed[1], NULL);
          R->prms.rp.delta = calloc(N,sizeof(double));
          *(R->prms.rp.delta) = strtod(header_parsed[2], NULL);
          R->prms.rp.beta = calloc(N,sizeof(double));
          *(R->prms.rp.beta) = strtod(header_parsed[3], NULL);
          break;
        }

      case KINEMATIC:
        {
          char **header_parsed = Table_ParseHeader(table->header, "#N=", "#gamma=", "#lambda=", "#rho_ab=", NULL);
          if (! (header_parsed[0] && header_parsed[1] && header_parsed[2] && header_parsed[3] && header_parsed[4])){
            fprintf(stderr,"Error: %s: Could not parse file \"%s\".\n",
                    REFLIBNAME,filename);
            exit(-1);
          }

          R->prms.rk.N = strtol(header_parsed[0], NULL, 10);
          R->prms.rk.Gamma = strtod(header_parsed[2], NULL);
          R->prms.rk.Lambda = strtod(header_parsed[3], NULL);
          R->prms.rk.rho_AB = strtod(header_parsed[4], NULL);
          break;
        }

      case ETH_PARAMETRIC:
        {
          stracpy(R->prms.rethpm.fname,filename,255);
          R->prms.rethpm.T=table;

          /*parse header for E_min E_max etc.*/
          char **header_parsed = Table_ParseHeader(table->header,"e_min=","e_max=","theta_min=","theta_max=",NULL);
          if (!(header_parsed[0] && header_parsed[1] && header_parsed[2] && header_parsed[3])){
            fprintf(stderr,"Error: %s: Could not parse file \"%s\".\n",
                    REFLIBNAME,filename); //1619
            exit(-1);
          }
          R->prms.rethpm.emin=strtod(header_parsed[0],NULL);
          R->prms.rethpm.emax=strtod(header_parsed[1],NULL);
          R->prms.rethpm.thetamin=strtod(header_parsed[2],NULL);
          R->prms.rethpm.thetamax=strtod(header_parsed[3],NULL);
          int rows = R->prms.rethpm.T->rows;
          int cols = R->prms.rethpm.T->columns;
          if(rows == 0){ //implies cols == 0 as well
            fprintf(stderr,"Error: %s: File %s contains no table.\n",
                    REFLIBNAME, filename);
            exit(1);
          } else {
            if(rows == 1){
              printf("Warning: %s: File %s contains only a single row. Setting e_step = 0.\n",
                     REFLIBNAME, filename);
              R->prms.rethpm.estep=0;
            } else {
              R->prms.rethpm.estep=(R->prms.rethpm.emax - R->prms.rethpm.emin)/(rows-1);
            }
            if(cols == 1){
              printf("Warning: %s: File %s contains only a single column. Setting theta_step = 0.\n",
                     REFLIBNAME, filename);
              R->prms.rethpm.thetastep=0;
            } else {
              R->prms.rethpm.thetastep=(R->prms.rethpm.thetamax - R->prms.rethpm.thetamin)/(cols-1);
            }
          }
          break;
        }
      case UNDETERMINED:
      default:
        {
          fprintf(stderr,"Warning: %s: Undetermined reflectivity parametrization type. R set to 1.\n",
                  REFLIBNAME);
          return 1;
        }
    }
    return 0;
}

enum reflec_Type get_table_reflec_type(t_Table *t){
    char **header_parsed = Table_ParseHeader(t->header,"param=",NULL);
    char *type = header_parsed[0];
    if(!type){
      /*type of reflectivity file is not specified - try to guess instead*/
      header_parsed = Table_ParseHeader(t->header,"Z",NULL);
      long Z = strtol(header_parsed[0],NULL,0);
      if(Z >0 && Z<116){
        /*this appears to be a coating file similar to Pt.txt and Be.txt (in the mcxtrace data library*/
        printf("INFO: %s: Datafile type not explicit in reflectivity file %s.\n"
               "      Inferring type COATING from datafile.\n",
               REFLIBNAME, t->filename);
        return COATING;
      }else{
        fprintf(stderr,"Error: %s: Could not determine reflectivity type from file %s.\n",
                REFLIBNAME, t->filename);
        exit(-1);
      }
    }
    /*for this to work well we need to trim the type string*/
    while (isspace(*type)){
      type++;
    }
    char *back = type+strlen(type);
    while (isspace(*(--back))){
      *(back++)='\0';
    }

    printf("INFO: %s: Reflectivity parameterization type: \'%s\'\n",REFLIBNAME, type);
    if(strcmp(type, NAME_CONSTANT) == 0){
        return CONSTANT;
    } else if(strcmp(type, NAME_BARE) == 0){
        return BARE;
    } else if(strcmp(type, NAME_COATING) == 0){
        return COATING;
    } else if(strcmp(type, NAME_Q_PARAMETRIC) == 0){
        return Q_PARAMETRIC;
    } else if(strcmp(type, NAME_PARRATT) == 0){
        return PARRATT;
    } else if(strcmp(type, NAME_ETH_PARAMETRIC) == 0){
        return ETH_PARAMETRIC;
    } else if(strcmp(type, NAME_KINEMATIC) == 0){
        return KINEMATIC;
    } else {
        printf("Error: %s: \'%s\'; is not a known parametrization!", REFLIBNAME, type);
        exit(-1);
    }
}

/*This section contains the functions that compute the actual reflectivity*/
cdouble reflec_coating(t_Reflec r_handle, double q, double g, double k){
    struct t_reflec_coating *ptr=&(r_handle.prms.rc);
    /*adjust p according to reflectivity*/
    double Qc,f1,f2,na,e;
    /*length of wavevector transfer may be compute from s and n_ above*/

    /*interpolate in material data*/
    e=K2E*k;
    f1=Table_Value(*(ptr->T),e,1);
    f2=Table_Value(*(ptr->T),e,2);
    /*the conversion factor in  the end is to transform the atomic density from cm^-3 to AA^-3
    -> therefore we get Q in AA^-1*/
    na=NA*ptr->rho/ptr->At*1e-24;
    Qc=4*sqrt(M_PI*na*RE*f1);

    cdouble qp,qn,rr;
    double b_mu,R;
    double qn_re;

    qn_re=q/Qc;
    qn=cplx(qn_re,0.0);
    /*delta=na*r0*2*M_PI/k2*f1;*/
    /*beta=na*r0*2*M_PI/k^2*f2; b_mu=beta*(2*k)^2 / Qc^2*/
    b_mu=8*M_PI*na*RE*f2/(Qc*Qc);
    if(qn_re==1){
        /*sqrt(b_mu)*(1+I)*/
        qp=rmul(sqrt(b_mu), cplx(1.0,1.0));
    }else {
        /*(qn*qn)-1+2*I*b_mu*/
        qp=csqrt(cadd(radd(-1.0, cmul(qn,qn)), cplx(0.0,2*b_mu)));
    }
    /*and from this compute the reflectivity*/
    rr=cdiv(csub(qn,qp), cadd(qn,qp));
    return rr;
}

cdouble reflec_bare(t_Reflec r_handle, double q, double g){
    return cplx(0.0,0.0);
}

cdouble reflec_kinematic(t_Reflec r_handle, double q, double g){
    cdouble r1,rN,num_factor,den_factor;
    struct t_reflec_kinematic *ptr=&(r_handle.prms.rk);
    double zeta=ptr->Lambda*q/(2*M_PI);
    double beta=0;
    double r1_scale;

    /*-2*I*RE* ptr->rho_AB * (pow(ptr->Lambda,2.0)*ptr->Gamma/zeta) * sin(M_PI*ptr->Gamma*zeta)/(M_PI*ptr->Gamma*zeta)
      -- everything except the leading -2*I*RE factor is real, so collapse it to a real scale first*/
    r1_scale = ptr->rho_AB * (pow(ptr->Lambda,2.0)*ptr->Gamma/zeta) * sin(M_PI*ptr->Gamma*zeta)/(M_PI*ptr->Gamma*zeta);
    r1 = cplx(0.0, -2*RE*r1_scale);

    /*cexp(I*2*M_PI*zeta*ptr->N)*exp(-beta*ptr->N)*/
    num_factor = rmul(exp(-beta*ptr->N), cexp(cplx(0.0, 2*M_PI*zeta*ptr->N)));
    /*cexp(I*2*M_PI*zeta)*exp(beta)*/
    den_factor = rmul(exp(beta), cexp(cplx(0.0, 2*M_PI*zeta)));

    /*r1*(1-num_factor)/(1-den_factor)*/
    rN = cdiv(cmul(r1, radd(1.0, cneg(num_factor))), radd(1.0, cneg(den_factor)));
    return rN;
}

cdouble reflec_q_prmtc(t_Reflec r_handle, double q, double g){
    double r;
    struct t_reflec_q_prmtc *ptr=&(r_handle.prms.rqpm);
    if (ptr->T->columns>2){
        double c=(ptr->T->columns-2)*g+1;
        r=Table_Value2d(*(ptr->T),ptr->T->rows*q/(ptr->qmax-ptr->qmin),c);
    }else{
        r=Table_Value(*(ptr->T),q,1);
    }
    return cplx(r,0.0);
}

cdouble reflec_eth_prmtc(t_Reflec r_handle, double g, double e, double th){
    double r,ec,thc;
    struct t_reflec_eth_prmtc *ptr=&(r_handle.prms.rethpm);
    ec=(ptr->T->rows-1) * (e-ptr->emin)/(ptr->emax - ptr->emin);
    thc=(ptr->T->columns-1)*(th-ptr->thetamin)/(ptr->thetamax - ptr->thetamin);
    r=Table_Value2d(*(ptr->T),ec,thc);
    return cplx(r,0.0);
}

/* Entry function to Parratt's recursive algorithm for multilayers.*/
cdouble reflec_parratt(t_Reflec r_handle, double q, double g, double k){
    cdouble r,qp,rp;
    double k2;
    cdouble qinf;
    cdouble qpd,rd,p;
    t_reflec_parratt *pp=&(r_handle.prms.rp);

    qp=cplx(q,0.0);
    /*q*q - 8*k2* *(pp->delta) + I*8*k2* *(pp->beta)*/
    qpd=csqrt(cplx(q*q - 8*k2* *(pp->delta), 8*k2* *(pp->beta)));
    k2=k*k;

    if (pp->N>0){
        rp=cdiv(csub(qp,qpd), cadd(qp,qpd));
        rd=parrat_reflec_bulk(pp->N,pp->delta,pp->beta,pp->d,k,q);
        /*I*qpd* *(pp->d)*/
        p=cexp(rmul(*(pp->d), cmul(cplx(0.0,1.0), qpd)));
        /*(rp+p*rd)/(1+rp*rd*p)*/
        r = cdiv(cadd(rp, cmul(p,rd)), radd(1.0, cmul(cmul(rp,rd),p)));
    }else{
        r = cdiv(csub(qp,qpd), cadd(qp,qpd));
    }
    return r;
}
/* Lower layer function fo rParratt's recursive algorithm. Here recursion
* takes place by calls to itself.*/
cdouble parrat_reflec_bulk(int N, double *delta, double *beta, double *d, double k, double q){
    cdouble qp,rp,rr;
    double k2=k*k;
    cdouble qinf;
    cdouble qpd,rpd,p;

    /*q*q - 8*k2* *delta + I*8*k2* *beta*/
    qp=csqrt(cplx(q*q - 8*k2* *delta, 8*k2* *beta));
    /*q*q - 8*k2* *(delta+1) + I*8*k2* *(beta+1)*/
    qpd=csqrt(cplx(q*q - 8*k2* *(delta+1), 8*k2* *(beta+1)));

    if (N>1){
        rp=cdiv(csub(qp,qpd), cadd(qp,qpd));
        rpd=parrat_reflec_bulk(N-1,(delta+1),(beta+1),(d+1),k,q);
        /*I*qpd* d[1]*/
        p=cexp(rmul(d[1], cmul(cplx(0.0,1.0), qpd)));
        /*(rp+p*rpd)/(1+rp*rpd*p)*/
        rr= cdiv(cadd(rp, cmul(p,rpd)), radd(1.0, cmul(cmul(rp,rpd),p)));
    }
    if (N==1){
        /*the bottom layer (on top of substrate)*/
        rr=cdiv(csub(qp,qpd), cadd(qp,qpd));
    }
    return rr;
}

/* Dispatcher functions that call the underlying computations depending on the type of reflectivity*/

cdouble refleccq( t_Reflec r_handle, double q, double g, double k, double theta){
    cdouble r;
    /*using the normalized coordinate g which lies along the grading direction*/

    switch(r_handle.type){
      case CONSTANT:
        {
          r=cplx(r_handle.prms.rconst.R,0.0);
          break;
        }
      case BARE:
        {
          r=reflec_bare(r_handle,q,g);
          break;
        }
      case COATING:
        {
          r=reflec_coating(r_handle,q,g,k);
          break;
        }
      case Q_PARAMETRIC:
        {
          r=reflec_q_prmtc(r_handle,q,g);
          break;
        }
/*      case PARRATT:*/
/*        {*/
/*          r=reflec_parratt(r_handle,q,g,k);*/
/*          break;*/
/*        }*/
      case ETH_PARAMETRIC:
        {
          r=reflec_eth_prmtc(r_handle,g,k*K2E,theta);
          break;
        }
      case KINEMATIC:
        {
          r=reflec_kinematic(r_handle,q,g);
          break;
        }
      default:
        {
#ifndef OPENACC
          fprintf(stderr,"Error: %s: Undetermined reflectivity type. R set to 1.\n",
                  REFLIBNAME);
#endif
          r=cplx(1.0,0.0);
        }
    }
    return r;
}

double reflecq( t_Reflec r_handle, double q, double g, double k, double theta){
    double r;

    switch(r_handle.type){
      case CONSTANT:
        {
          r=fabs(r_handle.prms.rconst.R);
          break;
        }
    case BARE:
        {
          r=cabs(reflec_bare(r_handle,q,g));
          break;
        }
    case COATING:
        {
          cdouble rp;
          rp=reflec_coating(r_handle,q,g,k);
          r= sqrt(creal(cmul(rp, conj(rp))));
          break;
        }
    case Q_PARAMETRIC:
        {
          r=cabs(reflec_q_prmtc(r_handle,q,g));
          break;
        }
        /*case PARRATT:
        {
          r=cabs(reflec_parratt(r_handle,q,g,k));
          break;
	  }*/
    case ETH_PARAMETRIC:
        {
            r=cabs(reflec_eth_prmtc(r_handle,g,k*K2E,theta));
            break;
        }
      case KINEMATIC:
        {
          r=cabs(reflec_kinematic(r_handle,q,g));
          break;
	  }
      default:
        {
#ifndef OPENACC
          fprintf(stderr,"Warning: %s: Undetermined reflectivity type. R set to 1.\n",
                  REFLIBNAME);
#endif
          r=1.0;
	}
    }
    return r;
}

double refleceth( t_Reflec r_handle,double e, double th, double g){
    double q;
    double k=e*E2K;
    q=k*2.0*sin(th);
    if(r_handle.type==PARRATT || r_handle.type==COATING){
      return reflecq(r_handle,q,g,k,0);
    }else{
      return reflecq(r_handle,q,g,k,th);
    }
}

cdouble reflecceth( t_Reflec r_handle,double e, double th, double g){
    double q;
    double k=e*E2K;
    q=k*2.0*sin(th);
    if(r_handle.type==PARRATT){
      return refleccq(r_handle,q,g,k,0);
    }else{
      return refleccq(r_handle,q,g,k,th);
    }
}
