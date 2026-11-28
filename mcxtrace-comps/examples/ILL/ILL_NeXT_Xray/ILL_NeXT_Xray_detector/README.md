# The `ILL_NeXT_Xray_detector` Instrument

*McXtrace: NeXT (ILL) X-ray arm: Varex PaxScan 2530HE flat-panel detector*

## Identification

- **Site:** ILL
- **Author:** DRAFT, Peter Willendrup with Claude from public ILL / NeXT-Grenoble information
- **Origin:** ILL
- **Date:** September 2026

## Description

```text
Building block, %include'd after the sample by ILL_NeXT_Xray and its variants,
see ILL_NeXT_Xray_source. The REMOVABLE Arm stands in for anode_spot of
ILL_NeXT_Xray_source so that this file is also a valid instrument on its own.

Two ideal pixelated monitors on the flat-panel plane at SDD:
Xray_counts:  photon counts (photon-counting response)
Xray_energy:  energy-weighted intensity [keV/s per pixel], a first-order model
of the energy-integrating CsI flat panel.
See ILL_NeXT_Xray for what is not modelled.
```

## Examples



## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| kV | kV | Tube acceleration voltage (sets the upper energy of the spectra) | 150 |
| SDD | m | Source - detector distance (0.50, 0.52, 0.54, 0.56) | 0.5 |
| det_binning | 1 | Detector binning (1: 1792x2176 native, 2, 4, ...) | 4 |

## Links

- [Source code](ILL_NeXT_Xray_detector.instr) for `ILL_NeXT_Xray_detector.instr`.

---
