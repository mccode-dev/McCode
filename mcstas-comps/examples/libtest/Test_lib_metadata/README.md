# The `Test_lib_metadata` Instrument

*McStas: Unit test of common/lib/share/metadata-r (the METADATA table accessors), with
METADATA of the instrument and of a component instance. Covers lookup by
source and by source:name, quoted names with and without spaces, the
--meta-list listing, a missing key, and the key "::".
The instrument exits in INITIALIZE if any check failed.*

## Identification

- **Site:** libtest
- **Author:** 
- **Origin:** 
- **Date:** 

## Description

```text
Tests: mccode-dev/McCode#3180
```

## Examples

- **Test: Ncount=10 Detector: PSD_I=10**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| Ncount | 1 | Number of particles | 10 |

## Links

- [Source code](Test_lib_metadata.instr) for `Test_lib_metadata.instr`.

---
