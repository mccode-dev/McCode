/*******************************************************************************
 *
 * mcvine-lib.h : shared runtime for the MCViNE scattering kernels ported to
 *                McStas 3.x components (MCViNE_*.comp).
 *
 * Contents
 *   - constants (MCViNE values where the kernels depend on them)
 *   - a small run-time expression evaluator (replaces MCViNE's fparser, so
 *     E(Q), S(Q), S(Q,E), sigma(Q) ... can be given as strings)
 *   - numeric text-file readers and 1D/2D/3D interpolated grids
 *   - sample shapes (box / cylinder / hollow cylinder / sphere)
 *   - the MCViNE "HomogeneousNeutronScatterer" transport (single + optional
 *     multiple scattering, uniform-depth sampling, attenuation)
 *   - kernel data structures and S() functions, one per MCViNE kernel
 *   - phonon helpers: DOS (nice_dos), Debye-Waller from DOS, IDF readers,
 *     periodic linearly-interpolated dispersion on grid
 *
 * Use in a component SHARE block:     %include "mcvine-lib"
 * Keep mcvine-lib.h and mcvine-lib.c next to the .comp files (or pass -I).
 *
 * Source of the physics: https://github.com/mcvine/mcvine (mccomponents/lib/
 * kernels/sample) and https://github.com/mcvine/acc (SANS2D_ongrid).
 *
 *******************************************************************************/
#ifndef MCVINE_LIB_H
#define MCVINE_LIB_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MCVINE_PI 3.14159265358979323846

/* MCViNE physical constants (mccomponents/physics/constants.h) */
#define MCVINE_HBAR 1.05457148e-34
#define MCVINE_AMU 1.66053886e-27
#define MCVINE_ECHARGE 1.60217653e-19
#define MCVINE_KELVIN2MEV (1.0 / 11.605)
#define MCVINE_HERTZ2MEV (1.05457148e-34 / 1.60217653e-22)
/* k^2 [AA^-2] -> E [meV]  ( = K2V*K2V*VS2E ) */
#define MCVINE_KSQ2E (K2V * K2V * VS2E)

/* ------------------------------------------------------------------------ */
/* expression evaluator                                                     */
/* ------------------------------------------------------------------------ */
#define MCVINE_EXPR_MAXVARS 8
#define MCVINE_EXPR_MAXSTACK 128
typedef struct {
  int n, cap;
  int* op;
  double* val;
  char text[1024];
} mcvine_expr;

/* compile 'src' using variables varnames[0..nvars-1]. returns 0 on success */
int mcvine_expr_compile (mcvine_expr* e, const char* src, int nvars, const char** varnames, const char* owner);
double mcvine_expr_eval (const mcvine_expr* e, const double* vars);
void mcvine_expr_free (mcvine_expr* e);

/* ------------------------------------------------------------------------ */
/* files, tables and grids                                                  */
/* ------------------------------------------------------------------------ */
typedef struct {
  int nrows;
  int* ncols;
  double** rows;
} mcvine_rows;

FILE* mcvine_fopen (const char* name, const char* mode);
int mcvine_read_rows (const char* file, mcvine_rows* r, const char* owner);
void mcvine_free_rows (mcvine_rows* r);

/* 1D table y(x), x ascending, linear interpolation, 0 outside */
typedef struct {
  int n;
  double *x, *y;
} mcvine_table1d;
int mcvine_table1d_load (mcvine_table1d* t, const char* file, const char* owner);
double mcvine_table1d_eval (const mcvine_table1d* t, double x);

/* 2D grid f(x,y) on (possibly non-uniform) ascending axes, bilinear, 0 outside */
typedef struct {
  int nx, ny;
  double *x, *y, *f;
} mcvine_grid2d;
double mcvine_grid2d_eval (const mcvine_grid2d* g, double x, double y);
/* S(Q,E) in the Isotropic_Sqw format: 1st numeric row = q values,
   2nd numeric row = w (E) values, then nq rows with nw values each */
int mcvine_grid2d_load_sqw (mcvine_grid2d* g, const char* file, const char* owner);
/* image format: 1st row x axis values, 2nd row y axis values,
   then ny rows with nx values each (row index = y)                        */
int mcvine_grid2d_load_image (mcvine_grid2d* g, const char* file, const char* owner);

/* 3D grid on uniform axes (point values, trilinear, 0 outside):
   rows 1-3 : "min max n" for x, y, z; then nx*ny rows of nz values
   (x slowest, z fastest)                                                  */
typedef struct {
  int n[3];
  double min[3], max[3], step[3];
  double* f;
} mcvine_grid3d;
int mcvine_grid3d_load (mcvine_grid3d* g, const char* file, const char* owner);
double mcvine_grid3d_eval (const mcvine_grid3d* g, double x, double y, double z);

