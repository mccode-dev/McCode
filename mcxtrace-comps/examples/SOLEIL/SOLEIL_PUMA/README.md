# The `SOLEIL_PUMA` Instrument

*McXtrace: PUMA beam-line at SOLEIL*

## Identification

- **Site:** SOLEIL
- **Author:** Uxue Saez Fernandez
- **Origin:** SOLEIL
- **Date:** 09/2026

## Description

```text
PUMA (an acronym for “Photons Used for Ancient Materials”)
is a hard X-ray beamline optimized for the heritage
science community, though not exclusively reserved for it.
Our microbeam terminal station offers a beam size of
5 (v) x 7 (h) µm², in an energy range from 4 to 22 keV.
Available experimental techniques include X-ray
fluorescence spectroscopy (XRF), X-ray absorption
spectroscopy (XANES), X-ray diffraction (XRD), and
X-ray-excited optical luminescence spectroscopy (XEOL)
```

## Examples

- **Test: E0=10 Detector: mon_sample_e_I=4.02276e+11**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Central energy selected by the DCM. Set to 0 to derive it from DCM_theta. | 10 |
| dE | keV | Half-width of the energy band emitted by the source. Reset to 3% of E0 if dE/E0 > 0.1. | 0.1 |
| DCM_theta | deg | Bragg angle of the Si DCM. Set to 0 to derive it from E0 and (hkl). | 0 |
| HFM_angle | mrad | Total deflection angle of the fixed horizontally focusing mirror (HFM, Ir coating) at 21 m. The grazing incidence angle is HFM_angle/2. | 1.3 |
| HFM_focal | m | Image distance of the HFM, used for its bending radius. The source distance is fixed at 21 m. | 40 |
| KB_angle | mrad | Total deflection angle of each KB mirror (VFM and HFM). The grazing incidence angle is KB_angle/2. | 2.9 |
| KB_coating | str | Reflectivity file for the KB mirror coating, e.g. "Rh.txt" (Rh or B4C). | "Rh.txt" |
| h | 1 | Miller index h of the Si DCM reflection. | 1 |
| k | 1 | Miller index k of the Si DCM reflection. | 1 |
| l | 1 | Miller index l of the Si DCM reflection. | 1 |

## Links

- [Source code](SOLEIL_PUMA.instr) for `SOLEIL_PUMA.instr`.
- <put here any reference/HTML link>

---
