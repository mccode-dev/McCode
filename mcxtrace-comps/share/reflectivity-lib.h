/*A library for computing reflectivity in surfaces with multilayers etc.*/
%include "mccode-complex-lib"
#include <math.h>
#include <stdarg.h>
double Table_Value2d(t_Table, double, double);
enum reflec_Type  {COATING_UNDEFINED=0,CONSTANT=1,BARE, COATING, Q_PARAMETRIC, PARRATT, ETH_PARAMETRIC, KINEMATIC, UNDETERMINED};
#define NAME_CONSTANT "constant"
#define NAME_BARE "bare"
#define NAME_COATING "coating"
#define NAME_Q_PARAMETRIC "q"
#define NAME_PARRATT "parratt"
#define NAME_ETH_PARAMETRIC "eth"
#define NAME_KINEMATIC "kinematic"
typedef struct t_reflec_constant{
  double R;
} t_reflec_constant;
typedef struct t_reflec_bare{
  char matrl[256];
  double d;
} t_reflec_bare;
typedef struct t_reflec_coating{
  char matrl[256];
  t_Table *T;
  double d;
  double rho,Z,At;
}t_reflec_coating;
typedef struct t_reflec_q_prmtc{
  char fname[256];
  t_Table *T;
  double qmin,qmax;
} t_reflec_q_prmtc;
typedef struct t_reflec_parratt{
  int N;
  double *d, *delta, *beta;
} t_reflec_parratt;
/**Multilayer in the kinematical approximation*/
typedef struct t_reflec_kinematic{
  int N;/**The number of bilayers in the multilayer*/
  double Gamma;/**The fraction of thickness of high e-density material*/
  double Lambda;/**The thickness of the bilayer*/
  double rho_AB;/**Electron density contrast between material A and B*/
} t_reflec_kinematic;
typedef struct t_reflec_eth_prmtc{
  char fname[256];
  t_Table *T;
  double emin,emax,estep;/*energy range*/
  double thetamin,thetamax,thetastep;/*incidence angle range*/
} t_reflec_theta_e_prmtc;
typedef struct reflec_T {
  enum reflec_Type type;
  struct t_reflec_bare rb;
  struct t_reflec_coating rc;
  struct t_reflec_q_prmtc rqpm;
  struct t_reflec_parratt rp;
  struct t_reflec_constant rconst;
  struct t_reflec_kinematic rk;
  struct t_reflec_eth_prmtc rethpm;
} t_Reflec;
/* reflectivity-lib.c */
int reflec_Init(t_Reflec *R, enum reflec_Type type, char *file, void *pars);
int reflec_Init_File(t_Reflec *R, char* filename);
int reflec_Init_parratt(t_Reflec *R, int N, double *d, double *delta, double *beta);
int reflec_Init_kinematic(t_Reflec *R, int N, double Gamma, double Lambda, double rhoAB);
int reflec_Init_const(t_Reflec *R, double R0);
#pragma acc routine
cdouble refleccq(t_Reflec r_handle, double q, double g, double k, double theta );
#pragma acc routine
double reflecq(t_Reflec r_handle, double q, double g, double k, double theta );
#pragma acc routine
cdouble reflecc(t_Reflec r_handle, double kix, double kiy, double kiz, double kfx, double kfy, double kfz, double g );
#pragma acc routine
cdouble reflec_coating(t_Reflec r_handle, double q, double g, double k);
#pragma acc routine
cdouble reflec_bare(t_Reflec r_handle, double q, double g);
#pragma acc routine
cdouble reflec_q_prmtc(t_Reflec r_handle, double q, double g);
#pragma acc routine
cdouble reflec_parratt(t_Reflec r_handle, double q, double g, double k);
#pragma acc routine
cdouble reflec_kinematic(t_Reflec r_handle, double q, double g);
#pragma acc routine
cdouble parrat_reflec_bulk(int lc, double *delta, double *beta, double *d, double k, double q);
#pragma acc routine
cdouble reflec_eth_prmtc(t_Reflec r_handle, double e, double theta, double g);
enum reflec_Type get_table_reflec_type(t_Table *t);
