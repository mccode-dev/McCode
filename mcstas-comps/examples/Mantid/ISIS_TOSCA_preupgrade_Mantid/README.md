# The `ISIS_TOSCA_preupgrade_Mantid` Instrument

*McStas: TOSCA (pre-upgrade) at ISIS with Mantid support.*

## Identification

- **Site:** Mantid
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

Please follow instructions at https://github.com/mccode-dev/McCode/wiki/McStas-and-Mantid
to compile with NeXus and generate a Mantid-IDF before attempting to use with Mantid.

To generate the right flightpath length in Mantid, this version of the instrument has
been created with transmitting rather than reflecting analysers, and the detectors are hence
also positioned differently than in the physical instrument.

Example: ISIS_TOSCA_preupgrade_Mantid.instr inc=Benzene_inc_CASTEP_MDANSE2018.sqw eventmode=1 Detector: Sphere_I=0.686792
Example: ISIS_TOSCA_preupgrade_Mantid.instr inc=Benzene_inc_CASTEP_MDANSE2018.sqw eventmode=1 --format=NeXus Detector: Sphere_I=0.686792
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| inc | filename | Incoherent scattering model to use | "Benzene_inc_CASTEP_MDANSE2018.sqw" |
| zdepth | m | Thickness of sample | 0.002 |
| Emin | meV | Minimum energy to generate at the source | 0.01 |
| Emax | meV | Maximum energy to generate at the source | 500 |
| eventmode | boolean | Flag to enable event monitors | 0 |

## Links

- [Source code](ISIS_TOSCA_preupgrade_Mantid.instr) for `ISIS_TOSCA_preupgrade_Mantid.instr`.
- <a href="https://www.isis.stfc.ac.uk/Pages/tosca.aspx">TOSCA instrument webpage</a>
- <a href="https://http://mdanse2018.essworkshop.org">MDANSE 2018 school webpage</a>

---
