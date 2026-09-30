/*******************************************************************************
*
* Used NCrystal_sample as basis for creating a Union process that can use the
* NCrystal library.
* Changes by Mads Bertelsen, 13/8 2020.
*
* This file is part of NCrystal (see https://mctools.github.io/ncrystal/)
*
* Copyright 2015-2022 NCrystal developers
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* McStas, neutron ray-tracing package
* Copyright(C) 2007 Risoe National Laboratory.
*
* Original header text for NCrystal_sample.comp:
* McStas sample component for the NCrystal scattering library. Find more
* information at <a href="https://github.com/mctools/ncrystal/wiki">the NCrystal
* wiki</a>. In particular, browse the available datafiles at <a
* href="https://github.com/mctools/ncrystal/wiki/Data-library">Data-library</a>
* and read about format of the configuration string expected in the "cfg"
* parameter at <a
* href="https://github.com/mctools/ncrystal/wiki/Using-NCrystal">Using-NCrystal</a>.
*
* NCrystal is available under the <a
* href="http://www.apache.org/licenses/LICENSE-2.0">Apache 2.0 license</a>.
* Depending on the configuration choices, optional NCrystal modules under
* different licenses might be enabled - see <a
* href="https://github.com/mctools/ncrystal/wiki/About">About</a> for more
* details.
*
* Shared NCrystal implementation for Union components.
******************************************************************************/

/******************************************************************************/
/* The original code contained the following license/copyright statement:     */
/******************************************************************************/
/*                                                                            */
/*  This file is part of NCrystal (see https://mctools.github.io/ncrystal/)   */
/*                                                                            */
/*  Copyright 2015-2019 NCrystal developers                                   */
/*                                                                            */
/*  Licensed under the Apache License, Version 2.0 (the "License");           */
/*  you may not use this file except in compliance with the License.          */
/*  You may obtain a copy of the License at                                   */
/*                                                                            */
/*      http://www.apache.org/licenses/LICENSE-2.0                            */
/*                                                                            */
/*  Unless required by applicable law or agreed to in writing, software       */
/*  distributed under the License is distributed on an "AS IS" BASIS,         */
/*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  */
/*  See the License for the specific language governing permissions and       */
/*  limitations under the License.                                            */
/*                                                                            */
/******************************************************************************/

#ifndef UNION_NCRYSTAL_C
#define UNION_NCRYSTAL_C

/* Common includes, defines, functions, etc. shared by all Union NCrystal components. */
#if defined(WIN32) || defined(_WIN32)
#include "NCrystal\\ncrystal.h"
#else
#include "NCrystal/ncrystal.h"
#endif
#include "stdio.h"
#include "stdlib.h"

#ifndef NCMCERR2
/* consistent/convenient error reporting */
#define NCMCERR2(compname, msg)                                                                                              \
  do {                                                                                                                       \
    fprintf (stderr, "\nNCrystal: %s: ERROR: %s\n\n", compname, msg);                                                     \
    exit (1);                                                                                                                \
  } while (0)
#endif

static int ncsample_reported_version_union = 0;

// Keep all instance-specific parameters on a few structs:
typedef struct {
  double density_factor;
  double inv_density_factor;
  ncrystal_scatter_t scat;
  ncrystal_process_t proc_scat, proc_abs;
  int proc_scat_isoriented;
  int absmode;
} ncrystalsample_t_union;

struct NCrystal_physics_storage_struct {
  // Very important to add a pointer to this struct in the union_ncrystal.c file.
  // Variables that need to be transferred between the following places:
  // the initialization in the component, the function calculating my, and the
  // function calculating scattering.
  // Avoid duplicates of output parameters and setting parameters in naming.
  ncrystalsample_t_union stored_params;
  double ncrystal_convfact_vsq2ekin;
  double ncrystal_convfact_ekin2vsq;
};

