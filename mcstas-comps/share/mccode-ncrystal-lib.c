/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2026, All rights reserved
*         DTU Physics, Kgs. Lyngby, Denmark
*
* Library: share/mccode-ncrystal-lib.c
*
* %Identification
* Written by: Thomas Kittelmann
* Date: 2026
* Origin: ESS
*
* Common functionality for McStas components using NCrystal. See
* mccode-ncrystal-lib.h.
*
*******************************************************************************/

#ifndef MCCODE_NCRYSTAL_LIB_H
#include "mccode-ncrystal-lib.h"
#endif

/* Same default as in read_table-lib.h: */
#ifndef MCSTAS
#ifdef WIN32
#define MCSTAS "C:\\mcstas\\lib"
#else
#define MCSTAS "/usr/local/lib/mcstas"
#endif
#endif

/* Absolute path of dir, with "." and ".." components resolved, and without
   repeated or trailing path separators (used to detect directories added more
   than once, and for printing). Returns 0 on failure. */
static int
mccode_ncrystal_normalise_dir (const char* dir, char* out, size_t out_size) {
  char tmp[2048];
  char cwd[1024];
  const char* c = tmp;
  size_t n = 0, len;
  if (dir[0] == '/' || dir[0] == MC_PATHSEP_C || (dir[0] && dir[1] == ':')) {
    snprintf (tmp, sizeof (tmp), "%s", dir);
  } else {
    if (!getcwd (cwd, sizeof (cwd)))
      return 0;
    snprintf (tmp, sizeof (tmp), "%s%c%s", cwd, MC_PATHSEP_C, dir);
  }
  /* Keep a Windows drive letter ("C:"), then add the components one by one: */
  if (tmp[0] && tmp[1] == ':') {
    out[n++] = tmp[0];
    out[n++] = ':';
    c += 2;
  }
  while (*c) {
    while (*c == '/' || *c == MC_PATHSEP_C)
      ++c;
    for (len = 0; c[len] && c[len] != '/' && c[len] != MC_PATHSEP_C; ++len)
      ;
    if (len == 0 || (len == 1 && c[0] == '.')) {
      /* nothing to add */
    } else if (len == 2 && c[0] == '.' && c[1] == '.') {
      while (n > 0 && out[n - 1] != MC_PATHSEP_C)
        --n; /* remove the last component */
      if (n > 0 && !(n == 1 || (n == 3 && out[1] == ':')))
        --n; /* and its separator (except for the root directory) */
    } else {
      if (n + len + 2 >= out_size)
        return 0;
      if (n == 0 || out[n - 1] != MC_PATHSEP_C)
        out[n++] = MC_PATHSEP_C;
      memcpy (out + n, c, len);
      n += len;
    }
    c += len;
  }
  if (n == 0 || (n == 2 && out[1] == ':'))
    out[n++] = MC_PATHSEP_C; /* root directory */
  out[n] = '\0';
  return 1;
}

/* Add an existing directory to the NCrystal search path, unless already added: */
static void
mccode_ncrystal_add_search_dir (const char* dir, char* added, size_t added_size) {
  static char done[4][1024];
  static int ndone = 0;
  char norm[1024];
  int i;
  DIR* handle = opendir (dir);
  if (!handle)
    return;
  closedir (handle);
  if (!mccode_ncrystal_normalise_dir (dir, norm, sizeof (norm)))
    return;
  for (i = 0; i < ndone; ++i)
    if (!strcmp (done[i], norm))
      return;
  if (ndone < 4)
    strcpy (done[ndone++], norm);
  ncrystal_add_custom_search_dir (dir);
  if (strlen (added) + strlen (norm) + 3 < added_size) {
    if (added[0])
      strcat (added, ", ");
    strcat (added, norm);
  }
}

/* Add the directory part of a file path (up to and including the last path
   separator), in the same way as Open_File() does it: */
static void
mccode_ncrystal_add_dir_of (const char* file, char* added, size_t added_size) {
  char dir[1024];
  const char* path_pos;
  size_t path_length;
  if (!file || file[0] == '\0')
    return;
  path_pos = strrchr (file, MC_PATHSEP_C);
  if (!path_pos)
    return;
  path_length = path_pos + 1 - file;
  if (path_length >= sizeof (dir))
    return;
  strncpy (dir, file, path_length);
  dir[path_length] = '\0';
  mccode_ncrystal_add_search_dir (dir, added, added_size);
}

