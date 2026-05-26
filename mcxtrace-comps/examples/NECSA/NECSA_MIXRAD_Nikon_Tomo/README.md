# The `NECSA_MIXRAD_Nikon_Tomo` Instrument

*McXtrace: Nikon XTH 225 Tomograph*

## Identification

- **Site:** NECSA
- **Author:** us all at Necsa
- **Origin:** Necsa, ZA
- **Date:** Oct 11th 2024

## Description

```text
This is a simple description of the MIXRAD Nikon X-ray tomograph installed at NECSA, ZA

- The source voltage is 30-225 kV with a electron beam current of 1 mA.
- Anode material: Mo; Cu; W; Ag
- The source spot size is 1-3 um.
- The emission cone is 25 degrees.
- The detector is 400x400 mm, with a 200 um pixel size (2000x2000).
- The source-sample distance is at least 61 cm.
- The source-detector distance is 1.147 m.

Example: <parameters=values>
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Source_material | str | source anode, e.g. W.txt | "W.txt" |
| Emin | keV | minimum energy at the source | 1 |
| Emax | keV | maximum energy at the source | 75 |
| Sample_material | str | sample material, e.g. chemical formulae or CIF file | "Fe2O3Au" |
| sample_shape | str | OFF/PLY file | "king.off" |
| Sample_rotation | deg | sample rotation angle around Y vertical | 90 |
| Sample_distance | deg | sample location wrt source. detector fixed at 1.147 m | 0.5 |

## Links

- [Source code](NECSA_MIXRAD_Nikon_Tomo.instr) for `NECSA_MIXRAD_Nikon_Tomo.instr`.
- https://www.necsa.co.za/service/micro-focus-x-ray-radiography-and-tomography-facility-mixrad/
- https://www.ndt.net/article/wcndt2012/papers/37_wcndtfinal00037.pdf

---
