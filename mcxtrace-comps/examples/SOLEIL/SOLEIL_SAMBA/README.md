# The `SOLEIL_SAMBA` Instrument

*McXtrace: SOLEIL SAMBA beamline: absorption spectroscopy*

## Identification

- **Site:** SOLEIL
- **Author:** FARHI Emmanuel
- **Origin:** SOLEIL
- **Date:** September 2026

## Description

```text
SAMBA (Spectroscopy Applied to Material Based on Absorption) is a hard X-ray
absorption spectroscopy (XAS) beamline. SAMBA is open to a broad scientific
community spanning from physics to chemistry, surface and environmental
sciences. The design of SAMBA optics is optimized in order to be very versatile
and to cover the 4.8-40 keV energy range with a high flux of photons and
stability and optimum energy resolution. The monochromator runs in continuous
scan mode, and a 35 pixels HPGe fluorescence detector is available for
measurements on highly diluted specimens.

Position | Element
---------|-------------------------------------------------------------
0        | Source: Bending Magnet (D09-1)
14       | M1: Collimating Mirror (Pd coated)
16       | MDC1: 1st Crystal (Flat, Si 220 or Si 111)
17       | MDC2: 2nd Crystal (Sagittally bent, Si 220 or Si 111)
18.5     | M2: Focusing Mirror (Pd coated)
30       | Sample Stage (XAS configuration)
35       | Sample Stage (SEXAFS configuration)
```

## Examples

- **Test: SAMBA_E0=15 Detector: SAMBA_det_fluo_I=1.23968e+10**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| SAMBA_E0 | keV | Central energy | 15 |
| SAMBA_dE | keV | Energy spread | 0.1 |
| SAMBA_M1_angle | mrad | Incidence angle of M1 (vertical collimation) | 3 |
| SAMBA_M2_angle | mrad | Incidence angle of M2 (vertical focusing) | 3 |
| SAMBA_sample | str | Sample structure file | "LaB6.cif" |
| splits |  |  | 10 |

## Links

- [Source code](SOLEIL_SAMBA.instr) for `SOLEIL_SAMBA.instr`.
- Documentation: APD Optique 6 / Samba.pdf

---
