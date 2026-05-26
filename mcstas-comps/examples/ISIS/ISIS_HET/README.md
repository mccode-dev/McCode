# The `ISIS_HET` Instrument

*McStas: HET: High Energy Transfer Chopper Spectrometer*

## Identification

- **Site:** ISIS
- **Author:** <a href="mailto:d.champion@rl.ac.uk">Dickon Champion</a>
- **Origin:** <a href="http://www.isis.rl.ac.uk">ISIS (UK)</a>
- **Date:** 22nd Jan 2004.

## Description

```text
This instrument is a simple model of the HET spectrometer at the ISIS neutron
facility. The input arguments are hardwired so that the Fermi chopper position
is 10 metres from the moderator. This instrument uses the FC module written by
Andrew Garret which comes with no guarantees as to its accuracy in modelling the
Fermi Chopper.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Emin | meV | lowest energy sampled | 443.0 |
| Emax | meV | highest energy sampled | 470.0 |
| nu_chop | Hz | frequency of chopper rotation | 600.0 |
| type |  | chopper package - sloppy chopper = 1, B chopper = 2 | 2 |

## Links

- [Source code](ISIS_HET.instr) for `ISIS_HET.instr`.

---
