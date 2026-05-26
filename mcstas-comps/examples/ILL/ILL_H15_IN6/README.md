# The `ILL_H15_IN6` Instrument

*McStas: The IN6 Time-of-Flight simulation, positioned as the first instrument in the
cold guide H15 (Nickel coating) at the ILL.*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:farhi@ill.fr">Emmanuel Farhi</a>
- **Origin:** <a href="http://www.ill.fr">ILL (France)</a>
- **Date:** 17th Jan 2005.

## Description

```text
IN6 is a time focussing time-of-flight spectrometer designed for quasielastic and
inelastic scattering for incident wavelengths in the range of 4 to 6 Angs.

An intense beam is extracted from the H15 guide by a vertically focussing
monochromator array. It consists of three composite pyrolytic graphite
monochromators using the full height (20 cm) of the guide and focussing the beam
at the sample position. In order to minimise the interference with the
subsequent instruments, the monochromator can deliver only four wavelengths:
4.1; 4.6; 5.1; and 5.9 Angs. The second order reflection from the graphite
monochromator is removed by a beryllium-filter cooled at liquid nitrogen
temperature.

To achieve the time-focussing condition, the beam is pulsed by a Fermi chopper.
It has a small slot length to ensure a good transmission. The normal distance
between the Fermi chopper and the sample is 38 cm. To prevent frame-overlap when
the chopper is rotating faster than 7500 rpm, a suppressor chopper is placed
before the Fermi chopper and rotates in phase with the latter.

The secondary spectrometer consists first of an evacuated sample area. The
detector bank is entirely covered with detector boxes, thus avoiding the
inconvenience of moving the counters.

This instrument model contains the complete H15 guide, a triple monochromator
(using the GROUP), two Fermi Choppers (including one background chopper), a
liquid sample handling coherent and incoherent processes (elastic and inelastic)
with multiple scattering, customized monitors, and the SPLIT mechanism to
improve the statistics.

Example: lambda=4.14 Detector: M_theta_t_all_I=70
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | Angs | wavelength within 4.14\|4.6\|5.12\|5.92 | 4.14 |
| dlambda | Angs | wavelength HALF spread. default is 0.075 | 0.075 |

## Links

- [Source code](ILL_H15_IN6.instr) for `ILL_H15_IN6.instr`.
- <a href="http://www.ill.fr/in6">The IN6@ILL Yellow Pages</a>
- R.Scherm et al, "Another Time of Flight Spectrometer", ILL Report 76S235, 1976
- R.Scherm, "A high-resolution spectrometer ...", report Jul-295-NP, Kernforschungsanlage Julich, 1965
- Y.Blanc, "Le spectrometre a temps de vol IN6", ILL Report 83BL21G, 1983
- K.H.Beckurts et al, Neutron physics, Kernforschungsanlage Karlsruhe, 1964 (p317)
- R.Scherm and T.Springer, "Proposal of a multiple Chopper...", Kernforschungsanlage Julich, 19xx

---
