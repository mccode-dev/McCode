# The `ILL_H16_IN5` Instrument

*McStas: The full IN5B: H16 guide & chopper system + sample + PSD and tof detector*

## Identification

- **Site:** ILL
- **Author:** E. Farhi, J. Ollivier, Celia Castan Guerrero
- **Origin:** ILL
- **Date:** Jan 2004-2009

## Description

```text
The IN5@ILL TOF spectrometer from cold source to final detector, with sample.
The detector model includes Fe housing and tube cross talk absorbing masks
with angle restriction (neutrons that scatter in Fe in front of a tube and
enter a different tube are absorbed).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | mean incident wavelength | 4.5 |
| dlambda | AA | wavelength half width. Use e.g. 0.8*lambda for white beam. | 0.09 |

## Links

- [Source code](ILL_H16_IN5.instr) for `ILL_H16_IN5.instr`.
- The <a href="http://www.ill.eu/in5">IN5@ILL</a> cold time of flight instrument

---
