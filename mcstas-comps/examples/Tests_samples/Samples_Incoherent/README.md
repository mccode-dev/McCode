# The `Samples_Incoherent` Instrument

*McStas: This instrument allows to compare incoherent scattering from different McStas
sample components.*

## Identification

- **Site:** Tests_samples
- **Author:** Peter Willendrup, Erik Knudsen, Aziz Aziz Daoud-aladine
- **Origin:** RISOE
- **Date:** August 14th, 2007

## Description

```text
This instrument allows to compare incoherent scattering from different McStas
sample components:

* PowderN
Single_crystal
Isotropic_sqw
Incoherent

Examples:

Compare incoherent scattering from all sample comps, beamstop in place.

mcrun Samples_Incoherent -d Some_directory -N5 SAMPLE=1,5 STOP=1

Compare incoherent scattering and direct beam attenuation for PowderN, Single_crystal
and Isotroic_sqw:

mcrun Samples_Incoherent -d Some_other_directory -N4 SAMPLE=2,5 STOP=0

Have a closer look of direct beam attentuation for PowderN, Single_crystal
and Isotroic_sqw (has the side-effect that back-scattered neutrons are absorbed):

mcrun Samples_Incoherent -d Some_third_directory -N4 SAMPLE=2,5 STOP=0 DB=1

The sample type selection is:
box:      1=Vanadium
2=PowderN
3=Single_Crystal
4=Isotropic_Sqw
5=Incoherent
10=Isotropic_Sqw with V.lau
cube.off: 6=PowderN,
7=Single_Crystal
8=Isotropic_Sqw
9=Incoherent
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L_min | AA | Minimum wavelength of neutrons | 0.5 |
| L_max | AA | Maximum wavelength of neutrons | 7 |
| SAMPLE |  | Sample choice, 1:Incoherent, 2:PowderN, 3:Single_crystal, 4:Isotropic_Sqw, 5:Incoherent with multiple scattering, 6:PowderN with OFF geometry, 7:Single_crystal with OFF geometry, 8:Isotropic_Sqw with OFF geometry, 9:Incoherent with OFF geometry, 10:Isotropic_Sqw with .laz input | 1 |
| STOP | 1 | Beamstop inserted? (Needed in case of comparison between V and SX/sqw). | 0 |
| Order | 1 | Maximum order of multiple scattering in SX/sqw | 0 |
| INC | barns | Incoherent scattering cross section | 5.08 |
| ABS | barns | Absorption cross section | 5.08 |
| DB | 1 | Direct Beam monitor inserted? (Side-effect: absorbs 'backscattered' neutrons). | 0 |
| ISISFACE | string | ISIS moderator face to look at | "hydrogen" |

## Links

- [Source code](Samples_Incoherent.instr) for `Samples_Incoherent.instr`.
- Validation of Single_crystal is now in progress!

---
