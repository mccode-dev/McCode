# The `` Instrument

*McStas: Regression test of the NeXus writer of the McCode runtime.*

## Identification

- **Site:** Tests_other
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** October 2026

## Description

```text
Monitors of every NeXus output kind (0D, 1D, 2D, Monitor_nD histogram,
Monitor_nD BINS maps of rank 2 and 3, and an event list) for
test_nexus_output.py, which runs this instrument with --format=NeXus,
serial and with MPI, and checks mccode.h5 with h5py:

- string values containing '%' are stored as given (instrument parameter
'tag', component parameter literal 'filename="f%%1"')
- all string attributes and data sets are printable text
(no uninitialised memory, e.g. in the Monitor_nD BINS output)
- no debug output on stdout
- a monitor with a very long file name ('longname', about 1000
characters) does not crash the simulation or lose other monitors
- a second run with --append adds entry2 to mccode.h5

The instrument also runs in the default (McCode) format.
```

## Examples



## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|

## Links

- [Source code](Test_NeXus_output.instr) for `Test_NeXus_output.instr`.
- test_nexus_output.py next to this instrument.

---
