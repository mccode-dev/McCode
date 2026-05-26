# The `templateSANS_Mantid` Instrument

*McStas: Test instrument for the Sans_spheres component. No guide / velocity selector
etc. Will be developed further at later time.*

## Identification

- **Site:** Mantid
- **Author:** <a href="mailto:kim.lefmann@risoe.dk">Kim Lefmann</a>
- **Origin:** <a href="http://www.risoe.dk">Risoe</a>
- **Date:** 19th Dec 2003.

## Description

```text
Very simple test instrument for the Sans_spheres componen

Modified to show a proof of concept method for storing a 'Mantid friendly' type of NeXus file.

Needed steps:
1) Compile your instrument with NeXus library support
2) Generate an IDF using mcdisplay templateSANS_Mantid --format=Mantid -n0
3) mcrun templateSANS_Mantid -n1e6 --format=NeXus
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| lambda | AA | Mean wavelength of neutrons | 6 |
| dlambda | AA | Wavelength spread of neutrons | 0.05 |
| r | AA | Radius of scattering hard spheres | 150 |
| PHI | 1 | Particle volume fraction | 1e-3 |
| Delta_Rho | fm/AA^3 | Excess scattering length density | 0.6 |
| sigma_abs | barns | Absorption cross section at 2200 m/s | 0.0 |

## Links

- [Source code](templateSANS_Mantid.instr) for `templateSANS_Mantid.instr`.

---
