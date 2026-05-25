# The `Test_Pol_TripleAxis` Instrument

*McStas: Based on Emmanuel Farhi's thermal H8 triple-axis spectrometer from
Brookhaven reactor*

## Identification

- **Site:** Tests_polarization
- **Author:** Peter Christiansen
- **Origin:** RISOE
- **Date:** July 2006.

## Description

```text
This instrument is a simple model of the thermal H8 triple-axis
spectrometer from former Brookhaven reactor. It has an (polarizing)
monochromator and a (polarizaing) analyzer. The sample is a vanadium
cylinder.
Three cases can be done:
-1) Spin flip (flipper is inserted): Up->Down
0) No polarization: Up/Down->Up/Down
1) No spin flip: Up->Up
The Vanadium sample should give I(0)=2*(I(-1)+I(1)) and I(-1)=2*I(1).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| OPTION | 1 | See above | 0 |
| LAMBDA | Angs | Source wavelength | 2.0 |
| MOZ | Arc minutes | Mosaicity | 40 |

## Links

- [Source code](Test_Pol_TripleAxis.instr) for `Test_Pol_TripleAxis.instr`.

---
