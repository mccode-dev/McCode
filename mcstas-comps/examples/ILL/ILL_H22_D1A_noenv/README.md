# The `ILL_H22_D1A` Instrument

*McStas: Simple monochromator Diffractometer for powders (D1A) installed on H22, with
container/sample environment and radial collimator.*

## Identification

- **Site:** ILL
- **Author:** E. Farhi
- **Origin:** ILL
- **Date:** 13 Apr 2006

## Description

```text
D1A is a reliable diffractometer for standard crystallographic problems. It gives
excellent results with the Rietveld method owing to its near perfect Gaussian
peak-shape in the 2θ-range 30° to 150°.

Special features include
a high fixed take-off angle of 122 deg, giving high resolution at large
scattering angles (up to 160 deg);
a bank of 25 high efficiency collimators and counters;
an anisotropically squashed germanium monochromator focussing a 250
mm high beam onto only 30 mm;
a wide choice of wavelengths, from 1.39 Angs to 2.99 Angs, quickly available by
simple rotation of the focussing monochromator;

The large monochromator take-off angle means that the diffraction pattern is
focussed for the parallel geometry shown (2θ = 122°). The counter can be swept
through 0 deg to 2θ = 160° deg for the highest angle counter, usually in steps
of 0.05 deg.
Monochromator           Neutron wavelength
Ge 117 DM=0.7946 AA     1.390
Ge 335 DM=0.8655 AA     1.514
Ge 115 DM=1.0925 AA     1.911 (optimal)
Ge 113 DM=1.712 AA      2.994

The sample is a powder, in a container can, all positioned in an
Al environment (e.g. cryostat/furnace shield).

This instrument was installed on the H22 guide.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | Wavelength at monochromator | 1.911 |
| dlambda | Angs | Wavelength-spread around lambda at source | 0.03 |
| DM | Angs | d-spacing of monochromator. Use DM=0 to compute the values from the requested wavelength. | 0 |
| RV | m | Radius of vertical focussing. flat for 0 | -1 |
| powder | str | File name for powder sample description | "Na2Ca3Al2F14.laz" |
| container | str | File name for container decription in Al cryostat/furnace | "V.laz" |

## Links

- [Source code](ILL_H22_D1A_noenv.instr) for `ILL_H22_D1A_noenv.instr`.

---
