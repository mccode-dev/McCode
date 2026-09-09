# The `Test_Powders` Instrument

*McStas: Test output of PowderN, NCrystal and Single_crystal on a spherical monitor / PSD.*

## Identification

- **Site:** Tests_samples
- **Author:** M. Bertelsen and P. Willendrup
- **Origin:** ESS DMSC
- **Date:** November 2024

## Description

```text
A test instrument for Powder output from different sample components.
```

## Examples

- **Test: lambda=2.5 directbeam=0 comp=0 material=Ge SPLITS=1 Detector: Sph_mon_I=3.67e+08**
- **Test: lambda=2.5 directbeam=0 comp=0 material=Ge SPLITS=1 Detector: psd_mon_I=7.35e+05**
- Example: lambda=2.5 directbeam=0 comp=1 material=Ge SPLITS=1 Detector: Sph_mon_I=4.2e+08
- Example: lambda=2.5 directbeam=0 comp=1 material=Ge SPLITS=1 Detector: psd_mon_I=8.3e+05
- **Test: lambda=2.5 directbeam=0 comp=2 material=Ge SPLITS=1 Detector: Sph_mon_I=4.2e+08**
- **Test: lambda=2.5 directbeam=0 comp=2 material=Ge SPLITS=1 Detector: psd_mon_I=8.3e+05**
- **Test: lambda=2.5 directbeam=0 comp=3 material=Ge SPLITS=1 Detector: Sph_mon_I=4.2e+08**
- **Test: lambda=2.5 directbeam=0 comp=3 material=Ge SPLITS=1 Detector: psd_mon_I=8.3e+05**
- **Test: lambda=2.5 directbeam=0 comp=0 material=Al twotheta=76.5 SPLITS=1 Detector: Sph_mon_I=9.1e+07**
- **Test: lambda=2.5 directbeam=0 comp=0 material=Al twotheta=76.5 SPLITS=1 Detector: psd_mon_I=1.94e+05**
- Example: lambda=2.5 directbeam=0 comp=1 material=Al twotheta=76.5 SPLITS=1 Detector: Sph_mon_I=1.1e+08
- Example: lambda=2.5 directbeam=0 comp=1 material=Al twotheta=76.5 SPLITS=1 Detector: psd_mon_I=2e+05
- **Test: lambda=2.5 directbeam=0 comp=2 material=Al twotheta=76.5 SPLITS=1 Detector: Sph_mon_I=1.1e+08**
- **Test: lambda=2.5 directbeam=0 comp=2 material=Al twotheta=76.5 SPLITS=1 Detector: psd_mon_I=2e+05**
- **Test: lambda=2.5 directbeam=0 comp=3 material=Al twotheta=76.5 SPLITS=1 Detector: Sph_mon_I=1.1e+08**
- **Test: lambda=2.5 directbeam=0 comp=3 material=Al twotheta=76.5 SPLITS=1 Detector: psd_mon_I=2.19e+05**
- **Test: lambda=2.5 directbeam=0 comp=0 material=LaMnO3 twotheta=80 SPLITS=1 Detector: Sph_mon_I=2.48e+08**
- **Test: lambda=2.5 directbeam=0 comp=0 material=LaMnO3 twotheta=80 SPLITS=1 Detector: psd_mon_I=1.78e+05**
- Example: lambda=2.5 directbeam=0 comp=1 material=LaMnO3 twotheta=80 SPLITS=1 Detector: Sph_mon_I=3.1e+08
- Example: lambda=2.5 directbeam=0 comp=1 material=LaMnO3 twotheta=80 SPLITS=1 Detector: psd_mon_I=2.1e+05
- **Test: lambda=2.5 directbeam=0 comp=2 material=LaMnO3 twotheta=80 SPLITS=1 Detector: Sph_mon_I=3.1e+08**
- **Test: lambda=2.5 directbeam=0 comp=2 material=LaMnO3 twotheta=80 SPLITS=1 Detector: psd_mon_I=1.9e+05**
- **Test: lambda=2.5 directbeam=0 comp=3 material=LaMnO3 twotheta=80 SPLITS=1 Detector: Sph_mon_I=3.1e+08**
- **Test: lambda=2.5 directbeam=0 comp=3 material=LaMnO3 twotheta=80 SPLITS=1 Detector: psd_mon_I=1.9e+05**
- **Test: lambda=2.5 directbeam=0 comp=0 material=NaCl twotheta=78 SPLITS=1 Detector: Sph_mon_I=2.13e+08**
- **Test: lambda=2.5 directbeam=0 comp=0 material=NaCl twotheta=78 SPLITS=1 Detector: psd_mon_I=3.2e+05**
- Example: lambda=2.5 directbeam=0 comp=1 material=NaCl twotheta=78 SPLITS=1 Detector: Sph_mon_I=2.5e+08
- Example: lambda=2.5 directbeam=0 comp=1 material=NaCl twotheta=78 SPLITS=1 Detector: psd_mon_I=3.4e+05
- **Test: lambda=2.5 directbeam=0 comp=2 material=NaCl twotheta=78 SPLITS=1 Detector: Sph_mon_I=2.5e+08**
- **Test: lambda=2.5 directbeam=0 comp=2 material=NaCl twotheta=78 SPLITS=1 Detector: psd_mon_I=3.4e+05**
- **Test: lambda=2.5 directbeam=0 comp=3 material=NaCl twotheta=78 SPLITS=1 Detector: Sph_mon_I=2.5e+08**
- **Test: lambda=2.5 directbeam=0 comp=3 material=NaCl twotheta=78 SPLITS=1 Detector: psd_mon_I=3.4e+05**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| comp | 1 | 0=PowderN-with-NCMAT, 1=PowderN, 2=Single_crystal, 3=NCrystal | 1 |
| lambda | Angs | Wavelength emitted from source, 1% wl-interval around. | 2.5 |
| L1 | m | Source-sample distance | 10 |
| directbeam | 1 | Suppress direct beam or not | 0 |
| material | str | Material string for picking up CIF (PowderN/Single_Crystal) and ncmat (NCrystal) | "Ge" |
| SPLITS | 1 | Number of SPLIT's before sample | 1 |
| frac_t | 1 | Fraction of stats assigned to unscattered, "direct beam" | 0.1 |
| sxmos | arcmin | Mosaicity to use in Single_crystal case | 60 |
| twotheta | deg | Angle for 2\theta detector for detailed view | 94 |

## Links

- [Source code](Test_Powders.instr) for `Test_Powders.instr`.

---
