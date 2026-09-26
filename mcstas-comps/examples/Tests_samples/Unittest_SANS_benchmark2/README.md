# The `Unittest_SANS_benchmark2` Instrument

*McStas: Test instrument for the SANS_benchmark2 component. No guide / velocity selector.*

## Identification

- **Site:** Tests_samples
- **Author:** Peter Willendrup
- **Origin:** <a href="http://www.ess.eu">ESS</a>
- **Date:** April 2023

## Description

```text
Very simple test instrument for the SANS_benchmark2 component from H. Frielinghaus.
```

## Examples

- **Test: modnum=1  Detector: detectorSANS_I=0.0123201**
- **Test: modnum=2  Detector: detectorSANS_I=0.00635726**
- **Test: modnum=3  Detector: detectorSANS_I=18.8412**
- **Test: modnum=4  Detector: detectorSANS_I=0.00600315**
- **Test: modnum=5  Detector: detectorSANS_I=0.0310337**
- **Test: modnum=6  Detector: detectorSANS_I=0.660282**
- **Test: modnum=7  Detector: detectorSANS_I=0.776435**
- **Test: modnum=8  Detector: detectorSANS_I=3.89201**
- **Test: modnum=9  Detector: detectorSANS_I=0.0427871**
- **Test: modnum=10 Detector: detectorSANS_I=0.0234776**
- **Test: modnum=11 Detector: detectorSANS_I=0.229431**
- **Test: modnum=12 Detector: detectorSANS_I=0.0026947**
- **Test: modnum=13 Detector: detectorSANS_I=0.00269978**
- **Test: modnum=14 Detector: detectorSANS_I=0.002694**
- **Test: modnum=15 Detector: detectorSANS_I=0.200408**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength of neutrons | 6 |
| dlambda | AA | Wavelength spread of neutrons | 0.05 |
| r | AA | Radius of scattering hard spheres | 150 |
| PHI | 1 | Particle volume fraction | 1e-3 |
| Delta_Rho | cm^-2 | Scattering length density | 6e10 |
| frac_dir | 1 | Fraction of statistics for direct beam | 0.03 |
| frac_inc | 1 | Fraction of statistics for incoherent scattering in scattered beam | 0.01 |
| modnum | int | Sample "modle number" from SANS_benchmark2 | 1 |

## Links

- [Source code](Unittest_SANS_benchmark2.instr) for `Unittest_SANS_benchmark2.instr`.

---
