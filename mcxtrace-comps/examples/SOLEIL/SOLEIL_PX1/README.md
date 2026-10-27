# The `SOLEIL_PX1` Instrument

*McXtrace: The PROXIMA-1 (PX1) MX beam-line at Synchrotron SOLEIL.*

## Identification

- **Site:** SOLEIL
- **Author:** E. Farhi
- **Origin:** Synchrotron SOLEIL
- **Date:** 2026-10-03

## Description

```text
PROXIMA-1 (PX1) is one of the two macromolecular crystallography (MX)
beam-lines at Synchrotron SOLEIL, operational since March 2008. The
end-station is dedicated to bio-molecular crystallography, with a layout
designed to favour anomalous diffraction data recording and studies of
crystals with large unit cell dimensions (thanks to the low horizontal
divergence of the beam at the sample). The X-ray photons originate from an
in-vacuum U20 undulator, and are modulated through a liquid-nitrogen-cooled
Si(111) channel-cut monochromator and a pair of Kirkpatrick-Baez (KB)
focusing mirrors. The standard energy range is 6.5-15 keV, with 12.67 keV
set as the reference energy. The beam size at the sample is 120 (H) x 20 (V)
um FWHM uncollimated (40 x 20 um when collimated), with a flux of
3.8e11 photons/s at 12.67 keV for a 500 mA ring current (2.1e11 when
collimated). The end-station hosts a chi-geometry three-axis goniometer
(SmarGon), a KETEK silicon drift detector for X-ray spectroscopy, an Oxford
Cryosystems 700-series cryo-stream, a CATS sample exchange robot (48 samples),
and the fast single-photon-counting pixel-array detector EigerX-16M,
routinely used at 100 Hz.

This is a simplified model with an undulator, a Si(111) channel-cut
monochromator, a KB mirror pair, a sample stage and a detector. It is
adapted from the SOLEIL_PX2a.instr model (M. Savko, E. Farhi), using:
- L.M.G. Chavas et al., "PROXIMA-1 beamline for macromolecular
crystallography measurements at Synchrotron SOLEIL",
J. Synchrotron Rad. (2021). 28, 970-976 (layout, Table 1, Fig. 1
element distances);
- the publication supporting information (measured U20 harmonics at the
5.5 mm minimum gap);
- the SOLEIL PROXIMA-1 web page (U20 with 100 periods of 20 mm);
- the SOLEIL PX1 synoptique drawings (TEC-CI-PL-0356 and PX1_SYNOPTIQUE).

List of important beamline elements. Distances are given from the center of
the undulator (from Chavas et al. 2021, Fig. 1; hutch boundary C04/CX1 is
approximate).

Element            | Distance (m) | Description | Hutch
-------------------|--------------|-------------|--------------------
U20 undulator      | 0.0          | In-vacuum undulator, 100 periods x 20 mm (magnetic length 2 m), minimum gap 5.5 mm, K_max=1.66. Odd harmonics give access to 5-20 keV. Undulator is tapered (30 um) in operation. | Source/Front-end
MBS, WBS           | ~10-16       | Main beam shutter, white-beam slits, beam-defining mask 0.27 (H) x 0.47 (V) mrad. Not modeled. | Front-end
CCM                | 16.397       | Liquid-nitrogen-cooled Si(111) single-crystal channel-cut monochromator with piezo pitch fine tuning. First optical element seen by the beam. Channel gap 6 mm (assumed, as PX2a). | OH
SeS                | 18.5         | Secondary slits; define the footprint on the KB mirrors. | OH
DSS                | ~19          | Downstream safety shutter. Not modeled. | OH
KB (V)             | 21.45        | Vertical-focusing KB mirror, first-generation Rh-coated glass, grazing angle 4.1 mrad. | OH
KB (H)             | 21.9         | Horizontal-focusing KB mirror, same coating/angle. Beam at sample: 120 (H) x 20 (V) um FWHM uncollimated, 40 x 20 um collimated; divergence 0.26 (H) x 0.48 (V) mrad. Mirror separation 0.45 m assumed. | OH
DT                 | ~22          | Diagnostic tools, diamond CVD X-ray beam positioning monitor. Not modeled. | OH/EH
ES                 | ~23          | Experimental slits. Not modeled. | EH
SaS                | ~23.7        | Sample slits; collimation of the beam down to 40 x 20 um. Not modeled. | EH
FS                 | ~23.8        | Fast shutter. Not modeled. | EH
SG (sample)        | 23.846       | Sample position: SmarGon chi-geometry 3-axis goniometer, Oxford Cryosystems 700 series, KETEK SDD, CATS sample changer (3 Unipucks, 48 samples). | EH
PAD (detector)     | 23.846+PX1_det_dist | EigerX-16M single-photon-counting pixel-array detector, 311 x 328 mm2, 75 um pixels, routinely used at 100 Hz. Sample-to-detector distance 0.096-1.55 m. | EH

Notes:
- Undulator harmonics: at the fixed minimum gap of 5.5 mm (K=1.66), the
measured spectrum shows harmonics 4 to 10 at 6.006, 7.558, 9.026, 10.558,
12.058, 13.622 and 15.084 keV, i.e. a fundamental at E1=1.507 keV. At the
default E0=12.67 keV the undulator is tuned onto the 7th harmonic, which
requires K=1.4028 (gap slightly opened). The K parameter is exposed; when
changing E0, adjust K to stay on a harmonic: with lu=20 mm and Ee=2.75 GeV
the fundamental is E1 = 3.591/(1+K*K/2) keV, i.e. K = sqrt(2*(3.591/E1 - 1))
with E1=E0/harmonic. Available odd harmonics (3,5,7,...) cover 5-20 keV.
- KB focusing: the default radii focus the source (undulator) onto the
sample, using the cylindrical mirror formula R = 2*p*q/((p+q)*sin(grazing angle)):
PX1_vfm_radius = 2*21.45*2.396/((21.45+2.396)*4.1e-3)      = 1051 m
PX1_hfm_radius = 2*21.90*1.946/((21.90+1.946)*4.1e-3)      =  872 m
In reality the KB pair focuses slightly downstream of the sample, onto
the detector surface when placed at the most common distance (about
0.46 m past the sample for a 2 A outer-shell resolution); use
PX1_vfm_radius=1230 and PX1_hfm_radius=1058 to model this configuration.
The real beam size is dominated by the mirror figure errors (first
generation KB), not by the source demagnification, so the simulated
focus is sharper than the measured 120 (H) x 20 (V) um.
- The front-end mask, shutters, slits, attenuators, Be/Kapton windows,
beam positioning monitors, the fast shutter and the undulator taper mode
are not modeled. The transfocator listed in some PX1 synoptiques has no
published parameters and is not modeled either.
- Electron beam sizes are nominal SOLEIL short straight section values
(beta_x=2.19 m, beta_y=1.09 m, emittance 3.74 nm rad, 1% coupling).
```

