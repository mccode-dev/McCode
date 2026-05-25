# The `NECSA_Brucker_D8_Advance` Instrument

*McXtrace: Brucker D8 Advance lab powder diffratometer*

## Identification

- **Site:** NECSA
- **Author:** us all in ZA !
- **Origin:** Necsa, ZA
- **Date:** 2024 Oct 11th

## Description

```text
This is a lab-scale diffractometer, Brucker D8 Advance type.

- X-ray Tubes Cu (1.54 Å), Co (1.79 Å), Cr (2.29 Å)
- Beam sizes 0.1 x 15 mm2; 1 mm collimator
- Distance btw souurce and 1st slit: 332mm
- sample-detector distance: 435 mm
- Detector LynxEye 1D 192 pixels 14.4 x 16 mm

Example: <parameters=values>
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| source_material | str | anode type, among Cu.txt, Co.txt, Cr.txt | "Cu.txt" |
| Emin | keV | minimum energy at the source | 1 |
| Emax | keV | maximum energy at the source | 9 |
| Gobel_radius | m | Gobel curved mirror curvature radius | 0.5 |
| Gobel_rotation | deg | Gobel curved mirror rotation | 20 |
| Gobel_coating | str | Gobel curved mirror coating, e.g. Ir.dat | "Ir.dat" |
| Sample_material | str | sample material, e.g. CIF/LAU/LAZ file | "Si.laz" |

## Links

- [Source code](NECSA_Brucker_D8_Advance.instr) for `NECSA_Brucker_D8_Advance.instr`.
- http://nanoqam.ca/wiki/lib/exe/fetch.php?media=d8_advance_discover_user_manual_vol._1_doc-m88-exx153_v6.pdf

---
