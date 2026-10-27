# The `SOLEIL_GALAXIES` Instrument

*McXtrace: The GALAXIES beam-line at Synchrotron SOLEIL - RIXS branch.*

## Identification

- **Site:** SOLEIL
- **Author:** E. Farhi, from SOLEIL technical notes by T. Moreno
- **Origin:** Synchrotron SOLEIL
- **Date:** 2026

## Description

```text
GALAXIES is dedicated to inelastic X-ray scattering (IXS/RIXS) and hard
X-ray photoemission (HAXPES), in the 2.3-12 keV range, with high resolution
and micro-beam. This model implements the RIXS station (35.5 m diffractometer
with a 1-2 m radius IXS spectrometer, multi-analyzer 4x1 or 2x2 setup).
The HAXPES branch ends at the secondary slits (28.2 m).

Optical layout (distances from the undulator center, EXP-OPT-CDC-1029):

Position | Element
---------|--------------------------------------------------------------------
0        | U20 undulator, 98 periods, lambda_u=20 mm, short section
10       | Absorber 2.5 x 2.5 mm^2
11.7     | Diaphragm 1.8 x 0.6 mm^2 (H x V): beamline acceptance
17       | Primary slits 2.0 x 0.7 mm^2 (H x V)
19       | Si(111) DCM, fixed exit, crystals 10x50 mm^2 (T x L)
22       | M1 spherical mirror R=8.81 km, 5 mrad, face up, 200 mm, B4C/Pd
23.5     | 2 channel-cuts (anti-symmetric) Si311 (1st order) / Si220 (2nd order)
24.89    | M2A toroid (microfocus): focus at secondary slits 28.2 m [HAXPES]
28.2     | Secondary slits 30x30 um^2 min (M2A focal plane, HAXPES sample)
32.20    | M2B toroid (intermediate focus): focus at RIXS sample 35.5 m
34.1     | VFM elliptical KB mirror, p=5.9 m, q=1.4 m, 320 mm, Pd
34.5     | HFM elliptical KB mirror, p=6.3 m, q=1.0 m, 320 mm, Pd
35.5     | RIXS sample / diffractometer
+R       | IXS spectrometer: spherical Si analyzers (Rowland R=0.5-2 m)

Toroid radii and M2 positions are the post-metrology (SESO measured) values
from EXP-OPT-CDC-1936: Rt=1.335 km, Rs=30.47 mm, M2A at 24.89 m and M2B at
32.20 m, grazing angle 4.95 mrad, so that the foci stay at 28.2 m (HAXPES)
and 35.5 m (RIXS).

The electron beam in the short section (EXP-OPT-CDC-0771):
sigma_x=388 um, sigma_z=8.1 um, sigma'_x=14.5 urad, sigma'_z=4.6 urad.

Expected performance (ray tracing, EXP-OPT-CDC-1029):
- M2B focus:  75 x 35 um^2 FWHM (H x V), ~6e13 ph/s @ 8 keV
- KB focus:   14 x  9 um^2 FWHM (open sec. slits), 5x7 um^2 (30x30 um^2 slits)
```

## Examples

- **Test: GALAXIES_E0=10 Detector: GALAXIES_mon_dcm_xy_I=6.05992e+12**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| GALAXIES_E0 | keV | Nominal energy (2.3 - 12). | 10 |
| GALAXIES_dE | keV | Energy half-bandwidth at the source. | 0.1 |
| GALAXIES_hrm | 1 | 1 = insert the high resolution channel-cuts. | 0 |
| GALAXIES_hrm_ref | str | "Si311" (1st order, >=5 keV) or "Si220" (2nd order, >=8 keV). | "Si311" |
| GALAXIES_sec_slits | m | Secondary slits opening (H and V) at the M2A focus (28.2 m). | 0.1 |
| GALAXIES_ana_R | m | Analyzer Rowland radius (0.5 - 2 m). | 1.0 |
| GALAXIES_ana_deg | deg | Spectrometer scattering angle (2-theta at the sample). | 10 |
| GALAXIES_sample | str | Sample LAU/CIF structure file for the RIXS sample. | "Si.lau" |
| splits |  |  | 10 |

## Links

- [Source code](SOLEIL_GALAXIES.instr) for `SOLEIL_GALAXIES.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/galaxies

---
