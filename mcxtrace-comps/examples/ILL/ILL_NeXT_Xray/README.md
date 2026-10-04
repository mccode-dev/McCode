# The `ILL_NeXT_Xray` Instrument

*McXtrace: NeXT (ILL) - the X-ray cone-beam tomography arm, operated simultaneously with
and perpendicular to the neutron beam (companion to ILL_H521_NeXT.instr)*

## Identification

- **Site:** ILL
- **Author:** DRAFT, Peter Willendrup with Claude from public ILL / NeXT-Grenoble information
- **Origin:** ILL
- **Date:** September 2026

## Description

```text
DRAFT model of the X-ray imaging option of NeXT-Grenoble at the ILL.

Hardware (Tengattini et al., NIM A 968 (2020) 163939):
- source:   Hamamatsu L12161-07 sealed microfocus tube, W target, Be window,
up to 150 kV / 500 uA, minimum focal spot 5 um, 43 deg cone
- detector: Varex PaxScan 2530HE flat panel, CsI scintillator,
1792 x 2176 pixels of 139 um (portrait), up to 9 Hz (33 Hz bin 2)
- source-detector distance: >= 500 mm (set by the 43 deg cone covering the
~25 x 30 cm panel), four source positions 20 mm apart
-> SDD = 0.50, 0.52, 0.54, 0.56 m
- the sample sits on the common (neutron) rotation axis; the source-object
distance SOD sets the magnification M = SDD/SOD. The quoted 5 um
resolution needs M ~ 28, i.e. SOD ~ 18-20 mm.
- the ILL web page (2026) quotes a 20-300 kV source, i.e. the tube may have
been upgraded; kV > 150 is allowed here but flagged.

Geometry: the X-ray axis is the McXtrace z axis. At NeXT it is horizontal and
perpendicular to the neutron beam (neutrons travel along -x or +x here, depending
on the side of the source); the source/detector pivot (+-20 deg) is not modelled.

The sample defaults to the same object as in ILL_H521_NeXT: a vertical Fe rod,
radius 5 mm, for direct bimodal comparison (the neutron model gives T~0.4 at the
rod centre in white beam). An inner inclusion (e.g. an H2O-filled bore) can be added.

Detector: two ideal pixelated monitors on the flat-panel plane
Xray_counts:  photon counts (photon-counting response)
Xray_energy:  energy-weighted intensity [keV/s per pixel], a first-order model
of the energy-integrating CsI flat panel.
NOT modelled: CsI absorption efficiency vs energy, scintillator/optical blur,
focal-spot growth with tube power, heel effect, scatter from the sample
environment, and the 6 mm Pb / 10 mm + 5 mm B4C neutron shielding of the panel
(5 mm B4C over the active area transmits roughly 80-90 % at 40-150 keV).

ASSUMPTIONS (to be checked):
- Be exit window 0.2 mm (the L12161-07 datasheet value should be checked)
- focal spot = electron-beam footprint on the anode (square, focal_spot)
- anode take-off angle 12 deg (reflection-type microfocus tube, typical)
- Source_lab is a thick-target Kramers model; absolute intensity is indicative
```

## Examples

- **Test: kV=150 SOD=0.1 SDD=0.5 Detector: Xray_counts_I=176042000000**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| kV | kV | Tube acceleration voltage (<=150 for L12161-07; 20-300 per ILL web) | 150 |
| tube_current | A | Electron beam current (<= 500e-6) | 100e-6 |
| focal_spot | m | Focal spot size (square electron footprint on the anode) | 5e-6 |
| Emin | keV | Lowest photon energy simulated | 5 |
| anode | str | Anode material file | "W.txt" |
| take_off | deg | Anode take-off angle | 12 |
| Be_window | m | Be exit window thickness | 0.2e-3 |
| filter | str | Pre-filter material file (e.g. "Cu.txt", "Al.txt", "Sn.txt") | "Cu.txt" |
| filter_t | m | Pre-filter thickness (0: no filter) | 0 |
| SOD | m | Source - object (rotation axis) distance | 0.1 |
| SDD | m | Source - detector distance (0.50, 0.52, 0.54, 0.56) | 0.5 |
| sample | 1 | 1: sample in beam, 0: flat field (open beam) | 1 |
| sample_mat | str | Sample (outer) material file | "Fe.txt" |
| sample_rho | g/cm3 | Sample density (0: take nominal density from file) | 0 |
| sample_radius | m | Radius of the cylindrical sample | 0.005 |
| sample_height | m | Height of the cylindrical sample | 0.05 |
| incl_mat | str | Material of a coaxial inner cylinder ("NULL" for none) | "NULL" |
| incl_radius | m | Radius of the inner cylinder (0 for none) | 0 |
| incl_rho | g/cm3 | Density of the inner cylinder (0: nominal) | 0 |
| sample_off | str | Optional OFF/PLY geometry for the outer sample, overrides the cylinder ("NULL": none) | "NULL" |
| omega | deg | Tomography rotation angle of the sample about the vertical axis | 0 |
| det_binning | 1 | Detector binning (1: 1792x2176 native, 2, 4, ...) | 4 |

## Links

- [Source code](ILL_NeXT_Xray.instr) for `ILL_NeXT_Xray.instr`.
- https://www.ill.eu/en/for-ill-users/instruments/instruments-list/next/
- A. Tengattini et al., NeXT-Grenoble, the Neutron and X-ray tomograph in Grenoble,
- Nucl. Instrum. Meth. A 968 (2020) 163939, doi:10.1016/j.nima.2020.163939
- NECSA_MIXRAD_Nikon_Tomo.instr (McXtrace examples) - similar lab-source tomograph

---