## Examples

- **Test: PX1_E0=12.67 Detector: PX1_sample_monitor_I=1.232e+20**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| PX1_E0 | keV | Energy selected at the Undulator (reference energy 12.67 keV, standard range 6.5-15 keV). | 12.67 |
| PX1_dE | keV | Energy half-bandwidth at the source. | 0.1 |
| PX1_K | 1 | Undulator deflection parameter (K=1.4028: 7th harmonic at 12.67 keV; K_max=1.66 at 5.5 mm gap). | 1.4028 |
| PX1_vfm_radius | m | vertically focusing mirror radius (tangential). | 1051 |
| PX1_hfm_radius | m | horizontally focusing mirror radius (tangential). | 872 |
| PX1_mirror_grazing_angle | rad | Tilt angle of the KB mirrors (4.1 mrad). | 4.1e-3 |
| PX1_det_dist | m | Sample-to-detector distance (0.096-1.55 m). | 0.2 |
| PX1_sample | str | Sample structure file, LAU/CIF format. | "adrenaline.lau" |
| PX1_rotX | deg | Sample rotation around X | 0 |
| PX1_rotY | deg | Sample rotation around Y | 0 |
| PX1_rotZ | deg | Sample rotation around Z | 0 |
| splits | 1 | Ray splitting factor at the sample. | 10 |

## Links

- [Source code](SOLEIL_PX1.instr) for `SOLEIL_PX1.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/proxima-1

---
