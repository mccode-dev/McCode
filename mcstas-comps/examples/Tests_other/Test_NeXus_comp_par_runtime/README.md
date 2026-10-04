# The `Test_NeXus_comp_par_runtime` Instrument

*McStas: Test of NeXus output of component parameter values at end of INITIALIZE.*

## Identification

- **Site:** Tests_other
- **Author:** Peter Willendrup
- **Origin:** DTU
- **Date:** October 2026

## Description

```text
Unit test for the 'runtime_value' / 'runtime_value_status' attributes written
to entry1/instrument/components/NNNN_<comp>/parameters/<par> when running in
NeXus mode (compile with -DUSE_NEXUS, run with --format=NeXus).

The 'value' attribute holds the literal from this file, 'runtime_value' the
value present in the component instance after all INITIALIZE sections.
Each component is chosen to exercise one case; expected results with the
default instrument parameters (status 'ok' unless stated):

source   (Source_simple)  instrument-parameter expressions + INIT-computed:
lambda0 = lambda          -> 4
dlambda = 0.1*lambda      -> 0.4
dist    = 0 (target_index=1, i.e. distance to slit) -> 1.5
slit     (Slit)           UNSET defaults computed in INIT:
xmin/xmax UNSET -> -0.01/0.01,  ymin/ymax UNSET -> -0.02/0.02
radius    UNSET -> UNSET
psd      (PSD_monitor)    int and string parameters + INIT-computed limits:
nx = npix -> 50,  ny = 2*npix -> 100,  filename = monfile -> psd.dat
xmin/xmax -0.05/0.05 -> -0.02/0.02,  ymin/ymax -> -0.03/0.03
psd_copy (COPY(psd))      second instance of the same type:
nx -> 10, ny -> 100, filename -> psd_copy.dat
lmon     (L_monitor)      expressions of instrument parameters:
Lmin = lambda-1 -> 3,  Lmax = lambda+1 -> 5
banana   (Monitor_nD)     long string, radius-derived size:
radius = L1/3 -> 0.5,  xwidth 0 -> 1,  options -> unchanged string
bender_default (Vertical_Bender) static {..} vectors from the defaults:
rTopPar, rBottomPar, rSidesPar -> {0.99, 0.219, 6.07, 0, 0.003}
bender_custom  (Vertical_Bender) static {..} vector literal per instance:
rTopPar -> {0.95, 0.0218, 4.38, 2, 0.003},  rBottomPar/rSidesPar default
conics   (Conics_PH)      pointer vector from DECLARE:
radii -> no runtime_value, status 'unsupported:pointer'
nshells -> 4, focal_length -> 10 (members after the pointer still 'ok')

No component may report 'layout-mismatch' (a warning is also printed on stderr).

Self-check: in NeXus mode, FINALLY closes the NeXus file and runs
dump_runtime.py (next to this file, needs python3 + h5py) with the expected
values, computed from the actual instrument parameters. The simulation exits
with value 1 if any check fails, 0 on success. Outside NeXus mode the check is
skipped. The python interpreter can be set with the MCCODE_PYTHON variable.
```

## Examples

- **Test: -y --format=NeXus Detector: psd_I=0.000568**

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| L1 | m | Source-slit distance (Source_simple computes dist from it) | 1.5 |
| lambda | AA | Mean wavelength | 4 |
| wslit | m | Slit width (slit height is 2*wslit) | 0.02 |
| npix | 1 | Number of horizontal PSD pixels (vertical is 2*npix) | 50 |
| monfile | str | Output filename of the psd monitor | "psd.dat" |

## Links

- [Source code](Test_NeXus_comp_par_runtime.instr) for `Test_NeXus_comp_par_runtime.instr`.
- <reference/HTML link>

---
