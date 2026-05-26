# The `Cryostat_example` Instrument

*McStas: Cryostat model made with Union components that can be included in other instruments*

## Identification

- **Site:** Union_sample_environments
- **Author:** Mads Bertelsen
- **Origin:** ESS DMSC
- **Date:** July 2024

## Description

```text
Model of a cryostat made using Union components that can be included in another
instrument or executed on its own. Several components are marked with the
removable keyword, meaning these are not included when this instrument is included
in another McStas instrument. This includes the source, but also the Union components
init, master and stop, which the instrument including this sample environment will need
to supply. This is done such that a sample can easily be added.
Only a simple model of Al is used for this cryostat, and it has been named Al_cryostat
to avoid conflicting with existing models of Al in the instrument file that includes this
sample environment.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| stick_displacement | m | height displacement of sample stick | 0 |

## Links

- [Source code](cryostat_example.instr) for `cryostat_example.instr`.

---
