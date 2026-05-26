# The `ISIS_GEM` Instrument

*McStas: McStas instrument for simulating the GEM diffractometer at ISIS TS1.*

## Identification

- **Site:** ISIS
- **Author:** E. Farhi, G. Cuello, M. Tucker
- **Origin:** ISIS
- **Date:** September 20, 2006

## Description

```text
McStas instrument for simulating GEM at ISIS TS1.
The sample is a powder. The detector is simplified as a banana shaped one.

The General Materials Diffractometer is a new generation neutron diffractometer
recently constructed at the ISIS pulsed neutron source. GEM can be used to
perform high intensity, high resolution experiments to study the structure of
disordered materials and crystalline powders.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| l_min | AA | Minimum wavelength produced at source | 0.1 |
| l_max | AA | Maximum wavelength produced at source | 4.2 |
| dist | m | Sample-detector distance (detector radius) | 1.3795 |
| sample | string | Reflection list for powder sample | "Y2O3.laz" |

## Links

- [Source code](ISIS_GEM.instr) for `ISIS_GEM.instr`.
- The <a href="http://essworkshop.org">ESS Instrument workshops</a> website.
- The <a href="http://www.isis.stfc.ac.uk/instruments/gem/">GEM instrument</a> at ISIS

---