static void
ncrystal_union_check_version (const char *component_name)
{
  // Print NCrystal version + sanity check setup.
  if (NCRYSTAL_VERSION != ncrystal_version ())
    NCMCERR2 (component_name, "Inconsistency detected between included ncrystal.h and linked NCrystal library!");
  if (ncsample_reported_version_union != ncrystal_version ()) {
    if (ncsample_reported_version_union)
      NCMCERR2 (component_name, "Inconsistent NCrystal library versions detected - this should normally not be possible!");
    ncsample_reported_version_union = ncrystal_version ();
    MPI_MASTER (printf ("NCrystal: McStas Union component(s) are using version %s of the NCrystal library.\n", ncrystal_version_str ()););
  }
}

static void
ncrystal_union_setup_conversion (double *vsq2ekin, double *ekin2vsq)
{
  // The following conversion factors might look slightly odd. They reflect the
  // fact that the various conversion factors used in McStas and NCrystal are not
  // completely consistent among each other (TODO: Follow up on this with McStas
  // developers!). Also McStas's V2K*K2V is not exactly 1. All in all, this can
  // give issues when a McStas user is trying to set up a narrow beam very
  // precisely in an instrument file, attempting to carefully hit a certain
  // narrow Bragg reflection in this NCrystal component. We can not completely
  // work around all issues here, but for now, we assume that the user is
  // carefully setting up things by specifying the wavelength to some source
  // component. That wavelength is then converted to the McStas state pars
  // (vx,vy,vz) via K2V. We thus here first use 1/K2V (and *not* V2K) to convert
  // back to a wavelength, and then we use NCrystal's conversion constants to
  // convert the resulting wavelength to kinetic energy needed for NCrystal's
  // interfaces.

  // 0.0253302959105844428609698658024319097260896937 is 1/(4*pi^2).
  *vsq2ekin = ncrystal_wl2ekin (1.0) * 0.0253302959105844428609698658024319097260896937 / (K2V * K2V);
  *ekin2vsq = 1.0 / *vsq2ekin;
}

static void
ncrystal_union_setup_rng (void)
{
  /* Make sure NCrystal uses the McStas RNG (ok if more than one component instance does this): */
#ifndef rand01
  /* Tell NCrystal to use the rand01 function provided by McStas: */
  ncrystal_setrandgen (rand01);
#else
  /* rand01 is actually a macro, not an actual C-function (most likely defined as */
  /* _rand01(_particle->randstate) for OPENACC purposes), which we can not       */
  /* register with NCrystal. As a workaround we tell NCrystal to use its own RNG */
  /* algorithm, with merely the seed provided by McStas:                         */
  ncrystal_setbuiltinrandgen_withseed (mcseed);
#endif
}

static double
ncrystal_union_get_numberdensity (ncrystal_info_t info, const char *component_name)
{
  double numberdensity;

  /* Access crystal structure to get number density (natoms/volume). */
#if NCRYSTAL_VERSION >= 1099001
  numberdensity = ncrystal_info_getnumberdensity (info);
#else
  // Older NCrystal versions provide number density through the crystal
  // structure information instead.
  unsigned cell_sg, cell_atnum;
  double cell_a, cell_b, cell_c, cell_alpha, cell_beta, cell_gamma, cell_volume;
  if (!ncrystal_info_getstructure (info, &cell_sg, &cell_a, &cell_b, &cell_c, &cell_alpha, &cell_beta, &cell_gamma, &cell_volume, &cell_atnum))
    NCMCERR2 (component_name, "Structure information is unavailable in the loaded NCrystal Info");
  numberdensity = cell_atnum / cell_volume;
#endif

  // Number density is in Aa^-3=1e30m^-3. With cross sections in barn
  // (1e-28m^2) and distances in metres, the unit conversion factor is 0.01.
  // The Union master samples with a positive density factor, unlike the
  // negative factor used by NCrystal_sample.
  // TODO for NC2: The density used here is in principle different from the one
  // used in the Geant4 interface, which is possibly inconsistent when not using
  // .ncmat files (more info at https://github.com/mctools/ncrystal/issues/9).
  if (numberdensity <= 0.0)
    NCMCERR2 (component_name, "Number density information is unavailable in the loaded NCrystal Info");
  return numberdensity;
}