void
mccode_setup_ncrystal_search_paths (void) {
  /* NB: This must add the same directories, in the same order, as searched by
     Open_File() in read_table-lib.c. Please keep the two in sync. */
  static int done = 0;
  char dir[1024];
  char added[4096];
  const char* mcstas_dir;

  if (done)
    return;
  done = 1;
  added[0] = '\0';

  mccode_ncrystal_add_dir_of (instrument_source, added, sizeof (added));
  mccode_ncrystal_add_dir_of (instrument_exe, added, sizeof (added));

  mcstas_dir = getenv ("MCSTAS") ? getenv ("MCSTAS") : MCSTAS;
  snprintf (dir, sizeof (dir), "%s%c%s", mcstas_dir, MC_PATHSEP_C, "data");
  mccode_ncrystal_add_search_dir (dir, added, sizeof (added));
  snprintf (dir, sizeof (dir), "%s%c%s", mcstas_dir, MC_PATHSEP_C, "contrib");
  mccode_ncrystal_add_search_dir (dir, added, sizeof (added));

  if (added[0])
    MPI_MASTER (printf ("NCrystal: Added to the NCrystal search path: %s\n", added););
}

#ifndef ncrystal_filtertable
/* For versions of NCrystal without ncrystal_filtertable: a dense table, with
   log-spaced wavelengths and two points at each Bragg edge (with the cross
   sections just below and above it). */
typedef struct {
  double wl, wl_eval;
} mccode_ncrystal_xspoint_t;

typedef struct {
  mccode_ncrystal_xspoint_t* points;
  unsigned n, capacity;
} mccode_ncrystal_xspoints_t;

static void
mccode_ncrystal_addxspoint (mccode_ncrystal_xspoints_t* pts, double wl, double wl_eval) {
  if (pts->n == pts->capacity) {
    pts->capacity = 2 * pts->capacity + 1024;
    pts->points = realloc (pts->points, pts->capacity * sizeof (mccode_ncrystal_xspoint_t));
    if (!pts->points) {
      fprintf (stderr, "NCrystal: ERROR: memory allocation failed\n");
      exit (-1);
    }
  }
  pts->points[pts->n].wl = wl;
  pts->points[pts->n].wl_eval = wl_eval;
  ++pts->n;
}

static void
mccode_ncrystal_addbraggedges (mccode_ncrystal_xspoints_t* pts, ncrystal_info_t info) {
  int i, h, k, l, mult;
  double fraction, dspacing, fsquared;
  ncrystal_info_t phase;
  if (ncrystal_info_nphases (info) > 0) {
    for (i = 0; i < ncrystal_info_nphases (info); ++i) {
      phase = ncrystal_info_getphase (info, i, &fraction);
      mccode_ncrystal_addbraggedges (pts, phase);
      ncrystal_unref (&phase);
    }
    return;
  }
  for (i = 0; i < ncrystal_info_nhkl (info); ++i) {
    ncrystal_info_gethkl (info, i, &h, &k, &l, &mult, &dspacing, &fsquared);
    mccode_ncrystal_addxspoint (pts, 2.0 * dspacing, 2.0 * dspacing * (1.0 - 1e-9));
    mccode_ncrystal_addxspoint (pts, 2.0 * dspacing, 2.0 * dspacing * (1.0 + 1e-9));
  }
}

static int
mccode_ncrystal_cmpxspoints (const void* a, const void* b) {
  const mccode_ncrystal_xspoint_t* pa = (const mccode_ncrystal_xspoint_t*)a;
  const mccode_ncrystal_xspoint_t* pb = (const mccode_ncrystal_xspoint_t*)b;
  if (pa->wl != pb->wl)
    return pa->wl < pb->wl ? -1 : 1;
  if (pa->wl_eval != pb->wl_eval)
    return pa->wl_eval < pb->wl_eval ? -1 : 1;
  return 0;
}

