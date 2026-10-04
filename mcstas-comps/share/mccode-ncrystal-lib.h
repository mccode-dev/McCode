/*******************************************************************************
*
* McStas, neutron ray-tracing package
*         Copyright (C) 1997-2026, All rights reserved
*         DTU Physics, Kgs. Lyngby, Denmark
*
* Library: share/mccode-ncrystal-lib.h
*
* %Identification
* Written by: Thomas Kittelmann
* Date: 2026
* Origin: ESS
*
* Common functionality for McStas components using NCrystal (NCrystal_sample,
* NCrystal_process, Union_NCrystal_material, NCrystal_filter, PowderN,
* Single_crystal), included with %include "mccode-ncrystal-lib" in a SHARE
* section. It also includes the NCrystal C API (ncrystal.h).
*
*******************************************************************************/

#ifndef MCCODE_NCRYSTAL_LIB_H
#define MCCODE_NCRYSTAL_LIB_H

#if defined(WIN32) || defined(_WIN32)
#include "NCrystal\\ncrystal.h"
#else
#include "NCrystal/ncrystal.h"
#endif

/* Set up NCrystal's search path for data files. To be called before loading
   any NCrystal materials. Calling it more than once has no further effect.

   This adds the same directories to the NCrystal search path as searched by
   Open_File() in read_table-lib (used by most McStas components to find data
   files): the directory of the instrument file, the directory of the
   executable, and the data/ and contrib/ directories of the McStas
   installation. So data files (e.g. "localdata/X.ncmat") are found by NCrystal
   in the same way as other data files. (NCrystal itself already searches
   relative to the current working directory, like Open_File()). */
void mccode_setup_ncrystal_search_paths( void );

/* Table of the macroscopic total cross section (scattering plus absorption, in
   1/m) of an isotropic material vs. the neutron wavelength (in Aa), e.g. for
   attenuating a beam passing through the material. */
typedef struct {
  unsigned n;
  double* wl;
  double* macroxs;
} mccode_ncrystal_xstable_t;

/* Initialise the table for the material given by the NCrystal cfg-string. This
   first calls mccode_setup_ncrystal_search_paths. The table is provided by
   NCrystal (ncrystal_filtertable), or created here with versions of NCrystal
   which do not provide it (with a warning). The compname is used in
   messages. */
void mccode_init_ncrystal_xstable( mccode_ncrystal_xstable_t* table,
                                   const char* cfgstr, const char* compname );

/* Macroscopic cross section (in 1/m) at the given wavelength (in Aa). Can also
   be used on GPUs. */
#pragma acc routine seq
double mccode_eval_ncrystal_xstable( const mccode_ncrystal_xstable_t* table,
                                     double wavelength );

/* Free the memory of the table. */
void mccode_free_ncrystal_xstable( mccode_ncrystal_xstable_t* table );

#endif
