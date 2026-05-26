# The `ILL_H512_D22` Instrument

*McStas: The H512 cold guide at the ILL, with D22*

## Identification

- **Site:** ILL
- **Author:** FARHI Emmanuel (farhi@ill.fr)
- **Origin:** ILL
- **Date:** May, 2011

## Description

```text
This model describes the H512 cold guide at the ILL, with D22.

The D22 Large dynamic range small-angle diffractometer
is fully simulated. A sample can be specified (liquid),
with monitoring of the scattering in angular (diffraction) and energy modes
(for spectroscopy).
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | central wavelength band for guide illumination and D22 velocity selector setting wavelength | 4.5 |
| dlambda | Angs | half width of guide wavelength band | .45 |
| D22_collimation |  |  | 2 |
| D22_sample | string | D22 liquid/powder/amorphous sample | "H2O_liq.qSq" |

## Links

- [Source code](ILL_H512_D22.instr) for `ILL_H512_D22.instr`.

---
