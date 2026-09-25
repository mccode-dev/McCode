/*******************************************************************************
*
*  McStas, neutron ray-tracing package
*  Copyright(C) 2007 Risoe National Laboratory.
*
* Library: share/phonon-dispersion-lib.h
*
* Written by: Peter Willendrup, port of MCViNE (J.Y.Y. Lin et al.) phonon kernel code
* Date: 23.09.2026
* Origin: DTU Physics
*
* Shared code for the Union processes CoherentPhononSingleXtal_process and
* CoherentPhononPowder_process:
*  - readers for phonon dispersions in the MCViNE IDF format (Qgridinfo, Omega2,
*    Polarizations, DOS) and MCViNE/diffpy xyz crystal structure files
*  - periodic, trilinearly interpolated phonon energies and polarizations
*    (PeriodicDispersion_3D, ChangeCoordinateSystem_forDispersion_3D and
*    LinearlyInterpolatedDispersionOnGrid_3D in MCViNE)
*  - one-phonon coherent structure factor (scattering_length.icc)
*  - Debye-Waller factor from the DOS (DWFromDOS.icc)
*  - root finding for omega(Q) = |Ei-Ef| (Omega_minus_deltaE, FindRootsEvenly, ZRidd)
*
* Usage: %include "read_table-lib" and then %include "phonon-dispersion-lib" in SHARE. Error messages are prefixed
* with the string "comp" passed by the caller.
*
*******************************************************************************/

#ifndef PHONON_DISPERSION_LIB_H
#define PHONON_DISPERSION_LIB_H

#include <ctype.h>

#define PHDISP_T2E (1.0 / 11.605)                 // Kelvin to meV, as in MCViNE
#define PHDISP_HBAR 1.05457148e-34                // J s
#define PHDISP_EV 1.60217653e-19                  // J
#define PHDISP_AMU 1.66053886e-27                 // kg
#define PHDISP_HZ2MEV (PHDISP_HBAR / (1e-3 * PHDISP_EV)) // angular frequency [rad/s] to meV
#define PHDISP_MAXATOMS 1024

// Phonon dispersion on a grid + unit cell content
struct phdisp_struct {
  // grid dispersion
  int n_atoms;
  int n_branches;
  int n[3];          // grid points along b1, b2, b3
  double b[3][3];    // reciprocal cell of the grid [AA^-1], b[i] is vector b_i
  double binv[3][3]; // binv[i] . b[j] = delta_ij, gives fractional coordinates
  double* E;         // [n1][n2][n3][branch] phonon energy [meV]
  double* pol;       // [n1][n2][n3][branch][atom][xyz][re/im]
  double* Emin;      // per branch
  double* Emax;      // per branch
  double Emax_all;   // maximum phonon energy
  // unit cell
  double uc_vol;     // [AA^3]
  double* pos;       // [atom][xyz] cartesian [AA]
  double* bc;        // coherent scattering length [fm]
  double* m;         // mass [amu]
  double sigma_coh;  // sum of coherent cross sections in unit cell [barn]
  double avg_mass;   // [amu]
};

// Context for the function omega(Q(vf)) - |Ei - Ef|
struct phdisp_omega_ctx {
  struct phdisp_struct* s;
  int branch;
  double vf_dir[3];
  double vi[3];
  double vi_l;
  int powder;     // 1: Q = |ki-kf| * qhat (orientational average of a powder)
  double qhat[3]; // unit vector in the crystal frame, used when powder=1
};

// Qgridinfo expression evaluator
#define PHDISP_MAXVARS 64
struct phdisp_val {
  int n;
  double v[3];
};

struct phdisp_parser {
  const char* p;
  int nvars;
  char names[PHDISP_MAXVARS][64];
  struct phdisp_val vals[PHDISP_MAXVARS];
  int error;
};

// public functions
void phdisp_read_dispersion (const char* dir, struct phdisp_struct* s, const char* comp);
void phdisp_read_xyz (const char* file, double b_default, double m_default, struct phdisp_struct* s, const char* comp);
void phdisp_cell_sums (struct phdisp_struct* s);
double phdisp_dw_core_from_dos (const char* dir, const char* file, double avg_mass, double T, const char* comp);
double phdisp_energy (struct phdisp_struct* s, int branch, double* Q);
void phdisp_polarization (struct phdisp_struct* s, int branch, int atom, double* Q, double* re, double* im);
double phdisp_structure_factor (struct phdisp_struct* s, int branch, double* Q);
double phdisp_omega_minus_dE (double vf, struct phdisp_omega_ctx* c);
int phdisp_find_roots (struct phdisp_omega_ctx* c, double x1, double x2, int nsteps, double xacc, double* roots);
double phdisp_bose (double omega, double T);
void phdisp_free (struct phdisp_struct* s);

#endif // PHONON_DISPERSION_LIB_H