int
NCrystal_physics_my (double *my, double *k_initial, union data_transfer_union data_transfer, struct focus_data_struct *focus_data, _class_particle *_particle)
{
  // Function for calculating my, the inverse penetration depth (for only this
  // scattering process). The input for this function and its order may not be
  // changed, but the names may be updated.
  double k_mag = sqrt (k_initial[0] * k_initial[0] + k_initial[1] * k_initial[1] + k_initial[2] * k_initial[2]);
  // Normalized direction dir[3].
  double dir[3];
  dir[0] = k_initial[0] / k_mag;
  dir[1] = k_initial[1] / k_mag;
  dir[2] = k_initial[2] / k_mag;

  double vsq2ekin = data_transfer.pointer_to_a_NCrystal_physics_storage_struct->ncrystal_convfact_vsq2ekin;
  // Kinetic energy.
  double ekin = vsq2ekin * K2V * k_mag * K2V * k_mag;
  double xsect_scat = 0.0;
  // Call NCrystal library, xsect_scat is updated.
  ncrystal_crosssection (data_transfer.pointer_to_a_NCrystal_physics_storage_struct->stored_params.proc_scat, ekin, (const double (*)[3]) &dir, &xsect_scat);

  double density_factor = data_transfer.pointer_to_a_NCrystal_physics_storage_struct->stored_params.density_factor;
  // Convert xsect_scat to inverse penetration depth.
  *my = xsect_scat / density_factor;
  return 1;
}

int
NCrystal_physics_scattering (double *k_final, double *k_initial, double *weight, union data_transfer_union data_transfer,
                             struct focus_data_struct *focus_data, _class_particle *_particle)
{
  // Function that provides description of a basic scattering event.
  // Do not change the function signature.
  // Magnitude of given wavevector.
  double k_mag = sqrt (k_initial[0] * k_initial[0] + k_initial[1] * k_initial[1] + k_initial[2] * k_initial[2]);
  // Normalized direction dir[3].
  double dir[3], dirout[3];
  dir[0] = k_initial[0] / k_mag;
  dir[1] = k_initial[1] / k_mag;
  dir[2] = k_initial[2] / k_mag;

  double vsq2ekin = data_transfer.pointer_to_a_NCrystal_physics_storage_struct->ncrystal_convfact_vsq2ekin;
  double ekin = vsq2ekin * K2V * k_mag * K2V * k_mag;
  double ekin_final = 0.0;
  // Call NCrystal library, dirout and delta_ekin is updated.
  ncrystal_samplescatter (data_transfer.pointer_to_a_NCrystal_physics_storage_struct->stored_params.scat, ekin, (const double (*)[3]) &dir, &ekin_final, &dirout);

  if (ekin_final <= 0) {
    // not expected to happen much, but an interaction could in principle bring
    // the neutron to rest. Equivalent to ABSORB.
    return 0;
  }

  double v2 = data_transfer.pointer_to_a_NCrystal_physics_storage_struct->ncrystal_convfact_ekin2vsq * ekin_final;
  double absv = sqrt (v2);
  k_mag = absv * V2K;
  k_final[0] = dirout[0] * k_mag;
  k_final[1] = dirout[1] * k_mag;
  k_final[2] = dirout[2] * k_mag;
  // A pointer to k_final is returned, and the wavevector will be set to k_final
  // after a scattering event. Return 1 for success; return 0 for failure, and
  // the ray will be absorbed.
  return 1;
}

#ifndef PROCESS_DETECTOR
#define PROCESS_DETECTOR dummy
#endif
#ifndef PROCESS_NCRYSTAL_DETECTOR
#define PROCESS_NCRYSTAL_DETECTOR dummy
#endif

#endif
