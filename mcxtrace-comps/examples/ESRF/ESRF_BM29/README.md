# The `ESRF_BM29` Instrument

*McXtrace: A simple BM29 model at ESRF.*

## Identification

- **Site:** ESRF
- **Author:** Martin Cramer Pedersen (mcpe@nbi.dk)
- **Origin:** KU-Science
- **Date:** October 28th, 2021

## Description

```text
Toy model used for testing various sample components for solution-SAXS
mimicking the geometry of the BM29 beamline at ESRF:
https://www.esrf.fr/home/UsersAndScience/Experiments/MX/About_our_beamlines/bm29/beamline-specifications.html

Note that this model does not include much of the beamline optics, instead it emulates their effect
by settgin relevant beam parameters from a model source.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| DistanceFromSourceToFirstPinhole | m | Distance to first pinhole from source. | 31.4 |
| DistanceFromSourceToSecondPinhole | m | Distance to second pinhole - used for focussing rays. | 42.5 |
| DistanceFromSecondPinholeToSample | m | Collimation length. | 0.1 |
| DistanceFromSampleToDetector | m | Sample-detector-distance. | 2.43 |
| RadiusOfDetector | m | Radius of the circular detector. | 0.18 |
| Lambda | AA | Wavelength of the rays emitted from source. | 1.00 |
| DLambda |  | Relative deviation of wavelength of the rays emitted from source. | 0.01 |

## Links

- [Source code](ESRF_BM29.instr) for `ESRF_BM29.instr`.

---