/* a scalar function of 1..3 variables given either as expression or file */
typedef struct {
  int mode; /* 0 const, 1 expression, 2 table1d, 3 grid2d, 4 grid3d */
  double c;
  mcvine_expr expr;
  mcvine_table1d t1;
  mcvine_grid2d g2;
  mcvine_grid3d g3;
} mcvine_func;
double mcvine_func_eval (const mcvine_func* f, const double* v);
/* set up from a file (if file non-empty; filetype 2=table1d, 3=sqw grid,
   4=grid3d) or else from an expression in the variables vars[]            */
int mcvine_func_setup (mcvine_func* f, const char* expr, const char* file, int filetype, int nvars, const char** vars, const char* owner);

/* ------------------------------------------------------------------------ */
/* vector helpers                                                           */
/* ------------------------------------------------------------------------ */
double mcvine_len3 (const double* a);
void mcvine_cross3 (const double* a, const double* b, double* c);
/* MCViNE local frame: e1 = v/|v|, e2 = (0,0,1) x e1 (or fallback), e3 = e1 x e2 */
void mcvine_frame (const double* v, double* e1, double* e2, double* e3);
/* dir = sin(t)cos(phi) e2 + sin(t) sin(phi) e3 + cos(t) e1 */
void mcvine_dir_from_frame (const double* e1, const double* e2, const double* e3, double cost, double sint, double phi, double* dir);

/* ------------------------------------------------------------------------ */
/* shapes and transport                                                     */
/* ------------------------------------------------------------------------ */
#define MCVINE_SHAPE_BOX 1
#define MCVINE_SHAPE_CYLINDER 2
#define MCVINE_SHAPE_SPHERE 3
typedef struct {
  int type;
  double xwidth, yheight, zdepth, radius, thickness;
} mcvine_shape;

int mcvine_shape_init (mcvine_shape* s, double radius, double xwidth, double yheight, double zdepth, double thickness, const char* owner);
/* time intervals [seg[2i], seg[2i+1]] (t>=0) spent inside the material */
int mcvine_shape_segments (const mcvine_shape* s, double x, double y, double z, double vx, double vy, double vz, double* seg);

/* kernel S function: modifies v (and may read r, t); multiplies *p by the
   kernel weight; returns 1 on success, 0 if no scattering is possible.    */
typedef int (*mcvine_S_fn) (void* kernel, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_shape shape;
  double mu2200;     /* absorption coefficient at 2200 m/s [1/m] */
  double sigma;      /* scattering coefficient [1/m]             */
  double pack;       /* packing factor                           */
  double p_transmit; /* MC fraction of unscattered (transmitted) events */
  int order;         /* max scattering order (1 = single scattering) */
} mcvine_scatterer;

void mcvine_scatterer_init (mcvine_scatterer* sc, const mcvine_shape* s, double mu2200, double sigma, double pack, double p_transmit, int order);
/* returns -1: event must be absorbed; 0: missed/transmitted; n>0: scattered n times */
int mcvine_scatterer_interact (const mcvine_scatterer* sc, void* kernel, mcvine_S_fn S, _class_particle* _particle);

/* cross section helper: barn / AA^3 -> 1/m */
double mcvine_xs2coeff (double xs_barn, double V_AA3);

/* ------------------------------------------------------------------------ */
/* kernels (one struct + one S function per MCViNE kernel)                  */
/* ------------------------------------------------------------------------ */
typedef struct {
  double m_E;
} mcvine_kernel_ConstantEnergyTransfer;
int mcvine_S_ConstantEnergyTransfer (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  double m_Q, m_E;
} mcvine_kernel_ConstantQE;
int mcvine_S_ConstantQE (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  double m_Q[3], m_E, m_dE;
} mcvine_kernel_ConstantvQE;
int mcvine_S_ConstantvQE (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_func m_E_Q, m_S_Q;
  double m_Qmin, m_Qmax;
  int m_unbiased;
} mcvine_kernel_E_Q;
int mcvine_S_E_Q (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_func m_E_Q, m_S_Q, m_W_Q;
  double m_Qmin, m_Qmax, m_Emin, m_Emax;
  int m_lorentzian, m_unbiased;
} mcvine_kernel_Broadened_E_Q;
void mcvine_Broadened_E_Q_init (mcvine_kernel_Broadened_E_Q* k);
int mcvine_S_Broadened_E_Q (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_func m_E_Q, m_S_Q;
  double m_Emax;
  int m_nsteps;
  double m_xacc;
} mcvine_kernel_E_vQ;
int mcvine_S_E_vQ (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_func m_S;
  double m_Qmin, m_Qmax;
} mcvine_kernel_SQ;
int mcvine_S_SQ (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_func m_S;
} mcvine_kernel_SvQ;
int mcvine_S_SvQ (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_func m_S;
  double m_Qmin, m_Qmax, m_Emin, m_Emax, m_Ef, m_dEf;
  int m_focusing;
} mcvine_kernel_SQE;
int mcvine_S_SQE (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  double m_target[3], m_target_radius, m_tof_at_target, m_dtof;
} mcvine_kernel_DGSSXRes;
int mcvine_DGSSXRes_final (const mcvine_kernel_DGSSXRes* k, const double* dir, double solid_angle, double L, double* v, double t, double* p,
                           _class_particle* _particle);
