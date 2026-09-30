# The `ILL_H521_NeXT` Instrument

*McStas: NeXT - Neutron and X-ray Tomography - cold neutron imaging at the end of H521*

## Identification

- **Site:** ILL
- **Author:** DRAFT, Peter Willendrup with Claude from public ILL web information
- **Origin:** ILL
- **Date:** September 2026

## Description

```text
DRAFT model of the NeXT cold-neutron imaging instrument at the ILL, located
at the end of the H521 guide, downstream of D16 and SuperADAM.

Source term and guide: the H5 horizontal cold source (HCS) and the H52 /
H521 guide geometry are copied from the ILL_H5_new example (proposed H5
layout from the 2011-2013 H5 upgrade design; the as-built geometry may differ).
The D16 and SuperADAM PG002 monochromators are kept IN TRANSMISSION: their
Bragg-reflected neutrons are removed from the beam (use upstream_monos=0 to
switch that off).

NeXT itself (public ILL data, 2026):
- pinhole diameters D = 5, 10, 15, 23, 30, 40 mm
- pinhole-sample distance L = 5.5 - 10.5 m (L/D up to ~2000)
- reference flux: 3e8 n/cm2/s at L/D = 333
- white beam, or wavelength selection by
mode=1: velocity selector, 2-20 AA, dl/l ~ 16 %
mode=2: double-crystal monochromator (DCM), 2-6 AA, dl/l ~ 5 %
- detector FoV up to 170x170 mm2 (scintillator + sCMOS, 2048x2048)
The sample is a simple cylinder (PowderN, e.g. Fe.laz) so that attenuation and
Bragg edges show up in the transmission spectra. Detector resolution/blur of
the scintillator/camera is NOT modelled (ideal position-sensitive monitor).

ASSUMPTIONS (to be checked against ILL drawings):
- L_guide_end: length of H521 guide after the SuperADAM monochromator
- selector (alpha, length, radius) chosen to give dl/l ~ 16 %, not the
actual NeXT selector specification
- DCM modelled as two parallel flat PG002 crystals, mosaic 30', r0=0.9,
first order only by default (no lambda/2 filter modelled)
- pinhole placed 0.8 m after the guide exit / selection optics
```

## Examples

- **Test: lambda=5.5 dlambda=4.5 mode=0 D=0.03 L_coll=10 Detector: NeXT_flux_sample_I=3e8**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Centre of the simulated source wavelength band | 5.5 |
| dlambda | AA | Half width of the simulated wavelength band | 4.5 |
| mode | 1 | 0=white beam, 1=velocity selector, 2=double crystal monochromator | 0 |
| sel_lambda | AA | Velocity selector wavelength (mode=1) | 4 |
| dcm_lambda | AA | DCM wavelength (mode=2), 2-6 AA | 4 |
| D | m | Pinhole diameter: 0.005, 0.010, 0.015, 0.023, 0.030, 0.040 | 0.03 |
| L_coll | m | Pinhole - sample distance, 5.5-10.5 | 10 |
| sample | 1 | 1: put the cylinder sample in the beam, 0: open beam | 1 |
| sample_file | str | PowderN reflection list of the sample (e.g. Fe.laz, Al.laz) | "Fe.laz" |
| sample_radius | m | Radius of the cylindrical sample | 0.005 |
| sample_height | m | Height of the cylindrical sample | 0.05 |
| det_dist | m | Sample - detector (scintillator) distance | 0.03 |
| det_size | m | Detector field of view (square), up to 0.17 | 0.17 |
| det_bins | 1 | Number of detector pixels along x and y | 200 |
| upstream_monos | 1 | 1: remove the D16 and SuperADAM monochromator reflections | 1 |
| D16_lambda | AA | D16 monochromator wavelength | 5.6 |
| SADAM_lambda | AA | SuperADAM monochromator wavelength | 4.4 |
| L_guide_end | m | Length of H521 guide after the SuperADAM monochromator (assumed) | 1.0 |
| dcm_order | 1 | DCM Bragg orders: 1 = first order only, 0 = all orders (lambda/2, lambda/3 | 1 |
| sel_alpha | deg | Velocity selector twist angle (sets dl/l; assumed value) | 26 |
| dcm_mosaic | arcmin | DCM crystal mosaic (sets dl/l; assumed value) | 60 |
| direct_focus | 1 | 1: source illuminates the H52 entrance directly (efficient), | 1 |

## Links

- [Source code](ILL_H521_NeXT.instr) for `ILL_H521_NeXT.instr`.
- [Additional information](ILL_H521_NeXT.md)
- https://www.ill.eu/en/for-ill-users/instruments/instruments-list/next/
- https://www.ill.eu/en/for-ill-users/instruments/instruments-list/next/characteristics/
- ILL_H5_new.instr (McStas examples/ILL) for the source and H52/H521 guide

---
