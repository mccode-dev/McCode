# The `ISIS_TOSCA_preupgrade` Instrument

*McStas: TOSCA (pre-upgrade) at ISIS*

## Identification

- **Site:** ISIS
- **Author:** Peter Willendrup (DTU/ESS), Sanghamitra Mukhopadhyay (ISIS STFC)
- **Origin:** DTU/ESS/ISIS
- **Date:** September 2018

## Description

```text
The model at hand is adapted from an earlier model by M.Zanetti and represents the
TOSCA instrument as it looked prior to the recent upgrade. The model at hand was
adapted during the MDANSE2018 workshop (http://mdanse2018.essworkshop.org) and by
default uses a model of the incoherent scattering from Benzene, created using
CASTEP (http://www.castep.org ) and converted to Sqw format iFit (http://ifit.mcode.org)

The model is naturally very challenging to run because of the indirect TOF geometry
and would benefit from an adaptive sampling approach.

Please use the version with name ISIS_TOSCA_preupgrade_Mantid to generate Mantid-oriented
output.

Example: ISIS_TOSCA_preupgrade.instr inc=Benzene_inc_CASTEP_MDANSE2018.sqw -n1e7
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| inc | filename | Incoherent scattering model to use | "Benzene_inc_CASTEP_MDANSE2018.sqw" |
| zdepth | m | Thickness of sample | 0.002 |
| Emin | meV | Minimum energy to generate at the source | 0.01 |
| Emax | meV | Maximum energy to generate at the source | 500 |

## Links

- [Source code](ISIS_TOSCA_preupgrade.instr) for `ISIS_TOSCA_preupgrade.instr`.
- <a href="https://www.isis.stfc.ac.uk/Pages/tosca.aspx">TOSCA instrument webpage</a>
- <a href="https://http://mdanse2018.essworkshop.org">MDANSE 2018 school webpage</a>

---