/* The table in barn per atom, and the number density: */
static void
mccode_ncrystal_create_xstable (const char* cfgstr, unsigned* n, double** wl, double** xs, double* numberdensity) {
  mccode_ncrystal_xspoints_t pts = { NULL, 0, 0 };
  ncrystal_info_t info = ncrystal_create_info (cfgstr);
  ncrystal_scatter_t scat = ncrystal_create_scatter (cfgstr);
  ncrystal_absorption_t absn = ncrystal_create_absorption (cfgstr);
  double *ekin, *xs_abs;
  unsigned i;
  if (!ncrystal_isnonoriented (ncrystal_cast_scat2proc (scat))) {
    fprintf (stderr, "NCrystal: ERROR: cross section tables are only supported for isotropic materials\n");
    exit (-1);
  }
  *numberdensity = ncrystal_info_getnumberdensity (info);
  mccode_ncrystal_addxspoint (&pts, 0.0, 1e-6);
  for (i = 0; i <= 1400; ++i)
    mccode_ncrystal_addxspoint (&pts, 1e-4 * pow (10.0, 0.005 * i), 1e-4 * pow (10.0, 0.005 * i));
  mccode_ncrystal_addbraggedges (&pts, info);
  qsort (pts.points, pts.n, sizeof (mccode_ncrystal_xspoint_t), mccode_ncrystal_cmpxspoints);
  *n = pts.n;
  *wl = malloc (pts.n * sizeof (double));
  *xs = malloc (pts.n * sizeof (double));
  ekin = malloc (pts.n * sizeof (double));
  xs_abs = malloc (pts.n * sizeof (double));
  if (!*wl || !*xs || !ekin || !xs_abs) {
    fprintf (stderr, "NCrystal: ERROR: memory allocation failed\n");
    exit (-1);
  }
  for (i = 0; i < pts.n; ++i) {
    (*wl)[i] = pts.points[i].wl;
    ekin[i] = ncrystal_wl2ekin (pts.points[i].wl_eval);
  }
  ncrystal_crosssection_nonoriented_many (ncrystal_cast_scat2proc (scat), ekin, pts.n, 1, *xs);
  ncrystal_crosssection_nonoriented_many (ncrystal_cast_abs2proc (absn), ekin, pts.n, 1, xs_abs);
  for (i = 0; i < pts.n; ++i)
    (*xs)[i] += xs_abs[i];
  free (pts.points);
  free (ekin);
  free (xs_abs);
  ncrystal_unref (&info);
  ncrystal_unref (&scat);
  ncrystal_unref (&absn);
}
#endif

void
mccode_init_ncrystal_xstable (mccode_ncrystal_xstable_t* table, const char* cfgstr, const char* compname) {
  double* wl;
  double* xs;
  double factor;
  unsigned i;
  mccode_setup_ncrystal_search_paths ();
#ifdef ncrystal_filtertable
  /* The table in 1/cm, copied to memory allocated here (accessible on GPUs): */
  ncrystal_filtertable (cfgstr, &table->n, &wl, &xs, NULL);
  factor = 100.0;
#else
  /* The table in barn per atom, where barn times atoms per cubic Aa is 1/cm: */
  mccode_ncrystal_create_xstable (cfgstr, &table->n, &wl, &xs, &factor);
  factor *= 100.0;
  MPI_MASTER (printf ("%s: WARNING: NCrystal %s does not provide ncrystal_filtertable yet, so a fallback table is used, which is"
                      " both slower and with less accuracy guarantees (ncrystal_filtertable is planned for NCrystal 4.5.0 and later).\n",
                      compname, ncrystal_version_str ()););
#endif
  table->wl = malloc (table->n * sizeof (double));
  table->macroxs = malloc (table->n * sizeof (double));
  if (!table->wl || !table->macroxs) {
    fprintf (stderr, "%s: ERROR: memory allocation failed\n", compname);
    exit (-1);
  }
  for (i = 0; i < table->n; ++i) {
    table->wl[i] = wl[i];
    table->macroxs[i] = xs[i] * factor;
  }
#ifdef ncrystal_filtertable
  ncrystal_dealloc_doubleptr (wl);
  ncrystal_dealloc_doubleptr (xs);
#else
  free (wl);
  free (xs);
#endif
  MPI_MASTER (printf ("%s: NCrystal %s, cross section table for \"%s\" with %u points\n", compname, ncrystal_version_str (), cfgstr, table->n););
}

double
mccode_eval_ncrystal_xstable (const mccode_ncrystal_xstable_t* table, double wavelength) {
  /* Linear interpolation, where at a discontinuity (two points with the same
     wavelength) the value above it is used. Beyond the last point, the last
     segment is extrapolated linearly (clamped at 0). */
  const unsigned n = table->n;
  const double* wl = table->wl;
  const double* macroxs = table->macroxs;
  unsigned lo, hi, mid;
  double slope, value;
  if (!(wavelength > wl[0]))
    return macroxs[0];
  if (wavelength >= wl[n - 1]) {
    if (!(wl[n - 1] > wl[n - 2]))
      return macroxs[n - 1];
    slope = (macroxs[n - 1] - macroxs[n - 2]) / (wl[n - 1] - wl[n - 2]);
    if (slope == 0.0)
      return macroxs[n - 1];
    value = macroxs[n - 1] + (wavelength - wl[n - 1]) * slope;
    return value > 0.0 ? value : 0.0;
  }
  lo = 0;
  hi = n - 1;
  while (hi - lo > 1) {
    mid = lo + (hi - lo) / 2;
    if (wl[mid] <= wavelength)
      lo = mid;
    else
      hi = mid;
  }
  return macroxs[lo] + ((wavelength - wl[lo]) / (wl[hi] - wl[lo]) * (macroxs[hi] - macroxs[lo]));
}

void
mccode_free_ncrystal_xstable (mccode_ncrystal_xstable_t* table) {
  free (table->wl);
  free (table->macroxs);
  table->wl = NULL;
  table->macroxs = NULL;
  table->n = 0;
}
