# The `Tomography` Instrument

*McStas: Instrument to study tomographic imaging by means of the feature of OFF shape samples.*

## Identification

- **Site:** Templates
- **Author:** Peter Willendrup, based on work by Reynald ARNERIN
- **Origin:** Risoe
- **Date:** June 20th, 2008

## Description

```text
Instrument to study tomographic imaging by means of the feature of OFF shape samples.

Example: mcrun Tomography.instr offfile=bunny.off -n1e4 -N18 omega=0,340 -d TomoScan
(Note that to achieve proper statistics for tomographic reconstruction, MUCH higher ncounts
are needed)

Use the provided Matlab tomo_recon.m function (requires imaging toolbox, PGPLOT output data
and a Unix/Mac) in the tools/matlab folder to reconstruct a 3D volume of the object. Use e.g.
isosurface to do thresholding for extraction of the object surface.
```

## Input parameters

Parameters in **boldface** are required; the others are optional.

| Name | Unit | Description | Default |
|------|------|-------------|---------|
| geometry | string | Name of the OFF file describing the sample shape | "socket.off" |
| omega | deg | Sample rotation around y | 0 |
| sigma_abs | barn | Sample absorption xs | 100 |
| frac_scatt | 1 | Fraction of neutrons to scatter in the sample | 0 |
| div_v | deg | Source vertical divergence (angular height) | 1e-4 |
| div_h | deg | Source horisontal divergence (angular width) | 1e-4 |
| source_w | m | Source width | 0.4 |
| source_h | m | Source height | 0.2 |
| det_w | m | Detector width | 0.4 |
| det_h | m | Detector height | 0.2 |
| opts | string | Monitor_nD options string | "x bins=80 y bins=40" |

## Links

- [Source code](Tomography.instr) for `Tomography.instr`.
- http://shape.cs.princeton.edu/benchmark/documentation/off_format.html

---
