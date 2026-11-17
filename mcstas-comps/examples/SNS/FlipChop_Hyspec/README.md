# The `FlipChop_Hyspec` Instrument

*McStas: Simulation of a statistical resonant RF neutron spin flip "chopper" at Hyspec.*

## Identification

- **Site:** SNS
- **Author:** Niels Geerits and Thomas Huegle
- **Origin:** ORNL
- **Date:** November 2026

## Description

```text
This instrument demos the FlipChopFlipper component, a resonant RF neutron spin flipper being
operated as a correlation chopper, on the HYSPEC spectrometer at the Spallation Neutron Source (SNS).

The instrument models the Fermi chopper, a vertically focusing HOPG monochromator, an ideal neutron polarizer,
the FlipChopFlipper component, an ideal polarization analyzer, and a time-of-flight detector.

The FlipChopFlipper applies an arbitrary binary modulation sequence to the neutron spin. The default is a 16 bit
Golay sequence. The spin rotation is calculated by integrating the RF interaction over the finite transit time
through the flipper. In TOF mode, the RF amplitude is ramped using the time elapsed since the source pulse.

The flipper is positioned 1 m upstream of the nominal sample position, and the detector is located 3.1 m downstream of the sample.

Example:
mcrun FlipChop_Hyspec.instr Polarizer_py=-1 Analyzer_my=1
```

## Examples

- **Test: Polarizer_py=-1 Analyzer_my=1 Detector: TOF_after_Analyzer_I=32942.5**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Polarizer_py | 1 | Initial neutron polarization along the y-axis | -1 |
| Analyzer_my | 1 | Analyzer polarization direction along the y-axis | 1 |

## Links

- [Source code](FlipChop_Hyspec.instr) for `FlipChop_Hyspec.instr`.
- https://mcstas.org
- https://neutrons.ornl.gov/hyspec

---
