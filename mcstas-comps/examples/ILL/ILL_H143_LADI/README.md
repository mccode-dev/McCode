# The `ILL_H143_LADI` Instrument

*McStas: The LADI protein crystallography cold Laue diffractometer*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** Sept 2014.

## Description

```text
LADI-III is a quasi-Laue neutron diffractometer used for single-crystal studies
of biological macromolecules at high resolution (1.5 - 2.5Å) in order to locate
individual protons or deuterons of special interest, water structures or other
small molecules that can be marked with deuterium to be particularly visible.
Data collection is feasible for samples with unit-cell edges ranging from 50
to 150Å using crystal volumes from ~0.05 to 0.5mm3, respectively.

LADI-III uses a large cylindrical area detector composed of neutron-sensitive
image-plates, which completely surround the crystal and allows large numbers
of reflections to be recorded simultaneously. Data are collected using a
quasi-Laue method in order to provide a rapid survey of reciprocal space, while
reducing the background on the detector compared to use of the full white beam.

LADI:
Guide H143:
H14_1     H14_2     H14_3 [OT1] H143_1 [OT2]  H143_2    last
Length  [m]   6.8       6         21          30            35        1
m             2         2         2           1,bot=1.4     1,bot=1.4 idem
Rh      [km]  -         2.7       4.0         -             -         -
Rv      [km]  -         -         -           2.0           2.0       2.0
n_elm.        2         5         11          15            14        last=1m
section [mm]  45x220    45x220    45x220-200  45x30+95      30x30+95  [w*h+v.offset]

Filter:      Ni/Ti multilayer band-pass filter, Quasi-Laue (dlambda/lambda<30%)
Collimation: Pinholes 0.5 to 2.9 mm
Detector:    image plate 1250 x 450 mm2
Sample:      at 2.710 mm from the end of the guide H143.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | wavelength | 3.2 |
| dlambda | Angs | wavelength HALF spread. | 1 |
| reflections | str | list of reflections describing the sample SX | "leucine.lau" |

## Links

- [Source code](ILL_H143_LADI.instr) for `ILL_H143_LADI.instr`.

---
