# The `Test_SANS` Instrument

*McStas: Toy model used for testing various sample components for solution-SANS.*

## Identification

- **Site:** Tests_samples
- **Author:** Martin Cramer Pedersen (mcpe@nbi.dk) and Søren Kynde (kynde@nbi.dk)
- **Origin:** KU-Science
- **Date:** October 29th, 2012

## Description

```text
Toy model used for testing various sample components for solution-SANS.

The following SANS samples are handled:
SANSSpheres               SAMPLE=0
SANSShells                SAMPLE=1
SANSCylinders             SAMPLE=2
SANSEllipticCylinders     SAMPLE=3
SANSLiposomes             SAMPLE=4
SANSNanodiscs             SAMPLE=5
SANSNanodiscsWithTags     SAMPLE=6
SANSPDB                   SAMPLE=7 (very slow, rather use SANSPDBFast)
SANSCurve                 SAMPLE=8 (inactivated)
SANSNanodiscsFast         SAMPLE=9
SANSNanodiscsWithTagsFast SAMPLE=10
SANSPDBFast               SAMPLE=11
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| DistanceFromSourceToFirstPinhole | m | Distance to first pinhole from source. | 1.0 |
| DistanceFromSourceToSecondPinhole | m | Distance to second pinhole - used for focusing rays. | 10.0 |
| DistanceFromSecondPinholeToSample | m | Collimation length. | 1.0 |
| DistanceFromSampleToDetector | m | Sample-detector-distance. | 10.0 |
| RadiusOfDetector | m | Radius of the circular detector. | 4.0 |
| Lambda | AA | Wavelength of the rays emitted from source. | 4.5 |
| DLambda |  | Relative deviation of wavelength of the rays emitted from source. | 0.1 |
| SAMPLE |  | Index of sample model, see above. | 0 |
| Ncount | 1 | Override the number of rays to simulate. | 0 |

## Links

- [Source code](Test_SANS.instr) for `Test_SANS.instr`.

---