int mcvine_S_DGSSXRes (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_grid2d m_S;
  double m_Qx_min, m_Qx_max, m_Qy_min, m_Qy_max;
} mcvine_kernel_SANS2D_ongrid;
int mcvine_S_SANS2D_ongrid (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

/* ---- phonon support ---- */
typedef struct {
  int n;
  double e0, de, emax;
  double* Z;
  double sod;
} mcvine_dos;
/* reads 2-column ASCII (E[meV] g(E)) or IDF binary DOS (THz); applies
   MCViNE's nice_dos() (resample >=500 pts, parabolic low-E, normalize)   */
int mcvine_dos_load (mcvine_dos* d, const char* file, int ascii_THz, const char* owner);
double mcvine_dos_value (const mcvine_dos* d, double E);
/* Debye-Waller core (2W = core*Q^2, AA^2) from DOS, MCViNE DWFromDOS */
double mcvine_dw_core_from_dos (const mcvine_dos* d, double mass_amu, double T, int nsample);
double mcvine_bose (double E, double T);               /* 1/(exp(|E|/kT)-1)   */
double mcvine_phonon_bose_factor (double E, double T); /* n+1 (E>0), n (E<0) */

typedef struct {
  double pos[3];
  double mass;
  double b_coh;
  double xs_coh, xs_inc, xs_abs;
} mcvine_atom;
/* atoms file: columns x y z [AA, cartesian] mass[amu] b_coh[fm] sigma_inc[barn] sigma_abs[barn] */
int mcvine_atoms_load (mcvine_atom** atoms, const char* file, const char* owner);

typedef struct {
  int natoms, nbranches;
  int n[3];
  double b[3][3];      /* reciprocal basis vectors b1,b2,b3 (rows) [AA^-1] */
  double a[3][3];      /* dual vectors: a_i . b_j = delta_ij               */
  double* E;           /* [n1][n2][n3][nbr]                                */
  double* eps;         /* [n1][n2][n3][nbr][natoms][3][2]                  */
  double *Emin, *Emax; /* per branch                                       */
  double ucvol;        /* (2 pi)^3/|b1.(b2 x b3)|  [AA^3]                   */
  mcvine_dos m_dos;
  int has_dos;
} mcvine_dispersion;
/* read an MCViNE IDF phonon directory: Qgridinfo, Omega2, Polarizations, DOS */
int mcvine_dispersion_load_idf (mcvine_dispersion* d, const char* dir, const char* owner);
double mcvine_dispersion_energy (const mcvine_dispersion* d, int branch, const double* Q);
void mcvine_dispersion_polarization (const mcvine_dispersion* d, int branch, int atom, const double* Q, double* re, double* im);

typedef struct {
  double m_dw_core;
} mcvine_kernel_Phonon_IncoherentElastic;
int mcvine_S_Phonon_IncoherentElastic (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_dos m_dos;
  double m_dw_core;
  double m_T, m_mass;
  double m_max_omega;
  int m_focusing;
  double m_Ef, m_dEf;
} mcvine_kernel_Phonon_IncoherentInelastic;
int mcvine_S_Phonon_IncoherentInelastic (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_dispersion* m_disp;
  mcvine_atom* m_atoms;
  int m_natoms;
  double m_dw_core, m_T, m_max_omega, m_min_omega, m_xs_coh_tot;
  int m_unbiased;
} mcvine_kernel_Phonon_CoherentInelastic_PolyXtal;
int mcvine_S_Phonon_CoherentInelastic_PolyXtal (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

typedef struct {
  mcvine_dispersion* m_disp;
  mcvine_atom* m_atoms;
  int m_natoms;
  double m_dw_core, m_T, m_xs_coh_tot, m_deltaV_Jacobi;
  double m_target[3], m_target_radius; /* target_radius<=0: 4pi */
  int m_nsteps;
  double m_xacc;
  int m_unbiased;
} mcvine_kernel_Phonon_CoherentInelastic_SingleXtal;
int mcvine_S_Phonon_CoherentInelastic_SingleXtal (void* k, const double* r, double* v, double t, double* p, _class_particle* _particle);

#endif /* MCVINE_LIB_H */
