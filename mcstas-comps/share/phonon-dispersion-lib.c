/*******************************************************************************
*
*  McStas, neutron ray-tracing package
*  Copyright(C) 2007 Risoe National Laboratory.
*
* Library: share/phonon-dispersion-lib.c
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
#include "phonon-dispersion-lib.h"
#endif

#ifndef PHONON_DISPERSION_LIB_C
#define PHONON_DISPERSION_LIB_C

// ---------------------------------------------------------------------------
// Grid lookup with periodic folding (PeriodicDispersion_3D +
// ChangeCoordinateSystem + LinearlyInterpolatedDispersionOnGrid_3D in MCViNE)
// ---------------------------------------------------------------------------
void
phdisp_grid_coords (struct phdisp_struct* s, double* Q, int* i0, double* r) {
  int d;
  for (d = 0; d < 3; d++) {
    double f = s->binv[d][0] * Q[0] + s->binv[d][1] * Q[1] + s->binv[d][2] * Q[2];
    f -= floor (f);
    if (f >= 1.0 || f < 0.0)
      f = 0.0;
    double x = f * (s->n[d] - 1);
    int i = (int)floor (x);
    double rem = x - i;
    if (i >= s->n[d] - 1) {
      i = s->n[d] - 2;
      rem = 1.0;
    }
    i0[d] = i;
    r[d] = rem;
  }
}

// trilinear interpolation of a quantity stored at data[q*stride + offset]
double
phdisp_interp (struct phdisp_struct* s, double* data, long stride, long offset, int* i0, double* r) {
  double res = 0;
  int dx, dy, dz;
  for (dx = 0; dx < 2; dx++)
    for (dy = 0; dy < 2; dy++)
      for (dz = 0; dz < 2; dz++) {
        double w = (dx ? r[0] : 1 - r[0]) * (dy ? r[1] : 1 - r[1]) * (dz ? r[2] : 1 - r[2]);
        if (w == 0)
          continue;
        long q = ((long)(i0[0] + dx) * s->n[1] + (i0[1] + dy)) * s->n[2] + (i0[2] + dz);
        res += w * data[q * stride + offset];
      }
  return res;
}

double
phdisp_energy (struct phdisp_struct* s, int branch, double* Q) {
  int i0[3];
  double r[3];
  phdisp_grid_coords (s, Q, i0, r);
  return phdisp_interp (s, s->E, s->n_branches, branch, i0, r);
}

void
phdisp_polarization (struct phdisp_struct* s, int branch, int atom, double* Q, double* re, double* im) {
  int i0[3], d;
  double r[3];
  long stride = (long)s->n_branches * s->n_atoms * 6;
  long base = ((long)branch * s->n_atoms + atom) * 6;
  phdisp_grid_coords (s, Q, i0, r);
  for (d = 0; d < 3; d++) {
    re[d] = phdisp_interp (s, s->pol, stride, base + 2 * d, i0, r);
    im[d] = phdisp_interp (s, s->pol, stride, base + 2 * d + 1, i0, r);
  }
}

// omega(Q) - |Ei-Ef| as a function of the final speed (Omega_minus_deltaE.cc)
double
phdisp_omega_minus_dE (double vf, struct phdisp_omega_ctx* c) {
  double Q[3];
  int d;
  for (d = 0; d < 3; d++)
    Q[d] = V2K * (c->vi[d] - vf * c->vf_dir[d]);
  if (c->powder) {
    double ql = sqrt (Q[0] * Q[0] + Q[1] * Q[1] + Q[2] * Q[2]);
    for (d = 0; d < 3; d++)
      Q[d] = ql * c->qhat[d];
  }
  return phdisp_energy (c->s, c->branch, Q) - VS2E * fabs (c->vi_l * c->vi_l - vf * vf);
}

// Ridder's method (mccomponents/math/rootfinding.cc). Returns 1 on success.
// fl, fh: function values at x1, x2 (already evaluated by the caller)
int
phdisp_zridd (struct phdisp_omega_ctx* c, double x1, double x2, double fl, double fh, double xacc, double* root) {
  const double UNUSED = -1.11e30;
  double ans, fm, fnew, s, xh, xl, xm, xnew, tmp;
  int j;
  if (fl * fh >= 0) {
    if (fl == 0) {
      *root = x1;
      return 1;
    }
    if (fh == 0) {
      *root = x2;
      return 1;
    }
    return 0;
  }
  int converged = 0;
  xl = x1;
  xh = x2;
  ans = UNUSED;
  for (j = 1; j < 60 && !converged; j++) {
    xm = 0.5 * (xl + xh);
    fm = phdisp_omega_minus_dE (xm, c);
    tmp = fm * fm - fh * fl;
    if (tmp >= 0) {
      s = sqrt (tmp);
      if (s == 0.0) {
        converged = 1;
        break;
      }
      xnew = xm + (xm - xl) * ((fl >= fh ? 1.0 : -1.0) * fm / s);
    } else {
      xnew = xm;
    }
    if (fabs (xnew - ans) <= xacc) {
      converged = 1;
      break;
    }
    ans = xnew;
    fnew = phdisp_omega_minus_dE (ans, c);
    if (fnew == 0.0) {
      converged = 1;
      break;
    }
    if ((fnew >= 0 ? fabs (fm) : -fabs (fm)) != fm) {
      xl = xm;
      fl = fm;
      xh = ans;
      fh = fnew;
    } else if ((fnew >= 0 ? fabs (fl) : -fabs (fl)) != fl) {
      xh = ans;
      fh = fnew;
    } else if ((fnew >= 0 ? fabs (fh) : -fabs (fh)) != fh) {
      xl = ans;
      fl = fnew;
    } else {
      return 0;
    }
    if (fabs (xh - xl) <= xacc)
      converged = 1;
  }
  if (!converged || ans == UNUSED)
    return 0;
  *root = ans;
  return 1;
}

// FindRootsEvenly: search roots in nsteps equal intervals of [x1,x2].
// The function is evaluated once per interval boundary and shared by neighbouring intervals.
int
phdisp_find_roots (struct phdisp_omega_ctx* c, double x1, double x2, int nsteps, double xacc, double* roots) {
  int i, nf = 0;
  double step = (x2 - x1) / nsteps, root;
  double fl = phdisp_omega_minus_dE (x1, c), fh;
  for (i = 0; i < nsteps; i++) {
    fh = phdisp_omega_minus_dE (x1 + step * (i + 1), c);
    if (phdisp_zridd (c, x1 + step * i, x1 + step * (i + 1), fl, fh, xacc, &root))
      roots[nf++] = root;
    fl = fh;
  }
  return nf;
}

// Bose factor: n+1 for phonon creation (omega>0), n for annihilation
double
phdisp_bose (double omega, double T) {
  if (omega == 0.0)
    return 1.0;
  double n = 1.0 / (exp (fabs (omega) / (T * PHDISP_T2E)) - 1.0);
  return omega > 0 ? 1.0 + n : n;
}

// ---------------------------------------------------------------------------
// File readers
// ---------------------------------------------------------------------------
FILE*
phdisp_open (const char* dir, const char* name, const char* mode, const char* comp) {
  char path[1024];
  FILE* fp;
  if (dir && strlen (dir))
    snprintf (path, 1024, "%s%c%s", dir, MC_PATHSEP_C, name);
  else
    snprintf (path, 1024, "%s", name);
  fp = Open_File (path, mode, NULL);
  if (!fp) {
    fprintf (stderr, "%s: ERROR: cannot open file %s\n", comp, path);
    exit (-1);
  }
  return fp;
}

// IDF header: 64 char filetype, int version, 1024 char comment
void
phdisp_idf_header (FILE* fp, const char* expected, const char* comp) {
  char filetype[65];
  char comment[1024];
  int version;
  if (fread (filetype, 1, 64, fp) != 64 || fread (&version, sizeof (int), 1, fp) != 1 || fread (comment, 1, 1024, fp) != 1024) {
    fprintf (stderr, "%s: ERROR: could not read IDF header of %s file\n", comp, expected);
    exit (-1);
  }
  filetype[64] = '\0';
  if (strncmp (filetype, expected, strlen (expected))) {
    fprintf (stderr, "%s: ERROR: file type is '%s', expected '%s'\n", comp, filetype, expected);
    exit (-1);
  }
}

// --- tiny evaluator for the python-like Qgridinfo file ---

void
phdisp_skipws (struct phdisp_parser* P) {
  while (*P->p == ' ' || *P->p == '\t' || *P->p == '\r')
    P->p++;
}

struct phdisp_val phdisp_parse_list (struct phdisp_parser* P);
struct phdisp_val phdisp_parse_expr (struct phdisp_parser* P);

struct phdisp_val
phdisp_scalar (double x) {
  struct phdisp_val r;
  r.n = 1;
  r.v[0] = x;
  r.v[1] = r.v[2] = 0;
  return r;
}

struct phdisp_val
phdisp_binop (struct phdisp_parser* P, struct phdisp_val a, struct phdisp_val b, char op) {
  struct phdisp_val r;
  int i, n = a.n > b.n ? a.n : b.n;
  if (a.n != b.n && a.n != 1 && b.n != 1) {
    P->error = 1;
    return a;
  }
  r.n = n;
  for (i = 0; i < 3; i++) {
    double x = a.v[a.n == 1 ? 0 : i], y = b.v[b.n == 1 ? 0 : i];
    switch (op) {
    case '+':
      r.v[i] = x + y;
      break;
    case '-':
      r.v[i] = x - y;
      break;
    case '*':
      r.v[i] = x * y;
      break;
    case '/':
      r.v[i] = x / y;
      break;
    case '^':
      r.v[i] = pow (x, y);
      break;
    }
  }
  return r;
}

struct phdisp_val
phdisp_parse_primary (struct phdisp_parser* P) {
  struct phdisp_val r = phdisp_scalar (0);
  phdisp_skipws (P);
  if (*P->p == '(' || *P->p == '[') {
    char close = (*P->p == '(') ? ')' : ']';
    P->p++;
    r = phdisp_parse_list (P);
    phdisp_skipws (P);
    if (*P->p != close)
      P->error = 1;
    else
      P->p++;
    return r;
  }
  if (isdigit (*P->p) || *P->p == '.') {
    char* end;
    r = phdisp_scalar (strtod (P->p, &end));
    P->p = end;
    return r;
  }
  if (isalpha (*P->p) || *P->p == '_') {
    char name[64];
    int len = 0, i;
    while ((isalnum (*P->p) || *P->p == '_' || *P->p == '.') && len < 63)
      name[len++] = *P->p++;
    name[len] = '\0';
    const char* fname = strncmp (name, "math.", 5) == 0 ? name + 5 : name;
    phdisp_skipws (P);
    if (*P->p == '(') { // function call
      P->p++;
      struct phdisp_val a = phdisp_parse_expr (P);
      phdisp_skipws (P);
      if (*P->p == ')')
        P->p++;
      else
        P->error = 1;
      double x = a.v[0];
      if (!strcmp (fname, "sqrt"))
        return phdisp_scalar (sqrt (x));
      if (!strcmp (fname, "sin"))
        return phdisp_scalar (sin (x));
      if (!strcmp (fname, "cos"))
        return phdisp_scalar (cos (x));
      if (!strcmp (fname, "tan"))
        return phdisp_scalar (tan (x));
      if (!strcmp (fname, "abs") || !strcmp (fname, "fabs"))
        return phdisp_scalar (fabs (x));
      if (!strcmp (fname, "float"))
        return phdisp_scalar (x);
      P->error = 1;
      return a;
    }
    if (!strcmp (fname, "pi"))
      return phdisp_scalar (PI);
    for (i = P->nvars - 1; i >= 0; i--)
      if (!strcmp (P->names[i], name))
        return P->vals[i];
    fprintf (stderr, "phonon-dispersion-lib: Qgridinfo: unknown name '%s'\n", name);
    P->error = 1;
    return r;
  }
  P->error = 1;
  return r;
}

struct phdisp_val phdisp_parse_unary (struct phdisp_parser* P);

struct phdisp_val
phdisp_parse_power (struct phdisp_parser* P) {
  struct phdisp_val a = phdisp_parse_primary (P);
  phdisp_skipws (P);
  if (P->p[0] == '*' && P->p[1] == '*') {
    P->p += 2;
    a = phdisp_binop (P, a, phdisp_parse_unary (P), '^');
  }
  return a;
}

struct phdisp_val
phdisp_parse_unary (struct phdisp_parser* P) {
  phdisp_skipws (P);
  if (*P->p == '-') {
    P->p++;
    return phdisp_binop (P, phdisp_scalar (-1), phdisp_parse_unary (P), '*');
  }
  if (*P->p == '+') {
    P->p++;
    return phdisp_parse_unary (P);
  }
  return phdisp_parse_power (P);
}

struct phdisp_val
phdisp_parse_term (struct phdisp_parser* P) {
  struct phdisp_val a = phdisp_parse_unary (P);
  for (;;) {
    phdisp_skipws (P);
    if ((*P->p == '*' && P->p[1] != '*') || *P->p == '/') {
      char op = *P->p++;
      a = phdisp_binop (P, a, phdisp_parse_unary (P), op);
    } else
      return a;
  }
}

struct phdisp_val
phdisp_parse_expr (struct phdisp_parser* P) {
  struct phdisp_val a = phdisp_parse_term (P);
  for (;;) {
    phdisp_skipws (P);
    if (*P->p == '+' || *P->p == '-') {
      char op = *P->p++;
      a = phdisp_binop (P, a, phdisp_parse_term (P), op);
    } else
      return a;
  }
}

// comma separated list of scalars -> vector (up to 3 components)
struct phdisp_val
phdisp_parse_list (struct phdisp_parser* P) {
  struct phdisp_val r, a;
  r.n = 0;
  r.v[0] = r.v[1] = r.v[2] = 0;
  for (;;) {
    phdisp_skipws (P);
    if (*P->p == '\0' || *P->p == '\n' || *P->p == ')' || *P->p == ']')
      break;
    a = phdisp_parse_expr (P);
    if (a.n == 1 && r.n < 3)
      r.v[r.n++] = a.v[0];
    else if (r.n == 0)
      r = a;
    else
      P->error = 1;
    phdisp_skipws (P);
    if (*P->p == ',')
      P->p++;
    else
      break;
  }
  return r;
}

void
phdisp_read_qgridinfo (const char* dir, struct phdisp_struct* s, const char* comp) {
  FILE* fp = phdisp_open (dir, "Qgridinfo", "r", comp);
  struct phdisp_parser* P = calloc (1, sizeof (struct phdisp_parser));
  char line[4096];
  int lineno = 0, i;
  while (fgets (line, sizeof (line), fp)) {
    char *c, *targets[16];
    int ntargets = 0;
    lineno++;
    if ((c = strchr (line, '#')))
      *c = '\0';
    c = line;
    while (*c == ' ' || *c == '\t')
      c++;
    if (*c == '\0' || *c == '\n' || *c == '\r' || !strncmp (c, "import ", 7) || !strncmp (c, "from ", 5))
      continue;
    // split "a = b = expr" on '='
    char* start = c;
    while ((c = strchr (start, '=')) && ntargets < 16) {
      *c = '\0';
      targets[ntargets++] = start;
      start = c + 1;
    }
    if (ntargets == 0) {
      fprintf (stderr, "%s: Qgridinfo line %d ignored: %s", comp, lineno, line);
      continue;
    }
    P->p = start;
    P->error = 0;
    struct phdisp_val v = phdisp_parse_list (P);
    if (P->error) {
      fprintf (stderr, "%s: ERROR: could not parse Qgridinfo line %d\n", comp, lineno);
      exit (-1);
    }
    for (i = 0; i < ntargets; i++) {
      char name[64];
      if (sscanf (targets[i], " %63[A-Za-z0-9_]", name) != 1)
        continue;
      if (P->nvars < PHDISP_MAXVARS) {
        strcpy (P->names[P->nvars], name);
        P->vals[P->nvars++] = v;
      }
    }
  }
  fclose (fp);
  const char* bn[3] = { "b1", "b2", "b3" };
  const char* nn[3] = { "n1", "n2", "n3" };
  int d, found;
  for (d = 0; d < 3; d++) {
    found = 0;
    for (i = P->nvars - 1; i >= 0 && !found; i--)
      if (!strcmp (P->names[i], bn[d]) && P->vals[i].n == 3) {
        s->b[d][0] = P->vals[i].v[0];
        s->b[d][1] = P->vals[i].v[1];
        s->b[d][2] = P->vals[i].v[2];
        found = 1;
      }
    if (!found) {
      fprintf (stderr, "%s: ERROR: %s (3-vector) not defined in Qgridinfo\n", comp, bn[d]);
      exit (-1);
    }
    found = 0;
    for (i = P->nvars - 1; i >= 0 && !found; i--)
      if (!strcmp (P->names[i], nn[d]) && P->vals[i].n == 1) {
        s->n[d] = (int)floor (P->vals[i].v[0] + 0.5);
        found = 1;
      }
    if (!found || s->n[d] < 2) {
      fprintf (stderr, "%s: ERROR: %s (>=2) not defined in Qgridinfo\n", comp, nn[d]);
      exit (-1);
    }
  }
  free (P);
}

// x_star = (y x z)/vol etc. (mcni::get_inversions)
double
phdisp_inversions (double a[3][3], double inv[3][3]) {
  int i, j;
  double c[3][3];
  for (i = 0; i < 3; i++) {
    int i1 = (i + 1) % 3, i2 = (i + 2) % 3;
    c[i][0] = a[i1][1] * a[i2][2] - a[i1][2] * a[i2][1];
    c[i][1] = a[i1][2] * a[i2][0] - a[i1][0] * a[i2][2];
    c[i][2] = a[i1][0] * a[i2][1] - a[i1][1] * a[i2][0];
  }
  double vol = a[0][0] * c[0][0] + a[0][1] * c[0][1] + a[0][2] * c[0][2];
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      inv[i][j] = c[i][j] / vol;
  return vol;
}

void
phdisp_read_dispersion (const char* dir, struct phdisp_struct* s, const char* comp) {
  FILE* fp;
  int D, Nb, Nq, br, q;
  long nE, nP, i;

  phdisp_read_qgridinfo (dir, s, comp);
  if (fabs (phdisp_inversions (s->b, s->binv)) < 1e-12) {
    fprintf (stderr, "%s: ERROR: b1, b2, b3 in Qgridinfo are not linearly independent\n", comp);
    exit (-1);
  }

  // Omega2: D, N_b (atoms), N_q, then N_q x (N_b*D) doubles
  fp = phdisp_open (dir, "Omega2", "rb", comp);
  phdisp_idf_header (fp, "Omega2", comp);
  if (fread (&D, sizeof (int), 1, fp) != 1 || fread (&Nb, sizeof (int), 1, fp) != 1 || fread (&Nq, sizeof (int), 1, fp) != 1) {
    fprintf (stderr, "%s: ERROR: corrupt Omega2 file\n", comp);
    exit (-1);
  }
  if (D != 3 || Nq != s->n[0] * s->n[1] * s->n[2] || Nb < 1 || Nb > PHDISP_MAXATOMS) {
    fprintf (stderr, "%s: ERROR: Omega2 has D=%d, N_atoms=%d, N_q=%d, but Qgridinfo gives %d x %d x %d points\n", comp, D,
             Nb, Nq, s->n[0], s->n[1], s->n[2]);
    exit (-1);
  }
  s->n_atoms = Nb;
  s->n_branches = 3 * Nb;
  nE = (long)Nq * s->n_branches;
  s->E = malloc (nE * sizeof (double));
  if (fread (s->E, sizeof (double), nE, fp) != (size_t)nE) {
    fprintf (stderr, "%s: ERROR: Omega2 file too short\n", comp);
    exit (-1);
  }
  fclose (fp);
  for (i = 0; i < nE; i++)
    s->E[i] = (s->E[i] < 0 ? 0 : sqrt (s->E[i])) * PHDISP_HZ2MEV;

  s->Emin = malloc (s->n_branches * sizeof (double));
  s->Emax = malloc (s->n_branches * sizeof (double));
  for (br = 0; br < s->n_branches; br++) {
    s->Emin[br] = 1e300;
    s->Emax[br] = -1e300;
    for (q = 0; q < Nq; q++) {
      double e = s->E[(long)q * s->n_branches + br];
      if (e < s->Emin[br])
        s->Emin[br] = e;
      if (e > s->Emax[br])
        s->Emax[br] = e;
    }
    if (br == 0 || s->Emax[br] > s->Emax_all)
      s->Emax_all = s->Emax[br];
  }

  // Polarizations: D, N_b, N_q, then N_q x (N_b*D) x N_b x D x 2 doubles
  fp = phdisp_open (dir, "Polarizations", "rb", comp);
  phdisp_idf_header (fp, "Polarizations", comp);
  int D2, Nb2, Nq2;
  if (fread (&D2, sizeof (int), 1, fp) != 1 || fread (&Nb2, sizeof (int), 1, fp) != 1 || fread (&Nq2, sizeof (int), 1, fp) != 1 || D2 != D || Nb2 != Nb
      || Nq2 != Nq) {
    fprintf (stderr, "%s: ERROR: Polarizations file inconsistent with Omega2 file\n", comp);
    exit (-1);
  }
  nP = (long)Nq * s->n_branches * Nb * 3 * 2;
  s->pol = malloc (nP * sizeof (double));
  if (fread (s->pol, sizeof (double), nP, fp) != (size_t)nP) {
    fprintf (stderr, "%s: ERROR: Polarizations file too short\n", comp);
    exit (-1);
  }
  fclose (fp);
}

void
phdisp_read_xyz (const char* file, double b_default, double m_default, struct phdisp_struct* s, const char* comp) {
  FILE* fp = phdisp_open (NULL, file, "r", comp);
  char line[4096];
  double lat[3][3], inv[3][3];
  int natoms = -1, stage = 0, ia = 0, d;
  while (fgets (line, sizeof (line), fp)) {
    char* c = line;
    while (*c == ' ' || *c == '\t')
      c++;
    if (*c == '\0' || *c == '\n' || *c == '\r' || *c == '#')
      continue;
    if (stage == 0) {
      natoms = atoi (c);
      if (natoms != s->n_atoms) {
        fprintf (stderr, "%s: ERROR: %s has %d atoms, but the dispersion has %d\n", comp, file, natoms, s->n_atoms);
        exit (-1);
      }
      s->pos = calloc (3 * natoms, sizeof (double));
      s->bc = calloc (natoms, sizeof (double));
      s->m = calloc (natoms, sizeof (double));
      stage = 1;
    } else if (stage == 1) {
      if (sscanf (c, "%lf %lf %lf %lf %lf %lf %lf %lf %lf", &lat[0][0], &lat[0][1], &lat[0][2], &lat[1][0], &lat[1][1], &lat[1][2], &lat[2][0], &lat[2][1],
                  &lat[2][2])
          != 9) {
        fprintf (stderr, "%s: ERROR: line 2 of %s must contain the 9 components of the lattice vectors\n", comp, file);
        exit (-1);
      }
      stage = 2;
    } else if (ia < natoms) {
      char sym[64];
      double f[3], extra[3];
      int nread = sscanf (c, "%63s %lf %lf %lf %lf %lf %lf", sym, &f[0], &f[1], &f[2], &extra[0], &extra[1], &extra[2]);
      if (nread < 4) {
        fprintf (stderr, "%s: ERROR: could not parse atom line in %s: %s", comp, file, line);
        exit (-1);
      }
      for (d = 0; d < 3; d++)
        s->pos[3 * ia + d] = f[0] * lat[0][d] + f[1] * lat[1][d] + f[2] * lat[2][d];
      if (nread >= 6) { // Symbol x y z b_coh mass
        s->bc[ia] = extra[0];
        s->m[ia] = extra[1];
      } else {
        s->bc[ia] = b_default;
        s->m[ia] = m_default;
      }
      if (s->bc[ia] < 0 || s->m[ia] <= 0) {
        fprintf (stderr,
                 "%s: ERROR: atom %d (%s) has no scattering length / mass. Give them as columns 5 and 6 in %s or set b_coh and "
                 "mass\n",
                 comp, ia, sym, file);
        exit (-1);
      }
      ia++;
    }
  }
  fclose (fp);
  if (ia != natoms) {
    fprintf (stderr, "%s: ERROR: expected %d atoms in %s, read %d\n", comp, natoms, file, ia);
    exit (-1);
  }
  s->uc_vol = fabs (phdisp_inversions (lat, inv));
}

// DOS (IDF): N_bins, dE [THz], then N_bins doubles. Returns 2W/Q^2 [AA^2] (DWFromDOS.icc)
double
phdisp_dw_core_from_dos (const char* dir, const char* file, double avg_mass, double T, const char* comp) {
  FILE* fp = phdisp_open (dir, file, "rb", comp);
  int nb, i, nSample = 100, first = -1;
  double dE, *Z, area = 0;
  phdisp_idf_header (fp, "DOS", comp);
  if (fread (&nb, sizeof (int), 1, fp) != 1 || fread (&dE, sizeof (double), 1, fp) != 1 || nb < 2) {
    fprintf (stderr, "%s: ERROR: corrupt DOS file\n", comp);
    exit (-1);
  }
  Z = malloc (nb * sizeof (double));
  if (fread (Z, sizeof (double), nb, fp) != (size_t)nb) {
    fprintf (stderr, "%s: ERROR: DOS file too short\n", comp);
    exit (-1);
  }
  fclose (fp);
  dE *= 2 * PI * 1e12 * PHDISP_HZ2MEV; // THz (not angular) -> meV
  for (i = 0; i < nb; i++)
    area += Z[i];
  area *= dE;
  for (i = 0; i < nb; i++)
    Z[i] /= area;
  double emin = 0, emax = dE * (nb - 1);
  double dw = (emax - emin) / (nSample - 1 + .00000001), core = 0;
  double* f = calloc (nSample, sizeof (double));
  for (i = 0; i < nSample; i++) {
    double w = dw * i + emin, z = 0;
    // linearly interpolated DOS, zero outside [emin, emax)
    if (w >= emin && w < emax) {
      double x = (w - emin) / dE;
      int j = (int)floor (x);
      if (j >= nb - 1) {
        j = nb - 2;
      }
      z = Z[j] + (x - j) * (Z[j + 1] - Z[j]);
    }
    if (w < emax / nSample / 100.)
      continue;
    if (first == -1)
      first = i;
    f[i] = (2 / (exp (w / (T * PHDISP_T2E)) - 1) + 1) / w * z;
    core += f[i] * (i == nSample - 1 ? 0.5 : 1);
  }
  if (first == -1 || first + 1 >= nSample) {
    fprintf (stderr, "%s: ERROR: invalid DOS (no data for E>0)\n", comp);
    exit (-1);
  }
  double f0 = f[first] - (dw * first + emin) * (f[first + 1] - f[first]) / dw;
  core += f0 / 2;
  core /= PHDISP_EV * 1e-3;
  core *= dw;
  core *= PHDISP_HBAR * PHDISP_HBAR / 2 / PHDISP_AMU / avg_mass;
  core *= 1e20;
  free (f);
  free (Z);
  return core;
}

// one-phonon coherent structure factor |sum_d b_d/sqrt(M_d) exp(iQ.d) (Q.e_d)|^2, normalised by the
// total coherent cross section as in MCViNE (scattering_length.icc + kernels):
// returns |..|^2 [fm^2 AA^-2 amu^-1] / 1e30 / (sigma_coh*1e-28)
double
phdisp_structure_factor (struct phdisp_struct* s, int branch, double* Q) {
  double sre = 0, sim = 0;
  int a;
  for (a = 0; a < s->n_atoms; a++) {
    double ere[3], eim[3];
    phdisp_polarization (s, branch, a, Q, ere, eim);
    double epslen = sqrt (ere[0] * ere[0] + ere[1] * ere[1] + ere[2] * ere[2] + eim[0] * eim[0] + eim[1] * eim[1] + eim[2] * eim[2]);
    if (epslen <= 0)
      continue;
    double qe_re = (Q[0] * ere[0] + Q[1] * ere[1] + Q[2] * ere[2]) / epslen;
    double qe_im = (Q[0] * eim[0] + Q[1] * eim[1] + Q[2] * eim[2]) / epslen;
    double qd = Q[0] * s->pos[3 * a] + Q[1] * s->pos[3 * a + 1] + Q[2] * s->pos[3 * a + 2];
    double pre = s->bc[a] / sqrt (s->m[a]);
    double cr = cos (qd), ci = sin (qd);
    sre += pre * (cr * qe_re - ci * qe_im);
    sim += pre * (cr * qe_im + ci * qe_re);
  }
  return (sre * sre + sim * sim) / 1e30 / (s->sigma_coh * 1e-28);
}

// sum of coherent cross sections and average mass of the unit cell
void
phdisp_cell_sums (struct phdisp_struct* s) {
  int ia;
  s->sigma_coh = 0;
  s->avg_mass = 0;
  for (ia = 0; ia < s->n_atoms; ia++) {
    s->sigma_coh += 4 * PI * s->bc[ia] * s->bc[ia] / 100.0; // fm^2 -> barn
    s->avg_mass += s->m[ia];
  }
  s->avg_mass /= s->n_atoms;
}

void
phdisp_free (struct phdisp_struct* s) {
  free (s->E);
  free (s->pol);
  free (s->Emin);
  free (s->Emax);
  free (s->pos);
  free (s->bc);
  free (s->m);
}

#endif // PHONON_DISPERSION_LIB_C
