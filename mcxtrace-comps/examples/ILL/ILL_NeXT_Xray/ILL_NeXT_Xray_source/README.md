# The `ILL_NeXT_Xray_source` Instrument

*McXtrace: NeXT (ILL) X-ray arm: microfocus tube up to the sample rotation axis*

## Identification

- **Site:** ILL
- **Author:** DRAFT, Peter Willendrup with Claude from public ILL / NeXT-Grenoble information
- **Origin:** ILL
- **Date:** September 2026

## Description

```text
Building block, %include'd by ILL_NeXT_Xray (Fe rod / flat field) and
ILL_NeXT_Xray_porous and ILL_NeXT_Xray_porous_Union:
%include "ILL/ILL_NeXT_Xray/ILL_NeXT_Xray_source/ILL_NeXT_Xray_source.instr"
(sample, AT (0,0,0) RELATIVE sample_rot)
%include "ILL/ILL_NeXT_Xray/ILL_NeXT_Xray_detector/ILL_NeXT_Xray_detector.instr"

Hamamatsu L12161-07 sealed microfocus tube, W target, Be window, up to
150 kV / 500 uA, 43 deg cone, focusing on the Varex PaxScan 2530HE panel at SDD.
Sample on the common (neutron) rotation axis at SOD, magnification M = SDD/SOD.
See ILL_NeXT_Xray for hardware details and assumptions.
```

## Examples

- **Test: ILL_NeXT_Xray_source.instr -y Detector: source_spectrum_I=1.97857e+11**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| kV | kV | Tube acceleration voltage (<=150 for L12161-07; 20-300 per ILL web) | 150 |
| tube_current | A | Electron beam current (<= 500e-6) | 100e-6 |
| focal_spot | m | Focal spot size (square electron footprint on the anode) | 5e-6 |
| Emin | keV | Lowest photon energy simulated | 5 |
| anode | str | Anode material file, e.g. Cu, Co, Cr, W, Mo | "W" |
| take_off | deg | Anode take-off angle | 12 |
| Be_window | m | Be exit window thickness | 0.2e-3 |
| filter | str | Pre-filter material file (e.g. "Cu.txt", "Al.txt", "Sn.txt") | "Cu.txt" |
| filter_t | m | Pre-filter thickness (0: no filter) | 0 |
| SOD | m | Source - object (rotation axis) distance | 0.1 |
| SDD | m | Source - detector distance (0.50, 0.52, 0.54, 0.56) | 0.5 |
| omega | deg | Tomography rotation angle of the sample about the vertical axis | 0 |
| det_binning | 1 | Detector binning (1: 1792x2176 native, 2, 4, ...) | 4 |

## Links

- [Source code](ILL_NeXT_Xray_source.instr) for `ILL_NeXT_Xray_source.instr`.
- https://www.ill.eu/en/for-ill-users/instruments/instruments-list/next/
- A. Tengattini et al., NeXT-Grenoble, the Neutron and X-ray tomograph in Grenoble,
- Nucl. Instrum. Meth. A 968 (2020) 163939, doi:10.1016/j.nima.2020.163939

---
