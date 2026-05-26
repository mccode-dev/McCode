# The `ILL_H15_SAM` Instrument

*McStas: The small-angle diffractometer SAM at the ILL.*

## Identification

- **Site:** ILL
- **Author:** <a href="mailto:nicolas.martin2@univ-grenoble-alpes.fr">Nicolas MARTIN</a>
- **Origin:** <a href="https://iramis.cea.fr/llb/">LLB</a>
- **Date:** 2026/04/08

## Description

```text
SAM is a CRG instrument built through a collaboration between the LLB & the ILL between
June 2023 and February 2024. After hot commissioning, first user experiments took place
in June 2024. SAM stands for "Small-Angle Modular instrument". It is a compact SANS
devoted to the study of mesostructures found in various contexts (soft matter, biophysics,
material sciences and magnetism), which also features polarized neutrons capabilities
(not included in this model). The beamline is fed by the completely renewed H15 cold
neutrons guide.

Instrument parameters:

Typical wavelength range: 3.25-24 AA
Guides cross section: 30 x 30 mm<sup>2</sup> (three moveable elements within collimator)
Collimation lengths: [9, 7, 5, 2.5] m
Adjustable 'reflectometer-type' diaphragms located at [9, 5, 2.5] m upstream of sample position.
Sample-to-detector distance: 0.75-6.85 m
Detector: 64 x 64 cm<sup>2</sup> monoblock 3He PSD
See the <a href="https://www.ill.eu/for-all-users/instruments/instruments-list/sam/description/instrument-layout">instrument page</a> on the ILL website for more details.

Example (typical "real-life" measurement sequence):

mcrun ILL_H15_SAM.instr --gravity -n1e7 -dLargeQ lambda=5.2
mcrun ILL_H15_SAM.instr --gravity -n1e7 -dSmallQ guide1=0 guide2=0 guide3=0 lsd=6.8

(Corresponding test-suite entries:)
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Wavelength mean value | 5.2 |
| dlambda | 1 | Wavelength range, full width | 0.2 |
| lambdasel | AA | Mean wavelength selected by the NVS | -1.0 |
| guide1 | 1 | 0 = out, 1 = in | 1 |
| guide2 | 1 | 0 = out, 1 = in | 1 |
| guide3 | 1 | 0 = out, 1 = in | 1 |
| s1w | mm | Width of the 1st adjustable diaphragm | 30 |
| s1h | mm | Height of the 1st adjustable diaphragm | 30 |
| s2w | mm | Width of the 2nd adjustable diaphragm | 30 |
| s2h | mm | Height of the 2nd adjustable diaphragm | 30 |
| s3w | mm | Width of the 3rd adjustable diaphragm | 30 |
| s3h | mm | Height of the 3rd adjustable diaphragm | 30 |
| ssw | mm | Width or diameter of sample slit | 7 |
| ssh | mm | Height of sample slit (0 for circular slit) | 10 |
| sampleno | 1 | Choice of sample: 0 = none, 1 = liquid water, 2 = powder ring, 3 = hard spheres in solution | 3 |
| lsd | m | Sample-to-detector distance | 0.85 |

## Links

- [Source code](ILL_H15_SAM.instr) for `ILL_H15_SAM.instr`.
- <https://www.ill.eu/for-all-users/instruments/instruments-list/sam/description/instrument-layout>

---
