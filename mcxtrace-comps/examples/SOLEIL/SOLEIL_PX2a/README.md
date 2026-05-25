# The `SOLEIL_PX2a` Instrument

*McXtrace: The PROXIMA-2a MX beam-line at Synchrotron SOLEIL.*

## Identification

- **Site:** SOLEIL
- **Author:** M. Savko and E. Farhi
- **Origin:** Synchrotron SOLEIL
- **Date:** 2022-03-23

## Description

```text
PROXIMA 2A (PX2-A) is a microfocus beamline dedicated to biological
crystallography and innovative micro-beam methodologies. Opened to the
scientific community since 2013, the topics treated on the beamline go beyond
standard protein crystallography and include drug discovery, membrane proteins,
virus crystallography, small molecule crystallography, powder diffraction and
even crystalluria. The beamline is highly automated and designed to help
scientists tackle the most challenging structural targets and biological
systems. The X-ray energy is rapidly tunable over a wide range (6-18 keV) making
the most commonly used absorption edges accessible for anomalous diffraction
experiments. The end station is equipped with a high capacity sample-changing
system (148 SPINE pins), a high performance micro-diffractometer with a
mini-kappa (MD2), an X-ray fluorescence detector (KETEK), and a fast, low-noise,
photon-counting, area detector - the EIGER X 9M (238 fps in 9M mode, 750 fps in
4M mode).

This is a simplified model with an Undulator, mirrors, a double crystal
monochromator, a sample stage and a detector.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| E0 | keV | Energy selected at the Undulator. | 12.65 |
| hfm_radius | m | horizontally focusing mirror radius | 495 |
| vfm_radius | m | vertically focusing mirror radius | 859 |
| mirror_grazing_angle | deg | Tilt angle of the mirrors. | 4e-3 |
| sample | str | Sample structure file, LAU/CIF format. | "adrenaline.lau" |
| rotX | deg | Sample rotation around X | 0 |
| rotY | deg | Sample rotation around Y | 0 |
| rotZ | deg | Sample rotation around Z | 0 |

## Links

- [Source code](SOLEIL_PX2a.instr) for `SOLEIL_PX2a.instr`.
- https://www.synchrotron-soleil.fr/en/beamlines/proxima-2a

---
